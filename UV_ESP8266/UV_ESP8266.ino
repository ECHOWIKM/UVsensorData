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

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "sakaiwei";  // 替换为您的WiFi名称
const char* password = "12345678";  // 替换为您的WiFi密码

ESP8266WebServer server(80);  // 创建Web服务器对象

int sensorValue;
long sum = 0;
int vout = 0;
int uv = 0;
int darkValue = 0;  // 存储暗值

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // 打印IP地址
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.println(F("UV Sensor Test"));
  display.println(F("Calibrating..."));
  display.display();
  
  // 开机校准暗值
  sum = 0;
  for(int i = 0; i < 64; i++) {
    sum += analogRead(A0);
    delay(2);
  }
  darkValue = sum >> 6;  // 取平均值
  
  Serial.print("Dark value: ");
  Serial.println(darkValue);
  
  // 设置Web服务器路由
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  EEPROM.begin(512);  // 初始化EEPROM
}

// 处理根目录请求
void handleRoot() {
  String html = "<html><head>";
  // 添加自动刷新和样式
  html += "<meta http-equiv='refresh' content='1'>";  // 每秒刷新一次
  html += "<style>";
  html += "body { font-family: Arial; margin: 20px; }";
  html += "table { border-collapse: collapse; width: 100%; }";
  html += "th, td { padding: 8px; text-align: left; border: 1px solid #ddd; }";
  html += "th { background-color: #4CAF50; color: white; }";
  html += ".current { background-color: #f2f2f2; }";
  html += "</style>";
  html += "</head><body>";
  
  // 添加标题和当前值
  html += "<h1>UV Sensor Data</h1>";
  html += "<h2>Current UV Index: " + String(uv) + "</h2>";
  html += "<h3>Current Voltage: " + String(vout) + "mV</h3>";
  
  // 显示UV等级
  html += "<p>UV Level: ";
  if(uv <= 2) html += "<span style='color: green;'>Low</span>";
  else if(uv <= 5) html += "<span style='color: yellow;'>Moderate</span>";
  else if(uv <= 7) html += "<span style='color: orange;'>High</span>";
  else if(uv <= 10) html += "<span style='color: red;'>Very High</span>";
  else html += "<span style='color: purple;'>Extreme</span>";
  html += "</p>";
  
  // 历史数据表格
  html += "<h3>History Data</h3>";
  html += "<table><tr><th>Time</th><th>UV Index</th></tr>";
  
  // 显示最近10次的记录
  for (int i = 0; i < 10; i++) {
    html += "<tr>";
    html += "<td>" + String(millis()/1000 - i*10) + "s ago</td>";  // 显示时间
    html += "<td>" + String(EEPROM.read(i)) + "</td>";
    html += "</tr>";
  }
  
  html += "</table></body></html>";
  server.send(200, "text/html", html);
}

// 获取滤波后的值
int getFilteredValue() {
  int values[10];
  int sum = 0;
  int max_value = 0;
  int min_value = 1023;
  
  // 采集10个样本
  for(int i = 0; i < 10; i++) {
    values[i] = analogRead(A0);
    sum += values[i];
    if(values[i] > max_value) max_value = values[i];
    if(values[i] < min_value) min_value = values[i];
    delay(1);
  }
  
  // 去除最大最小值后的平均
  return (sum - max_value - min_value) / 8;
}

void loop() {
  server.handleClient();  // 处理客户端请求

  sum = 0;
  for (int i = 0; i < 256; i++) {  // 减少采样次数
    sensorValue = getFilteredValue();  // 使用滤波后的值
    sum += sensorValue;
    delay(1);  // 减少延迟
  }
  vout = sum >> 8;  // 256次采样
  vout = vout * 3300.0 / 1024;

  // 减去暗值对应的电压
  int darkVoltage = darkValue * 3300.0 / 1024;
  vout -= darkVoltage;

  // 确保不会出现负值
  if (vout < 0) vout = 0;

  // UV指数转换
  if (vout < 100) {
    uv = 0;
  }
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

  // 更新历史数据
  // 将旧数据往后移动
  for (int i = 9; i > 0; i--) {
    EEPROM.write(i, EEPROM.read(i-1));
  }
  // 存储最新的UV数据
  EEPROM.write(0, uv);
  EEPROM.commit();

  // 串口输出
  Serial.print("UV Voltage: ");
  Serial.print(vout);
  Serial.println("mV");
  Serial.print("UV Index = ");
  Serial.println(uv);

  // OLED显示
  display.clearDisplay();
  
  // 显示电压值
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Voltage: ");
  display.print(vout);
  display.println("mV");
  
  // 显示UV指数
  display.setTextSize(2);
  display.setCursor(0,16);
  display.print("UV:");
  display.println(uv);
  
  // UV强度条
  display.drawRect(0, 45, display.width(), 10, SSD1306_WHITE);
  int barLength = map(uv, 0, 11, 0, display.width());
  display.fillRect(0, 45, barLength, 10, SSD1306_WHITE);
  
  // UV危险程度
  display.setTextSize(1);
  display.setCursor(0, 56);
  if(uv <= 2) display.print("Low");
  else if(uv <= 5) display.print("Moderate");
  else if(uv <= 7) display.print("High");
  else if(uv <= 10) display.print("Very High");
  else display.print("Extreme");
  
  display.display();
  delay(100);
}
