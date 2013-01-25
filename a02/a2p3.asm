lis $4 
	.word -1

add $3, $4 , $0

beq $2, $0, EXIT

add $5, $2, $4
add $5, $5, $5
add $5, $5, $5

add $5, $1, $5

lw $3, 0($5)	


EXIT:
	jr $31
