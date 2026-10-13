# Smart AC Control System

An Embedded C++ IoT project built with **PlatformIO** for controlling and monitoring AC units. This firmware includes state management, synchronization guards, configuration settings, and automated development tools for simulation using **Wokwi**.

---

## 📁 Repository Structure

```
.
├── include/
│   ├── config.hpp         # Hardware pins & global constants
│   ├── state.hpp          # Application state definitions
│   ├── sync_guard.hpp     # Thread/sync locks for state management
│   ├── enum.hpp           # Enum related
│   └── service            # AC Controller & Receiver service
├── src/
│   ├── main.cpp           # Main application entry point
│   ├── state.cpp          # State implementation details
│   ├── enum.cpp           # Enum implementation
│   └── ac_service.cpp     # service/ implementation
├── scripts/
│   └── load_env.py        # Environment loader script for build configurations
├── lib/                   # Custom libraries & local modules
├── test/                  # Unit and integration tests
├── .env                   # Local environment variables
├── diagram.json           # Wokwi circuit diagram design
├── wokwi.toml             # Wokwi simulator setup
└── platformio.ini         # PlatformIO configuration & dependencies
```

---

## ⚡ Features

- **AC Infrared Service:** Sends and decodes IR signals to control power, temperature, and modes.
- **State Guard & Thread Safety:** Utilizes custom sync guards (`sync_guard.hpp`) to maintain state integrity across async operations.
- **Wokwi Simulation Ready:** Integrated circuit diagrams (`diagram.json`) and Wokwi runner configurations (`wokwi.toml`).
- **Dynamic Config Loading:** Auto-loads variables from `.env` using Python pre-build scripts.
- **Double State:** State separated by ESP32 and factory's remote (updated via IR Receiver)

---

## 🚀 Getting Started

### Prerequisites

- [VS Code](https://code.visualstudio.com/) + [PlatformIO IDE Plugin](https://platformio.org/platformio-ide)
- [Wokwi Simulator Extension](https://marketplace.visualstudio.com/items?itemName=Wokwi.wokwi-vscode) (optional, for local hardware-free simulation)
- Python 3.x (for running helper scripts)

### Installation & Build

1. **Clone the repository:**

   ```bash
   git clone https://github.com/chesta132/esp32-ac-midea.git
   cd esp32-ac-midea
   ```

2. **Configure Environment:**
   Copy `.env` template or modify existing parameters:

   ```bash
   cp .env.example .env  # Adjust variables inside as needed
   ```

3. **Build the Project:**

   ```bash
   pio run
   ```

4. **Upload to Board:**
   Connect your microcontroller via USB and run:
   ```bash
   pio run -t upload
   ```

---

## 💻 Hardware Setup

Default pin assignments are defined in `include/config.hpp`.

| Component          | Pin      | Description                              |
| :----------------- | :------- | :--------------------------------------- |
| **IR Transmitter** | `GPIO 4` | Sends IR raw codes to the AC unit        |
| **IR Receiver**    | `GPIO 5` | Receives IR raw from AC factory's remote |

_Modify `include/config.hpp` if your hardware setup differs._

---

## 🧪 Simulation

### Running in Wokwi Simulator

1. Compile the code with PlatformIO.
2. Open the project in VS Code with the Wokwi extension installed.
3. Open `diagram.json` or press `F1` and select **Wokwi: Start Simulator**.
4. The board will automatically execute using settings from `wokwi.toml`.
