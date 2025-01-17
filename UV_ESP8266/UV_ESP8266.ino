#define MAIN_PROGRAM
#include "global_vars.h"
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
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <LittleFS.h>
#include <WebSocketsServer.h>
#include "html_template.h"
#include "html_cards.h"

const char* ssid = "010";  // 替换为您的WiFi名称
const char* password = "66666666";  // 替换为您的WiFi密码

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
    return now; // 添加8小时偏移
}

void initializeLittleFS() {
  if(!LittleFS.begin()) {
    Serial.println("LittleFS挂载失败!");  
    Serial.println("正在尝试格式化文件系统...");
    
    if(LittleFS.format()) {
      Serial.println("文件系统格式化成功");
      if(LittleFS.begin()) {
        Serial.println("LittleFS挂载成功!");
      } else {
        Serial.println("LittleFS挂载仍然失败，请检查硬件!");
      }
    } else {
      Serial.println("文件系统格式化失败!");
    }
  } else {
    Serial.println("LittleFS挂载成功!");
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
  
  // 初始化文件系统
  initializeLittleFS();
  
  // WiFi连接
  Serial.println("连接WiFi中...");
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
    
    // 初始化 WebSocket
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    
  } else {
    Serial.println("\nWiFi连接失败!");
  }
  
  // 初始化Web服务器
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/interval", handleInterval);
  server.on("/data", handleData);
  server.on("/clear", handleClear);
  server.on("/time", handleTime);
  server.on("/uvdata", handleUVData);
  server.on("/alert/settings", handleAlertSettings);
  server.begin();
  
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
    String html = FPSTR(HTML_HEAD);
    html += FPSTR(HTML_STYLES);
    html += FPSTR(HTML_BODY_START);
    
    // 添加动态内容
    html += createStatusCards();
    html += createChartCard();
    html += createControlPanel();
    html += createHistoryCard();
    
    html += FPSTR(HTML_SCRIPTS);
    html += FPSTR(HTML_BODY_END);
    
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
            
    Serial.println("保存到文件: " + String(filename));
            
    File file = LittleFS.open(filename, "a");
    if(!file) {
        Serial.println("打开文件失败!");
        return;
    }
    
    // 保存时间戳、UV值和电压值
    String dataLine = String(localTime) + "," + String(uv) + "," + String(vout) + "\n";
    
    if(file.print(dataLine)) {
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
        server.send(200, "application/json", "{\"labels\":[],\"data\":[],\"table\":[]}");
        return;
    }

    File file = LittleFS.open(filename, "r");
    if(!file) {
        server.send(500, "text/plain", "无法读取文件");
        return;
    }

    // 准备JSON数据 - 只读取最后20条记录
    const int maxRecords = 20;
    String records[maxRecords];
    int recordCount = 0;
    
    // 先读取所有行
    while(file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if(line.length() > 0) {
            if(recordCount < maxRecords) {
                records[recordCount] = line;
                recordCount++;
            } else {
                // 移动数组，删除最旧的记录
                for(int i = 0; i < maxRecords - 1; i++) {
                    records[i] = records[i + 1];
                }
                records[maxRecords - 1] = line;
            }
        }
    }
    
    file.close();

    // 构建JSON响应
    String labels = "[";
    String data = "[";
    String tableData = "[";
    
    for(int i = 0; i < recordCount; i++) {
        String line = records[i];
        int firstComma = line.indexOf(',');
        int secondComma = line.indexOf(',', firstComma + 1);
        
        if(firstComma > 0 && secondComma > 0) {
            time_t timestamp = line.substring(0, firstComma).toInt();
            String uvValue = line.substring(firstComma + 1, secondComma);
            String voltageValue = line.substring(secondComma + 1);
            
            // 转换时间戳为可读格式
            struct tm * timeinfo = localtime(&timestamp);
            char timeStr[20];
            sprintf(timeStr, "%02d:%02d:%02d",
                    timeinfo->tm_hour,
                    timeinfo->tm_min,
                    timeinfo->tm_sec);
            
            if(i > 0) {
                labels += ",";
                data += ",";
                tableData += ",";
            }
            
            labels += "\"" + String(timeStr) + "\"";
            data += voltageValue;
            tableData += "{\"time\":\"" + String(timeStr) + "\",";
            tableData += "\"uv\":" + uvValue + ",";
            tableData += "\"voltage\":" + voltageValue + "}";
        }
    }
    
    labels += "]";
    data += "]";
    tableData += "]";
    
    String jsonResponse = "{\"labels\":" + labels + ",";
    jsonResponse += "\"data\":" + data + ",";
    jsonResponse += "\"table\":" + tableData + "}";
    
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

// 添加处理警报设置的函数
void handleAlertSettings() {
    if(server.hasArg("threshold")) {
        uvAlertThreshold = server.arg("threshold").toInt();
    }
    if(server.hasArg("enabled")) {
        alertEnabled = server.arg("enabled") == "true";
    }
    
    String response = "{\"success\":true,\"threshold\":" + String(uvAlertThreshold);
    response += ",\"enabled\":" + String(alertEnabled ? "true" : "false") + "}";
    server.send(200, "application/json", response);
}

void loop() {
    server.handleClient();
    webSocket.loop();
    
    // 定期更新时间
    static unsigned long lastNtpUpdate = 0;
    if (millis() - lastNtpUpdate >= 60000) {
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
            syncNTP();
        }
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

        // 发送实时数据更新
        String jsonData = "{";
        jsonData += "\"uv\":" + String(uv) + ",";
        jsonData += "\"voltage\":" + String(vout) + ",";
        jsonData += "\"level\":\"" + getUVLevelText(uv) + "\",";
        jsonData += "\"color\":\"" + getUVLevelColor(uv) + "\"";
        jsonData += "}";
        webSocket.broadcastTXT(jsonData);
        
        // 如果需要，发送警报消息
        if(alertEnabled && uv >= uvAlertThreshold) {
            delay(100);
            String alertMsg = "{\"type\":\"alert\",\"message\":\"警告：UV指数已达到 " + String(uv);
            alertMsg += "，超过警报阈值 " + String(uvAlertThreshold) + "！请注意防护。\"}";
            webSocket.broadcastTXT(alertMsg);
        }
        
        // 保存数据到文件
        saveUVData();
        lastReadTime = millis();
    }
    
    delay(100);
}
