
# **SpaceMouse Firmware for TLx493D** !!!!!!!!! NOT WORKING!!!!!!

This repository contains the firmware for a custom SpaceMouse-like device using the Infineon TLx493D 3D magnetic Hall sensor and the XMC1100 microcontroller. The device emulates a 3D input device compatible with the SpaceNav protocol and communicates with the `spacenavd` daemon.

---

## **Features**
- **3D Magnetic Field Sensing:**
  - Reads data from the TLx493D sensor to capture motion in the X, Y, and Z axes.
- **SpaceNav Protocol Compatibility:**
  - Implements basic SpaceMouse functionality for integration with the `spacenavd` daemon.
- **Button Support:**
  - Simulates up to 4 configurable buttons.
- **Binary Data Transmission:**
  - Sends movement and button data in a compact binary format.
- **Command Handling:**
  - Responds to commands such as:
    - `@RESET` for resetting the device.
    - `vQ` for device identification.
    - `CB` to enable binary mode.
    - `MSSV` to enable automatic data transmission.

---

## **Getting Started**

### **Prerequisites**
1. **Hardware:**
   - Infineon TLx493D 3D magnetic Hall sensor.
   - XMC1100 microcontroller (part of the Infineon 2GO kit).
2. **Software:**
   - [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm).
   - [PlatformIO](https://platformio.org/)
   - [Infineon XMC: development platform for PlatformIO](https://github.com/Infineon/platformio-infineonxmc)
   - Serial communication tools like `minicom` or `picocom`.

---

### **Installation**

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/giutrec/spacemouse-tlx493d.git
   cd spacemouse-tlx493d
   ```

2. **Build the Firmware:**
   - using PlatformIO:
     ```bash
     platformio run
     ```
3. **Flash the Firmware:**
   - Use a tool like OpenOCD or the DAVE IDE to upload the firmware to the XMC1100.

4. **Run `spacenavd`:**
   - Start the SpaceNav daemon and check for device recognition:
     ```bash
     sudo spacenavd -v
     ```

---

### **Usage**

1. **Connect the Device:**
   - Attach the XMC1100 board to your computer via USB.

2. **Test the Device:**
   - Use a serial terminal to interact with the device:
     ```bash
     minicom -b 9600 -D /dev/ttyACM0
     ```
   - Send commands such as `@RESET`, `vQ`, or `CB` to verify responses.

3. **Integration with Applications:**
   - Ensure `spacenavd` is running to bridge the device with supported applications like Blender, CAD tools, or 3D viewers.

---

## **Project Structure**

```
spacemouse-tlx493d/
├── src/
│   ├── main.cpp        # Firmware source code
│   └── utils.cpp       # Helper functions (in the future)
├── include/
│   ├── device_config.h # Configuration constants (in the future)
│   └── protocol.h      # SpaceNav protocol definitions (in the future)
├── platformio.ini      # PlatformIO configuration 
├── README.md           # Project documentation
└── LICENSE             # Project license
```

---

## **Commands**

| Command   | Description                                   |
|-----------|-----------------------------------------------|
| `@RESET`  | Resets the device and sends initialization data. |
| `vQ`      | Returns the device identification string.     |
| `CB`      | Enables binary data transmission.            |
| `MSSV`    | Enables automatic packet sending.            |
| `k`       | Processes key-related events (optional).     |

---

## **Known Issues**

- Ensure `spacenavd` is configured to recognize serial devices.
- If the device is not detected, check the baud rate and USB permissions.

---

## **Contributing**

Contributions are welcome! Please fork the repository and submit a pull request for any enhancements or bug fixes.

---

## **License**

This project is licensed under the MIT License. See the `LICENSE` file for details.

