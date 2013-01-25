; Calling a procedure

   sw $31, -4($30)      ; save $31 on stack
   lis $31
   .word 4
   sub $30, $30, $31

                        ; call sumOneToN(13)
   lis $1
   .word 13
   lis $4               ; address of procedure
   .word sumOneToN
   jalr $4              ; put return address in $31
                        ; replace PC with contents of $1

   lis $31              ; restore $31 from stack
   .word 4
   add $30, $30, $31
   lw $31, -4($30)

   jr $31               ; return to OS
   

; sum the integers from 1 to N
; input: $1 is N
; output:$3 is the sum
; all registers except $3 must have initial value

; We will change $1 and $2.  Save and restore them.
; Need a label so we can call the procedure.

sumOneToN:
   sw $1, -4($30)       ; save $1 on the stack
   sw $2, -8($30)       ; save $2 on the stack
   lis $2               ; reset the stack pointer
   .word 8
   sub $30, $30, $2

   add $3, $0, $0       ; zero accumulator

beginLoop:
   add $3, $3, $1       ; add $1 to $3
   lis $2               ; decrement $2
   .word -1
   add $1, $1, $2
   bne $1, $0, beginLoop

   lis $2               ; reset the stack pointer
   .word 8
   add $30, $30, $2
   lw $1, -4($30)       ; restore $1 from stack
   lw $2, -8($30)       ; restore $2 from stack

   jr $31               ; return from sumOneToN
