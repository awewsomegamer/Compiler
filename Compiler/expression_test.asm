mov ax, 5+10+1
mov bx, 3
int 1

mov ax, 0xA
mov bx, 0
int 1

mov ax, 5*10+1
mov bx, 3
int 1

mov ax, 0xA
mov bx, 0
int 1

mov ax, 10/5+1
mov bx, 3
int 1

mov ax, 0xA
mov bx, 0
int 1

mov ax, 10-5+1
mov bx, 3
int 1


TERMINATE:
	jmp TERMINATE
