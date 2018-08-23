#include <stdio.h>
#include "debug.h"
#include "chunk.h"
#include "opcodes.h"


void disassemble_chunk(Chunk *chunk, char *name)
{
	printf("== %s ==\n", name);
	for (int i = 0; i < chunk->size;)
	{
		i = disassemble_instruction(chunk, i);
	}
}

static inline int simple_instruction(char *name, int sp)
{
	printf("%s\n", name);
	return sp + 1;
}


static inline int constant_instruction(const char *name, Chunk *chunk, int sp)
{
	uint8_t constant = chunk->code[sp + 1];
	printf("%-16s %4d '", name, constant);
	vm_value_print(chunk->constants.values[constant]);
	printf("'\n");
	return sp + 2;
}


int disassemble_instruction(Chunk *chunk, int sp)
{
	printf("%04d ", sp);
	if (sp > 0 && chunk->lines[sp] == chunk->lines[sp - 1])
	{
		printf("   | ");
	}
	else
	{
		printf("%4d ", chunk->lines[sp]);
	}
	uint8_t instruction = chunk->code[sp];
	switch (instruction)
	{
		case OP_RETURN:
			return simple_instruction("OP_RETURN", sp);
		case OP_CONSTANT:
			return constant_instruction("OP_CONSTANT", chunk, sp);
		case OP_SUB:
			return simple_instruction("OP_SUB", sp);
		case OP_MULT:
			return simple_instruction("OP_MULT", sp);
		case OP_NEGATE:
			return simple_instruction("OP_NEGATE", sp);
		case OP_ADD:
			return simple_instruction("OP_ADD", sp);
		case OP_DIV:
			return simple_instruction("OP_DIV", sp);
		case OP_EQUAL:
			return simple_instruction("OP_EQUAL", sp);
		case OP_GREATER:
			return simple_instruction("OP_GREATER", sp);
		case OP_LESS:
			return simple_instruction("OP_LESS", sp);
		case OP_NOT:
			return simple_instruction("OP_NOT", sp);
        case OP_BIT_NOT:
            return simple_instruction("OP_BIT_NOT", sp);
		case OP_BIT_XOR:
			return simple_instruction("OP_BIT_XOR", sp);
		case OP_BIT_AND:
			return simple_instruction("OP_BIT_AND", sp);
		case OP_BIT_OR:
			return simple_instruction("OP_BIT_OR", sp);
		case OP_RIGHT_SHIFT:
			return simple_instruction("OP_RIGHT_SHIFT", sp);
		case OP_RIGHT_SHIFT_LOGIC:
			return simple_instruction("OP_RIGHT_SHIFT_LOGIC", sp);
		case OP_LEFT_SHIFT:
			return simple_instruction("OP_LEFT_SHIFT", sp);
		case OP_POW:
			return simple_instruction("OP_POW", sp);
		case OP_OR:
			return simple_instruction("OP_OR", sp) + 1;
		case OP_AND:
			return simple_instruction("OP_AND", sp) + 1;
		case OP_ASSIGN:
			return simple_instruction("OP_ASSIGN", sp) + 1;
		case OP_TRUE:
			return simple_instruction("OP_TRUE", sp);
		case OP_NIL:
			return simple_instruction("OP_NIL", sp);
		case OP_FALSE:
			return simple_instruction("OP_FALSE", sp);
		default:
			printf("Unknown opcode %d\n", instruction);
			return sp + 1;
	}
}
