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
</style>
)rawliteral";

#endif 