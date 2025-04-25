# IoT-Enabled Driver Safety & Crash Detection System

A two-part project that:

1. **Simulates** an ESP32-based IoT sensor board (with OLED display, MPU6050, LED, buzzer, button, potentiometer, slide switch) in [Wokwi](https://wokwi.com)  
2. Runs a **Python** computer-vision module to detect driver drowsiness in real time using `dlib` and OpenCV

---

## Table of Contents

- [Features](#features)  
- [File Structure](#file-structure)  
- [Prerequisites](#prerequisites)  
- [Setup](#setup)  
  - [1. Wokwi Simulation](#1-wokwi-simulation)  
  - [2. Python Drowsiness Detector](#2-python-drowsiness-detector)  
- [Usage](#usage)  
- [Contributing](#contributing)  
- [License](#license)  
- [Author](#author)

---

## Features

- **Realistic hardware simulation** of an ESP32 dev-kit with peripherals in Wokwi  
- **OLED dashboard** showing sensor data  
- **IMU (MPU6050)** integration for motion sensing  
- **Buzzer and LED** alert on simulated conditions  
- **Pushbutton & slide-switch** user controls  
- **Python CV module** that:  
  - Captures webcam frames  
  - Computes Eye Aspect Ratio (EAR)  
  - Triggers a beep & on-screen alert when EAR remains below threshold

---

## File Structure

```
/
├── Wokwi/
│   ├── diagram.json           # Circuit layout for Wokwi
│   ├── libraries.txt          # Required Arduino libraries
│   ├── sketch.ino             # ESP32 Arduino sketch
│   └── wokwi-project.txt      # Link to simulate on Wokwi
│
├── Python Drowsiness Detection/
│   ├── drowsiness_detection.py      # Main CV script
│   └── model/
│       └── shape_predictor_68_face_landmarks.dat
│
└── README.md
```

---

## Prerequisites

- **Wokwi** account (free) to run the ESP32 simulation  
- **Arduino IDE** (if you wish to flash to a real board)  
- **Python 3.7+** on Windows (for `winsound`); on macOS/Linux replace the beep mechanism  
- Python packages:
  ```bash
  pip install opencv-python dlib imutils scipy
  ```

---

## Setup

### 1. Wokwi Simulation

1. Open `Wokwi/wokwi-project.txt` and follow the URL to load the project in Wokwi.  
2. Verify that `diagram.json` renders your ESP32 board with all peripherals.  
3. In the Wokwi editor, copy `Wokwi/sketch.ino` into the Arduino sketch pane.
4. **Fill in your credentials**: open `sketch.ino` and replace these placeholders with your values  
   ```cpp
   // WiFi Credentials
   const char* ssid = "WiFi_SSID";
   const char* password = "WiFi_Password";
   const char* thingSpeakApiKey = "ThingSpeak_API_Key";
    
   // Twilio Credentials
   const char* accountSid = "Twilio_Account_SID";
   const char* authToken  = "Twilio_Authentication_Token"; 
   const char* twilioNumber = "Sender_Number";
   const char* destinationNumber = "Destination_Number";
   ```
5. Ensure the libraries listed in `Wokwi/libraries.txt` are enabled:
   - Adafruit GFX  
   - Adafruit SSD1306  
   - Adafruit MPU6050  
   - Adafruit Sensor Lab & Calibration  
   - WiFi & HttpClient  
   - Pushbutton  
6. Click **“Play”** to start simulation. Observe sensor readings on the OLED and test button/buzzer behavior.

### 2. Python Drowsiness Detector

1. Clone/download this repo to your local machine.  
2. Place `shape_predictor_68_face_landmarks.dat` into the folder `Python Drowsiness Detection/model/`.  
   - You can download it from the [dlib model zoo](http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2).  
3. Install dependencies:
   ```bash
   pip install opencv-python dlib imutils scipy
   ```
4. (Windows only) ensure `winsound` is available (built-in). On other platforms, replace the beep with an alternative.

---

## Usage

1. **Run the Wokwi simulation** and verify the circuit is working.  
2. **Start your webcam** and in a separate terminal run:
   ```bash
   cd "Python Drowsiness Detection"
   python drowsiness_detection.py
   ```
3. The script will open a resizable window.  
4. When your eyes stay closed (EAR < 0.25) for 48 consecutive frames, you’ll hear a beep and see a “DROWSINESS ALERT!” message.

---

## Contributing

Feel free to open issues or submit pull requests for:

- Alternative alert mechanisms (SMS, email, dashboard)  
- Cross-platform audio support  
- Adjustable EAR thresholds or frame counts  
- Integration with real IoT dashboards  

---

## License

This project is licensed under the [MIT License](LICENSE).

---

## Author

**Siddhesh Save**  
- GitHub: [@Siddhesh51099](https://github.com/Siddhesh51099)  
- Syracuse University, M.S. Computer Science (’25)  
- Email: siddheshsave99@gmail.com  
