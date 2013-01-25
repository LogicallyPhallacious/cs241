.word 0x10000002 ; cookie beq $0, $0, 2
.word 0x00000040 ; endmodule
.word 0x0000001c ; endcode
.word 0x00000814 ; lis $1
.word 0x00000018 ; .word proc
.word 0x00200009 ; jalr $1
.word 0x03e00008 ; proc: jr $31
.word 0x00000001 ; relocation entry
.word 0x00000010 ; location
.word 0x00000005 ; external symbol definition entry
.word 0x00000018 ; location
.word 0x00000004 ; length
.word 0x00000070 ; 'p'
.word 0x00000072 ; 'r'
.word 0x0000006f ; 'o'
.word 0x00000063 ; 'c'
