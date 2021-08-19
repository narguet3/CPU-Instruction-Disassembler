/*
Description:
The folowing program processes instructions through bit masking.
The project and file below has four functions that process indivdual
binary instructions received via hexadecimal values. Program print_instructions
prints the operation performed, which registers are used, and if applicable,
the memory location or literal value used. Function Disassemble checks if 
a set of instructions in an array of instructions are all valid, and if so
the program prints the memory address in the array in hexadecimal and prints
the operation and corresponding information like in print_instrcution for
every element or instruction in the array. where_set, checks to see if a 
register reg_nbr is set and if so indicates where in the array. Function
is_safe checks if the program array is valid and if a register is set or 
given a value before it is used in an operation.
*/

#include <stdio.h>
#include "machine.h"

/*This enum is for what values are read or written to for any operation*/
enum registers { Read_Reg2_Reg3 = 2, Read_Reg2 = 1, Read_Reg1_Reg3 = 3, 
                Writes_Reg1 = -1, Read_Reg1 = 4, Halt =5 , Invalid = 0 };

/*Prototypes for helper functions*/
void print_helper (Wrd instruction);
int is_valid (Wrd instruction);

/*The function below, print_instruction prints the components of an instruction.
The function takes the parameter instruction of type Wrd and passes the value
through helper function is_valid which checks if the operation and the 
corresponding attributes are valid. If valid, the function passes 
the parameter to print_helper which deciphers the instruction and prints
the operation, extension, registers, and memory location/literal value
if applicable.
*/
unsigned short print_instruction (Wrd instruction) {
    /*instruction is tested for validity via is_valid*/
    if (is_valid (instruction) != Invalid) {
        print_helper (instruction);
        return 1; 
        }

    return 0;       
}

/*
The function checks if every instruction in program is valid and if so, 
prints the memory location in array, operation, and corresponding components 
per instruction. This is performed by first checking if the array program or
bad_instr are invalid against null. Also, the function checks that the number
of words in program does exceed the maximum allowed in N2. If any of the 
previous is not true, the function loops through the entire array and check 
for validity with the helper function is_valid. If not valid the function sets
bad_instr to the index of the instruction in the array and returns 0. If all 
the functions are valid, all the instructions are printed as in 
print_instructions but with the memory address of the word in the array 
preceeding. The instructions are printed via the helper function print_helper.
*/
unsigned short disassemble (const Wrd program[], unsigned short num_instrs, 
                            unsigned short *const bad_instr) {
                                         
    int memory_address = 0x0000;
    int index = 0;

    if (program == NULL || bad_instr == NULL || num_instrs > NUM_WORDS)
        return 0;  
    else {
        for (index = 0; index < num_instrs; index++) {
            if (is_valid(program[index]) == Invalid) {
                *bad_instr = index;
                return 0;
            }
        }

        for (index = 0; index < num_instrs; index++) {
            printf ("%04x: ", memory_address);
            /*helper function prints the instruction
            as in print_instruction*/
            print_helper (program[index]);
            printf ("\n");
            /*increases by 4 due to size of a word*/
            memory_address += 0x0004;
        }                                
    }

    return 1;     
}
/*
Where_set checks where a specific register is set in the program array.
The function checks if program is null or the array is larger than allowed
by N2 or if parameter register is a non-valid register. Since any modified
value by any operation is set to reg_1 in an instruction, the program goes
through program array and check if any of the instructions reg_1 is the 
same as the register parameter. If so, the function calls the helper 
fucntction is_valid and if the functions returns the enum value Write_Reg1
or Read_Reg2 or Read_Reg2_Reg2 indicating that the operation in the
instruction does set a value to its reg_1 value that is the same as the
parameter value. The function then returns the value of index where the
parameter register is set. If the register is not set, -1 is returned.
*/
short where_set (const Wrd program[], unsigned short num_words, 
                  unsigned short reg_nbr) {

    int index =0;

    if (program == NULL || num_words > NUM_WORDS || reg_nbr > 6)
        return -1;
    else {
        for (index = 0; index < num_words; index++) {
            int reg_1 = program[index] >> 21 & 0x0007;

            if (reg_1 == reg_nbr) {
                /*valued returned by is_valid indicates
                if the operation of instruction sets its
                reg_1*/   
                if (is_valid(program[index]) == Writes_Reg1
                    || is_valid(program[index]) == Read_Reg2
                    || is_valid(program[index]) == Read_Reg2_Reg3) {

                    return index;
                }   
            }
        }
    }
    return -1;
}


/*
The function is_safe check if an instruction is safe to use by checking if it
 was previously set. The function accomplishes this by first checking if 
 the program array is null or bad_instr is null or if pgm_size is larger than
 the maximum words allowed in N2. If the previous does not apply, the fucntion
 goes through the array and for every instruction it uses bit masking
 to get the 3 registers used by an instruction. The function 
 then checks if the instruction is not invalid via is_valid helper. If not, 
 the function uses the value returned by is_valid to know the registers
 used by an operation. the function where_set is then called for the 
 registers that are used to check if they were set and where. If the
 registers were not set or if they were not set in the array before
 the current index, bad_instr is initialized with index of the instruction
 and 0 is returned.
 */
unsigned short is_safe (const Wrd program[], unsigned short pgm_size,
                                        unsigned short *const bad_instr) {
    int index = 0;  

    if (program == NULL || bad_instr == NULL || pgm_size > NUM_WORDS)
        return 0;
    else {
        for (index = 0; index < pgm_size; index++) {
            /*uses bit masking to know what registers
            are used by an operation*/
            int reg_1 = program[index] >> 21 & 0x0007;
            int reg_2 = program[index] >> 18 & 0x0007;
            int reg_3 = program[index] >> 15 & 0x0007;
                        
            if (is_valid(program[index]) == Invalid){
                *bad_instr = index;
                return 0;
            } else {
                /*the operation uses reg_2 and reg_3 
                and thus are checked if set previously*/
                if (is_valid(program[index]) == Read_Reg2_Reg3) {
                    if ((where_set(program, pgm_size, reg_2) >= index 
                        || where_set(program, pgm_size, reg_2) == -1) 
                        || (where_set(program, pgm_size, reg_3) >= index 
                        || where_set(program, pgm_size, reg_3) == -1)) {
                                                    
                        *bad_instr = index;
                        return 0;
                    }
                /*the operation uses reg_2 and is checked for
                safety*/
                } else if (is_valid(program[index]) == Read_Reg2) {
                    if (where_set(program, pgm_size, reg_2) >= index 
                        || where_set(program, pgm_size, reg_2) == Writes_Reg1) {

                        *bad_instr = index;
                        return 0; 
                    }
                /*Reg_1 and Reg_3 are used by operation
                and checked for safety*/
                } else if (is_valid(program[index]) == Read_Reg1_Reg3) {
                    if ((where_set(program, pgm_size, reg_1) >= index 
                        || where_set(program, pgm_size, reg_1) == -1) 
                        || (where_set(program, pgm_size, reg_2) >= index
                        || where_set(program, pgm_size, reg_2) == -1)) {

                        *bad_instr = index;
                        return 0;
                }
                /*Read_1 is used and is checked
                for safety*/
                } else if (is_valid(program[index]) == Read_Reg1) {
                    if (where_set(program, pgm_size, reg_1) >= index 
                        || where_set(program, pgm_size, reg_1) == -1) {
                        *bad_instr = index;
                        return 0;
                    }
                }
            }                                
        }
    }
    /*1 is returned if all instructions are safe*/
    return 1;
}
/*The following checks if an instruction is valid and returns an integer
indicating what registers are used. This is done by first bit masking
the parameter instruction and concluding what the operation, extension, 
reg_1, reg_2, reg_3, and memory or literal values are. If the operation
is out of the allowable range, 0 is returned. The fuction organizes
functions that have the same number of register in their operation and
then checks for validity for the respective registered used. If the registers
are valid an enum indicating what registers are used is returned. If any 
of the registers are invalid 0 is returned.*/ 
int is_valid (Wrd instruction) {
    int program =  instruction >> 27 & 0x001F;
    int extension = instruction >> 24 & 0x0007;
    int reg_1 = instruction >> 21 & 0x0007;
    int reg_2 = instruction >> 18 & 0x0007;
    int reg_3 = instruction >> 15 & 0x0007;
    int memory_or_literal = instruction & 0xFFFF; 
    int is_valid = 0;
        

    if (program > SYS)
        return 0;
    else if ((program >= PLUS && program <= MOD) 
            || (program >= SHL && program <= BXOR) 
            || (program >= AND && program <= OR)) {

        if (reg_1 < R6 && reg_2 <= R6 && reg_3 <= R6)
            /*indicates to check for reg 2 and reg 3*/     
            is_valid = Read_Reg2_Reg3; 

    } else if (program == NEG || program == ABS 
               || program == BNEG 
               || program == NOT 
               || program == MOVE) {

        if (reg_1 < R6 && reg_2 <= R6)
            /*indicates to check for reg 2*/
            is_valid = Read_Reg2;

    } else if (program >= 16 && program <= 21) {
        if (reg_1 <= R6 && reg_2 <= R6 && memory_or_literal % 4 == 0)
            /*indicates to check reg 1 and reg 3*/
            is_valid = Read_Reg1_Reg3;

    } else if (program >= 23 && program <= 25) {
        if (reg_1 < R6 && program == 23 && memory_or_literal % 4 == 0)
            is_valid = Writes_Reg1;
        else if (reg_1 < R6 && program == 25)
            /*indicates to not check as only writes to reg 1 - no read*/
            is_valid = Writes_Reg1;
        else if (reg_1 <= R6 &&  program == 24 && memory_or_literal % 4 == 0)
            /*indicates to only check reg 1*/
            is_valid = Read_Reg1;

    } else if (program == 26 && extension >= 0 && extension <= 4) {
        if ((extension == 0 || extension == 2) && reg_1 < R6)
            is_valid = Writes_Reg1;
        else if ((extension == 1 || extension == 3) && reg_1 <= R6)
            is_valid = Read_Reg1;
        else if (extension == 4)
            /*halts checks nothing*/
            is_valid = Halt;
    }

    return is_valid;
} 

/*The helper function below takes in a word instruction as a parameter
and prints out the operation, extension, registers used, memory address 
or literal. The function uses bit masking to find out the previously 
mentioned compenonets of an operation. A switch statement is then used
to with the cases being the operation. The name and corresponding atributes 
are then printed*/
void print_helper (Wrd instruction) {
        int operation_isolater = instruction >> 27 & 0x001F;
        int extension = instruction >> 24 & 0x0007;
        int reg_1 = instruction >> 21 & 0x0007;
        int reg_2 = instruction >> 18 & 0x0007;
        int reg_3 = instruction >> 15 & 0x0007;
        int memory_or_literal = instruction & 0xFFFF;        

        switch (operation_isolater)
        {
            case PLUS :
                    printf ("plus R%d R%d R%d", reg_1, reg_2, reg_3);
                    break;
            case MINUS :
                    printf ("minus R%d R%d R%d", reg_1, reg_2, reg_3);
                    break;
            case TIMES :
                    printf ("times R%d R%d R%d", reg_1, reg_2, reg_3);
                    break;
            case DIV :
                    printf ("div R%d R%d R%d", reg_1, reg_2, reg_3);
                    break;
            case MOD :
                    printf ("mod R%d R%d R%d", reg_1, reg_2, reg_3);
                    break;
            case NEG :
                    printf ("neg R%d R%d", reg_1, reg_2);
                    break;
            case ABS :
                    printf ("abs R%d R%d", reg_1, reg_2);
                    break;
            case SHL :
                    printf ("shl R%d R%d R%d", reg_1, reg_2, reg_3);
                    break;
            case SHR :
                    printf ("shr R%d R%d R%d", reg_1, reg_2, reg_3);
                    break;
            case BAND :
                    printf ("band R%d R%d R%d", reg_1, reg_2, reg_3);
                    break;
            case BOR :
                    printf ("bor R%d R%d R%d", reg_1, reg_2, reg_3);
                    break;
            case BXOR :
                    printf ("bxor R%d R%d R%d", reg_1, reg_2, reg_3);
                    break;
            case BNEG :
                    printf ("bneg R%d R%d", reg_1, reg_2);
                    break;
            case AND :
                    printf ("and R%d R%d R%d", reg_1, reg_2, reg_3);;
                    break;       
            case OR :
                    printf ("or R%d R%d R%d", reg_1, reg_2, reg_3);
                    break;
            case NOT :
                    printf ("not R%d R%d", reg_1, reg_2);
                    break;
            case EQ :
                    printf ("eq R%d R%d %05d", reg_1, reg_2, 
                            memory_or_literal);
                    break;
            case NEQ : 
                    printf ("neq R%d R%d %05d", reg_1, reg_2, 
                            memory_or_literal);
                    break;
            case LE :
                    printf ("le R%d R%d %05d", reg_1, reg_2, 
                            memory_or_literal);;
                    break;
            case LT :
                    printf ("lt R%d R%d %05d", reg_1, reg_2, 
                            memory_or_literal);
                    break;
            case GE :
                    printf ("ge R%d R%d %05d", reg_1, reg_2, 
                            memory_or_literal);
                    break;
            case GT :
                    printf ("gt R%d R%d %05d", reg_1, reg_2, 
                            memory_or_literal);
                    break;
            case MOVE :
                    printf ("move R%d R%d", reg_1, reg_2);
                    break;
            case LW :
                    printf ("lw R%d %05d", reg_1, memory_or_literal);
                    break;
            case SW :
                    printf ("sw R%d %05d", reg_1, memory_or_literal);
                    break;
            case LI :
                    printf ("li R%d %d", reg_1, memory_or_literal);
                    break;
            case SYS : {
                    /*no register is used for extension 4 
                    thus only the extension is printed*/
                    if (extension == 4)
                            printf ("sys %d", extension);
                    else
                            printf ("sys %d R%d", extension, reg_1);
                }
        }
                
}
