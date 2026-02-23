#ifndef HTML_code_h
#define HTML_code_h

const char HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>STM32 Control Pro</title>
    <style>
        body { font-family: Arial, sans-serif; background-color: #2c3e50; color: #ecf0f1; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }
        .container { background: #34495e; padding: 30px; border-radius: 10px; box-shadow: 0 8px 16px rgba(0,0,0,0.3); width: 300px; }
        h2 { margin-top: 0; text-align: center; }
        .input-group, .status-box { margin-bottom: 20px; }
        input[type="number"], input[type="text"] { width: 100%; padding: 10px; margin-top: 5px; box-sizing: border-box; border-radius: 5px; border: 1px solid #7f8c8d; background-color: #ecf0f1; }
        input[type="button"] { width: 100%; padding: 10px; margin-top: 10px; background-color: #3498db; color: white; border: none; border-radius: 5px; cursor: pointer;}
        input[type="button"]:hover { background-color: #2980b9; }
        #connection { font-weight: bold; text-align: center; margin-bottom: 0; }
    </style>
</head>
<body>
    <div class="container">
        <h2>STM32 Motor Control</h2>
        
        <div class="input-group">
            <label>Motor Steps</label>
            <input type="number" id="MotorSteps" value="0">
            <input type="button" value="Send to Motor" onclick="sendData()">
        </div>
        <div class="input-group">
            <label>Encoder Steps</label>
            <input type="number" id="EncoderSteps" value="0" readonly>
        </div>
        <div class="input-group">
            <label>Encoder Degrees</label>
            <input type="text" id="EncoderDegrees" value="0.00" readonly>
        </div>
        
        <p id="connection">INITIALIZING...</p>
    </div>
    
    <script src="/script.js"></script>
</body>
</html>
)rawliteral";

const char javascript[] PROGMEM = R"rawliteral(
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;

    function initWebSocket() {
        websocket = new WebSocket(gateway);
        
        websocket.onopen = () => { 
            const status = document.getElementById('connection');
            status.innerHTML = "SYSTEM ONLINE"; 
            status.style.color = "#2ecc71"; 
        };
        
        websocket.onclose = () => { 
            const status = document.getElementById('connection');
            status.innerHTML = "SYSTEM OFFLINE - RECONNECTING..."; 
            status.style.color = "#e74c3c"; 
            setTimeout(initWebSocket, 2000); 
        };
        
        websocket.onmessage = (event) => {
            // Expecting: "MSteps:123,ESteps:456,EDeg:90.50"
            let data = event.data.split(',');
            if (data.length >= 2) {
                document.getElementById('EncoderSteps').value = data[1].split(':')[1];
                document.getElementById('EncoderDegrees').value = data[2].split(':')[1];
            }
        };
    }

    function sendData() {
        let val = document.getElementById('MotorSteps').value;
        if(websocket.readyState === WebSocket.OPEN) {
            websocket.send("SET_NUM:" + val);
        }
    }
    
    window.onload = initWebSocket;
)rawliteral";

#endif