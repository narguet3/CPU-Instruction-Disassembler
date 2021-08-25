# CPU-Instruction-Disassembler
This was project for my computer systems class. The program processes instructions through bit masking.
The project below has four functions that process indivdual binary instructions received via hexadecimal values. Program print_instructions
prints the operation performed, which registers are used, and if applicable,
the memory location or literal value used. Function Disassemble checks if 
a set of instructions in an array of instructions are all valid, and if so
the program prints the memory address in the array in hexadecimal and prints
the operation and corresponding information like in print_instrcution for
every element or instruction in the array. where_set, checks to see if a 
register reg_nbr is set and if so indicates where in the array. Function
is_safe checks if the program array is valid and if a register is set or 
given a value before it is used in an operation.
