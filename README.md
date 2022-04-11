# Assembler

This is a small assembler. The name of the assembler is to be decided.

<img src="https://image.shutterstock.com/image-photo/businessman-relaxing-office-chair-standing-260nw-7869196.jpg">
Image from Shutterstock

## Architecture
Bytes are organized in this fashion
<pre>
OPERATION CODE (8 bits), REGISTER MASK (8 bits), VALUE 1 (32 bits), VALUE 2 (32 bits)

Diagram (Little Endian)

0           8        12       16           48           112
+-----------+--------+--------+------------+------------+
| OPERATION | MASK 1 | MASK 2 | ARGUMENT 1 | ARGUMENT 2 |
+-----------+--------+--------+------------+------------+

Operation: The operation to preform
Mask 1: Describes what the first argument is (see Register mask values and descriptions)
Mask 2: Describes what the second argument is (see Register mask values and descriptions)
Argument 1: The first argument
Argument 2: The second argument

</pre>
All bytes result in a value that is greater than or equal to 0, the only time a byte will be -1 is when the end of the program is reached.

### Register mask values and descriptions
<pre>
+-------+---------------------------------------------------------------------------------+
|Number | Description                                                                     |
+-------+---------------------------------------------------------------------------------+
|  0    | Both arguments are not registers                                                |
+-------+---------------------------------------------------------------------------------+
|  1    | The first argument is a register                                                |
+-------+---------------------------------------------------------------------------------+
|  2    | The second argument is a register                                               |
+-------+---------------------------------------------------------------------------------+
|  3    | Both arguments are registers                                                    |
+-------+---------------------------------------------------------------------------------+
|  4    | The second argument is an address value, the first is a regular memory address  |
+-------+---------------------------------------------------------------------------------+
|  5    | The second argument is an address value, the first is a register                |
+-------+---------------------------------------------------------------------------------+
</pre>

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
