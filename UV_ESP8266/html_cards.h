#ifndef HTML_CARDS_H
#define HTML_CARDS_H

#include <Arduino.h>
#include "global_vars.h"

// 函数声明
String createStatusCard(const String& title, const String& value, const String& label, const String& color);
String createSwitch(const String& id, bool checked, const String& onchange);

// 创建开关
String createSwitch(const String& id, bool checked, const String& onchange) {
    String html = "<label class='switch-container'>";
    html += "<input type='checkbox' id='" + id + "' " + 
            (checked ? "checked" : "") + 
            " onchange='" + onchange + "'>";
    html += "<span class='switch-track'>";
    html += "<span class='switch-thumb'></span>";
    html += "</span>";
    html += "</label>";
    return html;
}

// 创建状态卡片
String createStatusCards() {
    String html = "<div class='grid'>";
    html += createStatusCard("UV指数", String(uv), getUVLevelText(uv), getUVLevelColor(uv));
    html += createStatusCard("传感器电压", String(vout), "mV", "");
    html += createStatusCard("当前时间", getFormattedDateTime(), "北京时间", "");
    html += "</div>";
    return html;
}

// 创建单个状态卡片
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
    String panel = "<div class='control-panel'>";
    
    // 标题栏
    panel += "<div class='panel-header' onclick='toggleControlPanel(this)'>";
    panel += "<div class='panel-title'>控制面板</div>";
    panel += "<div class='arrow'>▼</div>";
    panel += "</div>";

    // 内容区
    panel += "<div class='panel-content'>";
    panel += "<div class='control-cards'>";

    // 传感器控制卡片
    panel += "<div class='control-card'>";
    panel += "<div class='card-title'>传感器控制</div>";
    panel += "<div class='control-item'>";
    panel += "<span>传感器状态</span>";
    panel += createSwitch("sensorSwitch", sensorEnabled, "toggleSensor(this)");
    panel += "</div>";
    panel += "<div class='control-item'>";
    panel += "<span>读取间隔</span>";
    panel += "<div class='input-container'>";
    panel += "<input type='number' id='interval' value='" + String(readInterval) + "' min='1'>";
    panel += "<span class='unit'>秒</span>";
    panel += "<button onclick='setInterval()' class='action-btn'>设置</button>";
    panel += "</div>";
    panel += "</div>";
    panel += "</div>";

    // UV警报设置卡片
    panel += "<div class='control-card'>";
    panel += "<div class='card-title'>UV警报设置</div>";
    panel += "<div class='control-item'>";
    panel += "<span>警报阈值</span>";
    panel += "<div class='input-container'>";
    panel += "<input type='number' id='alertThreshold' value='" + String(uvAlertThreshold) + "' min='0' max='11'>";
    panel += "<span class='unit'>级</span>";
    panel += "<button onclick='setAlertThreshold()' class='action-btn'>设置</button>";
    panel += "</div>";
    panel += "</div>";
    panel += "<div class='control-item'>";
    panel += "<span>警报状态</span>";
    panel += createSwitch("alertSwitch", alertEnabled, "toggleAlert()");
    panel += "</div>";
    panel += "</div>";

    // 数据管理卡片
    panel += "<div class='control-card'>";
    panel += "<div class='card-title'>数据管理</div>";
    panel += "<div class='control-item'>";
    panel += "<div class='date-container'>";
    panel += "<input type='date' id='date' class='date-input'>";
    panel += "<button onclick='loadData()' class='action-btn'>查看</button>";
    panel += "</div>";
    panel += "</div>";
    panel += "<div class='control-item'>";
    panel += "<button onclick='clearData()' class='danger-btn'>清除所有历史数据</button>";
    panel += "</div>";
    panel += "</div>";

    panel += "</div>";
    panel += "</div>";
    panel += "</div>";
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