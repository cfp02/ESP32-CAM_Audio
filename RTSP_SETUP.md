# RTSP Setup for BirdNetGo

## Option 1: Using go2rtc (Recommended)

The easiest and most reliable way to get RTSP for BirdNetGo is to use **go2rtc** as an RTSP proxy. This converts your HTTP/MJPEG stream to RTSP.

### Setup Steps:

1. **Install go2rtc** (if not already installed)
   - Follow instructions at: https://github.com/AlexxIT/go2rtc

2. **Configure go2rtc** to use your ESP32-CAM stream:
   
   Add to your `go2rtc.yaml`:
   ```yaml
   streams:
     esp32cam:
       - ffmpeg:http://[YOUR_ESP32_IP]:81/stream#video=h264
   ```

   Or for MJPEG:
   ```yaml
   streams:
     esp32cam:
       - http://[YOUR_ESP32_IP]:81/stream
   ```

3. **Access RTSP stream**:
   ```
   rtsp://[go2rtc_server_ip]:8554/esp32cam
   ```

4. **Use in BirdNetGo**:
   - Add the RTSP URL from go2rtc to your BirdNetGo configuration
   - Example: `rtsp://192.168.1.10:8554/esp32cam`

## Option 2: Direct RTSP (Basic Implementation)

The code includes a basic RTSP server implementation, but it only handles protocol negotiation. For full RTSP/RTP streaming, you would need to implement RTP packetization, which is complex.

**RTSP URL**: `rtsp://[ESP32_IP]:554/stream1`

**Note**: The current implementation may not work directly with BirdNetGo as it requires proper RTP streaming. Using go2rtc (Option 1) is strongly recommended.

## Why go2rtc?

- ✅ Converts HTTP/MJPEG to proper RTSP/RTP
- ✅ Low latency
- ✅ Reliable and well-tested
- ✅ Supports multiple clients
- ✅ Easy to configure
- ✅ Works perfectly with BirdNetGo

## Troubleshooting

If the direct RTSP doesn't work with BirdNetGo:
1. Use go2rtc as described in Option 1
2. Ensure your ESP32-CAM HTTP stream is working: `http://[IP]:81/stream`
3. Verify go2rtc can access the stream
4. Test the RTSP URL from go2rtc in VLC or another RTSP client before adding to BirdNetGo

