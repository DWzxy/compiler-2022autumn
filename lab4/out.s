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

changeCal:
  addi $sp, $sp, -8
  sw $ra, 4($sp)
  sw $fp, 0($sp)
  move $fp, $sp
  addi $sp, $sp, -432
  lw $t0, 8($fp)
  li $t2, 345
  addi $t1, $t0, 345
  addi $t3, $t1, -345
  mul $t4, $t0, $t5
  add $t6, $t3, $t4
  mul $t7, $t0, $t5
  sub $s0, $t6, $t7
  mul $s1, $t0, $t0
  add $s2, $s0, $s1
  mul $s3, $t0, $t0
  sub $s4, $s2, $s3
  move $t5, $s4
  li $s6, 2
  mul $s5, $t5, $s6
  li $t8, 3
  mul $s7, $t5, $t8
  sub $t9, $s5, $s7
  sw $t0, -4($fp)
  mul $t0, $t5, $s6
  sw $t1, -8($fp)
  add $t1, $t9, $t0
  sw $t3, -12($fp)
  li $t3, 34
  div $t3, $s6
  mflo $t2
  sw $t4, -16($fp)
  add $t4, $t1, $t2
  sw $t5, -20($fp)
  sw $t6, -24($fp)
  li $t6, 10
  div $t6, $t8
  mflo $t5
  sw $t7, -28($fp)
  add $t7, $t4, $t5
  sw $s0, -32($fp)
  sw $s1, -36($fp)
  li $s1, 20
  sw $s2, -40($fp)
  li $s2, 1
  div $s1, $s2
  mflo $s0
  sw $s3, -44($fp)
  sub $s3, $t7, $s0
  sw $s4, -48($fp)
  lw $s4, -20($fp)
  move $s4, $s3
  sw $s5, -52($fp)
  li $s6, 7
  sw $s7, -56($fp)
  li $s7, 6
  addi $s5, $s7, 7
  sw $t9, -60($fp)
  li $t9, 11
  mul $t8, $t9, $s2
  sw $t0, -64($fp)
  sub $t0, $s5, $t8
  move $s4, $t0
  sw $t1, -68($fp)
  sw $t2, -72($fp)
  li $t2, 4
  li $t3, 5
  mul $t1, $t2, $t3
  sw $t4, -76($fp)
  sw $t5, -80($fp)
  lw $t5, -4($fp)
  add $t4, $t5, $t1
  sw $t7, -84($fp)
  li $t7, 10
  sw $s0, -88($fp)
  li $s0, 2
  mul $t6, $t7, $s0
  sub $s1, $t4, $t6
  div $t3, $s7
  mflo $s2
  sw $s3, -92($fp)
  sub $s3, $s1, $s2
  move $s4, $s3
  move $v0, $s4
  sw $t0, -104($fp)
  sw $t1, -108($fp)
  sw $t4, -112($fp)
  sw $t5, -4($fp)
  sw $t6, -116($fp)
  sw $s1, -120($fp)
  sw $s2, -124($fp)
  sw $s3, -128($fp)
  sw $s4, -20($fp)
  sw $s5, -96($fp)
  sw $t8, -100($fp)
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
  addi $sp, $sp, -1380
  li $t1, 7
  li $t2, 6
  addi $t0, $t2, 7
  li $t4, 11
  li $t5, 1
  mul $t3, $t4, $t5
  sub $t6, $t0, $t3
  move $t7, $t6
  li $s1, 8
  li $s2, 4
  addi $s0, $s2, -8
  move $s3, $s0
  addi $s4, $s2, 4
  li $s6, 5
  mul $s5, $s6, $t2
  li $t8, 3
  div $s5, $t8
  mflo $s7
  add $t9, $s4, $s7
  sw $t0, -4($fp)
  addi $t0, $t9, -4
  move $t1, $t0
  add $t2, $t7, $s3
  sw $t3, -8($fp)
  sub $t3, $t2, $t1
  move $t4, $t3
  add $t5, $t7, $s3
  sw $t6, -12($fp)
  sw $t7, -16($fp)
  li $t7, 2
  mul $t6, $t1, $t7
  sw $s0, -20($fp)
  add $s0, $t5, $t6
  move $s1, $s0
  sw $s3, -24($fp)
  lw $s3, -16($fp)
  sw $s4, -28($fp)
  lw $s4, -24($fp)
  add $s2, $s3, $s4
  sw $s5, -32($fp)
  add $s5, $s2, $t1
  move $s6, $s5
  sw $s7, -36($fp)
  li $t8, 42
  move $s7, $t8
  sw $t9, -40($fp)
  sw $t0, -44($fp)
  li $t0, 0
  move $t9, $t0
  sw $t1, -48($fp)
  add $t1, $s3, $s4
  sw $t2, -52($fp)
  sw $t3, -56($fp)
  lw $t3, -48($fp)
  add $t2, $t1, $t3
  sw $t4, -60($fp)
  sw $t5, -64($fp)
  lw $t5, -60($fp)
  sub $t4, $t2, $t5
  sw $t6, -68($fp)
  sub $t6, $t4, $s1
  add $t7, $t6, $s6
  move $s6, $t7
  move $a0, $s3
  jal write
  move $a0, $s4
  jal write
  move $a0, $t3
  jal write
  move $a0, $t5
  jal write
  move $a0, $s1
  jal write
  move $a0, $s6
  jal write
  sw $t1, -100($fp)
  sw $t2, -104($fp)
  sw $t3, -48($fp)
  sw $t4, -108($fp)
  sw $t5, -60($fp)
  sw $t6, -112($fp)
  sw $t7, -116($fp)
  sw $s0, -72($fp)
  sw $s1, -76($fp)
  sw $s2, -80($fp)
  sw $s3, -16($fp)
  sw $s4, -24($fp)
  sw $s5, -84($fp)
  sw $s6, -88($fp)
  sw $s7, -92($fp)
  sw $t9, -96($fp)
LABEL3:
  lw $t1, -24($fp)
  lw $t2, -16($fp)
  sub $t0, $t1, $t2
  lw $t3, -88($fp)
  sw $t0, -120($fp)
  sw $t1, -24($fp)
  sw $t2, -16($fp)
  sw $t3, -88($fp)
  blt $t0, $t3, LABEL1
  j LABEL2
LABEL1:
  lw $t1, -96($fp)
  li $t2, 4
  mul $t0, $t1, $t2
  lw $t4, -92($fp)
  add $t3, $t4, $t0
  li $t6, 3
  addi $t5, $t3, 3
  addi $t7, $t5, 4
  li $s1, 5
  addi $s0, $t7, 5
  move $t4, $s0
  lw $s2, -24($fp)
  sw $s2, -4($sp)
  addi $sp, $sp, -4
  sw $t0, -124($fp)
  sw $t1, -96($fp)
  sw $t3, -128($fp)
  sw $t4, -92($fp)
  sw $t5, -132($fp)
  sw $t7, -136($fp)
  sw $s0, -140($fp)
  sw $s2, -24($fp)
  jal changeCal
  move $t0, $v0
  lw $t2, -88($fp)
  sub $t1, $t2, $t0
  lw $t4, -16($fp)
  li $t5, 2
  mul $t3, $t4, $t5
  add $t6, $t1, $t3
  lw $s0, -48($fp)
  lw $s1, -60($fp)
  mul $t7, $s0, $s1
  add $s2, $t6, $t7
  sub $s3, $s2, $t2
  move $s4, $s3
  lw $s6, -96($fp)
  li $s7, 1
  addi $s5, $s6, 1
  li $t9, 0
  addi $t8, $s5, 0
  move $s6, $t8
  sw $t0, -144($fp)
  sw $t1, -148($fp)
  li $t1, 3
  addi $t0, $s6, 3
  sw $t2, -88($fp)
  addi $t2, $t0, 1
  move $s6, $t2
  sw $t3, -152($fp)
  addi $t3, $s6, -2
  sw $t4, -16($fp)
  addi $t4, $t3, -2
  move $s6, $t4
  div $s6, $t1
  mflo $t5
  sw $t6, -156($fp)
  mul $t6, $t5, $t1
  sw $t7, -160($fp)
  sub $t7, $s6, $t6
  sw $s0, -48($fp)
  lw $s0, -16($fp)
  sw $s0, -4($sp)
  addi $sp, $sp, -4
  sw $t0, -184($fp)
  sw $t2, -188($fp)
  sw $t3, -192($fp)
  sw $t4, -196($fp)
  sw $t5, -200($fp)
  sw $t6, -204($fp)
  sw $t7, -208($fp)
  sw $s0, -16($fp)
  sw $s1, -60($fp)
  sw $s2, -164($fp)
  sw $s3, -168($fp)
  sw $s4, -172($fp)
  sw $s5, -176($fp)
  sw $s6, -96($fp)
  sw $t8, -180($fp)
  jal changeCal
  move $t0, $v0
  lw $t2, -16($fp)
  sub $t1, $t2, $t0
  lw $t4, -24($fp)
  add $t3, $t1, $t4
  sub $t5, $t3, $t4
  lw $t7, -48($fp)
  add $t6, $t5, $t7
  sub $s0, $t6, $t7
  lw $s1, -208($fp)
  sw $t0, -212($fp)
  sw $t1, -216($fp)
  sw $t2, -16($fp)
  sw $t3, -220($fp)
  sw $t4, -24($fp)
  sw $t5, -224($fp)
  sw $t6, -228($fp)
  sw $t7, -48($fp)
  sw $s0, -232($fp)
  sw $s1, -208($fp)
  beq $s1, $s0, LABEL4
  j LABEL5
LABEL4:
  lw $t1, -88($fp)
  li $t2, 1
  addi $t0, $t1, 1
  addi $t3, $t0, 1
  move $t1, $t3
  sw $t0, -236($fp)
  sw $t1, -88($fp)
  sw $t3, -240($fp)
LABEL5:
  lw $t1, -88($fp)
  li $t2, 2
  addi $t0, $t1, -2
  li $t4, 1
  addi $t3, $t0, 1
  move $t1, $t3
  sw $t0, -244($fp)
  sw $t1, -88($fp)
  sw $t3, -248($fp)
  j LABEL3
LABEL2:
  lw $t1, -92($fp)
  li $t2, 2
  addi $t0, $t1, -2
  li $t4, 3
  addi $t3, $t0, 3
  move $t5, $t3
  move $a0, $t5
  jal write
  lw $t6, -96($fp)
  move $t6, $t1
  sw $t0, -252($fp)
  sw $t1, -92($fp)
  sw $t3, -256($fp)
  sw $t5, -260($fp)
  sw $t6, -96($fp)
LABEL8:
  li $t1, 1200
  li $t2, 22
  addi $t0, $t2, 1200
  lw $t3, -92($fp)
  sw $t0, -264($fp)
  sw $t3, -92($fp)
  bge $t3, $t0, LABEL6
  j LABEL7
LABEL6:
  lw $t1, -92($fp)
  li $t2, 1024
  addi $t0, $t1, 1024
  lw $t3, -96($fp)
  move $t3, $t0
  sw $t1, -4($sp)
  addi $sp, $sp, -4
  sw $t0, -268($fp)
  sw $t1, -92($fp)
  sw $t3, -96($fp)
  jal changeCal
  move $t0, $v0
  li $t2, 1
  addi $t1, $t0, -1
  lw $t3, -92($fp)
  move $t3, $t1
  lw $t4, -96($fp)
  move $t4, $t3
  sw $t0, -272($fp)
  sw $t1, -276($fp)
  sw $t3, -92($fp)
  sw $t4, -96($fp)
  j LABEL8
LABEL7:
  lw $t0, -92($fp)
  move $a0, $t0
  jal write
  lw $t2, -16($fp)
  lw $t3, -24($fp)
  add $t1, $t2, $t3
  move $t2, $t1
  add $t4, $t2, $t3
  move $t3, $t4
  add $t5, $t2, $t3
  lw $t6, -48($fp)
  move $t6, $t5
  move $a0, $t6
  jal write
  li $t7, 0
  move $v0, $t7
  sw $t0, -92($fp)
  sw $t1, -280($fp)
  sw $t2, -16($fp)
  sw $t3, -24($fp)
  sw $t4, -284($fp)
  sw $t5, -288($fp)
  sw $t6, -48($fp)
  move $sp, $fp
  lw $fp, 0($sp)
  lw $ra, 4($sp)
  addi $sp, $sp, 8
  jr $ra
