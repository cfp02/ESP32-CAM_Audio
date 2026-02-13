#include "rtsp_server.h"
#include <WiFiUdp.h>

// RTSP server port
#define RTSP_PORT 554

WiFiServer rtspServer(RTSP_PORT);
WiFiClient rtspClient;
bool rtspSessionActive = false;
String rtspSessionId = "";

// Simple RTSP server for MJPEG streaming
// Note: This is a basic implementation. For full RTSP/RTP support,
// consider using go2rtc or a dedicated RTSP proxy server

void handleRTSP() {
    if (rtspServer.hasClient()) {
        if (!rtspClient || !rtspClient.connected()) {
            rtspClient = rtspServer.available();
            Serial.println("New RTSP client connected");
        } else {
            // Reject new client if one is already connected
            WiFiClient newClient = rtspServer.available();
            newClient.stop();
        }
    }

    if (!rtspClient || !rtspClient.connected()) {
        if (rtspSessionActive) {
            rtspSessionActive = false;
            Serial.println("RTSP client disconnected");
        }
        return;
    }

    if (!rtspClient.available()) {
        return;
    }

    String request = "";
    while (rtspClient.available()) {
        char c = rtspClient.read();
        request += c;
        if (request.endsWith("\r\n\r\n")) {
            break;
        }
    }

    if (request.length() == 0) {
        return;
    }

    // Parse RTSP request
    if (request.indexOf("OPTIONS") >= 0) {
        rtspClient.print("RTSP/1.0 200 OK\r\n");
        rtspClient.print("CSeq: 1\r\n");
        rtspClient.print("Public: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n");
        rtspClient.print("\r\n");
    }
    else if (request.indexOf("DESCRIBE") >= 0) {
        String sdp = "v=0\r\n";
        sdp += "o=- 0 0 IN IP4 " + WiFi.localIP().toString() + "\r\n";
        sdp += "s=ESP32-CAM Stream\r\n";
        sdp += "c=IN IP4 " + WiFi.localIP().toString() + "\r\n";
        sdp += "t=0 0\r\n";
        sdp += "m=video 0 RTP/AVP 26\r\n";
        sdp += "a=control:stream1\r\n";
        sdp += "a=rtpmap:26 JPEG/90000\r\n";

        rtspClient.print("RTSP/1.0 200 OK\r\n");
        rtspClient.print("CSeq: 1\r\n");
        rtspClient.print("Content-Type: application/sdp\r\n");
        rtspClient.print("Content-Length: ");
        rtspClient.print(sdp.length());
        rtspClient.print("\r\n\r\n");
        rtspClient.print(sdp);
    }
    else if (request.indexOf("SETUP") >= 0) {
        rtspSessionId = String(random(10000, 99999));
        rtspClient.print("RTSP/1.0 200 OK\r\n");
        rtspClient.print("CSeq: 1\r\n");
        rtspClient.print("Session: ");
        rtspClient.print(rtspSessionId);
        rtspClient.print("\r\n");
        rtspClient.print("Transport: RTP/AVP/UDP;unicast;client_port=5000-5001\r\n");
        rtspClient.print("\r\n");
        rtspSessionActive = true;
        Serial.println("RTSP session established");
    }
    else if (request.indexOf("PLAY") >= 0) {
        rtspClient.print("RTSP/1.0 200 OK\r\n");
        rtspClient.print("CSeq: 1\r\n");
        rtspClient.print("Session: ");
        rtspClient.print(rtspSessionId);
        rtspClient.print("\r\n");
        rtspClient.print("Range: npt=0.000-\r\n");
        rtspClient.print("\r\n");
        Serial.println("RTSP stream started");
    }
    else if (request.indexOf("TEARDOWN") >= 0) {
        rtspClient.print("RTSP/1.0 200 OK\r\n");
        rtspClient.print("CSeq: 1\r\n");
        rtspClient.print("Session: ");
        rtspClient.print(rtspSessionId);
        rtspClient.print("\r\n\r\n");
        rtspSessionActive = false;
        rtspClient.stop();
        Serial.println("RTSP session torn down");
    }
    else {
        rtspClient.print("RTSP/1.0 501 Not Implemented\r\n");
        rtspClient.print("CSeq: 1\r\n\r\n");
    }
}

void startRTSP() {
    rtspServer.begin();
    Serial.print("RTSP server started on rtsp://");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.print(RTSP_PORT);
    Serial.println("/stream1");
    Serial.println("Note: For BirdNetGo, you may need to use go2rtc as an RTSP proxy");
    Serial.println("      Configure go2rtc to use: http://[IP]:81/stream");
}
