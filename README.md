# -smart-energy-monitor-
An IoT-based power monitoring system that tracks electricity usage in real-time, helping users identify energy consumption patterns and reduce electricity bills.

## Features
- Real-time electricity usage monitoring
- Device-specific power consumption tracking
- Historical data visualization and analysis
- Customizable alerts for unusual consumption patterns
- Remote monitoring via web dashboard
- Energy-saving recommendations based on usage patterns

## Technologies Used
- **Hardware**: ESP32, ACS712 Current Sensors, Voltage Dividers, Custom PCB
- **Firmware**: C/C++, Arduino Framework
- **Backend**: MQTT, Node-RED, InfluxDB
- **Frontend**: Grafana, Responsive Web Dashboard
- **Communication**: WiFi, MQTT Protocol

## Repository Structure

├── /hardware
│   ├── /schematics       # Circuit diagrams
│   ├── /pcb              # PCB design files (KiCad)
│   └── /3d_models        # 3D printable enclosure files
├── /firmware
│   ├── /src              # ESP32 firmware source code
│   ├── /lib              # Libraries
│   └── /include          # Header files
├── /server
│   ├── /node-red         # Node-RED flows
│   ├── /influxdb         # Database configuration
│   └── /grafana          # Dashboard templates
├── /docs
│   ├── /images           # Project images and diagrams
│   ├── /setup_guide      # Installation instructions
│   └── /api              # API documentation
└── README.md


## Installation & Setup

### Hardware Setup
1. Assemble the circuit according to the schematics in `/hardware/schematics`
2. Connect current sensors to the main power lines (with proper safety precautions)
3. Power the ESP32 using a 5V power supply
4. Ensure the device has access to your WiFi network

### Firmware Installation
```bash
# Clone this repository
git clone https://github.com/[your-username]/smart-energy-monitor.git

# Navigate to firmware directory
cd smart-energy-monitor/firmware

# Install PlatformIO (if not already installed)
pip install platformio

# Build and upload firmware
platformio run --target upload

Server Setup

# Install Docker and Docker Compose
sudo apt install docker docker-compose

# Navigate to server directory
cd smart-energy-monitor/server

# Start the server stack
docker-compose up -d

Dashboard Preview

The system provides a comprehensive dashboard showing:

Current power usage in real-time
Daily, weekly, and monthly consumption trends
Device-specific energy usage breakdown
Cost estimates based on local electricity rates
Energy-saving recommendations
Calibration

For accurate readings, the system requires calibration:

Access the calibration page at 
http://[device-ip]/calibrate
Connect a known load (e.g., 60W light bulb)
Follow the on-screen instructions to calibrate voltage and current sensors
Save the calibration values
Future Enhancements

[ ] Machine learning for automatic device recognition
[ ] Integration with smart home systems (HomeAssistant, SmartThings)
[ ] Mobile app development
[ ] Support for three-phase power systems
[ ] Solar production monitoring integration
Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

License

This project is licensed under the MIT License - see the LICENSE file for details.

Acknowledgements

https://esp32.com/
 for their invaluable resources
https://github.com/openenergymonitor/EmonLib
 for energy monitoring algorithms
All contributors who have helped shape this project
