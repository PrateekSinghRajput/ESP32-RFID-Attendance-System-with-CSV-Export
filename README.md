
# 📝 ESP32 RFID Attendance System with CSV Export

An IoT-based attendance logging system using the **ESP32**, **RC522 RFID module**, and local data management. This system scans RFID tags, displays user info on a **16x2 LCD**, and provides audio-visual feedback via a **Buzzer** and **LEDs**.



## 🚀 Features
*   🆔 **RFID Authentication:** Quick scanning using the MFRC522 module.
*   📟 **Real-time Feedback:** 16x2 LCD displays "Access Granted" or "Unknown Tag."
*   🔔 **Audio-Visual Alerts:** Buzzer beeps and LEDs flash upon successful/failed scans.
*   📊 **Data Logging:** Attendance records are exported/stored in **CSV format**.
*   🌐 **Wireless Ready:** Powered by ESP32 for potential Wi-Fi/Cloud integration.

---

## 🛠️ Components Used
*   **Microcontroller:** ESP32 (NodeMCU) 🧠
*   **RFID Reader:** RC522 Module 💳
*   **Display:** 16x2 LCD with I2C Module 📺
*   **Indicators:** 5V Buzzer & 2x LEDs (Red/Green) 🚥
*   **Others:** Breadboard, Jumper Wires, and Resistors 🔌

---

## 🖇️ Circuit Connection
| Component | ESP32 Pin |
| :--- | :--- |
| **RC522 SDA** | GPIO 02 |
| **RC522 SCK** | GPIO 18 |
| **RC522 MOSI** | GPIO 23 |
| **RC522 MISO** | GPIO 19 |
| **RC522 RST** | GPIO 15 |
| **LCD SCL** | GPIO 22 |
| **LCD SDA** | GPIO 21 |
| **Buzzer** | GPIO 14 |
| **Green LED** | GPIO 26 |
| **Red LED** | GPIO 27 |

---

## 💻 Software Setup
1.  **Arduino IDE:** Ensure you have the ESP32 board library installed.
2.  **Required Libraries:**
    *   `MFRC522` (by GithubCommunity)
    *   `LiquidCrystal_I2C` (by Frank de Brabander)
    *   `SPI.h`
3.  **Clone the Repo:**
    ```bash
    git clone https://github.com/YOUR_USERNAME/ESP32-RFID-Attendance-System.git
    ```
4.  **Upload:** Open the `.ino` file and upload it to your ESP32.

---

## 📊 CSV Export Logic
The system records the **UID** of the RFID tag along with a **Timestamp**. The data is formatted as:
`ID, Date, Time, Status`

You can retrieve the CSV file via:
*   [ ] Serial Monitor data dump.
*   [ ] SD Card Module (if added).
*   [ ] Web Server interface (built-in ESP32).

---

## 📸 Screenshots / Demo
*(Add your project photos or a GIF here!)*
> [!TIP]
> Drag and drop an image of your physical build here to make the README pop!

---

## 🤝 Contributing
Contributions, issues, and feature requests are welcome! Feel free to check the [issues page](https://github.com/YOUR_USERNAME/ESP32-RFID-Attendance-System/issues).

## 📜 License
Distributed under the MIT License. See `LICENSE` for more information.

---

**Made with ❤️ byThis is a solid hardware stack for an attendance system. A great README should be clear, visual, and make it easy for others to replicate your work.

Here is a structured template you can copy and paste directly into your `README.md` file.

---

# 📝 ESP32 RFID Attendance System with CSV Export

An IoT-based attendance logging system using the **ESP32**, **RC522 RFID module**, and local data management. This system scans RFID tags, displays user info on a **16x2 LCD**, and provides audio-visual feedback via a **Buzzer** and **LEDs**.



## 🚀 Features
*   🆔 **RFID Authentication:** Quick scanning using the MFRC522 module.
*   📟 **Real-time Feedback:** 16x2 LCD displays "Access Granted" or "Unknown Tag."
*   🔔 **Audio-Visual Alerts:** Buzzer beeps and LEDs flash upon successful/failed scans.
*   📊 **Data Logging:** Attendance records are exported/stored in **CSV format**.
*   🌐 **Wireless Ready:** Powered by ESP32 for potential Wi-Fi/Cloud integration.

---

## 🛠️ Components Used
*   **Microcontroller:** ESP32 (NodeMCU) 🧠
*   **RFID Reader:** RC522 Module 💳
*   **Display:** 16x2 LCD with I2C Module 📺
*   **Indicators:** 5V Buzzer & 2x LEDs (Red/Green) 🚥
*   **Others:** Breadboard, Jumper Wires, and Resistors 🔌

---

## 🖇️ Circuit Connection
| Component | ESP32 Pin |
| :--- | :--- |
| **RC522 SDA** | GPIO 21 (or VSPI SS) |
| **RC522 SCK** | GPIO 18 |
| **RC522 MOSI** | GPIO 23 |
| **RC522 MISO** | GPIO 19 |
| **RC522 RST** | GPIO 22 |
| **LCD SCL** | GPIO 22 |
| **LCD SDA** | GPIO 21 |
| **Buzzer** | GPIO 25 |
| **Green LED** | GPIO 26 |
| **Red LED** | GPIO 27 |

---

## 💻 Software Setup
1.  **Arduino IDE:** Ensure you have the ESP32 board library installed.
2.  **Required Libraries:**
    *   `MFRC522` (by GithubCommunity)
    *   `LiquidCrystal_I2C` (by Frank de Brabander)
    *   `SPI.h`
3.  **Clone the Repo:**
    ```bash
    git clone https://github.com/YOUR_USERNAME/ESP32-RFID-Attendance-System.git
    ```
4.  **Upload:** Open the `.ino` file and upload it to your ESP32.

---

## 📊 CSV Export Logic
The system records the **UID** of the RFID tag along with a **Timestamp**. The data is formatted as:
`ID, Date, Time, Status`

You can retrieve the CSV file via:
*   [ ] Serial Monitor data dump.
*   [ ] SD Card Module (if added).
*   [ ] Web Server interface (built-in ESP32).

---

## 📸 Screenshots / Demo
*(Add your project photos or a GIF here!)*
> [!TIP]
> Drag and drop an image of your physical build here to make the README pop!

---

## 🤝 Contributing
Contributions, issues, and feature requests are welcome! Feel free to check the [issues page](https://github.com/YOUR_USERNAME/ESP32-RFID-Attendance-System/issues).

## 📜 License
Distributed under the MIT License. See `LICENSE` for more information.

---

**Made with ❤️ by Prateek**
