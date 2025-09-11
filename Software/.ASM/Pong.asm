// Wait for 'j' key (ASCII 106). When pressed, draw 8x8 'j' at row 10, col 0.

(LOOP)
    @24576        // KBD
    D=M
    @LOOP
    D;JEQ         // nothing pressed -> keep listening

    @106
    D=D-A
    @LOOP
    D;JNE         // not 'j' -> keep listening

    // R3 = screen pointer = SCREEN + (row*32) + wordcol
    // row=10, wordcol=0  => 16384 + 10*32 = 16704
    @16704
    D=A
    @3
    M=D

    // Row 0: 0x08
    @8
    D=A
    @3
    A=M
    M=D
    @32
    D=A
    @3
    M=D+M        // ptr += 32 (next screen row)

    // Row 1: 0x00
    @0
    D=A
    @3
    A=M
    M=D
    @32
    D=A
    @3
    M=D+M

    // Row 2: 0x08
    @8
    D=A
    @3
    A=M
    M=D
    @32
    D=A
    @3
    M=D+M

    // Row 3: 0x08
    @8
    D=A
    @3
    A=M
    M=D
    @32
    D=A
    @3
    M=D+M

    // Row 4: 0x08
    @8
    D=A
    @3
    A=M
    M=D
    @32
    D=A
    @3
    M=D+M

    // Row 5: 0x08
    @8
    D=A
    @3
    A=M
    M=D
    @32
    D=A
    @3
    M=D+M

    // Row 6: 0x38
    @56
    D=A
    @3
    A=M
    M=D
    @32
    D=A
    @3
    M=D+M

    // Row 7: 0x30
    @48
    D=A
    @3
    A=M
    M=D

// Wait for key release so we don't redraw repeatedly on the same press
(WAIT_RELEASE)
    @24576
    D=M
    @WAIT_RELEASE
    D;JNE

    @LOOP
    0;JMP
