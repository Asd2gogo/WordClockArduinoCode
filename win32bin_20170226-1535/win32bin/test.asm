	.include "Appnotes/m8def.inc"

	ldi	r16,low(ramend)
	out	spl,r16
	ldi	r16,high(ramend)
	out	sph,r16

	ldi	r16,0xff
	out 	ddrb,r16
	ldi	r16,1
loop:
	lsr	r16
	brcc	no_carry
	sbr	r16,0x80
no_carry:
	out	portb,r16
	rcall	delay
	rjmp	loop

	.include "zweite.asm"

