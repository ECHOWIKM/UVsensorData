#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include <Arduino.h>

// 全局变量声明
extern int sensorValue;
extern long sum;
extern int vout;
extern int uv;
extern int darkValue;
extern bool sensorEnabled;
extern int readInterval;
extern unsigned long lastReadTime;
extern String currentDate;
extern int uvAlertThreshold;
extern bool alertEnabled;

// 辅助函数声明
String getUVLevelColor(int uvIndex);
String getUVLevelText(int uvIndex);
String getFormattedDateTime();

// 在 UV_ESP8266.ino 中定义这些变量
#ifdef MAIN_PROGRAM
int sensorValue = 0;
long sum = 0;
int vout = 0;
int uv = 0;
int darkValue = 0;
bool sensorEnabled = true;
int readInterval = 10;
unsigned long lastReadTime = 0;
String currentDate = "";
int uvAlertThreshold = 8;
bool alertEnabled = true;
#endif

#endif 