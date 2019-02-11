/*     __    ___________ __       ___                             __    __     __
 *    / /   / ____/__  // /_     /   |  _____________  ____ ___  / /_  / /__  / /
 *   / /   / /     /_ </ __ \   / /| | / ___/ ___/ _ \/ __ `__ \/ __ \/ / _ \/ / 
 *  / /___/ /___ ___/ / /_/ /  / ___ |(__  |__  )  __/ / / / / / /_/ / /  __/_/  
 * /_____/\____//____/_.___/  /_/  |_/____/____/\___/_/ /_/ /_/_.___/_/\___(_)   
 * Ben Belov [bvb328] | Tianshu Huang [th28794] | EE460N Spring 2019
 */


/*   _ _ _          _         _         _        
 *  | (_) |__  __  (_)_ _  __| |_  _ __| |___ ___
 *  | | | '_ \/ _| | | ' \/ _| | || / _` / -_|_-<
 *  |_|_|_.__/\__| |_|_||_\__|_|\_,_\__,_\___/__/
 */                                              

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>


/*                      _                _            
 *   _  _ ___ ___ _ _  | |_  ___ __ _ __| |___ _ _ ___
 *  | || (_-</ -_) '_| | ' \/ -_) _` / _` / -_) '_(_-<
 *   \_,_/__/\___|_|   |_||_\___\__,_\__,_\___|_| /__/
 */                                                   

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

void UndefinedLabelException();
void InvalidOpcodeException();
void InvalidConstantException();
void OtherException();

#endif


/*******************************************************************************/


#ifndef SYMBOL_H
#define SYMBOL_H



enum argtype_t { REG, IMM, LABEL, NONE, REG_OR_IMM };


struct argument_t {
	enum argtype_t type;
	int value;
	char *label;
};

struct line_t {
	char* opcode;
	uint16_t address;
	struct argument_t *args;
	struct line_t *next;
};


struct symbol_t {
	char* name;
	uint16_t address;
	struct symbol_t* next;
};


struct instruction_t {
	uint16_t opcode;
	const char* name;
	uint16_t (*instructionAssemble)(
		uint16_t opcode,
		struct line_t *line,
		struct symbol_t* symbolTable);
	enum argtype_t types[3];
};


#endif


/*******************************************************************************/



#ifndef TABLE_H
#define TABLE_H

/** Symbol Table */
void addSymbol(struct symbol_t *root, char *name, uint16_t address);
void deleteTable(struct symbol_t *node);
uint16_t search(struct symbol_t *root, char *target);

/** Instruction Buffer */
void addLine(
	struct line_t *root, char *opcode,
	uint16_t address, struct argument_t *args);
void deleteBuffer(struct line_t *node);


#endif


/*******************************************************************************/


#define MAX_LINE_LENGTH 255

#define NO_ORIG -1
#define END_SUCC -2



/**
 * Parse a single line
 * @param file - input file pointer
 * @param address - current memory address (for building symbol table)
 * @param symbolTable - symbol table to add to
 * @param lineBuffer - line buffer to add to
 * @return false if EOF
 */
bool parseLine(
	FILE *file, int *address,
	struct symbol_t *symbolTable, struct line_t *lineBuffer);


/*******************************************************************************/


#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

uint16_t assembleLine(struct line_t *line, struct symbol_t *symbolTable);
bool isOpcode(char *str);
uint16_t orig(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable);

#endif


/*******************************************************************************/


#ifndef ARGUMENT_H
#define ARGUMENT_H


struct argument_t *strtoarg(char *str, struct argument_t *arg);

#endif


/*                   _     
 *   __   __ ___  __| |___ 
 *  / _| / _/ _ \/ _` / -_)
 *  \__| \__\___/\__,_\___|
 */                        
/**
 * exceptions.c : standard exceptions in order to manage throwing
 */


/**
 * Exit code 1: undefined label
 */
void UndefinedLabelException() {
	printf("UndefinedLabelException\n");
	exit(1);
}


/**
 * Exit code 2: invalid opcode
 */
void InvalidOpcodeException() {
	printf("InvalidOpcodeException\n");
	exit(2);
}


/**
 * Exit code 3: invalid constant (constant not in valid range)
 */
void InvalidConstantException() {
	printf("InvalidConstantException\n");
	exit(3);
}


/**
 * Other exception:
 *	- wrong operand type
 *	- wrong number of operands
 *	- miscallaneous shenanigans
 */
void OtherException() {
	printf("OtherException\n");
	exit(4);
}


/*******************************************************************************/

/** 
 * table.c : symbol table and line buffer manipulation routines
 */



/**
 * Add new symbol to symbol table
 * @param root - root node of symbol table
 * @param name - name of symbol to add (should be malloc'd string)
 * @param address - address of symbol
 */
void addSymbol(struct symbol_t *root, char *name, uint16_t address) {
    struct symbol_t *newSymbol = malloc(sizeof(struct symbol_t));
    newSymbol->name = name;
    newSymbol->address = address;
    newSymbol->next = 0;
    while(root->next != 0) { root = root->next; }
    root->next = newSymbol;
}


/**
 * Search symbol table for target symbol
 * @param node - symbol table root node
 * @param target - target symbol
 * @return address of target symbol
 */
uint16_t search(struct symbol_t* node, char* target) {
    while(node != 0) {
        if(strcmp(node->name, target) == 0) { return node->address; }
        node = node->next;
    }
    printf("Label %s not found.\n", target);
    UndefinedLabelException();
}


/**
 * Delete symbol table
 * @param node - root node of table to delete
 */
void deleteTable(struct symbol_t *node) {
    while(node != 0) {
        free(node->name);
        struct symbol_t *prev = node;
        node = node->next;
        free(prev);
    }
}


/**
 * Add new line
 * @param root - root node of program buffer LL
 * @param label - string containing (optional) label
 * @param opcode - string containing line opcode
 * @param address - computed line address
 * @param args - array containing line arguments
 */
void addLine(
    struct line_t *root, char *opcode, 
    uint16_t address, struct argument_t *args)
{
    struct line_t *next = malloc(sizeof(struct line_t));
    next->opcode = opcode;
    next->address = address;
    next->args = args;
    next->next = 0;
    while(root->next != 0) { root = root->next; }
    root->next = next;
}


/**
 * Delete program line buffer
 */
void deleteBuffer(struct line_t *node) {

    while(node != 0) {
        free(node->opcode);
        free(node->args);
        struct line_t *prev = node;
        node = node->next;
        free(prev);
    }
}


/*******************************************************************************/

/**
 * parse.c : line parsing functions
 */


/** 
 * Convert null terminated string to upper case
 * @param str - string to convert
 */
void toUpper(char *str) {
    for(int i = 0; i < strlen(str); i++) {
        str[i] = toupper(str[i]);
    }
}

/**
 * Set end of line to NULL and return EOL ptr
 * @param ptr - string to find end of line for
 * @return ptr to EOL
 */
char *setLineEnd(char *ptr) {
    for(; *ptr != ';' && *ptr != '\0' && *ptr!= '\n'; ptr++) {}    
    *ptr = '\0';
    return ptr;
}


/**
 * Add args to line_t struct and add to line buffer
 * @param char_ptr - string to convert
 * @param address - current instruction memory address
 * @param lineBuffer - line buffer to append to
 * @return pointer to opcode string
 */
char *setArgs(char *char_ptr, uint16_t address, struct line_t *lineBuffer) {

    // Set opcode
    char *opcode = malloc(sizeof(char) * (strlen(char_ptr) + 1));
    strcpy(opcode, char_ptr);

    // Parse args
    struct argument_t *args = malloc(3 * sizeof(struct argument_t));
    for(int i = 0; i < 3; i++) {
        char_ptr = strtok(NULL, "\t\n ,");
        if(char_ptr != 0) {
            args[i].label = malloc(sizeof(char) * (strlen(char_ptr) + 1));
            strcpy(args[i].label, char_ptr);
        }
        strtoarg(char_ptr, &args[i]);
    }

    // Add line
    addLine(lineBuffer, opcode, address, args);
    
    return opcode;
}


/**
 * Add to symbol table
 * @param char_ptr - string to use as symbol
 * @param address - address of symbol
 * @param symbolTable - table to append to
 * @return updated string pointer
 */
char *setSymbol(char *char_ptr, uint16_t address, struct symbol_t *symbolTable) {

    if(
        !isalpha(char_ptr[0]) ||
        char_ptr[0] == 'X' ||
        (strcmp(char_ptr, "IN") == 0) ||
        (strcmp(char_ptr, "OUT") == 0) ||
        (strcmp(char_ptr, "PUTS") == 0) ||
        (strcmp(char_ptr, "GETC") == 0)) { OtherException(); }
    for(int i = 0; char_ptr[i] != 0; i++) {
        if(!isalnum(char_ptr[i])) { OtherException(); }
    }


    char *label = malloc(sizeof(char) * (strlen(char_ptr) + 1));
    strcpy(label, char_ptr);
    addSymbol(symbolTable, label, address);
}


bool parseLine(
	FILE *file, int *address, struct symbol_t *symbolTable, struct line_t *lineBuffer) {

    char *char_ptr;
    char line[MAX_LINE_LENGTH + 1];

    // Check for EOF
    if(!fgets(line, MAX_LINE_LENGTH, file)) { return false; };

    // Convert to upper case
    toUpper(line);
    // Set EOL to NULL
    char_ptr = setLineEnd(line);

    // Check for empty Line
    if(!(char_ptr = strtok(line, "\t\n ,"))) { return true; }

    // Found Label => add to symbol table
    if(isOpcode(char_ptr) == false && char_ptr[0] != '.') {
        char_ptr = setSymbol(char_ptr, *address, symbolTable);
        if(!(char_ptr = strtok(NULL, "\t\n ,"))) { return true; }
    }

    if(strcmp(char_ptr, ".END") == 0) {
        if(*address > 0xFFFF + 2) { OtherException(); }
        *address = END_SUCC;
        return false;
    }

    // Set up args and add line
    char *opcode = setArgs(char_ptr, *address, lineBuffer);

    // Check for .ORIG and .END
    if(*address == NO_ORIG) {
        if(strcmp(opcode, ".ORIG") == 0) {
            *address = orig(0x0000, lineBuffer->next, symbolTable);
            printf("Set .orig to 0x%.4X\n", *address);
            return true;
        }
        else
            OtherException(); // Instruction prior to .ORIG
    }

    *address += 2;

    return true;
}


/*******************************************************************************/

/**
 * instructions.c : Functions to assemble a single instruction
 */


/**
 * Helper function to assemble opcode, DR, and SR1
 * @param opcode - instruction opcode
 * @param line - line struct encoding current line
 */
uint16_t op_dr_sr1(uint16_t opcode, struct line_t* line) {
    return (
        opcode | ((line->args)[0].value << 9) |
        ((line->args)[1].value << 6));
}


/**
 * ALU instruction (ADD, AND, XOR) assembler
 * @param opcode - instruction opcode (expanded to include all constant portions
 *      of the instruction to account for different operations that share the same
 *      opcode)
 * @param line - line struct encoding current line
 * @param symbolTable - pointer to root node of symbol table
 * @return assembled line (16-bit )
 */
uint16_t alu(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable) {

    uint16_t base = op_dr_sr1(opcode, line);
    if((line->args)[2].type == REG) {
        return base | (0 << 5) | (line->args)[2].value;
    }
    else if((line->args)[2].type == IMM) {
        int imm5 = (line->args)[2].value;
        if(imm5 > 15 || imm5 < -16) { InvalidConstantException(); }
        return base | (1 << 5) | (0x1 << 12) | (imm5 & 0x1F);
    }
    else {
        OtherException();
    }
}


/**
 * Shift instruction (LSHF, RSHFL, RSHFA) assembler
 * @param opcode - instruction opcode (expanded to include all constant portions
 *      of the instruction to account for different operations that share the same
 *      opcode)
 * @param line - line struct encoding current line
 * @param symbolTable - pointer to root node of symbol table
 * @return assembled line (16-bit )
 */
uint16_t shift(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable) {
    int amount4 = (line->args)[2].value;
    if(amount4 < 0 || amount4 > 15) { InvalidConstantException(); }
    return(op_dr_sr1(opcode, line) | amount4 << 0);
}


/**
 * Memory instruction (LDB, LDW, STB, STW) assembler
 * @param opcode - instruction opcode (expanded to include all constant portions
 *      of the instruction to account for different operations that share the same
 *      opcode)
 * @param line - line struct encoding current line
 * @param symbolTable - pointer to root node of symbol table
 * @return assembled line (16-bit )
 */
uint16_t memory(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable) {
    int boffset6 = (line->args)[2].value;
    if(boffset6 < -32 || boffset6 > 31) { InvalidConstantException(); }
    return(op_dr_sr1(opcode, line) | (boffset6 & 0x3F));
}


/**
 * NOT instruction
 * @param opcode - instruction opcode (expanded to include all constant portions
 *      of the instruction to account for different operations that share the same
 *      opcode)
 * @param line - line struct encoding current line
 * @param symbolTable - pointer to root node of symbol table
 * @return assembled line (16-bit )
 */
uint16_t not(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable) {
    return(op_dr_sr1(opcode, line));
}


/**
 * Constant (0-argument) instruction (RET, RTI)
 * @param opcode - instruction opcode (expanded to include all constant portions
 *      of the instruction to account for different operations that share the same
 *      opcode)
 * @param line - line struct encoding current line
 * @param symbolTable - pointer to root node of symbol table
 * @return assembled line (16-bit )
 */
uint16_t constant(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable) {
    return opcode;
}


/**
 * TRAP instruction
 * @param opcode - instruction opcode (expanded to include all constant portions
 *      of the instruction to account for different operations that share the same
 *      opcode)
 * @param line - line struct encoding current line
 * @param symbolTable - pointer to root node of symbol table
 * @return assembled line (16-bit )
 */
uint16_t trap(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable) {
    int trapvec = (line->args)[0].value;
    if(trapvec < 0 || trapvec > 0xFF || (line->args)[0].label[0] != 'X') { InvalidConstantException(); }
    return opcode | trapvec;
}


/**
 * Branch to register instructions (JMP, JSRR)
 * @param opcode - instruction opcode (expanded to include all constant portions
 *      of the instruction to account for different operations that share the same
 *      opcode)
 * @param line - line struct encoding current line
 * @param symbolTable - pointer to root node of symbol table
 * @return assembled line (16-bit )
 */
uint16_t jmp(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable) {
    return opcode | ((line->args)[0].value << 6);
}


/**
 * JSR instruction
 * @param opcode - instruction opcode (expanded to include all constant portions
 *      of the instruction to account for different operations that share the same
 *      opcode)
 * @param line - line struct encoding current line
 * @param symbolTable - pointer to root node of symbol table
 * @return assembled line (16-bit )
 */
uint16_t jsr(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable) {
    int addr = (search(symbolTable, ((line->args)[0].label)) - line->address - 2) / 2;
    if(addr > 1023 || addr < -1024) { InvalidConstantException(); }
    return (opcode | (addr & 0x7FF));
}


/**
 * LEA instruction
 * @param opcode - instruction opcode (expanded to include all constant portions
 *      of the instruction to account for different operations that share the same
 *      opcode)boffset6
 * @param line - line struct encoding current line
 * @param symbolTable - pointer to root node of symbol table
 * @return assembled line (16-bit )
 */
uint16_t lea(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable) {
    int addr = (search(symbolTable, ((line->args)[1].label)) - line->address - 2) / 2;
    if(addr > 255 || addr < -256) { InvalidConstantException(); }
    return (opcode | ((line->args)[0].value << 9) | addr & 0x01FF);
}

    
/**
 * BR instruction and variants
 * @param opcode - instruction opcode (expanded to include all constant portions
 *      of the instruction to account for different operations that share the same
 *      opcode)
 * @param line - line struct encoding current line
 * @param symbolTable - pointer to root node of symbol table
 * @return assembled line (16-bit )
 */
uint16_t br(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable) {
    int addr = (search(symbolTable, ((line->args)[0].label)) - line->address - 2) / 2;
    if(addr > 255 || addr < -256) { printf("%d\n", addr); InvalidConstantException(); }
    return (opcode | 0x01FF & addr);
}


/**
 * .FILL pseudo-op
 * @param opcode - instruction opcode (expanded to include all constant portions
 *      of the instruction to account for different operations that share the same
 *      opcode)
 * @param line - line struct encoding current line
 * @param symbolTable - pointer to root node of symbol table
 * @return assembled line (16-bit )
 */
uint16_t fill(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable) {
    int value = (line->args)[0].value;
    if((line->args)[0].type != IMM || value > 32767 || value < -32768) {
        InvalidConstantException();
    }
    return value & 0xFFFF;
}


uint16_t orig(uint16_t opcode, struct line_t* line, struct symbol_t* symbolTable) {
	uint16_t value = (line->args)[0].value;
	if(value % 2 != 0 || value > 0xFFFF || value < 0) { InvalidConstantException(); }
	return value & 0xFFFF;
}


struct instruction_t instructions[30] = {
    {0x1000, "ADD",   *alu,      {REG,   REG,   REG_OR_IMM}},
    {0x5000, "AND",   *alu,      {REG,   REG,   REG_OR_IMM}},
    {0x0E00, "BR",    *br,       {LABEL, NONE,  NONE}},
    {0x0800, "BRN",   *br,       {LABEL, NONE,  NONE}},
    {0x0400, "BRZ",   *br,       {LABEL, NONE,  NONE}},
    {0x0200, "BRP",   *br,       {LABEL, NONE,  NONE}},
    {0x0C00, "BRNZ",  *br,       {LABEL, NONE,  NONE}},
    {0x0A00, "BRNP",  *br,       {LABEL, NONE,  NONE}},
    {0x0600, "BRZP",  *br,       {LABEL, NONE,  NONE}},
    {0x0E00, "BRNZP", *br,       {LABEL, NONE,  NONE}},
    {0xC000, "JMP",   *jmp,      {REG,   NONE,  NONE}},
    {0x4800, "JSR",   *jsr,      {LABEL, NONE,  NONE}},
    {0x4000, "JSRR",  *jmp,      {REG,   NONE,  NONE}},
    {0x2000, "LDB",   *memory,   {REG,   REG,   IMM}},
    {0x6000, "LDW",   *memory,   {REG,   REG,   IMM}},
    {0xE000, "LEA",   *lea,      {REG,   LABEL, NONE}},
    {0x903F, "NOT",   *not,      {REG,   REG,   NONE}},
    {0xC1C0, "RET",   *constant, {NONE,  NONE,  NONE}},
    {0x8000, "RTI",   *constant, {NONE,  NONE,  NONE}},
    {0xD000, "LSHF",  *shift,    {REG,   REG,   IMM}},
    {0xD010, "RSHFL", *shift,    {REG,   REG,   IMM}},
    {0xD030, "RSHFA", *shift,    {REG,   REG,   IMM}},
    {0x3000, "STB",   *memory,   {REG,   REG,   IMM}},
    {0x7000, "STW",   *memory,   {REG,   REG,   IMM}},
    {0xF000, "TRAP",  *trap,     {IMM,   NONE,  NONE}},
    {0x9000, "XOR",   *alu,      {REG,   REG,   REG_OR_IMM}},
    {0xF025, "HALT",  *constant, {NONE,  NONE,  NONE}},
    {0x0000, "NOP",   *constant, {NONE,  NONE,  NONE}},
    {0x0000, ".FILL", *fill,     {IMM,   NONE,  NONE}},
	{0x0000, ".ORIG", *orig,     {IMM,   NONE,  NONE}}
};


/**
 * Check operand types; throws OtherException if violation found
 * @param line - line to check
 * @param ins - instruction to check against
 */
void checkTypes(struct line_t *line, struct instruction_t *ins) {
    for(int i = 0; i < 3; i++) {

        if((ins->types)[i] == REG_OR_IMM) {
            if((line->args)[i].type != REG && (line->args)[i].type != IMM) {
                printf(
                    "Operand #%d of %s is %s, should be REG_OR_IMM\n",
                    i + 1, line->opcode, (line->args)[i].label);
                OtherException();
            }
        }
        else if((line->args)[i].type != (ins->types)[i]) {
            const char *typename;
            if((ins->types)[i] == REG) { typename = "REG"; }
            else if((ins->types)[i] == IMM) { typename = "IMM"; }
            else if((ins->types)[i] == LABEL) { typename = "LABEL"; }
            else { typename = "NONE"; }

            printf(
                "Operand #%d of %s is %s, should be %s\n",
                i + 1, line->opcode, (line->args)[i].label, typename);
            OtherException();
        }
    }    
}


/**
 * Assemble a single line
 * @param line - line to assemble
 * @param symbolTable - symbol table reference
 * @return line as 16-bit integer
 */
uint16_t assembleLine(struct line_t *line, struct symbol_t *symbolTable) {

    for(int i = 0; i < 30; i++) {
        if(strcmp(instructions[i].name, line->opcode) == 0) {

            checkTypes(line, &instructions[i]);
            return instructions[i].instructionAssemble(
                instructions[i].opcode, line, symbolTable);
        }
    }
    printf("Invalid opcode: %s\n", line->opcode);
    InvalidOpcodeException();
}


/**
 * Check if string is opcode
 * @param str - string to check
 */
bool isOpcode(char *str) {
    for(int i = 0; i < 30; i++) {
        if(strcmp(instructions[i].name, str) == 0) {
            return true;
        }
    }
    return false;
}


/*******************************************************************************/

/**
 * argument.c : argument parsing functions
 */



/**
 * Parse decimal string
 * @param numStr - input string
 * @return integer value
 */
int parseDecimal(char *numStr) {

    char *t_ptr = numStr;
    int t_length, k;
    int lNum = 0;
    long int lNumLong;

    // Negative
    if(*numStr == '-') { t_ptr++; }

    t_length = strlen(t_ptr);
    for(k=0; k < t_length; k++) {
        if(!isdigit(*t_ptr)) { InvalidConstantException(); }
        t_ptr++;
    }
    lNum = atoi(numStr);

    return lNum;
}


/**
 * Parse hex string
 * @param numStr - input string
 * @return integer value
 */
int parseHex(char *numStr) {

    char *t_ptr = numStr;
    int t_length, k;
    int lNum = 0;
    long int lNumLong;

    // Negative
    if(*numStr == '-') { t_ptr++; }

    t_length = strlen(t_ptr);

    for(k=0; k < t_length; k++) {
        if (!isxdigit(*t_ptr)) { InvalidConstantException(); }
        t_ptr++;
    }
    lNumLong = strtol(numStr, NULL, 16);
    lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
    // May need to check exceptions here
    lNum = (lNumLong < INT_MIN) ? INT_MIN : lNumLong;

    return lNum;
}


/**
 * Convert string into argument type
 * @param str - argument string
 * @param arg - destination argument
 */
struct argument_t *strtoarg(char *str, struct argument_t *arg) {
    if (str == 0) {
        arg->type = NONE;
        arg->value = 0; 
    }
    else if(*str == '#') {
        arg->type = IMM;
        arg->value = parseDecimal(str + 1);
    }
    else if(*str == 'X') {
        arg->type = IMM;
        arg->value = parseHex(str + 1);
    }
    else if(*str == 'R') {
        arg->type = REG;
        arg->value = (*(str + 1)) - '0';
        if(arg->value < 0 || arg->value > 7) { OtherException(); }
    }
    else if(*str == '\n' || *str == 0 || *str == ';') {
        arg->type = NONE;
        arg->value = 0;
    }
    else {
        arg->type = LABEL;
        arg->value = 0;
    }
   
}


/*******************************************************************************/

/**
 * assembler.c : main function
 */

#define __NAME__ "LC3b Assemble!"
#define __AUTHOR__ "Ben Belov [bvb328] | Tianshu Huang [th28794] | EE460N Spring 2019"



FILE* infile = NULL;
FILE* outfile = NULL;

int main(int argc, char* argv[]) {

    printf("\33[91m\33[1m"
"   _     ____ _____ _                                                  \n"
"  | |   / ___|___ /| |__                             _    _     _      \n"   
"  | |  | |     |_ \\| '_ \\      __ _ ______ ___ _ __ | |__| |___| |   \n"
"  | |__| |___ ___) | |_) |    / _` (_-<_-</ -_) '  \\| '_ \\ / -_)_|   \n"
"  |_____\\____|____/|_.__/     \\__,_/__/__/\\___|_|_|_|_.__/_\\___(_) \n"
"  Ben Belov, Tianshu Huang  |  EE 460N, Spring 2019\n\n\33[0m");

    infile = fopen(argv[1], "r");
    outfile = fopen(argv[2], "w");
  
    if(!infile) {
        printf("Error: Cannot open file %s\n", argv[1]);
        OtherException();
    }
    if(!outfile) {
        printf("Error: Cannot open file %s\n", argv[2]);
        OtherException();
    }

    struct symbol_t symbolTable;
    symbolTable.next = 0;
    struct line_t lineBuffer;
    lineBuffer.next = 0;

    int currentaddr = NO_ORIG;
    while(parseLine(infile, &currentaddr, &symbolTable, &lineBuffer)) {}

    printf("Finished first pass.\n");

    if(currentaddr != END_SUCC) {
        printf("No .END\n");
        OtherException();
    }

    struct symbol_t *node = symbolTable.next;
    printf("Symbol Table\n------------\n");
    while(node != 0) {
        printf("0x%.4X : %s\n", node->address, node->name);
        node = node->next;
    }
    printf("\n");

    struct line_t *line = lineBuffer.next;
    for(; line != 0; line = line->next) {
        // printf("0x%.4X\n", assembleLine(line, symbolTable.next));
        fprintf(outfile, "0x%.4X\n", assembleLine(line, symbolTable.next));
    }

    printf("Finished second pass.\n");

    deleteTable(symbolTable.next);
    deleteBuffer(lineBuffer.next);

    fclose(infile);
    fclose(outfile);

    printf("Success!\n");
}
