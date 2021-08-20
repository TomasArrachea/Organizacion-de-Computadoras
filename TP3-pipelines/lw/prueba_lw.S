.data
	cero: .word	0
	uno: .word	1
	dos: .word	2

.text
	li $t0, 4
	li $t1, 4
	lw $t2, $t1, $t0
	add $t3, $t1, $t2