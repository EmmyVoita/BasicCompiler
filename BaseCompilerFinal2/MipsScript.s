.text
.globl main
main:
	li $t1, 18 
	move $a0, $t1 
	li $v0, 1 
	syscall 
	li $t4, 48 
	move $a0, $t4 
	li $v0, 1 
	syscall 
	li $v0, 10
	syscall