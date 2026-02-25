/**
 * ESP32 Sniper Addon System
 * Created by Kaustav Ray
 * Date: 25 Feb, 2026
 * Copyright (c) 2026 Kaustav Ray. All Rights Reserved.
 */

#include "WebInterface.h"
#include "Config.h"

// HTML Dashboard content
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Sniper Interface</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: 'Courier New', monospace; background-color: #121212; color: #00ff00; margin: 0; padding: 20px; text-align: center; }
    h1 { text-shadow: 0 0 10px #00ff00; }
    .container { display: flex; flex-wrap: wrap; justify-content: center; gap: 20px; }
    .card { background: #1e1e1e; padding: 20px; border: 2px solid #00ff00; border-radius: 10px; width: 300px; box-shadow: 0 0 15px rgba(0, 255, 0, 0.2); }
    .video-feed { width: 100%; max-width: 640px; border: 4px solid #ff0000; border-radius: 5px; }
    .data-row { display: flex; justify-content: space-between; margin: 10px 0; border-bottom: 1px dashed #333; }
    .alert { color: #ff0000; font-weight: bold; animation: blink 1s infinite; display: none; }
    .locked { border-color: #ff0000 !important; box-shadow: 0 0 20px #ff0000; }
    @keyframes blink { 50% { opacity: 0; } }
  </style>
</head>
<body>
  <h1>TACTICAL DASHBOARD</h1>
  <div class="container">

    <!-- Camera Feed -->
    <div class="card" id="feed-card">
      <h3>TARGET FEED</h3>
      <img src="/capture" id="stream" class="video-feed" onload="refreshImage()" onerror="setTimeout(refreshImage, 500)">
      <div id="target-alert" class="alert">TARGET ACQUIRED</div>
      <div id="lock-status" style="margin-top:10px; font-size: 1.2em;">SYSTEM: SCANNING</div>
    </div>

    <!-- Sensor Data -->
    <div class="card">
      <h3>ENVIRONMENT</h3>
      <div class="data-row"><span>Temp:</span> <span id="temp">--</span> &deg;C</div>
      <div class="data-row"><span>Humidity:</span> <span id="hum">--</span> %</div>
      <div class="data-row"><span>Pressure:</span> <span id="pres">--</span> hPa</div>
      <div class="data-row"><span>Altitude:</span> <span id="alt">--</span> m</div>
      <div class="data-row"><span>Air Density:</span> <span id="density">--</span> kg/m3</div>
    </div>

    <!-- Ballistics -->
    <div class="card">
      <h3>BALLISTICS</h3>
      <div class="data-row"><span>Wind Speed:</span> <span id="wind">--</span> m/s</div>
      <div class="data-row"><span>Distance:</span> <span id="dist">--</span> m</div>
      <div class="data-row"><span>Correction (Pan):</span> <span id="pan">--</span> deg</div>
      <div class="data-row"><span>Correction (Tilt):</span> <span id="tilt">--</span> deg</div>
    </div>
  </div>

<script>
  function refreshImage() {
    // Refresh immediately to simulate stream (~10-15 FPS possible depending on network)
    document.getElementById("stream").src = "/capture?t=" + new Date().getTime();
  }

  function updateData() {
    fetch('/api/data')
      .then(response => response.json())
      .then(data => {
        document.getElementById("temp").innerText = data.temp.toFixed(1);
        document.getElementById("hum").innerText = data.hum.toFixed(1);
        document.getElementById("pres").innerText = data.pres.toFixed(1);
        document.getElementById("alt").innerText = data.alt.toFixed(1);
        document.getElementById("wind").innerText = data.wind.toFixed(1);
        document.getElementById("dist").innerText = (data.dist > 900) ? "OUT OF RANGE" : (data.dist / 100).toFixed(2); // Convert cm to m
        document.getElementById("pan").innerText = (data.pan - 90).toFixed(1); // Deviation from center
        document.getElementById("tilt").innerText = (data.tilt - 90).toFixed(1);

        // Simple Air Density Approximation: p / (R_specific * T)
        // R_specific for dry air = 287.058 J/(kg·K)
        // T in Kelvin
        let T_k = data.temp + 273.15;
        let P_pa = data.pres * 100;
        let density = P_pa / (287.058 * T_k);
        document.getElementById("density").innerText = density.toFixed(3);

        // Visual Feedback
        const feedCard = document.getElementById("feed-card");
        const alertBox = document.getElementById("target-alert");
        const lockStatus = document.getElementById("lock-status");

        if (data.motion) {
           alertBox.style.display = "block";
        } else {
           alertBox.style.display = "none";
        }

        if (data.locked) {
           feedCard.classList.add("locked");
           lockStatus.innerText = "SYSTEM: LOCKED";
           lockStatus.style.color = "#ff0000";
        } else {
           feedCard.classList.remove("locked");
           lockStatus.innerText = "SYSTEM: SCANNING";
           lockStatus.style.color = "#00ff00";
        }
      })
      .catch(err => console.error(err));
  }

  // Update data every 1 second
  setInterval(updateData, 1000);
</script>
</body>
</html>
)rawliteral";

WebInterface::WebInterface(CameraHandler* cam) : server(WEB_SERVER_PORT), camera(cam) {}

void WebInterface::begin() {
    server.on("/", [this](){ handleRoot(); });
    server.on("/capture", [this](){ handleCapture(); });
    server.on("/api/data", [this](){ handleData(); });
    server.begin();
}

void WebInterface::handleClient() {
    server.handleClient();
}

void WebInterface::updateSensorData(float t, float h, float p, float g, float a, float w, float d, bool l, bool m, String gen, int panPos, int tiltPos) {
    temp = t;
    hum = h;
    pres = p;
    gas = g;
    alt = a;
    wind = w;
    dist = d;
    locked = l;
    motion = m;
    gender = gen;
    pan = panPos;
    tilt = tiltPos;
}

void WebInterface::handleRoot() {
    server.send(200, "text/html", INDEX_HTML);
}

void WebInterface::handleCapture() {
    if (!camera) {
        server.send(500, "text/plain", "Camera not initialized");
        return;
    }

    camera_fb_t* fb = camera->capture();
    if (!fb) {
        server.send(500, "text/plain", "Capture failed");
        return;
    }

    WiFiClient client = server.client();
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: image/jpeg");
    client.println("Content-Length: " + String(fb->len));
    client.println("Cache-Control: no-cache, no-store, must-revalidate");
    client.println("Pragma: no-cache");
    client.println("Expires: 0");
    client.println("Connection: close");
    client.println();
    client.write(fb->buf, fb->len);

    camera->release(fb);
}

void WebInterface::handleData() {
    // Create JSON response
    StaticJsonDocument<512> doc;
    doc["temp"] = temp;
    doc["hum"] = hum;
    doc["pres"] = pres;
    doc["gas"] = gas;
    doc["alt"] = alt;
    doc["wind"] = wind;
    doc["dist"] = dist;
    doc["locked"] = locked;
    doc["motion"] = motion;
    doc["gender"] = gender;
    doc["pan"] = pan;
    doc["tilt"] = tilt;

    String response;
    serializeJson(doc, response);

    server.send(200, "application/json", response);
}
