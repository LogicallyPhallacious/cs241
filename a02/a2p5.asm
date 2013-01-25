lis $4
	.word 4
lis $7
	.word 1
lis $8 
	.word 0x40
lis $9
	.word 0xffff000c
lis $10 
	.word 0x20


LOOP:
	beq $2, $0, EXIT

	lw $5, 0($1)

	bne $5, $0, NOTSPACE
	add $5, $10, $0
	sw $5, 0($9)
	beq $0, $0, COUNTER

	NOTSPACE:
	add $5, $5, $8
	sw $5, 0($9)

	COUNTER: ;; in-/decrement counter
        add $1, $1, $4
        sub $2, $2, $7
	beq $0, $0, LOOP
	
EXIT: 
	jr $31
