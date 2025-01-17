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

#endif 