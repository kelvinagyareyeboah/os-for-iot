from flask import Flask, render_template_string
from flask_socketio import SocketIO, emit
import subprocess
import threading
import sys
import re

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")

HTML_TEMPLATE = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>IoT OS Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://cdn.socket.io/4.0.0/socket.io.min.js"></script>
    <style>
        body { font-family: Arial, sans-serif; background: #2b2b2b; color: white; margin: 0; padding: 20px; }
        .container { max-width: 1200px; margin: 0 auto; }
        .header { text-align: center; margin-bottom: 20px; }
        .stats { display: flex; justify-content: space-around; margin-bottom: 20px; }
        .stat { background: #34495e; padding: 20px; border-radius: 8px; text-align: center; }
        .stat h3 { margin: 0 0 10px 0; font-size: 1.2em; }
        .stat div { font-size: 2.5em; font-weight: bold; color: #1abc9c; }
        .chart-container { background: #34495e; padding: 20px; border-radius: 8px; margin-bottom: 20px; }
        .logs { background: #34495e; padding: 20px; border-radius: 8px; height: 300px; overflow-y: auto; }
        pre { margin: 0; white-space: pre-wrap; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>IoT OS Dashboard</h1>
            <div id="status">Status: <span id="status-text">WAITING</span></div>
        </div>
        <div class="stats">
            <div class="stat">
                <h3>Current Temperature</h3>
                <div id="current-temp">--°C</div>
            </div>
            <div class="stat">
                <h3>Min Temperature</h3>
                <div id="min-temp">--°C</div>
            </div>
            <div class="stat">
                <h3>Avg Temperature</h3>
                <div id="avg-temp">--°C</div>
            </div>
            <div class="stat">
                <h3>Max Temperature</h3>
                <div id="max-temp">--°C</div>
            </div>
        </div>
        <div class="chart-container">
            <canvas id="tempChart"></canvas>
        </div>
        <div class="logs">
            <h3>Logs</h3>
            <pre id="log-output"></pre>
        </div>
    </div>

    <script>
        const socket = io();
        const tempData = [];
        const ctx = document.getElementById('tempChart').getContext('2d');
        const chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Temperature (°C)',
                    data: tempData,
                    borderColor: '#1abc9c',
                    backgroundColor: 'rgba(26, 188, 156, 0.2)',
                    fill: true,
                }]
            },
            options: {
                responsive: true,
                scales: {
                    x: { title: { display: true, text: 'Time' } },
                    y: { title: { display: true, text: 'Temperature (°C)' } }
                }
            }
        });

        socket.on('data', (data) => {
            document.getElementById('log-output').textContent += data.line + '\\n';
            if (data.temp !== null) {
                tempData.push(data.temp);
                if (tempData.length > 50) tempData.shift();
                document.getElementById('current-temp').textContent = data.temp + '°C';
                document.getElementById('min-temp').textContent = Math.min(...tempData) + '°C';
                document.getElementById('avg-temp').textContent = (tempData.reduce((a, b) => a + b, 0) / tempData.length).toFixed(1) + '°C';
                document.getElementById('max-temp').textContent = Math.max(...tempData) + '°C';
                chart.data.labels = tempData.map((_, i) => i + 1);
                chart.update();
            }
            if (data.status) {
                document.getElementById('status-text').textContent = data.status;
            }
        });
    </script>
</body>
</html>
"""

@app.route('/')
def index():
    return render_template_string(HTML_TEMPLATE)

def run_os():
    socketio.emit('data', {'line': 'OS starting...', 'temp': None, 'status': 'ACTIVE'})  # Test emit
    process = subprocess.Popen(['make', 'run'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, cwd='/home/yibrahim021/iot-os')
    while True:
        line = process.stdout.readline()
        if not line:
            break
        line = line.strip()
        print(f"OS Line: {line}")  # Debug print
        socketio.emit('data', {'line': line, 'temp': None, 'status': None})
        if '[SENSOR] Temp:' in line:
            match = re.search(r'Temp: (\d+)', line)
            if match:
                temp = int(match.group(1))
                print(f"Parsed temp: {temp}")  # Debug print
                socketio.emit('data', {'line': '', 'temp': temp, 'status': None})
        if '[SYS]' in line:
            if 'Power: Low' in line:
                socketio.emit('data', {'line': '', 'temp': None, 'status': 'SLEEP'})
            elif 'Power: High' in line:
                socketio.emit('data', {'line': '', 'temp': None, 'status': 'ACTIVE'})
            print(f"Parsed status: {'SLEEP' if 'Power: Low' in line else 'ACTIVE' if 'Power: High' in line else 'unknown'}")  # Debug print

if __name__ == '__main__':
    threading.Thread(target=run_os, daemon=True).start()
    socketio.run(app, host='0.0.0.0', port=5000, debug=False)