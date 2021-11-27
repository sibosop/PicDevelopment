;
; Queueing macro include file
;
	nolist


qnum	set 0



;
; queuing macros 
;
;
; definition of queue structure
;
; is struct q {
;		int qhead;
;		int qtail;
;		char queue[NUM_QENTS];
;		}
	constant qhead		=	0
	constant qtail		=	1
	constant qptr		=	2
	constant num_qents	=	D'41'	
	constant queue_size	=	(num_qents+2)
	constant qempty		=	0

;
; initialize the queue
initqueue	macro	queue
	banksel	queue
	clrf	(queue+qhead)
	clrf	(queue+qtail)
	endm
	
	

;
; enqueue a value into a queue using the queue struct
;	queue = beginning of queue struct
;	litflag = enqueue literal = 1 else enqueue a file
;
enqueue	macro	queue,val,litflag
	banksel	queue
	movf	(queue+qhead),w	; get the header value
	addlw	(queue+qptr)	; calculate the queue position
	movwf	FSR		; set the indirect pointer
	incf	(queue+qhead),w	; increment the header into w
	sublw	num_qents 	; compare to end of queue
	btfss	STATUS,Z 	; zero means we're at the end
	goto	bumpq#v(qnum)
	clrf	(queue+qhead)	; wrap around the queue
	goto	putval#v(qnum)
bumpq#v(qnum)
	incf	(queue+qhead),f
putval#v(qnum)
	if litflag == 1		; test for literal enqueu
	movlw	val		; get the literal value to enqueue
	else
	banksel	val
	movf	val,w		; get the direct value to enqueue
	banksel	queue
	endif
	movwf	INDF		; store value in the queue
qnum	++
	endm
	
;
; dequeue a value from a queue 
;	Z bit set, then queue is empty
;	if queue not empty then w contains value 
;
dequeue	macro	queue
	banksel	queue
	movf	(queue+qtail),w	; get the tail value
	subwf	(queue+qhead),w	; see if head and tail are the same
	btfsc	STATUS,Z	; is queue empty
	goto	qexit#v(qnum)
	movf	(queue+qtail),w ; not empty, get the tail value
	addlw	(queue+qptr)	; calculate the queue position
	movwf	FSR		; set the indirect pointer
	incf	(queue+qtail),w	; increment the tail into w
	sublw	num_qents	; compare to end of queue
	btfss	STATUS,Z	; zero means we need to wrap
	goto	bumpq#v(qnum)
	clrf	(queue+qtail)
	goto	getval#v(qnum)
bumpq#v(qnum)
	incf	(queue+qtail),f
getval#v(qnum)
	movf	INDF,w		; get the value 
	bcf	STATUS,Z	; z flag means w has value
qexit#v(qnum)
qnum	++
	endm
	list
