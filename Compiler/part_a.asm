mov bx, MY_STRING 

print_string:
	mov ax, [bx]
	cmp ax, 0
	je print_end

	push bx
	mov bx, 0
	int 0
	pop bx

	add bx, 1

	jmp print_string

print_end:
	jmp print_end
