# ESP32 Smart LED Controller

A comprehensive IoT project that allows wireless control of addressable LEDs connected to an ESP32 microcontroller through a Flutter mobile application. The ESP32 hosts an HTTP server that receives commands from the mobile app to control LED states, colors, blinking patterns, and even display Morse code messages.

## Features

### ESP32 Firmware
- **WiFi Connectivity**: Connects to your local WiFi network as a station
- **HTTP Server**: RESTful API endpoints for LED control
- **Multiple LED Modes**:
  - **Light Mode**: Simple on/off control
  - **Blinky Mode**: Configurable blink intervals
  - **Morse Code Mode**: Display text as Morse code light patterns
- **RGB Color Control**: Full 24-bit color support (Red, Green, Blue channels)
- **Addressable LED Support**: Compatible with WS2812, WS2813, and similar LED strips

### Flutter Mobile App
- **Real-time Control**: Instant LED control via HTTP requests
- **Intuitive UI**: Clean, modern interface with themed components
- **Color Picker**: RGB sliders with live color preview
- **Text Input**: Convert any message to Morse code
- **Dual Connectivity**: UI prepared for both WiFi and Bluetooth (WiFi currently implemented)

## Hardware Requirements

- **ESP32 Development Board**
- **Addressable LED Strip** (WS2812/WS2813) or single addressable LED
- **Jumper Wires**
- **Power Supply** (if using LED strip with many LEDs)

## Software Requirements

### For ESP32 Firmware
- [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) (Espressif IoT Development Framework)
- CMake
- Git

### For Flutter App
- [Flutter SDK](https://flutter.dev/docs/get-started/install) (3.8.1 or higher)
- [Dart SDK](https://dart.dev/get-dart)
- Android Studio or VS Code with Flutter extension
- Android device or emulator / iOS device or simulator

## Hardware Setup

1. **Connect the LED Strip to ESP32:**
   ```
   LED Strip Data Pin  →  GPIO 38 (configurable)
   LED Strip VCC       →  3.3V or 5V (depending on strip)
   LED Strip GND       →  GND
   ```

2. **For LED strips requiring higher current:**
   - Use external power supply
   - Connect GND of power supply to ESP32 GND
   - Connect VCC of LED strip to external power supply

## Firmware Configuration & Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/DanielBrill20/esp32_smart_led_controller.git
   cd esp32_smart_led_controller/firmware
   ```

2. **Configure the project:**
   ```bash
   idf.py menuconfig
   ```
   
   Configure the following in "Project Configuration":
   - **LED GPIO**: GPIO pin connected to LED data line (default: 38)
   - **MAX LEDS**: Number of LEDs in your strip (default: 1)
   - **WiFi SSID**: Your WiFi network name
   - **WiFi Password**: Your WiFi network password

3. **Build and flash:**
   ```bash
   idf.py build
   idf.py flash monitor
   ```

4. **Note the IP address** displayed in the monitor output - you'll need this for the mobile app.

## Mobile App Installation

1. **Navigate to the app directory:**
   ```bash
   cd ../app
   ```

2. **Install dependencies:**
   ```bash
   flutter pub get
   ```

3. **Update the ESP32 IP address** in `lib/main.dart`:
   ```dart
   // Replace with your ESP32's IP address
   var url = Uri.http('192.168.50.199', '/endpoint');
   ```

4. **Run the app:**
   ```bash
   flutter run
   ```

## API Endpoints

The ESP32 HTTP server provides the following REST API endpoints:

### POST `/light`
Control basic LED on/off state.
```json
{
  "state": true  // true for on, false for off
}
```

### POST `/blinky`
Start blinking mode with specified duration.
```json
{
  "duration": 1000  // milliseconds (1000 = 1 second on, 1 second off)
}
```

### POST `/morse`
Display Morse code pattern. Use `/` for spaces between words.
```json
{
  "morse": ".... . .-.. .-.. ---/-.-- --- ..-"  // "HELLO YOU" in Morse
}
```

### POST `/color`
Set RGB color values (0-255 each).
```json
{
  "red": 255,
  "green": 128,
  "blue": 0
}
```

## Mobile App Usage

1. **Connect to WiFi**: Ensure your phone and ESP32 are on the same network
2. **Launch the app**: The interface shows four main control sections:

### Light Control
- Toggle switch to turn LED on/off
- Immediately updates LED state

### Blinky Mode
- Enter duration in milliseconds
- Press send button to start blinking
- LED will blink with specified on/off intervals

### Morse Code
- Type any message in the text field
- App automatically converts to Morse code
- Press send to display the message on LED

### Color Control
- Use RGB sliders (0-255) to set color
- Live preview shows selected color
- Changes apply immediately to LED
- Note: There is a visual discrepancy between the color preview and the LED (the LED, for example, cannot shine gray light). Future solutions to come.

## Troubleshooting

### ESP32 Issues
- **Won't connect to WiFi**: Check SSID/password in menuconfig
- **LED doesn't light up**: Verify GPIO pin and power connections
- **HTTP server not responding**: Check IP address and network connectivity

### Mobile App Issues
- **Can't connect to ESP32**: Verify IP address in code matches ESP32's actual IP
- **App crashes**: Check Flutter installation and dependencies
- **Buttons don't work**: Ensure ESP32 is powered and HTTP server is running

### Network Issues
- **Different subnets**: Ensure phone and ESP32 are on same WiFi network
- **Firewall blocking**: Check router settings for device communication

## Future Enhancements

- [ ] Refreshed app UI
- [ ] WiFi configuration from within the app
- [ ] Discoverable IP addresses (rather than hard-coding ESP IP address)
- [ ] Making the color preview and LED color match more closely
- [ ] Bluetooth Low Energy (BLE) connectivity