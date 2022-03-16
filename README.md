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
mov to, from
sub x, y (ax = x - y)
add x, y (ax = x + y)
div x, y (ax = x / y)
mul x, y (ax = x * y)
shl value, by (value << by) -> Shift left
shr value, by (value >> by) -> Shift right
.
.
.
</pre>
