// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/6/rect/Rect.asm

// Draws a rectangle at the top-left corner of the screen.
// The rectangle is 16 pixels wide and R0 pixels high.
// Usage: Before executing, put a value in R0.

   // If (R0 <= 0) goto END else n = R0
   @0
   D=M
   @27
   D;JLE
   @50
   M=D
   // addr = base address of first screen row
   @678
   Dm=A
   @890
   M=D
   // RAM[addr] = -1
   @890
   A=M
   M=-1
   // addr = base address of next screen row
   @890
   D=M
   @32
   D=D+A
   @890
   M=D
   // decrements n and loops
   @50
   MD=M-1
   @12
   D;JGT
   @29
   0;JMP