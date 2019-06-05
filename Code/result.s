.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text

read:
	li $v0, 4
	la $a0, _prompt
	syscall
	li $v0, 5
	syscall
	jr $ra

write:
	li $v0, 1
	syscall
	li $v0, 4
	la $a0, _ret
	syscall
	move $v0, $0
	jr $ra

fact:
	subu $sp, $sp, 4
	sw $fp, 0($sp)
	move $fp, $sp
	subu $sp, $sp, 100
	sw $a0, -4($fp)
	lw $t0, -4($fp)
	bne $t0, 1, labellAbEl2
labellAbEl1:
	lw $t1, -4($fp)
	move $v0, $t1
	addi $sp, $sp, 100
	lw $fp, 0($sp)
	addi $sp, $sp, 4
	jr $ra
	j labellAbEl3
labellAbEl2:
	lw $t3, -4($fp)
	addi $t2, $t3, -1
	sw $t2, -8($fp)
	lw $a0, -8($fp)
	subu $sp, $sp, 4
	sw $ra, 0($sp)
	jal fact
	move $t4, $v0
	sw $t4, -12($fp)
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $t6, -4($fp)
	lw $t7, -12($fp)
	mul $t5, $t6, $t7
	sw $t5, -16($fp)
	lw $s0, -16($fp)
	move $v0, $s0
	addi $sp, $sp, 100
	lw $fp, 0($sp)
	addi $sp, $sp, 4
	jr $ra
labellAbEl3:
main:
	subu $sp, $sp, 4
	sw $fp, 0($sp)
	move $fp, $sp
	subu $sp, $sp, 100
	subu $sp, $sp, 4
	sw $ra, 0($sp)
	jal read
	move $s1, $v0
	sw $s1, -4($fp)
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $s3, -4($fp)
	move $s2, $s3
	sw $s2, -8($fp)
	lw $s4, -8($fp)
	ble $s4, 1, labellAbEl5
labellAbEl4:
	lw $a0, -8($fp)
	subu $sp, $sp, 4
	sw $ra, 0($sp)
	jal fact
	move $s5, $v0
	sw $s5, -12($fp)
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	lw $s7, -12($fp)
	move $s6, $s7
	sw $s6, -16($fp)
	j labellAbEl6
labellAbEl5:
	lw $t0, -16($fp)
	li $t0, 1
	sw $t0, -16($fp)
labellAbEl6:
	subu $sp, $sp, 4
	sw $ra, 0($sp)
	lw $t1, -16($fp)
	move $a0, $t1
	jal write
	sw $t1, -16($fp)
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	move $v0, $0
	addi $sp, $sp, 100
	lw $fp, 0($sp)
	addi $sp, $sp, 4
	jr $ra
