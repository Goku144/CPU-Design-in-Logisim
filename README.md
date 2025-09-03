## CPU-Design-in-Logisim
## 📖 Overview

This project is a complete working computer built in Logisim, inspired by the Nand2Tetris
 course.
The goal was to go beyond theory and actually design, simulate, and run programs on a Hack-style computer built entirely from logic gates.

***

## ✨ Features

Custom CPU built from the ground up:

Primary gates → combinational circuits → sequential logic → full CPU.

Instruction Set Architecture (ISA) based on the Hack computer.

Memory-mapped I/O:

Screen and Keyboard follow the Hack specifications.

Programs can draw pixels and capture keystrokes.

Custom Logisim version:

Modified with the help of AI tools to better support Hack-specific features.

Allows smooth integration of screen (0x4000+) and keyboard (0x6000) devices.

Compatible with Hack assembly programs (e.g., Pong, rectangles, etc.).

***

## ⚙️ Setup Notes

When you open computer.circ in Logisim, the tool will ask to include Hardware\CPU\CPU.circ as a library.

If prompted, just accept.

If it doesn’t ask automatically, you can add it manually:

In Logisim, go to Project → Load Library → Logisim Library…

Select Hardware\CPU\CPU.circ.

Now the CPU will be available inside computer.circ.

***

## 🚀 Getting Started

Clone the repository:

git clone https://github.com/Goku144/CPU-Design-in-Logisim.git


Open computer.circ in Logisim (custom version included).

Load a program into ROM (for example, Pong.hex or your own .hack file).

Run the simulation and watch your computer in action.

***

## 🖥️ Example Programs

Draw rectangles or shapes directly on the Hack screen.

Run classical Nand2Tetris demos like Pong.

Experiment with your own assembly code.

***

## 🙌 Inspiration

This work was inspired by the Nand2Tetris course.
I extended the concepts by implementing them in a customized version of Logisim, with AI-assisted modifications to support Hack’s memory-mapped screen and keyboard directly.

To build the computer, I followed a layered abstraction approach:

At the very bottom level, I experimented with constructing NAND gates from PMOS and NMOS transistors, to understand the hardware foundations of digital logic.

On top of that, I built higher-order components (combinational logic, sequential elements, ALU, etc.) strictly from NAND gates, verifying correctness at each stage.

Once these components worked as expected, I gradually abstracted them into higher-level blocks, and in later stages replaced them with Logisim’s built-in chips for the sake of performance and usability.

This way, the CPU is both a faithful low-level design (starting all the way from transistor logic) and a practical tool that can run Hack assembly programs with visible graphics and keyboard input.