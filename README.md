# T100S_to_servo
THIS PROJECT IS IN EXPERIMENTAL!!!. It still need practical testing and adjustments.

This repo works by using T100S LED controller to get pattern data from LEDEDIT and translate the data via SPI protocol to microcontroller (in this case ESP32) that convert the RGB data to servo angle
Using a **logic level shifter** to safely interface the **T1000S** (5V logic) with the **ESP32** (3.3V logic) is essential to prevent potential damage to the ESP32. Below is the wiring setup and explanation for the logic level shifter and other hardware components:

---

### **Wiring with Logic Level Shifter**

1. **Logic Level Shifter Connections**:
   A bidirectional logic level shifter has two sides:
   - **High Voltage (HV) Side**: Operates at 5V (connected to the T1000S).
   - **Low Voltage (LV) Side**: Operates at 3.3V (connected to the ESP32).

| Shifter Pin | Connect To                         |
|-------------|------------------------------------|
| **HV**      | 5V power source (T1000S VCC)      |
| **LV**      | 3.3V power source (ESP32 3.3V pin)|
| **GND**     | Common ground (ESP32 + T1000S)    |
| **HV1**     | T1000S `DATA_OUT`                 |
| **LV1**     | ESP32 `MISO` (GPIO 19)            |
| **HV2**     | T1000S `CLOCK_OUT`                |
| **LV2**     | ESP32 `SCK` (GPIO 18)             |

---

2. **T1000S Connections**:

| T1000S Pin   | Connect To                              |
|--------------|-----------------------------------------|
| **DATA_OUT** | `HV1` of Logic Level Shifter            |
| **CLOCK_OUT**| `HV2` of Logic Level Shifter            |
| **GND**      | Common ground (shared with ESP32 GND)   |
| **VCC**      | 5V power source (shared with logic shifter HV) |

---

3. **ESP32 Connections**:

| ESP32 Pin    | Connect To                              |
|--------------|-----------------------------------------|
| **3.3V**     | `LV` of Logic Level Shifter             |
| **GND**      | Common ground (shared with T1000S GND)  |
| **MISO** (GPIO 19)| `LV1` of Logic Level Shifter        |
| **SCK** (GPIO 18)| `LV2` of Logic Level Shifter         |

---

4. **I2C Servo Driver PCA9685 (LCD and Keypad optional) Connections**:

| Component     | ESP32 Pin          | Description          |
|---------------|---------------------|----------------------|
| **PCA SDA**   | GPIO 21             | I2C data             |
| **PCA SCL**   | GPIO 22             | I2C clock            |
| **LCD SDA**   | GPIO 21             | I2C data             |
| **LCD SCL**   | GPIO 22             | I2C clock            |
| **Keypad SDA**| GPIO 21             | I2C data             |
| **Keypad SCL**| GPIO 22             | I2C clock            |
| **VCC**       | 3.3 V               | power input for chip |
| **GND**       | Common ground       | Shared with ESP32 and T1000S |

---

5. **Power Supply**:
   - **ESP32**: Powered via USB or 3.3V from a regulated source.
   - **T1000S**: Powered via its own 5V power supply.
   - **Logic Level Shifter**:
     - **HV (High Voltage)** connected to the T1000S 5V supply.
     - **LV (Low Voltage)** connected to the ESP32 3.3V pin.

---

### **Complete Wiring Summary**

| Hardware         | Connection Details                                     |
|------------------|--------------------------------------------------------|
| **T1000S DATA**  | → `HV1` of Logic Level Shifter → `LV1` → ESP32 GPIO 19 |
| **T1000S CLOCK** | → `HV2` of Logic Level Shifter → `LV2` → ESP32 GPIO 18 |
| **T1000S GND**   | Shared ground with ESP32 and Logic Level Shifter       |
| **T1000S VCC**   | 5V power source → HV of Logic Level Shifter            |
| **ESP32 3.3V**   | → LV of Logic Level Shifter                            |
| **ESP32 GND**    | Shared ground                                          |
| **PCA9685 I2C**  | SDA → GPIO 21, SCL → GPIO 22                           |
| **LCD I2C**      | SDA → GPIO 21, SCL → GPIO 22                           |
| **Keypad I2C**   | SDA → GPIO 21, SCL → GPIO 22                           |

---

### **Wiring Diagram**

1. **Logic Level Shifter** is positioned between the **T1000S** (5V) and **ESP32** (3.3V) for safe signal handling.
2. Ensure all components share a **common ground** to prevent communication errors.
