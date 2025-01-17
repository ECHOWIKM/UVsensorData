#ifndef HTML_STYLES_H
#define HTML_STYLES_H

const char HTML_STYLES[] PROGMEM = R"rawliteral(
<style>
    * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; }
    body { background: #f5f7fa; color: #333; padding: 20px; }
    .container { max-width: 1200px; margin: 0 auto; }
    .card { background: white; border-radius: 15px; padding: 20px; margin-bottom: 20px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
    .card-header { font-size: 18px; color: #666; margin-bottom: 15px; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin-bottom: 20px; }
    .data-box { text-align: center; padding: 20px; }
    .data-value { font-size: 36px; font-weight: bold; margin: 10px 0; }
    .data-label { color: #666; font-size: 14px; }
    .chart-container { height: 300px; margin-top: 20px; }
    .control-panel { padding: 20px; }
    .control-group { display: flex; flex-wrap: wrap; gap: 20px; }
    .control-item { flex: 1; min-width: 200px; margin-bottom: 15px; }
    .input-group { display: flex; gap: 10px; }
    .form-input { padding: 8px; border: 1px solid #ddd; border-radius: 4px; flex: 1; }
    .btn { padding: 8px 20px; border: none; border-radius: 8px; background: linear-gradient(135deg, #2196F3, #1976D2); color: white; cursor: pointer; transition: all 0.3s ease; font-weight: 500; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
    .btn:hover { background: linear-gradient(135deg, #1976D2, #1565C0); transform: translateY(-1px); box-shadow: 0 4px 8px rgba(0,0,0,0.15); }
    .btn-danger { background: linear-gradient(135deg, #f44336, #d32f2f); width: auto; padding: 10px 24px; }
    .btn-danger:hover { background: linear-gradient(135deg, #d32f2f, #c62828); }
    .switch {
        position: relative;
        display: inline-block;
        width: 60px;
        height: 30px;
    }
    .switch input {
        opacity: 0;
        width: 0;
        height: 0;
    }
    .slider {
        position: absolute;
        cursor: pointer;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #e0e0e0;
        transition: .4s;
        border-radius: 30px;
        box-shadow: inset 0 1px 3px rgba(0,0,0,0.2);
    }
    .slider:before {
        position: absolute;
        content: "";
        height: 22px;
        width: 22px;
        left: 4px;
        bottom: 4px;
        background-color: white;
        transition: .4s;
        border-radius: 50%;
        box-shadow: 0 2px 4px rgba(0,0,0,0.2);
    }
    input:checked + .slider {
        background: linear-gradient(135deg, #2196F3, #1976D2);
    }
    input:checked + .slider:before {
        transform: translateX(30px);
    }
    input:focus + .slider {
        box-shadow: inset 0 1px 3px rgba(0,0,0,0.2), 0 0 0 3px rgba(33, 150, 243, 0.1);
    }
    .control-section { 
        background: #f8f9fa; 
        border-radius: 8px; 
        padding: 20px; 
        margin-bottom: 20px; 
    }
    .control-section:last-child { margin-bottom: 0; }
    .control-section h3 { 
        color: #2196F3; 
        font-size: 16px; 
        margin: 0 0 15px 0; 
        padding-bottom: 10px;
        border-bottom: 1px solid #dee2e6;
    }
    .control-content { display: flex; flex-direction: column; gap: 15px; }
    .control-row { 
        display: flex; 
        align-items: center; 
        gap: 15px; 
    }
    .control-row label { 
        min-width: 100px;
        color: #666;
    }
    .input-with-unit {
        display: flex;
        align-items: center;
        background: white;
        border: 2px solid #e0e0e0;
        border-radius: 8px;
        overflow: hidden;
        transition: all 0.3s ease;
    }
    .input-with-unit:focus-within {
        border-color: #2196F3;
        box-shadow: 0 0 0 3px rgba(33, 150, 243, 0.1);
    }
    .input-with-unit input {
        border: none;
        padding: 10px;
        width: 100px;
        font-size: 14px;
    }
    .input-with-unit .unit {
        padding: 10px;
        background: #f5f5f5;
        color: #666;
        font-weight: 500;
    }
    .date-input {
        min-width: 150px;
    }
    /* 警报提示样式 */
    .alert-message {
        position: fixed;
        top: 20px;
        right: 20px;
        background: #f44336;
        color: white;
        padding: 15px 40px 15px 20px;
        border-radius: 4px;
        box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        z-index: 1000;
        animation: slideIn 0.3s ease;
    }
    
    @keyframes slideIn {
        from { transform: translateX(100%); }
        to { transform: translateX(0); }
    }
    
    /* 控制面板样式 */
    .control-header {
        width: 100%;
        padding: 15px 20px;
        background: linear-gradient(135deg, #2196F3, #1976D2);
        color: white;
        border: none;
        border-radius: 12px;
        font-size: 16px;
        cursor: pointer;
        display: flex;
        align-items: center;
        justify-content: space-between;
        margin-bottom: 15px;
        transition: all 0.3s ease;
        box-shadow: 0 2px 5px rgba(0,0,0,0.1);
    }
    
    .control-header:hover {
        background: linear-gradient(135deg, #1976D2, #1565C0);
        transform: translateY(-1px);
        box-shadow: 0 4px 8px rgba(0,0,0,0.15);
    }
    
    .control-header i {
        transition: transform 0.3s;
    }
    
    .control-header.active i {
        transform: rotate(180deg);
    }
    
    .control-content {
        display: none;
    }
    
    .control-content.show {
        display: block;
        animation: fadeIn 0.3s ease;
    }
    
    .section-content {
        background: white;
        padding: 20px;
        border-radius: 12px;
        margin-top: 10px;
        box-shadow: 0 2px 5px rgba(0,0,0,0.05);
    }
    
    .control-section {
        margin-bottom: 20px;
        background: #f8f9fa;
        border-radius: 8px;
        padding: 15px;
    }
    
    .control-section:last-child {
        margin-bottom: 0;
    }
    
    .control-section h3 {
        color: #2196F3;
        font-size: 16px;
        margin: 0;
        padding-bottom: 10px;
    }

    /* 添加新的网格布局样式 */
    .control-grid {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
        gap: 20px;
        padding: 20px;
    }

    /* 修改控制区域样式 */
    .control-section {
        background: white;
        border-radius: 12px;
        padding: 20px;
        box-shadow: 0 1px 3px rgba(0,0,0,0.1);
        transition: all 0.3s ease;
        height: 100%;
    }

    .control-section:hover {
        transform: translateY(-2px);
        box-shadow: 0 4px 12px rgba(0,0,0,0.12);
    }

    /* iOS风格开关样式 */
    .ios-switch {
        position: relative;
        display: inline-block;
        width: 44px;
        height: 26px;
    }

    .ios-switch input {
        opacity: 0;
        width: 0;
        height: 0;
        margin: 0;
    }

    .ios-switch .slider {
        position: absolute;
        cursor: pointer;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #e9e9ea;
        transition: .3s cubic-bezier(.4,0,.2,1);
        border-radius: 26px;
    }

    .ios-switch .slider:before {
        position: absolute;
        content: "";
        height: 22px;
        width: 22px;
        left: 2px;
        bottom: 2px;
        background-color: white;
        transition: .3s cubic-bezier(.4,0,.2,1);
        border-radius: 50%;
        box-shadow: 0 2px 2px rgba(0,0,0,0.1);
    }

    .ios-switch input:checked + .slider {
        background-color: #34C759;
    }

    .ios-switch input:checked + .slider:before {
        transform: translateX(18px);
    }

    .ios-switch input:focus + .slider {
        box-shadow: 0 0 1px #34C759;
    }

    /* 改进输入框组样式 */
    .input-group {
        display: flex;
        gap: 10px;
        align-items: center;
    }

    .input-with-unit {
        display: flex;
        align-items: center;
        background: #f5f5f7;
        border-radius: 8px;
        overflow: hidden;
        border: 1px solid #e0e0e0;
        transition: all 0.3s ease;
    }

    .input-with-unit:focus-within {
        border-color: #34C759;
        box-shadow: 0 0 0 3px rgba(52, 199, 89, 0.1);
    }

    .input-with-unit input {
        border: none;
        background: transparent;
        padding: 8px 12px;
        width: 80px;
        font-size: 14px;
    }

    .input-with-unit .unit {
        padding: 8px 12px;
        color: #666;
        background: #ebebeb;
        font-weight: 500;
    }

    /* 改进按钮样式 */
    .btn {
        padding: 8px 16px;
        border: none;
        border-radius: 8px;
        background: #007AFF;
        color: white;
        font-weight: 500;
        cursor: pointer;
        transition: all 0.3s ease;
    }

    .btn:hover {
        background: #0066d6;
        transform: translateY(-1px);
    }

    .btn-danger {
        background: #FF3B30;
        width: 100%;
    }

    .btn-danger:hover {
        background: #d63029;
    }

    /* 日期输入组样式 */
    .date-group {
        display: flex;
        gap: 10px;
        align-items: center;
    }

    .date-input {
        flex: 1;
        padding: 8px 12px;
        border: 1px solid #e0e0e0;
        border-radius: 8px;
        font-size: 14px;
        transition: all 0.3s ease;
    }

    .date-input:focus {
        border-color: #34C759;
        box-shadow: 0 0 0 3px rgba(52, 199, 89, 0.1);
    }

    /* 控制行样式优化 */
    .control-row {
        display: flex;
        align-items: center;
        justify-content: space-between;
        padding: 10px 0;
    }

    .control-row label {
        color: #333;
        font-size: 14px;
        font-weight: 500;
    }

    /* 移动端响应式布局 */
    @media screen and (max-width: 768px) {
        .control-grid {
            grid-template-columns: 1fr;
            padding: 10px;
            gap: 15px;
        }
        
        .control-section {
            padding: 15px;
        }
        
        .control-row {
            padding: 12px 0;
        }
    }

    /* 自定义新的开关样式 */
    .custom-switch {
        position: relative;
        display: inline-block;
        width: 52px;
        height: 32px;
        border-radius: 16px;
        background: #eee;
        padding: 2px;
        transition: background 0.3s;
        cursor: pointer;
        box-shadow: inset 0 1px 3px rgba(0,0,0,0.1);
    }

    .custom-switch input {
        display: none;
    }

    .custom-switch .switch-handle {
        position: absolute;
        top: 2px;
        left: 2px;
        width: 28px;
        height: 28px;
        background: white;
        border-radius: 50%;
        box-shadow: 0 2px 4px rgba(0,0,0,0.2);
        transition: transform 0.3s cubic-bezier(0.4, 0.0, 0.2, 1);
    }

    .custom-switch input:checked + .switch-handle {
        transform: translateX(20px);
    }

    .custom-switch input:checked ~ .switch-bg {
        background: #007AFF;
    }

    .switch-bg {
        position: absolute;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        border-radius: 16px;
        transition: background 0.3s;
    }

    /* 改进输入框样式 */
    .input-with-unit {
        height: 40px;
    }

    .input-with-unit input {
        height: 100%;
        padding: 0 12px;
        font-size: 16px;
    }

    .input-with-unit .unit {
        height: 100%;
        padding: 0 12px;
        line-height: 40px;
    }

    /* 改进按钮样式 */
    .btn {
        height: 40px;
        padding: 0 20px;
        font-size: 16px;
        border-radius: 8px;
    }

    /* 历史记录样式优化 */
    #historicalData {
        max-height: none;
        overflow-y: visible;
    }

    .history-table {
        width: 100%;
        border-collapse: collapse;
    }

    .history-table tr {
        border-bottom: 1px solid #eee;
    }

    .history-table td {
        padding: 12px;
        text-align: center;
    }
</style>
)rawliteral";

#endif 