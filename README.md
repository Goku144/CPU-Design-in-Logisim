Here’s a good starting point for your **README.md** that explains your project clearly and credits your inspiration from Nand2Tetris. You can copy and adapt this directly into your repo:

---

# CPU-Design-in-Logisim

## 📖 Overview

This project is a complete **working computer built in Logisim**, inspired by the [Nand2Tetris](https://www.nand2tetris.org/) course.
The goal was to go beyond theory and actually design, simulate, and run programs on a Hack-style computer built entirely from logic gates.

## ✨ Features

* **Custom CPU** built from the ground up:

  * Primary gates → combinational circuits → sequential logic → full CPU.
* **Instruction Set Architecture (ISA)** based on the **Hack computer**.
* **Memory-mapped I/O**:

  * Screen and Keyboard follow the Hack specifications.
  * Programs can draw pixels and capture keystrokes.
* **Custom Logisim version**:

  * Modified with the help of AI tools to better support Hack-specific features.
  * Allows smooth integration of screen (0x4000+) and keyboard (0x6000) devices.
* Compatible with **Hack assembly programs** (e.g., Pong, rectangles, etc.).

## 🚀 Getting Started

1. Clone the repository:

   ```bash
   git clone https://github.com/<your-username>/CPU-Design-in-Logisim.git
   ```
2. Open the project in **Logisim (custom version included)**.
3. Load a program into ROM (for example, `Pong.hex` or your own `.hack` file).
4. Run the simulation and watch your computer in action.

## 🖥️ Example Programs

* Draw rectangles or shapes directly on the Hack screen.
* Run classical Nand2Tetris demos like Pong.
* Experiment with your own assembly code.

## 🙌 Inspiration

This work was **inspired by the Nand2Tetris course**.
I wanted to extend the concepts by implementing them in a **customized version of Logisim**, with AI-assisted modifications to support Hack’s memory-mapped **screen** and **keyboard** directly.

This way, the CPU isn’t just a schematic — it can actually run Hack programs with visible graphics and user input.

