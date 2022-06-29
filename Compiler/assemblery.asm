; ../../Emulator/Emulator/test/bf_test.asm
; mov ax, 0 ; Current cell value
; mov cx, 0x1000 ; Current cell ptr
; mov bx, DATA ; Source ptr

; mov bp, PROGRAM_END
; add bp, 0x5000
mov bp, 0x1000
mov sp, bp

; mov i1, 0
; mov i2, 0

; Draw pixel
; mov i1, 10
; shl i1, 16
; or i1, 10

; mov i2, 0x0000FF00
; int 1

; Successfully set interrupt (27/5/2022)
sivte 3, KEY_INTERRUPT

LOOP:
	jmp LOOP

KEY_INTERRUPT:
	; Determine if key was pressed or released, in the case of the latter just end the interrupt
	ind ax, 0
	and ax, 0x00FF0000
	cmp ax, 0x00FF0000
	jne END_INT

	;
	ind ax, 0
	and ax, 0xFF

	; Check if backspace key was pressed
	cmp ax, 42
	je HANDLE_BACKSPACE

	; Print regular character
	mov bx, CHARACTERS
	add bx, ax
	mov ax, [bx]
	mov bx, 0

	int 0
	
	jmp END_INT

	HANDLE_BACKSPACE:

	; Update cursor
	mov bx, 2
	int 0
	mov bx, ax
	and bx, 0xFFFF
	shr ax, 16
	sub ax, 8
	shl ax, 16
	or ax, bx
	mov bx, 1
	int 0

	; Clear char
	mov ax, ' '
	mov bx, 0
	int 0

	; Update cursor
	mov bx, 2
	int 0
	mov bx, ax
	and bx, 0xFFFF
	shr ax, 16
	sub ax, 8
	shl ax, 16
	or ax, bx
	mov bx, 1
	int 0
	
	END_INT:
	ret

CHARACTERS:
	ds "    ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890     "


; LOOP:
; 	cmp [bx], '+'
; 	jne END_INC_SEC
	
; 	add [cx], 1
; 	jmp COMMON_END_STUB

; 	END_INC_SEC:

; 	cmp [bx], '.'
; 	jne END_PRINT_SEC

; 	mov ax, [cx]
; 	int 0

; 	jmp COMMON_END_STUB
	
; 	END_PRINT_SEC:

; 	cmp [bx], '-'
; 	jne END_DEC_SEC
; 	cmp [cx], 0
; 	je END_DEC_SEC

; 	sub [cx], 1

; 	jmp COMMON_END_STUB
; 	END_DEC_SEC:

; 	cmp [bx], '>'
; 	jne END_ARROW_RIGHT_SEC

; 	add cx, 1

; 	jmp COMMON_END_STUB
; 	END_ARROW_RIGHT_SEC:

; 	cmp [bx], '<'
; 	jne END_ARROW_LEFT_SEC

; 	sub cx, 1

; 	jmp COMMON_END_STUB
; 	END_ARROW_LEFT_SEC:

; 	COMMON_END_STUB:

; 	add bx, 1
; 	cmp [bx], 0
; 	je TERMINATE
	
; 	jmp LOOP

; TERMINATE:
; 	jmp TERMINATE

; DATA:
; 	; ds "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.>" ; H
; 	; ds "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.>" ; e
; 	; ds "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++..>" ; l
; 	; ds "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++." ; o
; 	ds "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.>++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++<.>.."

; 	db 0

; PROGRAM_END:

; +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -> A

