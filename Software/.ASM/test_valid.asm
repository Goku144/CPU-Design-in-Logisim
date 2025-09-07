// --- test_valid.asmhack — comprehensive Hack assembly test ---

// Touch a few predefined symbols
@SP
M=0
@LCL
M=1

@2
D=A
@ARG
M=D

@3
D=A
@THIS
M=D

@4
D=A
@THAT
M=D

// numeric A-instruction + new variable
@123
D=A
@tmp               // new variable; should get first free address (typically 16)
M=D

// sum R0 + R1 -> sum
@R0
D=M
@R1
D=D+M
@sum              // new variable symbol
M=D

// set a pointer to SCREEN base
@SCREEN           // 16384
D=A
@ptr
M=D

// a few dest variants
@R2
MD=M-1
@R3
AD=M+1
@R4
AM=D

// main loop: draws 0s until a key is pressed, then draws -1s
(LOOP$main)       // '$' allowed in symbols
@KBD              // 24576
D=M
@DRAW
D;JNE             // if key pressed jump to DRAW

// idle path: write 0, advance pointer, repeat
@ptr
A=M
M=0
@ptr
M=M+1
@LOOP$main
0;JMP

(DRAW)            // draw path
@ptr
A=M
M=-1
@ptr
M=M+1
@LOOP$main
0;JMP

(Sys.init)        // '.' allowed in symbols
@sum
D=M
@result:val       // ':' allowed inside symbol
M=D

(_halt)           // '_' allowed to start a symbol
@_halt
0;JMP
