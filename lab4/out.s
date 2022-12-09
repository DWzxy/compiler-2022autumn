.data
_prompt: .asciiz "Enter an integer:  "
_ret: .asciiz "\n"
.globl main
.text


read:
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  li $v0, 4
  la $a0, _prompt
  syscall
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

func_a:
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -72
  lw $t0, 8($fp)
  lw $t1, 12($fp)
  div $t0, $t1
  mflo $t2
  mul $t3, $t2, $t1
  sub $t4, $t0, $t3
  move $v0, $t4
  sw $t0, -4($fp)
  sw $t1, -8($fp)
  sw $t2, -12($fp)
  sw $t3, -16($fp)
  sw $t4, -20($fp)
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra

func_b:
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -192
  lw $t0, 8($fp)
  li $t2, 2
  move $t1, $t2
  sw $t0, -4($fp)
  sw $t1, -8($fp)
LABEL3:
  lw $t0, -8($fp)
  lw $t1, -4($fp)
  bge $t0, $t1, LABEL2
  sw $t0, -4($sp)
  sw $t1, -8($sp)
  addi $sp, $sp, -8
  sw $t0, -8($fp)
  sw $t1, -4($fp)
  jal func_a
  move $t0, $v0
  move $t1, $t0
  sw $t0, -12($fp)
  sw $t1, -16($fp)
  lw $t0, -16($fp)
  li $t1, 0
  bne $t0, $t1, LABEL5
  move $v0, $t1
  sw $t0, -16($fp)
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra
LABEL5:
  lw $t1, -8($fp)
  li $t2, 1
  addi $t0, $t1, 1
  move $t1, $t0
  sw $t0, -20($fp)
  sw $t1, -8($fp)
  j LABEL3
LABEL2:
  li $t0, 1
  move $v0, $t0
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra

func_c:
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -216
  lw $t0, 8($fp)
  li $t2, 2
  move $t1, $t2
  sw $t0, -4($fp)
  sw $t1, -8($fp)
LABEL8:
  lw $t1, -8($fp)
  mul $t0, $t1, $t1
  sw $t0, -12($fp)
  sw $t1, -8($fp)
  lw $t0, -12($fp)
  lw $t1, -4($fp)
  bgt $t0, $t1, LABEL7
  lw $t2, -8($fp)
  sw $t2, -4($sp)
  addi $sp, $sp, -4
  sw $t0, -12($fp)
  sw $t1, -4($fp)
  sw $t2, -8($fp)
  jal func_b
  move $t0, $v0
  sw $t0, -16($fp)
  lw $t0, -16($fp)
  li $t1, 0
  beq $t0, $t1, LABEL10
  lw $t3, -8($fp)
  mul $t2, $t3, $t3
  sw $t0, -16($fp)
  sw $t2, -20($fp)
  sw $t3, -8($fp)
  lw $t0, -20($fp)
  lw $t1, -4($fp)
  bne $t0, $t1, LABEL12
  li $t2, 1
  move $v0, $t2
  sw $t0, -20($fp)
  sw $t1, -4($fp)
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra
LABEL12:
LABEL10:
  lw $t1, -8($fp)
  li $t2, 1
  addi $t0, $t1, 1
  move $t1, $t0
  sw $t0, -24($fp)
  sw $t1, -8($fp)
  j LABEL8
LABEL7:
  li $t0, 0
  move $v0, $t0
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
  addi $sp, $sp, -332
  li $t1, 2
  move $t0, $t1
  li $t3, 0
  move $t2, $t3
  addi $t5, $fp, -8
  add $t4, $t5, $t2
  sw $t0, -12($fp)
  sw $t2, -16($fp)
  sw $t4, -20($fp)
  jal read
  move $t0, $v0
  lw $t2, -20($fp)
  sw $t0, 0($t2)
  li $t4, 4
  move $t3, $t4
  add $t6, $t5, $t3
  sw $t0, -28($fp)
  sw $t1, -32($fp)
  sw $t2, -20($fp)
  sw $t3, -36($fp)
  sw $t6, -40($fp)
  jal read
  move $t0, $v0
  lw $t2, -40($fp)
  sw $t0, 0($t2)
  li $t4, 0
  move $t3, $t4
  sw $t0, -44($fp)
  sw $t1, -48($fp)
  sw $t2, -40($fp)
  sw $t3, -52($fp)
LABEL15:
  lw $t0, -52($fp)
  lw $t1, -12($fp)
  bge $t0, $t1, LABEL14
  li $t3, 4
  mul $t2, $t0, $t3
  add $t4, $t5, $t2
  lw $t4, 0($t4)
  sw $t4, -4($sp)
  addi $sp, $sp, -4
  sw $t0, -52($fp)
  sw $t1, -12($fp)
  sw $t2, -56($fp)
  sw $t4, -60($fp)
  sw $t6, -64($fp)
  jal func_c
  move $t0, $v0
  sw $t0, -68($fp)
  lw $t0, -68($fp)
  li $t1, 0
  beq $t0, $t1, LABEL17
  lw $t3, -52($fp)
  li $t4, 4
  mul $t2, $t3, $t4
  add $t6, $t5, $t2
  lw $t6, 0($t6)
  move $a0, $t6
  jal write
  sw $t0, -68($fp)
  sw $t2, -72($fp)
  sw $t3, -52($fp)
  sw $t6, -76($fp)
  sw $t7, -80($fp)
LABEL17:
  lw $t1, -52($fp)
  li $t2, 1
  addi $t0, $t1, 1
  move $t1, $t0
  sw $t0, -84($fp)
  sw $t1, -52($fp)
  j LABEL15
LABEL14:
  li $t0, 0
  move $v0, $t0
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra
