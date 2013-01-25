add $5, $0, $0
add $3, $0, $0
add $2, $1, $0

heightSearch:
sw $31, -4($30)
sw $1, -8($30)
sw $4, -12($30)
sw $7, -16($30)
sw $5, -20($30)
lis $7
        .word -20
add $30, $30, $7

;; increment counter
lis $7
        .word 1
add $5, $5, $7

;;;;;;;;;;;;; left node ;;;;;;;;;;;;;;;;;
;; check the existence of children
lis $7
	.word 4
add $1, $1, $7
lw $4, 0($1)


;; no child
lis $7
	.word -1
beq $4, $7, heightRight

; has children
lw $1, 12($30)


lis $7
	.word 4
mult $4, $7
mflo $4
add $1, $2, $4  ;; move to the index of a child
lis $7
	.word heightSearch
jalr $7
;;;;;;;;;;;;;;;;;; right node ;;;;;;;;;;;;;;;;;;

heightRight:
lw $1, 12($30)

;; check the existence of children
lis $7
        .word 8
add $1, $1, $7
lw $4, 0($1)


;; no child
lis $7
        .word -1
beq $4, $7, heightSearchEnd

;; has children
lw $1, 12($30)


lis $7
        .word 4
mult $4, $7
mflo $4
add $1, $2, $4  ;; move to the index of a child
lis $7
        .word heightSearch
jalr $7

heightSearchEnd:
slt $7, $3, $5
beq $7, $0, heightExit
add $3, $5, $0

;heightSearchEnd:
;slt $7, $3, $5
;beq $7, $0, heightExit
;add $3, $5, $0

heightExit:
lis $7
        .word 20
add $30, $30, $7
lw $31, -4($30)
lw $1, -8($30)
lw $4, -12($30)
lw $7, -16($30)
lw $5, -20($30)

exit:
jr $31
