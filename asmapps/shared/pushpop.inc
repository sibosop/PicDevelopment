
	nolist
;
; push val on stack, if lit = 1 then push literal else push f
; i
;

push	macro	val, litflag, intrflag
	if intrflag == 0	; is this in an interrup
	disable			; disable if not
	endif			; 
	movf	STATUS,w
	movwf	stat_save	; save the page flag
	bcf	STATUS,RP0	; set to page 0
	movf	stackptr,w	; get the stack ptr
	addwf	stack,w		; add it to the stack base
	movwf	FSR		; set indirection
	if litflag == 1		; is this literal
	movlw	val		; yes, move literal
	else			;
	movf	val,w		; else move direct
	endif
	movwf	INDF		; store on stack
	decf	stackptr,f	; decrement for next push
	movf	stat_save,w
	movwf	STATUS		; all we care about is the page flag
	if intrflag == 0
	enable			; enable if in interrupt routine
	endif
	endm

;
; pop a value off the stack if storeflag then put in val, else toss it
; 
pop	macro	val, storeflag, intrflag
	if intrflag == 0	; is this in an interrup
	disable			; disable if not
	endif			; 
	movf	STATUS,w
	movwf	stat_save	; save the page flag
	bcf	STATUS,RP0	; set to page 0
	incf	stackptr,f	; increment the stack ptr before pop
	if storeflag == 1 
	movf	stackptr,w	; get the stack ptr
	addwf	stack,w		; add it to the stack base
	movwf	FSR		; set indirection
	movf	INDF,w		; get the value on the stack
	movwf	val		; store it in val
	endif
	movf	stat_save,w
	movwf	STATUS		; all we care about is the page flag
	if intrflag == 0
	enable			; enable if in interrupt routine
	endif
	endm

	list
