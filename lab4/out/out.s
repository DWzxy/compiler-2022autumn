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

main:
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -204
  li $t1, 0
  move $t0, $t1
  li $t3, 1
  move $t2, $t3
  move $t4, $t1
  sw $t0, -4($fp)
  sw $t2, -8($fp)
  sw $t4, -12($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  j read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t0, $v0
  move $t1, $t0
LABEL3:
  lw $t2, -12($fp)
  sw $t0, -16($fp)
  sw $t1, -20($fp)
  sw $t2, -12($fp)
  beq $t2, $t1, LABEL2
  lw $t1, -4($fp)
  lw $t2, -8($fp)
  add $t0, $t1, $t2
  move $t3, $t0
  sw $t0, -24($fp)
  sw $t1, -4($fp)
  sw $t2, -8($fp)
  sw $t3, -28($fp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  j write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $t0, -8($fp)
  move $t0, $v0
  lw $t1, -4($fp)
  move $t1, $t0
  lw $t2, -28($fp)
  move $t0, $t2
  lw $t4, -12($fp)
  li $t5, 1
  addi $t3, $t4, 1
  move $t4, $t3
  sw $t0, -8($fp)
  sw $t1, -4($fp)
  sw $t2, -28($fp)
  sw $t3, -32($fp)
  sw $t4, -12($fp)
  j LABEL3
LABEL2:
  li $t0, 0
  move $v0, $t0
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra