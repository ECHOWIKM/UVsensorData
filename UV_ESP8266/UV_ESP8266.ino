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

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "sakaiwei";  // 替换为您的WiFi名称
const char* password = "12345678";  // 替换为您的WiFi密码

ESP8266WebServer server(80);  // 创建Web服务器对象

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
  html += "<style>";
  html += "body { font-family: Arial; margin: 20px; }";
  html += "table { border-collapse: collapse; width: 100%; }";
  html += "th, td { padding: 8px; text-align: left; border: 1px solid #ddd; }";
  html += "th { background-color: #4CAF50; color: white; }";
  html += ".current { background-color: #f2f2f2; }";
  html += ".switch { position: relative; display: inline-block; width: 60px; height: 34px; }";
  html += ".switch input { opacity: 0; width: 0; height: 0; }";
  html += ".slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 34px; }";
  html += ".slider:before { position: absolute; content: ''; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }";
  html += "input:checked + .slider { background-color: #2196F3; }";
  html += "input:checked + .slider:before { transform: translateX(26px); }";
  html += ".control-panel { margin: 20px 0; padding: 15px; background: #f8f8f8; border-radius: 5px; }";
  html += "</style>";
  
  html += "<script>";
  // 实时更新函数
  html += "function updateAll() {";
  // 更新时间
  html += "  fetch('/time').then(r=>r.text()).then(time => {";
  html += "    document.getElementById('currentTime').innerHTML = time;";
  html += "  });";
  // 更新UV数据
  html += "  fetch('/uvdata').then(r=>r.json()).then(data => {";
  html += "    document.getElementById('uvIndex').innerHTML = data.uv;";
  html += "    document.getElementById('voltage').innerHTML = data.voltage;";
  html += "    document.getElementById('uvLevel').innerHTML = data.level;";
  html += "    document.getElementById('uvLevel').style.color = data.color;";
  html += "  });";
  // 更新历史数据
  html += "  fetch('/data?date=' + document.getElementById('date').value).then(r=>r.text()).then(data => {";
  html += "    document.getElementById('historicalData').innerHTML = formatData(data);";
  html += "  });";
  html += "}";
  
  // 页面加载完成后的初始化
  html += "document.addEventListener('DOMContentLoaded', function() {";
  html += "  updateAll();"; // 立即更新一次
  html += "  setInterval(updateAll, 1000);"; // 每秒更新一次
  html += "});";
  
  // 格式化数据的函数
  html += "function formatData(data) {";
  html += "  if(!data) return '无数据';";
  html += "  const rows = data.split('\\n').filter(r => r.trim());";
  html += "  let html = '<table style=\"width:100%; border-collapse: collapse;\">';";
  html += "  html += '<tr style=\"background-color: #4CAF50; color: white;\"><th>时间</th><th>UV指数</th></tr>';";
  html += "  rows.forEach(row => {";
  html += "    const [timestamp, uv] = row.split(',');";
  html += "    const date = new Date(timestamp * 1000);"; // 时间戳已经是本地时间
  html += "    const formattedDate = date.getFullYear() + '年' + ";
  html += "      (date.getMonth() + 1) + '月' + ";
  html += "      date.getDate() + '日 ' + ";
  html += "      String(date.getHours()).padStart(2, '0') + ':' + ";
  html += "      String(date.getMinutes()).padStart(2, '0') + ':' + ";
  html += "      String(date.getSeconds()).padStart(2, '0');";
  html += "    html += `<tr><td>${formattedDate}</td><td>${uv}</td></tr>`;";
  html += "  });";
  html += "  return html + '</table>';";
  html += "}";
  
  html += "function toggleSensor(cb) { fetch('/toggle?state=' + (cb.checked ? '1' : '0')); }";
  html += "function setInterval() { fetch('/interval?minutes=' + document.getElementById('interval').value); }";
  html += "function loadData() {";
  html += "  const dateInput = document.getElementById('date');";
  html += "  if(!dateInput.value) {";
  html += "    const today = new Date();";
  html += "    const year = today.getFullYear();";
  html += "    const month = String(today.getMonth() + 1).padStart(2, '0');";
  html += "    const day = String(today.getDate()).padStart(2, '0');";
  html += "    dateInput.value = `${year}-${month}-${day}`;";
  html += "  }";
  html += "  fetch('/data?date=' + dateInput.value).then(r=>r.text()).then(data => {";
  html += "    document.getElementById('historicalData').innerHTML = formatData(data);";
  html += "  });";
  html += "}";
  html += "function clearData() { if(confirm('确定要删除所有历史数据吗？此操作不可恢复！')) { fetch('/clear').then(r=>r.text()).then(response => { alert(response); location.reload(); }); } }";
  
  html += "</script>";
  html += "</head><body>";
  
  // 修改时间显示
  html += "<h2>当前时间: <span id='currentTime'>" + getFormattedDateTime() + "</span></h2>";
  
  // 控制面板
  html += "<div class='control-panel'>";
  // 传感器开关
  html += "<div><label class='switch'><input type='checkbox' " + String(sensorEnabled ? "checked" : "") + " onchange='toggleSensor(this)'>";
  html += "<span class='slider'></span></label> 传感器状态</div><br>";
  
  // 间隔设置
  html += "<div>读取间隔(秒): <input type='number' id='interval' value='" + String(readInterval) + "' min='1'>";
  html += "<button onclick='setInterval()'>设置</button></div><br>";
  
  // 添加清除数据按钮
  html += "<div><button onclick='clearData()' style='background-color: #ff4444; color: white; padding: 10px; border: none; border-radius: 5px;'>清除所有历史数据</button></div><br>";
  
  // 日期选择
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  String todayDate = String(ptm->tm_year + 1900) + "-" +
                    (String(ptm->tm_mon + 1).length() < 2 ? "0" : "") + String(ptm->tm_mon + 1) + "-" +
                    (String(ptm->tm_mday).length() < 2 ? "0" : "") + String(ptm->tm_mday);
  
  html += "<div>选择日期: <input type='date' id='date' value='" + todayDate + "'>";
  html += "<button onclick='loadData()'>查看历史数据</button></div>";
  html += "</div>";
  
  // 修改数据显示部分，添加ID以便动态更新
  html += "<h1>UV Sensor Data</h1>";
  html += "<h2>Current UV Index: <span id='uvIndex'>" + String(uv) + "</span></h2>";
  html += "<h3>Current Voltage: <span id='voltage'>" + String(vout) + "</span>mV</h3>";
  
  // UV等级显示
  html += "<p>UV Level: <span id='uvLevel' style='color: ";
  String levelColor;
  if(uv <= 2) levelColor = "green";
  else if(uv <= 5) levelColor = "yellow";
  else if(uv <= 7) levelColor = "orange";
  else if(uv <= 10) levelColor = "red";
  else levelColor = "purple";
  html += levelColor + "'>";
  
  if(uv <= 2) html += "Low";
  else if(uv <= 5) html += "Moderate";
  else if(uv <= 7) html += "High";
  else if(uv <= 10) html += "Very High";
  else html += "Extreme";
  html += "</span></p>";
  
  // 历史数据显示区域
  html += "<div id='historicalData'></div>";
  
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleToggle() {
  if(server.hasArg("state")) {
    sensorEnabled = server.arg("state") == "1";
  }
  server.send(200, "text/plain", sensorEnabled ? "开启" : "关闭");
}

void handleInterval() {
  if(server.hasArg("minutes")) {
    readInterval = server.arg("minutes").toInt();
    if(readInterval < 1) readInterval = 1;
  }
  server.send(200, "text/plain", String(readInterval));
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
  if(server.hasArg("date")) {
    String requestDate = server.arg("date");
    // 将YYYY-MM-DD格式转换为文件名格式
    requestDate.replace("-", "");
    String filename = "/" + requestDate + ".txt";
    
    if(LittleFS.exists(filename)) {
      File file = LittleFS.open(filename, "r");
      String data = file.readString();
      file.close();
      
      // 修改JavaScript中的时间显示格式
      String html = "<script>";
      html += "function formatTimestamp(timestamp) {";
      html += "  const date = new Date(timestamp * 1000);";
      html += "  date.setTime(date.getTime());"; // 使用本地时间
      html += "  return `${date.getFullYear()}年${date.getMonth()+1}月${date.getDate()}日 ${String(date.getHours()).padStart(2,'0')}:${String(date.getMinutes()).padStart(2,'0')}:${String(date.getSeconds()).padStart(2,'0')}`;";
      html += "}";
      html += "</script>";
      
      server.send(200, "text/html", html + data);
    } else {
      server.send(404, "text/plain", "无数据");
    }
  } else {
    // 如果没有提供日期参数，默认使用今天的日期
    time_t epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime ((time_t *)&epochTime);
    String today = String(ptm->tm_year + 1900);
    today += (String(ptm->tm_mon + 1).length() < 2 ? "0" : "") + String(ptm->tm_mon + 1);
    today += (String(ptm->tm_mday).length() < 2 ? "0" : "") + String(ptm->tm_mday);
    
    String filename = "/" + today + ".txt";
    if(LittleFS.exists(filename)) {
      File file = LittleFS.open(filename, "r");
      String data = file.readString();
      file.close();
      server.send(200, "text/plain", data);
    } else {
      server.send(404, "text/plain", "今日暂无数据记录");
    }
  }
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
  int count = 0;
  while (dir.next()) {
    if (LittleFS.remove("/" + dir.fileName())) {
      count++;
    }
  }
  server.send(200, "text/plain", "已清除 " + String(count) + " 个数据文件");
}

// 添加新的处理函数用于获取当前时间
void handleTime() {
  server.send(200, "text/plain", getFormattedDateTime());
}

// 添加新的处理函数用于获取UV数据
void handleUVData() {
  String response = "{";
  response += "\"uv\":" + String(uv) + ",";
  response += "\"voltage\":" + String(vout) + ",";
  response += "\"level\":\"";
  if(uv <= 2) response += "Low";
  else if(uv <= 5) response += "Moderate";
  else if(uv <= 7) response += "High";
  else if(uv <= 10) response += "Very High";
  else response += "Extreme";
  response += "\",";
  response += "\"color\":\"";
  if(uv <= 2) response += "green";
  else if(uv <= 5) response += "yellow";
  else if(uv <= 7) response += "orange";
  else if(uv <= 10) response += "red";
  else response += "purple";
  response += "\"";
  response += "}";
  server.send(200, "application/json", response);
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
    }
    
    delay(100);
}
