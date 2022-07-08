mov ax, 5+10
mov bx, 3
int 1

mov ax, 5*10
mov bx, 3
int 1

TERMINATE:
	jmp TERMINATE
