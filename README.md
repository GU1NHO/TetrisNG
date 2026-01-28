# Tetris NG (Next Generation)

**Tetris NG** is a modern and robust recreation of the classic puzzle game, developed in C++ using the **Raylib** library. The project focuses on modularity, performance, and multiplayer features, supporting local matches and online play via direct LAN/IP.

![Tetris NG Banner](Assets/background.png)

## 🎮 Features

* **Classic Singleplayer:** Play the traditional mode with a scoring system and progressive levels.
* **Local Multiplayer (Dual Window):** Two players compete on the same computer with a split screen.
* **Online Multiplayer:** Connect via IP (LAN or VPN) to play against friends remotely.
* **Modern Mechanics:**
  * **Ghost Piece:** Visualizes where the piece will land for greater precision.
  * **DAS (Delayed Auto Shift):** Smooth movement system, allowing for quick piece sliding.
  * **Seed Synchronization (RNG):** Ensures both players receive the same sequence of pieces in online mode.

---

## 🕹️ Controls

The game supports Keyboard and Mouse for menu navigation.

### Main Menu
* **Arrows / W, S:** Select Option
* **Enter / Left Click:** Confirm
* **ESC:** Back / Exit

### In-Game

| Action | Singleplayer | Dual P1 | Dual P2 |
| :--- | :---: | :---: | :---: |
| **Move Left** | Left Arrow / A | A | Left Arrow |
| **Move Right** | Right Arrow / D | D | Right Arrow |
| **Soft Drop** | Down Arrow / S | S | Down Arrow |
| **Rotate** | Up Arrow / W | W | Up Arrow |
| **Pause** | P | P | P |
| **Restart Game** | R | R | R |
| **Back to Menu** | M | M | M |

> **Note:** In Online mode, each player uses the standard controls (Arrows or WASD) on their own computer, acting as a local "Singleplayer", but synchronized via the network.

---

## 📦 Installation and Execution

This package contains the compiled game, ready to run on **Windows (64-bit)** systems.

### How to Run
1. **Extract the .zip file** completely to a folder of your choice.
   * ⚠️ **Important:** Do not try to run the game directly from inside the zip file. This will prevent images and fonts from loading.
2. Ensure the following file structure is maintained in the same folder as the executable:
   * `Assets/` folder (contains images)
   * `Font/` folder (contains fonts)
   * `.dll` files (`raylib.dll`, `libenet.dll` or `enet.dll`)
3. Run the **`Tetris.exe`** file.

### Playing Online
The game uses a direct IP connection (port **1234**).
1. **Host (Server):** Select "Host Game". The game will wait for a connection and display your local IP on the screen.
2. **Client (Player 2):** Select "Join Game" in the menu. Enter the Host's IP (numbers and dots) and press Enter or click CONNECT.
3. **Network Note:** If you are on different networks, use a VPN (like Hamachi/Radmin) or ensure port 1234 is forwarded on the Host's router.

---

## 📂 Code Structure

The project was structured following Object-Oriented principles, separating game logic, rendering, and networking.

### 1. Core
* **`main.cpp`:** Entry point. Manages the main Loop, state machine (Menu -> Game -> Game Over), and window initialization.
* **`game.cpp / .hpp`:** The main class. Controls core logic, scoring, levels, and orchestrates grid and block rendering.

### 2. Tetris Logic
* **`grid.cpp / .hpp`:** Represents the board matrix (20x10). Manages collisions, boundaries, and clearing of full lines.
* **`block.cpp / .hpp`:** Base class for pieces (Tetrominoes). Manages rotation, movement, and individual drawing.
* **`blocks.cpp`:** Defines specific shapes (I, J, L, O, S, T, Z) inheriting from `Block`.
* **`position.hpp`:** Helper structure for coordinates (row, column).
* **`colors.cpp / .hpp`:** Centralized color palette management.

### 3. Systems and Networking
* **`NetworkManager.cpp / .hpp`:** Manages ENet (UDP) connection. Handles Input packets, Seed Synchronization (RNG), Gravity Ticks, and Pause/Restart requests.
* **`input_handler.cpp / .hpp`:** Implements DAS (Delayed Auto Shift) to ensure precise movement.
* **`ui_manager.cpp / .hpp`:** Static classes to draw buttons and interface overlays (Pause, Game Over) in a standardized way.
* **`menu.cpp / .hpp`:** Logic for navigation and rendering of the Main Menu.
* **`utils.cpp / .hpp`:** Utility timing functions to control gravity and frame events.

---

## 🛠️ Dependencies

* **[Raylib 5.0](https://www.raylib.com/):** Library used for graphics rendering, window management, and audio.
* **[ENet 1.3.18](http://enet.bespin.org/):** Network library (reliable UDP) used for multiplayer communication.
* **C++ Compiler (MinGW-w64 GCC 13.2.0):** Required if you wish to recompile the source code.

---

## 📄 Credits

Developed as an academic Object-Oriented Programming project.

* **Authors:** João Victor Ferro Amorosini and Thiago Rosa Carvalho
* **Institution:** ENSTA Paris
* **Year:** 2026