mov bx, DATA

outb 0x4, 0x41
outd 0x6, 440

print_string:
	mov ax, [bx]
	cmp ax, 0
	je print_string_end

	push bx
	mov bx, 0
	int 1
	pop bx

	add bx, 1

	jmp print_string
	
print_string_end:


TERMINATE:
	jmp TERMINATE

DATA:
	inc# "hello_world.txt"
