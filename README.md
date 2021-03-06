# Szikra Assembler

## Architecture
Bytes are organized in this fashion
<pre>
OPERATION CODE (8 bits), REGISTER MASK (8 bits), VALUE 1 (32 bits), VALUE 2 (32 bits)

Diagram of largest instruction (Little Endian):

0           8         12        16           48           80
+-----------+---------+---------+------------+------------+
| OPERATION | LENGHTS | INDICES | ARGUMENT 1 | ARGUMENT 2 |
+-----------+---------+---------+------------+------------+

Operation: The operation to preform
Lengths: Describes the length of the arguments in bytes
Indices: Describes the type of arguments

Bits:

  OP CODE    L2 L1 I2 I1
00 00 00 00  00 00 00 00 ...

OP CODE: 256 different values
L1: 0 - 3 (Length of the first argument in bytes - 1)
L2: 0 - 3 (Length of the second argument in bytes - 1)
I1: 0 - 3 (Description of the first argument)
I2: 0 - 3 (Description of the second argument)
... 32 bit compressed arguments

</pre>
All bytes result in a value that is greater than or equal to 0, the only time a byte will be -1 is when the end of the program is reached.

### Register mask values and descriptions
<pre>
+-------+-----------------------------------------------------------------------------+
|Number | Description                                                                 |
+-------+-----------------------------------------------------------------------------+
|  0    | Regular value                                                               |
+-------+-----------------------------------------------------------------------------+
|  1    | Register value                                                              |
+-------+-----------------------------------------------------------------------------+
|  2    | Value of register in memory                                                 |
+-------+-----------------------------------------------------------------------------+
|  3    | Value of memory at value                                                    |
+-------+-----------------------------------------------------------------------------+
</pre>

<pre>
|Registers:
|        
+----AX -> 32 bit register
|
+----BX -> 32 bit register
|      
+----CX -> 32 bit register
|
+----DX -> 32 bit register
|
+----IP -> Instruction pointer
|
+----SP -> Stack pointer
|
+----BP -> Base pointer of the stack
|
+----I1 -> 32 bit integer
|
+----I2 -> 32 bit integer
|
+----I3 -> 32 bit integer
|
+----I4 -> 32 bit integer
|
-
</pre>

## Syntax
INSTRUCTION SIZE VALUE, SIZE VALUE <br>
INSTRUCTION: One of the instructions from the next section <br>
SIZE: byte, word, dword <br>
VALUE: A register, integer, or pointer <br>


## Instructions
<pre>

nop - preforms no operation

mov v1, v2 - move v2 into v1
sub v1, v2 - subtract v2 from v1
add v1, v2 - add v2 to v1
div v1, v2 - divide v1 by v2
mul v1, v2 - multiply v1 by v2

or v1, v2 - bitwise or
xor v1, v2 - bitwise xor
not v1 - bitwise not
shl v1, v2 - shift v1 left by v2
shr v1, v2 - shift v1 right by v2

sivte v1, v2 - set interrupt v1 to function v2
rivte v1 - reset interrupt v1 to its original function
int v1 - call interrupt v1

call v1 - call label v1
jmp v1 - jump to label v1

cmp v1, v2 - compare v1 to v2, set proper flags
je v1 - jump to label v1 if previous cmp instruction resulted in both values equalling
jne v1 -jump to label v1 if previous cmp instruction did not result in both values equalling 
jg v1 - jump to label v1 if previous cmp instruction resulted in the first value being larger than the second
jge v1 - jump to label v1 if previous cmp instruction resulted in the first value being larger than the or equal to the second
jl v1 - jump to label v1 if previous cmp instruction resulted in the first value being less than the second
jle v1 - jump to label v1 if previous cmp instruction resulted in the first value being less than or equal to the second
jz v1 - jump to label v1 if the zero flag is set
jnz v1 - jump to label v1 if the zero flag is not set
jc v1 - jump to label v1 if the carry flag is set
jnc v1 - jump to label v1 if the carry flag is not set

ret - return to the last value on the stack (use in calls only)

push v1 - push v1 to the stack
pop v1 - pop the top of the stack into v1

inb v1, v2 - read in a byte from address v2 into v1 from the IO port
inw v1, v2 - read in a word from address v2 into v1 from the IO port
ind v1, v2 - read in a double word from address v2 into v1 from the IO port

outb v1, v2 - put out a byte, v2, to address v1
outw v1, v2 - put out a word, v2, to address v1
outd v1, v2 - put out a double word, v2, to address v1

</pre>
