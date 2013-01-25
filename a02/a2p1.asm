slt $4, $1, $2
bne $4, $0, LESS
add $3, $1, $0
beq $0, $0, EXIT
LESS: 
	add $3, $2, $0
EXIT: 
	jr $31
