@var0
M=0
          // D = 27

(loop)
@27 
D=A
@var0
D=D-M         // D = 27 - var0
@END
D;JEQ         // if equal, exit

@var0
M=M+1         // else increment and loop
@loop
0;JMP

(END)
// ... continue
