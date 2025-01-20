#ifndef HTML_TEMPLATE_H
#define HTML_TEMPLATE_H

#include "html_styles.h"
#include "html_scripts.h"

// HTML基础结构
const char HTML_HEAD[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>UV环境监测</title>
    <script src='https://cdn.jsdelivr.net/npm/chart.js'></script>
)rawliteral";

const char HTML_BODY_START[] PROGMEM = R"rawliteral(
</head>
<body>
    <div class='container'>
)rawliteral";

const char HTML_BODY_END[] PROGMEM = R"rawliteral(
    </div>
</body>
</html>
)rawliteral";

const char HTML_HELP_BUTTON[] PROGMEM = R"rawliteral(
<style>
    .help-button {
        position: fixed;
        right: 20px;
        bottom: 20px;
        background-color: red;
        color: white;
        border: none;
        border-radius: 5px;
        padding: 10px;
        cursor: pointer;
        z-index: 1000;
    }
    .help-modal {
        display: none;
        position: fixed;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
        background-color: white;
        border: 1px solid #ccc;
        border-radius: 10px;
        padding: 20px;
        box-shadow: 0 4px 8px rgba(0,0,0,0.2);
        z-index: 1001;
        overflow: hidden; /* 防止内容溢出 */
    }
    .help-modal h2 {
        margin-top: 0;
    }
    .close-button {
        background-color: #f44336;
        color: white;
        border: none;
        border-radius: 5px;
        padding: 5px 10px;
        cursor: pointer;
    }
    .watermark {
        position: absolute;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        opacity: 0.1; /* 设置透明度 */
        z-index: -1; /* 使水印在背景 */
    }
</style>
<button class="help-button" onclick="toggleHelp()">帮助</button>
<div class="help-modal" id="helpModal">
    <img src="logo.png" class="watermark" alt="公司Logo">
    <h2>传感器说明</h2>
    <p>这是一个 UV 传感器，用于测量紫外线强度。它可以帮助用户了解紫外线的水平，从而采取适当的防护措施。</p>
    <h3>电压等级含义范围</h3>
    <ul>
        <li>0 - 100 mV: UV指数 0 (无紫外线)</li>
        <li>101 - 227 mV: UV指数 1 (低)</li>
        <li>228 - 318 mV: UV指数 2 (中等)</li>
        <li>319 - 408 mV: UV指数 3 (高)</li>
        <li>409 - 503 mV: UV指数 4 (很高)</li>
        <li>504 - 606 mV: UV指数 5 (极高)</li>
        <li>607 - 696 mV: UV指数 6 (极高)</li>
        <li>697 - 795 mV: UV指数 7 (极高)</li>
        <li>796 - 881 mV: UV指数 8 (极高)</li>
        <li>882 - 976 mV: UV指数 9 (极高)</li>
        <li>977 - 1079 mV: UV指数 10 (极高)</li>
        <li>1080 mV 及以上: UV指数 11 (极高)</li>
    </ul>
    <button class="close-button" onclick="toggleHelp()">关闭</button>
</div>
<script>
    function toggleHelp() {
        var modal = document.getElementById("helpModal");
        if (modal.style.display === "none" || modal.style.display === "") {
            modal.style.display = "block";
        } else {
            modal.style.display = "none";
        }
    }
</script>
)rawliteral";

#endif 