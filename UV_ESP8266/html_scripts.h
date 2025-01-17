#ifndef HTML_SCRIPTS_H
#define HTML_SCRIPTS_H

const char HTML_SCRIPTS[] PROGMEM = R"rawliteral(
<script>
    let chart;
    
    function initCharts() {
        const ctx = document.getElementById('uvChart').getContext('2d');
        chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: '传感器电压 (mV)',
                    data: [],
                    borderColor: '#2196F3',
                    tension: 0.1,
                    fill: false
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: '电压 (mV)'
                        }
                    },
                    x: {
                        title: {
                            display: true,
                            text: '时间'
                        }
                    }
                }
            }
        });
    }

    function loadData() {
        const date = document.getElementById('date').value;
        console.log('请求日期:', date);
        
        fetch('/data?date=' + date)
            .then(response => {
                console.log('响应状态:', response.status);
                return response.json();
            })
            .then(data => {
                console.log('收到的数据:', data);
                if(data.labels && data.data && data.table) {
                    console.log('数据格式正确');
                    console.log('标签数量:', data.labels.length);
                    console.log('数据点数量:', data.data.length);
                    console.log('表格行数:', data.table.length);
                    updateChart(data);
                    updateTable(data.table);
                } else {
                    console.error('数据格式不完整:', data);
                }
            })
            .catch(error => {
                console.error('加载数据错误:', error);
            });
    }

    function updateChart(data) {
        if (!data || !data.labels || !data.data) {
            console.error('数据格式错误:', data);
            return;
        }
        
        chart.data.labels = data.labels;
        chart.data.datasets[0].data = data.data;
        chart.update();
    }

    function updateTable(data) {
        if (!data || data.length === 0) {
            document.getElementById('historicalData').innerHTML = '<p style="text-align: center; padding: 20px;">没有历史数据</p>';
            return;
        }
        
        let html = '<table style="width:100%; border-collapse: collapse;">';
        html += `
            <thead>
                <tr style="background: #f8f9fa;">
                    <th style="padding: 12px; text-align: center; border-bottom: 2px solid #dee2e6;">时间</th>
                    <th style="padding: 12px; text-align: center; border-bottom: 2px solid #dee2e6;">UV指数</th>
                    <th style="padding: 12px; text-align: center; border-bottom: 2px solid #dee2e6;">电压(mV)</th>
                </tr>
            </thead>
            <tbody>
        `;
        
        data.forEach((row, index) => {
            const bgColor = index % 2 === 0 ? '#ffffff' : '#f8f9fa';
            let uvColor;
            let uvText;
            let uvIcon = '☀️';
            
            // UV指数颜色和文本
            if (row.uv <= 2) {
                uvColor = '#4CAF50';
                uvText = '低';
                uvIcon = '🌤️';
            } else if (row.uv <= 5) {
                uvColor = '#FFC107';
                uvText = '中等';
                uvIcon = '⛅';
            } else if (row.uv <= 7) {
                uvColor = '#FF9800';
                uvText = '高';
                uvIcon = '🌤️';
            } else if (row.uv <= 10) {
                uvColor = '#F44336';
                uvText = '很高';
                uvIcon = '☀️';
            } else {
                uvColor = '#9C27B0';
                uvText = '极高';
                uvIcon = '🌞';
            }
            
            html += `
                <tr style="background: ${bgColor};">
                    <td style="padding: 12px; text-align: center; border-bottom: 1px solid #dee2e6;">${row.time}</td>
                    <td style="padding: 12px; text-align: center; border-bottom: 1px solid #dee2e6;">
                        <span style="font-size: 20px;">${uvIcon}</span>
                        <span style="color: ${uvColor}; font-weight: bold; margin-left: 5px;">${row.uv}</span>
                        <small style="color: #666; margin-left: 5px;">(${uvText})</small>
                    </td>
                    <td style="padding: 12px; text-align: center; border-bottom: 1px solid #dee2e6;">${row.voltage}</td>
                </tr>
            `;
        });
        
        html += '</tbody></table>';
        document.getElementById('historicalData').innerHTML = html;
    }

    function toggleSensor(element) {
        fetch('/toggle?state=' + element.checked)
            .then(response => response.json())
            .then(data => {
                if(!data.success) element.checked = !element.checked;
            });
    }

    function setInterval() {
        const value = document.getElementById('interval').value;
        fetch('/interval?value=' + value)
            .then(response => response.json())
            .then(data => {
                if(data.success) alert('间隔已设置为 ' + value + ' 秒');
            });
    }

    function clearData() {
        if(confirm('确定要清除所有历史数据吗？')) {
            fetch('/clear')
                .then(response => response.json())
                .then(data => {
                    if(data.success) {
                        alert(data.message);
                        loadData();
                    }
                });
        }
    }

    function setAlertThreshold() {
        const value = document.getElementById('alertThreshold').value;
        fetch('/alert/settings?threshold=' + value)
            .then(response => response.json())
            .then(data => {
                if(data.success) alert('警报阈值设置成功');
            });
    }

    function toggleAlert() {
        const enabled = document.getElementById('alertEnabled').checked;
        fetch('/alert/settings?enabled=' + enabled)
            .then(response => response.json())
            .then(data => {
                if(!data.success) element.checked = !element.checked;
            });
    }

    function showMessage(message) {
        // 移除现有的警报消息
        const existingMsg = document.querySelector('.alert-message');
        if(existingMsg) existingMsg.remove();
        
        const msgDiv = document.createElement('div');
        msgDiv.className = 'alert-message';
        msgDiv.textContent = message;
        
        const closeBtn = document.createElement('button');
        closeBtn.innerHTML = '×';
        closeBtn.style.cssText = 'position:absolute; right:10px; top:50%; transform:translateY(-50%); background:none; border:none; color:white; font-size:24px; cursor:pointer;';
        closeBtn.onclick = function() { msgDiv.remove(); };
        msgDiv.appendChild(closeBtn);
        
        document.body.appendChild(msgDiv);
    }

    // WebSocket连接
    let ws = new WebSocket('ws://' + window.location.hostname + ':81/');
    
    // WebSocket消息处理
    ws.onmessage = function(event) {
        const data = JSON.parse(event.data);
        if(data.type === 'alert') {
            showMessage(data.message);
        } else {
            // 更新UV指数和电压值显示
            updateRealTimeData(data);
            // 如果当前日期是今天，也更新图表和表格
            const today = new Date().toISOString().split('T')[0];
            if(document.getElementById('date').value === today) {
                loadData();
            }
        }
    };

    // 添加实时数据更新函数
    function updateRealTimeData(data) {
        // 更新UV指数
        const uvBox = document.querySelector('.grid .card:nth-child(1)');
        if(uvBox) {
            const valueDiv = uvBox.querySelector('.data-value');
            const labelDiv = uvBox.querySelector('.data-label');
            if(valueDiv && labelDiv) {
                valueDiv.style.color = data.color;
                valueDiv.textContent = data.uv;
                labelDiv.textContent = data.level;
            }
        }

        // 更新电压值
        const voltageBox = document.querySelector('.grid .card:nth-child(2)');
        if(voltageBox) {
            const valueDiv = voltageBox.querySelector('.data-value');
            if(valueDiv) {
                valueDiv.textContent = data.voltage;
            }
        }
    }

    // 修改控制面板切换函数
    function toggleControlPanel(header) {
        header.classList.toggle('active');
        const content = header.nextElementSibling;
        content.classList.toggle('show');
    }

    // 初始化
    window.onload = function() {
        initCharts();
        document.getElementById('date').valueAsDate = new Date();
        loadData();
    };
</script>
)rawliteral";

#endif 