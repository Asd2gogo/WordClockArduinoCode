	;Delay for approx. 1 sec by 1MHz
delay:
	push	r17
	push	r18
	push	r19
	ldi	r17,59
	ldi	r18,13
	ldi	r19,3
delay1:
	subi	r17,1
	sbci	r18,0
	sbci	r19,0
	brcc	delay1
	pop	r19
	pop	r18
	pop	r17
	ret
