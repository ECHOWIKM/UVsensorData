#ifndef HTML_STYLES_H
#define HTML_STYLES_H

const char HTML_STYLES[] PROGMEM = R"rawliteral(
<style>
    * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif; }
    body { background: #f5f7fa; color: #333; padding: 20px; margin: 0; }
    .container { max-width: 100%; margin: 0 auto; padding: 0 20px; }
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
    
    /* 控制面板基础样式 */
    .control-panel {
        background: #fff;
        border-radius: 20px;
        box-shadow: 0 4px 20px rgba(0,0,0,0.1);
        overflow: hidden;
        margin-bottom: 20px;
        transition: all 0.3s ease;
    }

    .control-panel:hover {
        transform: translateY(-2px);
        box-shadow: 0 6px 25px rgba(0,0,0,0.15);
    }

    .panel-header {
        display: flex;
        align-items: center;
        justify-content: space-between;
        padding: 20px 25px;
        background: linear-gradient(135deg, #2196F3, #1976D2);
        color: white;
        cursor: pointer;
        position: relative;
        overflow: hidden;
        border-radius: 20px;
    }

    .panel-header::before {
        content: '';
        position: absolute;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background: linear-gradient(45deg, rgba(255,255,255,0.1), rgba(255,255,255,0));
        opacity: 0;
        transition: opacity 0.3s;
    }

    .panel-header:hover::before {
        opacity: 1;
    }

    .panel-title {
        font-size: 18px;
        font-weight: 500;
        letter-spacing: 0.5px;
    }

    .arrow {
        font-size: 20px;
        transition: transform 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    }

    .panel-header.active .arrow {
        transform: rotate(180deg);
    }

    .panel-content {
        display: none;
        padding: 25px;
        background: #f8f9fa;
    }

    .panel-content.show {
        display: block;
        animation: slideDown 0.4s cubic-bezier(0.4, 0, 0.2, 1);
    }

    /* 控制卡片网格布局 */
    .control-cards {
        display: grid;
        grid-template-columns: repeat(3, 1fr); /* 默认三列 */
        gap: 20px;
        margin: 0;
    }

    /* 控制卡片样式 */
    .control-card {
        background: white;
        border-radius: 16px;
        padding: 25px;
        box-shadow: 0 2px 10px rgba(0,0,0,0.05);
        transition: all 0.3s ease;
    }

    .control-card:hover {
        transform: translateY(-2px);
        box-shadow: 0 4px 15px rgba(0,0,0,0.1);
    }

    /* 开关样式优化 */
    .switch-container {
        position: relative;
        display: inline-block;
        width: 51px;
        height: 31px;
        margin: 0;
    }

    .switch-container input {
        display: none;
    }

    .switch-track {
        position: absolute;
        cursor: pointer;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #e9e9ea;
        transition: .3s cubic-bezier(0.4, 0, 0.2, 1);
        border-radius: 31px;
    }

    .switch-thumb {
        position: absolute;
        height: 27px;
        width: 27px;
        left: 2px;
        bottom: 2px;
        background-color: white;
        transition: .3s cubic-bezier(0.4, 0, 0.2, 1);
        border-radius: 50%;
        box-shadow: 0 2px 4px rgba(0,0,0,0.2);
    }

    input:checked + .switch-track {
        background-color: #34C759;
    }

    input:checked + .switch-track .switch-thumb {
        transform: translateX(20px);
    }

    /* 输入框样式 */
    .input-container {
        display: flex;
        align-items: center;
        gap: 12px;
    }

    .input-container input {
        width: 80px;
        height: 36px;
        padding: 0 12px;
        border: 1px solid #e0e0e0;
        border-radius: 8px;
        font-size: 14px;
        background: #f8f9fa;
        transition: all 0.3s ease;
    }

    .input-container input:focus {
        border-color: #2196F3;
        background: white;
        outline: none;
        box-shadow: 0 0 0 3px rgba(33, 150, 243, 0.1);
    }

    .unit {
        color: #666;
        font-size: 14px;
        font-weight: 500;
        min-width: 30px;
    }

    /* 按钮样式 */
    .action-btn {
        height: 36px;
        padding: 0 20px;
        border: none;
        border-radius: 8px;
        background: #2196F3;
        color: white;
        font-size: 14px;
        font-weight: 500;
        cursor: pointer;
        transition: all 0.3s;
    }

    .action-btn:hover {
        background: #1976D2;
    }

    .danger-btn {
        width: 100%;
        height: 36px;
        border: none;
        border-radius: 8px;
        background: #dc3545;
        color: white;
        font-size: 14px;
        font-weight: 500;
        cursor: pointer;
        transition: all 0.3s;
        margin-top: 15px;
    }

    .danger-btn:hover {
        background: #c82333;
    }

    /* 日期选择器样式 */
    .date-container {
        display: flex;
        gap: 12px;
        width: 100%;
    }

    .date-input {
        flex: 1;
        height: 36px;
        padding: 0 12px;
        border: 1px solid #e0e0e0;
        border-radius: 8px;
        font-size: 14px;
        background: #f8f9fa;
        transition: all 0.3s ease;
    }

    .date-input:focus {
        border-color: #2196F3;
        background: white;
        outline: none;
        box-shadow: 0 0 0 3px rgba(33, 150, 243, 0.1);
    }

    /* 响应式布局优化 */
    @media screen and (max-width: 1400px) {
        .control-cards {
            grid-template-columns: repeat(2, 1fr); /* 中等屏幕两列 */
        }
    }

    @media screen and (max-width: 992px) {
        .control-cards {
            grid-template-columns: 1fr; /* 小屏幕单列 */
        }
        
        .container {
            padding: 0 15px;
        }
        
        .control-card {
            padding: 20px;
        }
    }

    @media screen and (max-width: 480px) {
        body {
            padding: 10px;
        }
        
        .container {
            padding: 0 10px;
        }
        
        .control-card {
            padding: 15px;
        }

        .control-item {
            padding: 12px 0;
        }

        .input-container {
            flex-wrap: wrap;
            gap: 8px;
        }

        .input-container input {
            width: 100%;
        }

        .action-btn {
            width: 100%;
            margin-top: 8px;
        }
    }

    /* 添加平滑过渡 */
    .control-panel,
    .control-card,
    .input-container,
    .action-btn,
    .date-input,
    .danger-btn {
        transition: all 0.3s ease;
    }
</style>
)rawliteral";

#endif 