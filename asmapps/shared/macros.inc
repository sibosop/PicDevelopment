;
; General macro include file
;


	nolist
;
; disable interrupts see app not
; about interrupt race condition
; when clearing GIE
;
mnum	set 0

disable	macro	
__dl#v(mnum)
	banksel	INTCON	
	bcf	INTCON,GIE	; attempt to clear flag
	btfsc	INTCON,GIE	; did it really get clear?
	goto	__dl#v(mnum)	; no, try again
mnum ++
	endm	

;
; enable interrupts (see app note 
;
enable	macro	
__el#v(mnum)
	banksel	INTCON
	bsf	INTCON,GIE	; attempt to set flag
	btfss	INTCON,GIE	; did it really get set
	goto	__el#v(mnum)	; no, try again
mnum ++
	endm


;
; shift left file
;
slf	macro file,dest
	bcf	STATUS,C	; clear the carry bit
	rlf	file,dest		; rotate the file left
	endm
;
; shift right file
;
srf	macro file,dest
	bcf	STATUS,C	; clear the carry bit
	rrf	file,dest 	; rotate the file right
	endm
	

;
; one byte integer divide
; parameters:
;	assumes variables are already
;	bankselected and in the same banke
;	t - temporary register
;	r - return value
;	a - dividee (returns as remainder
;	b - divider
;
div	macro	t,r,a,b
	movlw	1
	movwf	t		; t = 1
div#v(mnum)
	btfsc	b,6		; while ( (b & 0x40) == 0 )
	goto	div1#v(mnum)	; 
	slf	b,f		; {b << 1
	slf	t,f		;  t << 1
	goto	div#v(mnum)
div1#v(mnum)
	clrf	r		; r = 0
div2#v(mnum)			; do
	movf	b,w		; if ( a - b ) >= 0 )
	subwf	a,w		; {
	btfsc	STATUS,Z	; != 0
	goto	div4#v(mnum)
	btfss	STATUS,C	; C set on a-b > 0 
	goto	div3#v(mnum)
div4#v(mnum)
	movwf	a		; a = a - b
	movf	t,w		; r = r + t
	addwf	r,f		; }
div3#v(mnum)
	srf	b,f		; b >> 1
	srf	t,f		; t >> 1
	movf	t,f		; 
	btfss	STATUS,Z	; } while ( t != 0 )
	goto	div2#v(mnum)
				; r = result, a = remainder
mnum ++
	endm
	

;
; computed goto table
;
table	macro	name
name
	movwf	tmp4
	movlw	high name
	movwf	PCLATH
	movf	tmp4,w
	addwf	PCL,f
	endm

	
;
; clear a block of memory
;
clrblk	macro	addr,cnt
	local	a = 0
	while 	a < cnt
	clrf	addr + a
a += 1
	endw
	endm

