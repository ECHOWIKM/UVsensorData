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
        <div id='historicalData' style='max-height: 400px; overflow-y: auto;'></div>
)rawliteral";

const char HTML_BODY_END[] PROGMEM = R"rawliteral(
    </div>
</body>
</html>
)rawliteral";

#endif 