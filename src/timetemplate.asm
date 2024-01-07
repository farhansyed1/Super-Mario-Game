  # timetemplate.asm

.macro	PUSH (%reg)
	addi	$sp,$sp,-4
	sw	%reg,0($sp)
.end_macro

.macro	POP (%reg)
	lw	%reg,0($sp)
	addi	$sp,$sp,4
.end_macro

	.data
	.align 2
mytime:	.word 0x5957	# ska vara 5957
timstr:	.ascii "text more text lots of text\0"
two: .ascii "TWO"
	.text

main:
	# print timstr
	la	$a0,timstr
	li	$v0,4
	syscall
	nop
	# wait a little
	li	$a0,2
	jal	delay
	nop
	# call tick
	la	$a0,mytime
	jal	tick
	nop
	# call your function time2string
	la	$a0,timstr
	la	$t0,mytime
	lw	$a1,0($t0)
	jal	time2string
	nop
	# print a newline
	li	$a0,10
	li	$v0,11
	syscall
	nop
	# go back and do it all again
	j	main
	nop
# tick: update time pointed to by $a0
tick:	lw	$t0,0($a0)	# get time
	addiu	$t0,$t0,1	# increase
	andi	$t1,$t0,0xf	# check lowest digit
	sltiu	$t2,$t1,0xa	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x6	# adjust lowest digit
	andi	$t1,$t0,0xf0	# check next digit
	sltiu	$t2,$t1,0x60	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa0	# adjust digit
	andi	$t1,$t0,0xf00	# check minute digit
	sltiu	$t2,$t1,0xa00	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x600	# adjust digit
	andi	$t1,$t0,0xf000	# check last digit
	sltiu	$t2,$t1,0x6000	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa000	# adjust last digit
tiend:	sw	$t0,0($a0)	# save updated result
	jr	$ra		# return
	nop
 
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
 
delay:
 	PUSH $a0
 	PUSH $ra
 			# ms
  	
delayWhile: 
 	ble $a0,0,whileStop		# stop if ms less than 0 
 	nop
 	addi $a0,$a0,-1			
 	addi $t0, $0, 3000			# the 4711 constant is now 10
 	
 	
delayForLoop: 	
 	addi $t0, $t0, -1		# decreasing index 
    	bgtz $t0,delayForLoop		# repeat for loop if constant greater than 0 
    	nop
    	j delayWhile
    	nop
    	    
 whileStop:    
    	POP $ra
    	POP $a0
    	jr $ra
    	nop	
 
 time2string:
    PUSH $ra
    
    PUSH $s0
    PUSH $s7
    PUSH $s6
    PUSH $s5
    PUSH $s4
    PUSH $s3
    PUSH $a0
   
    
    # Extracting the bits
    sll $s0, $a1,0x10
    
    # 00:0X
    srl $s0, $s0,0x04
    sll $s6, $s0,0x10
    srl $s6, $s6,0x1C
    
    # 00:X0
    srl $s0, $s0,0x04
    sll $s5, $s0,0x10
    srl $s5, $s5,0x1C
    
    # 0X:00
    srl $s0, $s0,0x04
    sll $s4, $s0,0x10
    srl $s4, $s4,0x1C
    
    # X0:00
    srl $s0, $s0,0x04
    sll $s3, $s0,0x10
    srl $s3, $s3,0x1C
    
    # Converting to ASCII
    # X0:00
    addi $a0, $s3, 0
    jal hexasc
    nop
    move $s3, $v0
    
    # 0X:00
    addi $a0, $s4, 0
    jal hexasc
    nop
    move $s4, $v0
    
    # Colon
    li $s7, 0x3A
    move $a0, $s7

    # 00:X0
    addi $a0, $s5, 0
    jal hexasc
    nop
    move $s5, $v0
    
    # 00:0X
    addi $a0, $s6, 0
    jal hexasc
    nop
    move $s6, $v0
    
    # Rewriting the memory address that the pointer points to
    POP $a0
    
    li $s0, 0x00
    sb $s3, 0($a0)
    sb $s4, 1($a0)
    sb $s7, 2($a0)
    sb $s5, 3($a0)
    sb $s6, 4($a0)
    sb $s0, 5($a0)
    
    POP $s3
    POP $s4
    POP $s5
    POP $s6
    POP $s7
    POP $s0
    POP $ra
    jr $ra
    nop
