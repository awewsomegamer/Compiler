# Compiler

This is a small compiler that is supposed to be assembly like. The name of the compiler is to be decided.

<img src="https://image.shutterstock.com/image-photo/businessman-relaxing-office-chair-standing-260nw-7869196.jpg">
Image from Shutterstock

## Architecture
<pre>
|Registers:
|        +- AH (higher 8 bits)
+----AX--+
|        +- AL (lower 8 bits)
|
|        +- BH (higher 8 bits)
+----BX--+
|        +- BL (lower 8 bits)
|
|        +- CH (higher 8 bits)
+----CX--+
|        +- CL (lower 8 bits)
|
|        +- DH (higher 8 bits)
+----DX--+
|        +- DL (lower 8 bits)
|
+----IP -> Instruction pointer
|
+----SP -> Stack pointer
|
+----BP -> Base pointer of the stack
|
+----I1 -> 16 bit integer
|
+----I2 -> 16 bit integer
|
+----I3 -> 16 bit integer
|
+----I4 -> 16 bit integer
|
-
</pre>

## Instructions
<pre>
Definitions of symbols:
  x -> register / value
  y -> register / value
  to -> to register / label
  from -> from register / label
  
mov to, from

sub x, y (ax = x - y)

add x, y (ax = x + y)

div x, y (ax = x / y)

mul x, y (ax = x * y)

and x, y (x = x & y)

or x, y (x = x | y)

xor x, y (x = x ^ y)

not x (x = ~x)

shl value, by (value << by) -> Shift left

shr value, by (value >> by) -> Shift right

int x -> Triggers interrupt of value x

call label / address -> Executes code at that label / address

jmp label / address -> jumps to the give label / address and proceeds execution from there

cmp x, y -> compares x to y

je label / address -> Jumps to label / address if equal flag is set

jne label / address -> Jumps to label / address if equal flag is not set

jz label / address -> Jumps to label / address if zero flag is set

jnz label / address -> Jumps to label / address if zero flag is not set

jc label / address -> Jumps to label / address if carry flag is set

jnc label / address -> Jumps to label / address if carry flag is not set

</pre>
