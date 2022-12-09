.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text


read:
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  li $v0, 5
  syscall
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra

write:
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra

main:
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -216
  li $t1, 0
  move $t0, $t1
  li $t3, 1
  move $t2, $t3
  move $t4, $t1
  sw $t0, -4($fp)
  sw $t2, -8($fp)
  sw $t4, -12($fp)
  jal read
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
  move $a0, $t2
  jal write
  move $t1, $t2
  move $t2, $t3
  lw $t5, -12($fp)
  li $t6, 1
  addi $t4, $t5, 1
  move $t5, $t4
  sw $t0, -24($fp)
  sw $t1, -4($fp)
  sw $t2, -8($fp)
  sw $t3, -28($fp)
  sw $t4, -32($fp)
  sw $t5, -12($fp)
  j LABEL3
LABEL2:
  li $t0, 123456
  move $a0, $t0
  jal write
  li $t1, 0
  move $v0, $t1
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra
