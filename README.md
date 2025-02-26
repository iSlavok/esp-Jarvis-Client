# ESP-Jarvis-Client

Client side (ESP32) for my voice assistant - Jarvis. This school project represents the ESP32 device code that works together with the [server part](https://github.com/iSlavok/esp-Jarvis-Server).

## Project Overview

ESP-Jarvis-Client provides the hardware functionality for the voice assistant:
- Audio capture through microphone
- Audio streaming to server for processing
- Playback of synthesized speech
- Handling assistant states (waiting, recording, responding, speaking)

## Technical Details

- **Programming Languages:** C, C++
- **Target Device:** ESP32
- **Communication:** MQTT, UDP
- **Audio:** Capture and playback via I2S interface

## Features

- Activation via button or voice command
- Audio streaming to server
- State synchronization with server
- Volume control
- Visual status indication

## Note

This project is published for portfolio and educational purposes only. It was created as a school project and is not intended for production use.

© 2025 iSlavok