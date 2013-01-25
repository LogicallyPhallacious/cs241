lis $4
	.word 4
lis $7
	.word 1
lw $3, 0($1)



LOOP:
	beq $2, $0, EXIT

;; in-/decrement counter
	add $1, $1, $4
	sub $2, $2, $7

	lw $5, 0($1)
	slt $6, $3, $5
	beq $6, $0, LOOP
	add $3, $5, $0
	beq $0, $0, LOOP 

EXIT: 
	jr $31
