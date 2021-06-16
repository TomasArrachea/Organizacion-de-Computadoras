	.text
	.align 2
	.globl proximo
	.ent proximo

proximo:	
	subu $sp, $sp, 24 # 2 words (SRA) + 4 words (LTA) + 0 words (ABA) = 24 bytes

	# SRA
	sw $gp, 20($sp)
	sw $fp, 16($sp)
	move $fp, $sp
	#guardo los argumentos en el stack del callee
		#$a0 = a
		#$a1 = i
		#$a2 = j
		#$a3 = regla
	sw $a0, 24($fp)
	sw $a1, 28($fp) 
	sw $a2, 32($fp) 
	sw $a3, 36($fp) 

	# LTA 
		#$t0 = posicion
		#$t1 = digito1
		#$t2 = digito2
		#$t3 = digito3
		#t$4 = n
	lw $t4, 40($fp)		#carga n desde el stack del callee
	
#programa:
	sll $a1, $a1, 2		# i * 4, son punteros de 4 bytes
	add $a0, $a0, $a1 	# $a0 = a + i
	lw $a0, 0($a0)		
	add $a0, $a0, $a2	# $a0 = a[i] + j

#digito2 = estados[i][j]
	lb $t2, 0($a0)
	
##if (j == 0)
	beq $zero, $a2, lim_inferior
##if (j == n-1)
	addi $t4, $t4, -1
	beq $t4, $a2, lim_superior
##else 
	#digito1 = a[i][j-1]
	#digito3 = a[i][j+1]
	lb $t1, -1($a0)
	lb $t3, 1($a0)
	j exit
	
lim_inferior:
	#digito1 = a[i][n-1]
	#digito3 = a[i][j+1]
	lb $t3, 1($a0)
	addu $a0, $a0, $t4
	lb $t1, ($a0)
	j exit
lim_superior:
	#digito1 = a[i][j-1]
	#digito3 = a[i][0]
	lb $t1, -1($a0)
	subu $a0, $a0, $a2
	lb $t3, ($a0)
	j exit

exit:

#digito1 = digito1 << 2
#digito2 = digito2 << 1
	sll $t1, $t1, 2
	sll $t2, $t2, 1

#posicion = digito1 << 2 | digito2 << 1 | digito3 << 0
	or $t0, $t1, $t2
	or $t0, $t0, $t3

#regla >> posicion
	srlv $a3, $a3, $t0

#$v0 = regla & 1.
	andi $v0, $a3, 1

#liberar Stack
    lw $fp, 16($sp)
    lw $gp, 20($sp)
    addiu $sp, $sp, 24

	jr $ra
.end proximo