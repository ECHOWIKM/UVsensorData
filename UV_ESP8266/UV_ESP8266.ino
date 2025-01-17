/*
 * Sensor: CJMCU-GUVA-S12SD/CJMCU-S12D
 * ESP8266 + OLED Display
 * 
 * Sensor     ESP8266
 * SIG    ->    A0
 * GND    ->    GND
 * VCC    ->    3.3V
 * 
 * OLED      ESP8266
 * VCC   ->   3.3V
 * GND   ->   GND
 * SCL   ->   D1
 * SDA   ->   D2
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <LittleFS.h>
#include <WebSocketsServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "sakaiwei";  // 替换为您的WiFi名称
const char* password = "12345678";  // 替换为您的WiFi密码

ESP8266WebServer server(80);  // 创建Web服务器对象
WebSocketsServer webSocket = WebSocketsServer(81);  // 使用81端口

// 定义多个NTP服务器
const char* ntpServers[] = {
  "ntp.aliyun.com",
  "ntp1.aliyun.com",
  "ntp2.aliyun.com",
  "ntp.ntsc.ac.cn",
  "cn.ntp.org.cn",
  "pool.ntp.org"
};
const int ntpServerCount = sizeof(ntpServers) / sizeof(char*);
int currentNtpServer = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.aliyun.com", 28800, 60000);  // 28800 = UTC+8

// 全局变量
int sensorValue;
long sum = 0;
int vout = 0;
int uv = 0;
int darkValue = 0;  // 存储暗值
bool sensorEnabled = true;
int readInterval = 10; // 默认10秒读取一次
unsigned long lastReadTime = 0;
String currentDate = "";

// 数据记录结构
struct UVRecord {
  time_t timestamp;
  int uvIndex;
};

// 获取UV等级对应的颜色
String getUVLevelColor(int uvIndex) {
  if(uvIndex <= 2) {
    return "#4CAF50"; // 绿色
  } else if(uvIndex <= 5) {
    return "#FFC107"; // 黄色
  } else if(uvIndex <= 7) {
    return "#FF9800"; // 橙色
  } else if(uvIndex <= 10) {
    return "#F44336"; // 红色
  } else {
    return "#9C27B0"; // 紫色
  }
}

// 获取UV等级对应的文本描述
String getUVLevelText(int uvIndex) {
  if(uvIndex <= 2) {
    return "低";
  } else if(uvIndex <= 5) {
    return "中等";
  } else if(uvIndex <= 7) {
    return "高";
  } else if(uvIndex <= 10) {
    return "很高";
  } else {
    return "极高";
  }
}

// 添加时间转换辅助函数
time_t getLocalTime() {
    time_t now = timeClient.getEpochTime();
    return now - 28800; // 添加8小时偏移
}

void initializeLittleFS() {
  if(!LittleFS.begin()) {
    Serial.println("LittleFS挂载失败!");  
    Serial.println("正在尝试格式化文件系统...");
    
    // 尝试格式化文件系统
    if(LittleFS.format()) {
      Serial.println("文件系统格式化成功");
      // 重新尝试挂载
      if(LittleFS.begin()) {
        Serial.println("LittleFS挂载成功!");
      } else {
        Serial.println("LittleFS挂载仍然失败，请检查硬件!");
        // 在OLED上显示错误信息
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0,0);
        display.println("FS Error!");
        display.println("Check hardware");
        display.display();
        delay(2000);
      }
    } else {
      Serial.println("文件系统格式化失败!");
    }
  } else {
    Serial.println("LittleFS挂载成功!");
    
    // 检查文件系统信息
    FSInfo fs_info;
    LittleFS.info(fs_info);
    Serial.println("文件系统信息:");
    Serial.print("总空间: "); Serial.print(fs_info.totalBytes); Serial.println(" bytes");
    Serial.print("已用空间: "); Serial.print(fs_info.usedBytes); Serial.println(" bytes");
    Serial.print("块大小: "); Serial.print(fs_info.blockSize); Serial.println(" bytes");
    Serial.print("页大小: "); Serial.print(fs_info.pageSize); Serial.println(" bytes");
    
    // 列出所有文件
    Serial.println("\n当前文件列表:");
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
      Serial.print("文件: ");
      Serial.print(dir.fileName());
      Serial.print(" - 大小: ");
      Serial.println(dir.fileSize());
    }
  }
}

// 添加NTP同步函数
bool syncNTP() {
  Serial.println("正在同步NTP时间...");
  
  // 尝试所有NTP服务器
  for(int i = 0; i < ntpServerCount; i++) {
    timeClient.setPoolServerName(ntpServers[i]);
    timeClient.begin();
    timeClient.setTimeOffset(28800); // UTC+8
    
    // 多次尝试当前服务器
    for(int retry = 0; retry < 3; retry++) {
      Serial.print("尝试NTP服务器: ");
      Serial.print(ntpServers[i]);
      Serial.print(" (尝试 ");
      Serial.print(retry + 1);
      Serial.println("/3)");
      
      if(timeClient.forceUpdate()) {
        Serial.println("NTP同步成功!");
        currentNtpServer = i;
        return true;
      }
      
      delay(1000);
    }
    
    Serial.println("切换到下一个NTP服务器...");
  }
  
  Serial.println("所有NTP服务器同步失败!");
  return false;
}

// 添加 WebSocket 事件处理函数
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] 断开连接!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] 连接来自 %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
            }
            break;
    }
}

void setup() {
  Serial.begin(9600);
  Serial.println("\n正在启动...");
  
  // 初始化显示屏
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306初始化失败"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.println(F("系统启动中..."));
  display.display();
  
  // 初始化文件系统
  initializeLittleFS();
  
  // WiFi连接
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("连接WiFi中...");
  display.display();
  
  WiFi.begin(ssid, password);
  
  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifiAttempts < 20) {
    delay(500);
    Serial.print(".");
    wifiAttempts++;
  }
  
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi连接成功");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
    
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("WiFi已连接");
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.display();
    
    // 设置系统时间
    configTime(28800, 0, "ntp.aliyun.com", "ntp1.aliyun.com");  // 28800 = UTC+8
    
    // 初始化NTP客户端
    timeClient.begin();
    timeClient.setTimeOffset(28800); // 设置UTC+8
    
    // 强制更新时间
    bool ntpSynced = false;
    for(int i = 0; i < 5; i++) {
        if(timeClient.forceUpdate()) {
            ntpSynced = true;
            Serial.println("NTP同步成功!");
            break;
        }
        delay(1000);
    }
    
    if(!ntpSynced) {
        Serial.println("NTP同步失败，将继续尝试...");
    }
    
    // 在 WiFi 连接成功后，初始化 WebSocket
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    
  } else {
    Serial.println("\nWiFi连接失败!");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("WiFi连接失败!");
    display.display();
  }
  
  delay(2000); // 显示状态信息
  
  // 初始化Web服务器
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/interval", handleInterval);
  server.on("/data", handleData);
  server.on("/clear", handleClear);
  server.on("/time", handleTime);
  server.on("/uvdata", handleUVData);
  server.begin();
  
  // 初始化传感器
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.println(F("UV Sensor Test"));
  display.println(F("Calibrating..."));
  display.display();
  
  // 校准暗值
  sum = 0;
  for(int i = 0; i < 64; i++) {
    sum += analogRead(A0);
    delay(2);
  }
  darkValue = sum >> 6;
  Serial.print("Dark value: ");
  Serial.println(darkValue);
}

void handleRoot() {
  String html = "<html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>UV环境监测</title>";
  
  // 添加Chart.js
  html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
  
  // 添加样式
  html += "<style>";
  html += "* { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; }";
  html += "body { background: #f5f7fa; color: #333; padding: 20px; }";
  html += ".container { max-width: 1200px; margin: 0 auto; }";
  
  // 卡片样式
  html += ".card { background: white; border-radius: 15px; padding: 20px; margin-bottom: 20px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
  html += ".card-header { font-size: 18px; color: #666; margin-bottom: 15px; }";
  
  // 网格布局
  html += ".grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin-bottom: 20px; }";
  
  // 数据显示样式
  html += ".data-box { text-align: center; padding: 20px; }";
  html += ".data-value { font-size: 36px; font-weight: bold; margin: 10px 0; }";
  html += ".data-label { color: #666; font-size: 14px; }";
  
  // 图表容器
  html += ".chart-container { height: 300px; margin-top: 20px; }";
  
  // 开关样式
  html += ".switch { position: relative; display: inline-block; width: 60px; height: 34px; }";
  html += ".switch input { opacity: 0; width: 0; height: 0; }";
  html += ".slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 34px; }";
  html += ".slider:before { position: absolute; content: ''; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }";
  html += "input:checked + .slider { background-color: #2196F3; }";
  html += "input:checked + .slider:before { transform: translateX(26px); }";
  
  // 按钮样式
  html += ".btn { background: #2196F3; color: white; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer; }";
  html += ".btn:hover { background: #1976D2; }";
  html += ".btn-danger { background: #f44336; }";
  html += ".btn-danger:hover { background: #d32f2f; }";
  
  html += "</style>";
  
  // JavaScript
  html += "<script>";
  
  // 图表初始化函数
  html += "function initCharts() {";
  html += "  const ctx = document.getElementById('uvChart').getContext('2d');";
  html += "  new Chart(ctx, {";
  html += "    type: 'line',";
  html += "    data: {";
  html += "      labels: [],"; // 将从后端获取数据
  html += "      datasets: [{";
  html += "        label: 'UV指数',";
  html += "        data: [],";
  html += "        borderColor: '#2196F3',";
  html += "        tension: 0.4";
  html += "      }]";
  html += "    },";
  html += "    options: {";
  html += "      responsive: true,";
  html += "      maintainAspectRatio: false,";
  html += "      plugins: { legend: { display: false } }";
  html += "    }";
  html += "  });";
  html += "}";
  
  // 页面加载完成后初始化
  html += "document.addEventListener('DOMContentLoaded', function() {";
  html += "  initCharts();";
  html += "  const today = new Date();";
  html += "  document.getElementById('date').value = today.toISOString().split('T')[0];";
  html += "  loadData();";
  html += "});";
  
  // 添加前端JavaScript函数
  
  // 加载数据的函数
  html += "function loadData() {";
  html += "  const date = document.getElementById('date').value;";
  html += "  fetch('/data?date=' + date)";
  html += "    .then(response => response.json())";
  html += "    .then(data => {";
  html += "      updateChart(data);";
  html += "      updateTable(data.table);";
  html += "    });";
  html += "}";
  
  // 更新图表的函数
  html += "function updateChart(data) {";
  html += "  const chart = Chart.getChart('uvChart');";
  html += "  if (chart) {";
  html += "    chart.data.labels = data.labels;";
  html += "    chart.data.datasets[0].data = data.data;";
  html += "    chart.update();";
  html += "  }";
  html += "}";
  
  // 更新表格的函数
  html += "function updateTable(data) {";
  html += "  let html = '<table style=\"width:100%; border-collapse: collapse;\">';";
  html += "  html += '<tr><th>时间</th><th>UV指数</th></tr>';";
  html += "  data.forEach(row => {";
  html += "    html += `<tr><td>${row.time}</td><td>${row.uv}</td></tr>`;";
  html += "  });";
  html += "  html += '</table>';";
  html += "  document.getElementById('historicalData').innerHTML = html;";
  html += "}";
  
  // 清除数据的函数
  html += "function clearData() {";
  html += "  if(confirm('确定要清除所有历史数据吗？')) {";
  html += "    fetch('/clear').then(r=>r.json()).then(data => {";
  html += "      alert(data.message);";
  html += "      loadData();";
  html += "    });";
  html += "  }";
  html += "}";
  
  // 切换传感器状态的函数
  html += "function toggleSensor(element) {";
  html += "  fetch('/toggle?state=' + element.checked)";
  html += "    .then(r=>r.json())";
  html += "    .then(data => {";
  html += "      if(!data.success) element.checked = !element.checked;";
  html += "    });";
  html += "}";
  
  // 设置读取间隔的函数
  html += "function setInterval() {";
  html += "  const value = document.getElementById('interval').value;";
  html += "  fetch('/interval?value=' + value)";
  html += "    .then(r=>r.json())";
  html += "    .then(data => {";
  html += "      if(data.success) alert('间隔设置成功');";
  html += "    });";
  html += "}";
  
  // 添加 WebSocket 连接代码
  html += "let ws = new WebSocket('ws://' + window.location.hostname + ':81/');";
  html += "ws.onmessage = function(event) {";
  html += "    const data = JSON.parse(event.data);";// 获取所有卡片中的数据框";
  html += "    const dataBoxes = document.querySelectorAll('.grid .card .data-box');";// 更新 UV 指数卡片 (第一个数据框)";
  html += "    const uvBox = dataBoxes[0];";
  html += "    uvBox.querySelector('.data-value').textContent = data.uv;";
  html += "    uvBox.querySelector('.data-value').style.color = data.color;";
  html += "    uvBox.querySelector('.data-label').textContent = data.level;";// 更新传感器电压卡片 (第二个数据框)";
  html += "    dataBoxes[1].querySelector('.data-value').textContent = data.voltage;";// 更新图表";
  html += "    loadData();";
  html += "};";
  html += "ws.onclose = function() {";
  html += "    setTimeout(function() {";
  html += "        ws = new WebSocket('ws://' + window.location.hostname + ':81/');";
  html += "    }, 1000);";
  html += "};";
  
  html += "</script>";
  html += "</head><body>";
  
  // HTML结构
  html += "<div class='container'>";
  
  // 顶部状态卡片
  html += "<div class='grid'>";
  
  // UV指数卡片
  html += "<div class='card'>";
  html += "<div class='card-header'>UV指数</div>";
  html += "<div class='data-box'>";
  html += "<div class='data-value' style='color: " + getUVLevelColor(uv) + "'>" + String(uv) + "</div>";
  html += "<div class='data-label'>" + getUVLevelText(uv) + "</div>";
  html += "</div>";
  html += "</div>";
  
  // 电压值卡片
  html += "<div class='card'>";
  html += "<div class='card-header'>传感器电压</div>";
  html += "<div class='data-box'>";
  html += "<div class='data-value'>" + String(vout) + "</div>";
  html += "<div class='data-label'>mV</div>";
  html += "</div>";
  html += "</div>";
  
  // 时间卡片
  html += "<div class='card'>";
  html += "<div class='card-header'>当前时间</div>";
  html += "<div class='data-box'>";
  html += "<div class='data-value' style='font-size: 24px'>" + getFormattedDateTime() + "</div>";
  html += "<div class='data-label'>北京时间</div>";
  html += "</div>";
  html += "</div>";
  
  html += "</div>";
  
  // 图表卡片
  html += "<div class='card'>";
  html += "<div class='card-header'>UV指数趋势</div>";
  html += "<div class='chart-container'>";
  html += "<canvas id='uvChart'></canvas>";
  html += "</div>";
  html += "</div>";
  
  // 控制面板卡片
  html += "<div class='card'>";
  html += "<div class='card-header'>控制面板</div>";
  html += "<div style='display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; padding: 20px;'>";
  
  // 传感器开关
  html += "<div>";
  html += "<label class='switch'>";
  html += "<input type='checkbox' " + String(sensorEnabled ? "checked" : "") + " onchange='toggleSensor(this)'>";
  html += "<span class='slider'></span>";
  html += "</label>";
  html += "<span style='margin-left: 10px'>传感器状态</span>";
  html += "</div>";
  
  // 读取间隔设置
  html += "<div>";
  html += "<input type='number' id='interval' value='" + String(readInterval) + "' min='1' style='width: 80px; padding: 5px;'>";
  html += "<button class='btn' onclick='setInterval()' style='margin-left: 10px'>设置间隔(秒)</button>";
  html += "</div>";
  
  // 日期选择
  html += "<div>";
  html += "<input type='date' id='date' style='padding: 5px;'>";
  html += "<button class='btn' onclick='loadData()' style='margin-left: 10px'>查看历史</button>";
  html += "</div>";
  
  // 清除数据按钮
  html += "<div>";
  html += "<button class='btn btn-danger' onclick='clearData()'>清除历史数据</button>";
  html += "</div>";
  
  html += "</div>";
  html += "</div>";
  
  // 历史数据表格卡片
  html += "<div class='card'>";
  html += "<div class='card-header'>历史记录</div>";
  html += "<div id='historicalData'></div>";
  html += "</div>";
  
  html += "</div>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleToggle() {
  if(server.hasArg("state")) {
    sensorEnabled = (server.arg("state") == "true");
    String response = "{\"success\":true,\"enabled\":" + String(sensorEnabled ? "true" : "false") + "}";
    server.send(200, "application/json", response);
  } else {
    server.send(400, "text/plain", "缺少状态参数");
  }
}

void handleInterval() {
  if(server.hasArg("value")) {
    int newInterval = server.arg("value").toInt();
    if(newInterval >= 1) {
      readInterval = newInterval;
      String response = "{\"success\":true,\"interval\":" + String(readInterval) + "}";
      server.send(200, "application/json", response);
    } else {
      server.send(400, "text/plain", "间隔必须大于等于1秒");
    }
  } else {
    server.send(400, "text/plain", "缺少间隔参数");
  }
}

void saveUVData() {
    if(!timeClient.isTimeSet()) {
        Serial.println("时间未同步，跳过保存");
        return;
    }
    
    time_t localTime = getLocalTime();
    struct tm * timeinfo;
    timeinfo = localtime(&localTime);
    
    char filename[32];
    sprintf(filename, "/%04d%02d%02d.txt",
            timeinfo->tm_year + 1900,
            timeinfo->tm_mon + 1,
            timeinfo->tm_mday);
            
    File file = LittleFS.open(filename, "a");
    if(!file) {
        Serial.println("打开文件失败!");
        return;
    }
    
    // 保存本地时间戳和UV值
    String dataLine = String(localTime) + "," + String(uv);
    if(file.println(dataLine)) {
        Serial.println("数据已保存: " + dataLine);
    } else {
        Serial.println("写入数据失败!");
    }
    file.close();
}

void handleData() {
  if(!server.hasArg("date")) {
    server.send(400, "text/plain", "需要日期参数");
    return;
  }

  String requestDate = server.arg("date");
  requestDate.replace("-", "");
  String filename = "/" + requestDate + ".txt";
  
  if(!LittleFS.exists(filename)) {
    // 返回空数据的JSON格式
    server.send(200, "application/json", "{\"labels\":[],\"data\":[]}");
    return;
  }

  File file = LittleFS.open(filename, "r");
  if(!file) {
    server.send(500, "text/plain", "无法读取文件");
    return;
  }

  // 准备JSON数据
  String jsonResponse = "{\"labels\":[],\"data\":[],\"table\":[]}";
  String labels = "";
  String data = "";
  String tableData = "";
  
  while(file.available()) {
    String line = file.readStringUntil('\n');
    if(line.length() > 0) {
      int commaIndex = line.indexOf(',');
      if(commaIndex > 0) {
        String timestamp = line.substring(0, commaIndex);
        String uvValue = line.substring(commaIndex + 1);
        
        // 转换时间戳为可读格式
        time_t ts = timestamp.toInt();
        struct tm * timeinfo = localtime(&ts);
        char timeStr[20];
        sprintf(timeStr, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        
        // 添加到数组
        if(labels.length() > 0) {
          labels += ",";
          data += ",";
          tableData += ",";
        }
        labels += "\"" + String(timeStr) + "\"";
        data += uvValue;
        
        // 为表格准备完整时间格式
        char fullTimeStr[30];
        sprintf(fullTimeStr, "%d年%d月%d日 %02d:%02d:%02d",
                timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        tableData += "{\"time\":\"" + String(fullTimeStr) + "\",\"uv\":" + uvValue + "}";
      }
    }
  }
  file.close();

  // 构建完整的JSON响应
  jsonResponse = "{\"labels\":[" + labels + "],\"data\":[" + data + "],\"table\":[" + tableData + "]}";
  server.send(200, "application/json", jsonResponse);
}

int getFilteredValue() {
  int values[10];
  int sum = 0;
  int max_value = 0;
  int min_value = 1023;
  
  for(int i = 0; i < 10; i++) {
    values[i] = analogRead(A0);
    sum += values[i];
    if(values[i] > max_value) max_value = values[i];
    if(values[i] < min_value) min_value = values[i];
    delay(1);
  }
  
  return (sum - max_value - min_value) / 8;
}

void handleClear() {
  Dir dir = LittleFS.openDir("/");
  int filesDeleted = 0;
  
  while(dir.next()) {
    if(dir.fileName().endsWith(".txt")) {
      LittleFS.remove("/" + dir.fileName());
      filesDeleted++;
    }
  }
  
  String response = "{\"success\":true,\"message\":\"已删除 " + String(filesDeleted) + " 个文件\"}";
  server.send(200, "application/json", response);
}

// 添加新的处理函数用于获取当前时间
void handleTime() {
  server.send(200, "text/plain", getFormattedDateTime());
}

// 添加新的处理函数用于获取UV数据
void handleUVData() {
  String jsonResponse = "{";
  jsonResponse += "\"uv\":" + String(uv) + ",";
  jsonResponse += "\"voltage\":" + String(vout) + ",";
  jsonResponse += "\"level\":\"" + getUVLevelText(uv) + "\",";
  jsonResponse += "\"color\":\"" + getUVLevelColor(uv) + "\",";
  jsonResponse += "\"time\":\"" + getFormattedDateTime() + "\"";
  jsonResponse += "}";
  server.send(200, "application/json", jsonResponse);
}

// 添加自动清理旧文件的功能
void cleanOldFiles(int daysToKeep = 7) {
  Dir dir = LittleFS.openDir("/");
  time_t now = timeClient.getEpochTime();
  time_t cutoff = now - (daysToKeep * 86400);
  
  while (dir.next()) {
    String fileName = dir.fileName();
    if(fileName.endsWith(".txt")) {
      // 从文件名提取时间戳
      time_t fileDate = fileName.substring(1, fileName.length() - 4).toInt();
      if(fileDate < cutoff) {
        if(LittleFS.remove("/" + fileName)) {
          Serial.println("已删除旧文件: " + fileName);
        }
      }
    }
  }
}

// 修改格式化时间函数
String getFormattedDateTime() {
    if(!timeClient.isTimeSet()) {
        return "等待时间同步...";
    }
    
    time_t localTime = getLocalTime();
    struct tm * timeinfo;
    timeinfo = localtime(&localTime);
    
    char buffer[30];
    sprintf(buffer, "%d年%d月%d日 %02d:%02d:%02d",
            timeinfo->tm_year + 1900,
            timeinfo->tm_mon + 1,
            timeinfo->tm_mday,
            timeinfo->tm_hour,
            timeinfo->tm_min,
            timeinfo->tm_sec);
    
    return String(buffer);
}

void loop() {
    server.handleClient();
    webSocket.loop();  // 处理 WebSocket 事件
    
    // 定期更新时间
    static unsigned long lastNtpUpdate = 0;
    if (millis() - lastNtpUpdate >= 60000) { // 每分钟更新一次
        timeClient.update();
        lastNtpUpdate = millis();
    }
    
    // 如果WiFi断开，尝试重连
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi断开，尝试重连...");
        WiFi.reconnect();
        delay(5000);
        if(WiFi.status() == WL_CONNECTED) {
            Serial.println("WiFi重连成功");
            syncNTP(); // WiFi重连后重新同步时间
        }
    }
    
    // 确保时间同步
    if (millis() % 60000 == 0) { // 每分钟更新一次时间
        timeClient.forceUpdate();
    }
    
    // 更新当前日期
    String newDate = String(timeClient.getEpochTime() / 86400L * 86400L);
    if(newDate != currentDate) {
        currentDate = newDate;
    }
    
    if(sensorEnabled && (millis() - lastReadTime >= readInterval * 1000)) {
        sum = 0;
        for (int i = 0; i < 256; i++) {
            sensorValue = getFilteredValue();
            sum += sensorValue;
            delay(1);
        }
        vout = sum >> 8;
        vout = vout * 3300.0 / 1024;

        int darkVoltage = darkValue * 3300.0 / 1024;
        vout -= darkVoltage;

        if (vout < 0) vout = 0;

        // UV指数转换
        if (vout < 100) uv = 0;
        else if (vout < 227) uv = 1;
        else if (vout < 318) uv = 2;
        else if (vout < 408) uv = 3;
        else if (vout < 503) uv = 4;
        else if (vout < 606) uv = 5;
        else if (vout < 696) uv = 6;
        else if (vout < 795) uv = 7;
        else if (vout < 881) uv = 8;
        else if (vout < 976) uv = 9;
        else if (vout < 1079) uv = 10;
        else uv = 11;

        saveUVData();
        lastReadTime = millis();

        // OLED显示更新
        display.clearDisplay();
        
        display.setTextSize(1);
        display.setCursor(0,0);
        display.print("Voltage: ");
        display.print(vout);
        display.println("mV");
        
        display.setTextSize(2);
        display.setCursor(0,16);
        display.print("UV:");
        display.println(uv);
        
        display.drawRect(0, 45, display.width(), 10, SSD1306_WHITE);
        int barLength = map(uv, 0, 11, 0, display.width());
        display.fillRect(0, 45, barLength, 10, SSD1306_WHITE);
        
        display.setTextSize(1);
        display.setCursor(0, 56);
        if(uv <= 2) display.print("Low");
        else if(uv <= 5) display.print("Moderate");
        else if(uv <= 7) display.print("High");
        else if(uv <= 10) display.print("Very High");
        else display.print("Extreme");
        
        display.display();
        
        // 构建 JSON 数据
        String jsonData = "{";
        jsonData += "\"uv\":" + String(uv) + ",";
        jsonData += "\"voltage\":" + String(vout) + ",";
        jsonData += "\"level\":\"" + getUVLevelText(uv) + "\",";
        jsonData += "\"color\":\"" + getUVLevelColor(uv) + "\"";
        jsonData += "}";
        
        // 向所有连接的客户端广播数据
        webSocket.broadcastTXT(jsonData);
    }
    
    delay(100);
}
