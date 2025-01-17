#ifndef HTML_CARDS_H
#define HTML_CARDS_H

#include <Arduino.h>
#include "global_vars.h"

// 函数声明 - 移除默认参数
String createStatusCard(const String& title, const String& value, const String& label, const String& color);

// 创建状态卡片
String createStatusCards() {
    String html = "<div class='grid'>";
    html += createStatusCard("UV指数", String(uv), getUVLevelText(uv), getUVLevelColor(uv));
    html += createStatusCard("传感器电压", String(vout), "mV", "");
    html += createStatusCard("当前时间", getFormattedDateTime(), "北京时间", "");
    html += "</div>";
    return html;
}

// 创建单个状态卡片 - 保留默认参数
String createStatusCard(const String& title, const String& value, const String& label, const String& color = "") {
    String card = "<div class='card'>";
    card += "<div class='card-header'>" + title + "</div>";
    card += "<div class='data-box'>";
    if (color != "") {
        card += "<div class='data-value' style='color: " + color + "'>" + value + "</div>";
    } else {
        card += "<div class='data-value'>" + value + "</div>";
    }
    card += "<div class='data-label'>" + label + "</div>";
    card += "</div></div>";
    return card;
}

// 创建图表卡片
String createChartCard() {
    String card = "<div class='card'>";
    card += "<div class='card-header'>传感器电压趋势</div>";
    card += "<div class='chart-container'>";
    card += "<canvas id='uvChart'></canvas>";
    card += "</div></div>";
    return card;
}

// 创建控制面板
String createControlPanel() {
    String panel = "<div class='card'>";
    panel += "<div class='control-header' onclick='toggleControlPanel(this)'>";
    panel += "控制面板 <i>▼</i>";
    panel += "</div>";
    panel += "<div class='control-content'>";

    // 传感器控制区域
    panel += "<div class='control-section'>";
    panel += "<h3>传感器控制</h3>";
    panel += "<div class='section-content'>";
    // 传感器开关
    panel += "<div class='control-row'>";
    panel += "<label>传感器状态</label>";
    panel += "<label class='switch'>";
    panel += "<input type='checkbox' " + String(sensorEnabled ? "checked" : "") + " onchange='toggleSensor(this)'>";
    panel += "<span class='slider'></span>";
    panel += "</label>";
    panel += "</div>";
    // 读取间隔设置
    panel += "<div class='control-row'>";
    panel += "<label>读取间隔</label>";
    panel += "<div class='input-with-unit'>";
    panel += "<input type='number' id='interval' value='" + String(readInterval) + "' min='1' class='form-input'>";
    panel += "<span class='unit'>秒</span>";
    panel += "</div>";
    panel += "<button onclick='setInterval()' class='btn'>设置</button>";
    panel += "</div>";
    panel += "</div>";
    panel += "</div>";

    // UV警报设置区域
    panel += "<div class='control-section'>";
    panel += "<h3>UV警报设置</h3>";
    panel += "<div class='section-content'>";
    // 警报阈值设置
    panel += "<div class='control-row'>";
    panel += "<label>警报阈值</label>";
    panel += "<div class='input-with-unit'>";
    panel += "<input type='number' id='alertThreshold' value='" + String(uvAlertThreshold) + "' min='0' max='11' class='form-input'>";
    panel += "<span class='unit'>级</span>";
    panel += "</div>";
    panel += "<button onclick='setAlertThreshold()' class='btn'>设置</button>";
    panel += "</div>";
    // 警报开关
    panel += "<div class='control-row'>";
    panel += "<label>警报状态</label>";
    panel += "<label class='switch'>";
    panel += "<input type='checkbox' id='alertEnabled' " + String(alertEnabled ? "checked" : "") + " onchange='toggleAlert()'>";
    panel += "<span class='slider'></span>";
    panel += "</label>";
    panel += "</div>";
    panel += "</div>";
    panel += "</div>";

    // 数据管理区域
    panel += "<div class='control-section'>";
    panel += "<h3>数据管理</h3>";
    panel += "<div class='section-content'>";
    panel += "<div class='control-row'>";
    panel += "<label>选择日期</label>";
    panel += "<input type='date' id='date' class='form-input date-input'>";
    panel += "<button onclick='loadData()' class='btn'>查看</button>";
    panel += "</div>";
    panel += "<div class='control-row' style='justify-content: flex-end;'>";
    panel += "<button onclick='clearData()' class='btn btn-danger'>清除所有历史数据</button>";
    panel += "</div>";
    panel += "</div>";
    panel += "</div>";

    panel += "</div></div>";
    return panel;
}

// 创建历史记录卡片
String createHistoryCard() {
    String card = "<div class='card'>";
    card += "<div class='card-header'>历史记录</div>";
    card += "<div id='historicalData' style='max-height: 400px; overflow-y: auto;'></div>";
    card += "</div>";
    return card;
}

#endif 