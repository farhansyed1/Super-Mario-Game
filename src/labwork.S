# labwork.S

.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #


.global hexasc
hexasc:
	sll $t0, $a0, 0x1c
	srl $t0, $t0, 0x1c

	slti $t1, $t0, 0xA			#0-9 = dont run convert
	beq $t1, $0, aToFConvert		#10-15 = run convert
	nop
	addi $v0 , $t0, 0x30			#the number plus 48 gives the correct ascii
	jr $ra
	nop

aToFConvert:
	addi $v0, $t0, 0x37  			#the number plus 55 gives the correct ascii
	jr $ra
	nop

 .global delay
delay:
 	PUSH $a0
 	PUSH $ra
 	li $a0, 1000			# ms
 	jal delayWhile
 	nop
 	jr $ra
  	nop

delayWhile:
 	ble $a0,0,whileStop		# stop if ms less than 0
 	nop
 	addi $a0,$a0,-1
 	addi $t0, $0, 9000			# the 4711 constant is now 1
 	j delayForLoop

delayForLoop:
 	addi $t0, $t0, -1		# decreasing index
    	bgtz $t0,delayForLoop		# repeat for loop if constant less than 0
    	nop
    	j delayWhile
    	nop

whileStop:
    	POP $ra
    	POP $a0
    	jr $ra
    	nop

 .global time2string
time2string:
    add $t8, $ra, $0
    PUSH $a0

    sll $t0, $a1,0x10

    # 8
    srl $t0, $t0,0x04
    sll $t6, $t0,0x10
    srl $t6, $t6,0x1C

    # 5
    srl $t0, $t0,0x04
    sll $t5, $t0,0x10
    srl $t5, $t5,0x1C

    # 9
    srl $t0, $t0,0x04
    sll $t4, $t0,0x10
    srl $t4, $t4,0x1C

    # 5
    srl $t0, $t0,0x04
    sll $t3, $t0,0x10
    srl $t3, $t3,0x1C

    # 5
    addi $a0, $t3, 0
    jal hexasc
    nop

    move $t3, $v0

    # 9
    addi $a0, $t4, 0
    jal hexasc
    nop

    move $t4, $v0

    # Colon
    li $t7, 0x3A
    move $a0, $t7

    # 5
    addi $a0, $t5, 0
    jal hexasc
    nop

    move $t5, $v0

    # 8
    addi $a0, $t6, 0
    jal hexasc
    nop

    move $t6, $v0
    POP $a0

    li $t0, 0x00
    sb $t3, 0($a0)
    sb $t4, 1($a0)
    sb $t7, 2($a0)
    sb $t5, 3($a0)
    sb $t6, 4($a0)
    sb $t0, 5($a0)

    jr $t8
    nop

.global enable_interrupt
enable_interrupt:
  ei
  jr $ra
  nop
