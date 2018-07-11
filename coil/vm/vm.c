//
// Created by Christoffer Lernö on 2018-07-09.
// Copyright (c) 2018 Christoffer Lernö. All rights reserved.
//

#include <stdio.h>
#include "vm.h"
#include "opcodes.h"
#include "chunk.h"
#include "vmvalue.h"
#include "debug.h"
#include "compiler.h"
VM vm;

static inline void reset_stack()
{
	vm.stack_top = vm.stack;
}

void init_vm()
{
	reset_stack();
}

void free_vm()
{

}

void push(vm_value value)
{
	*(vm.stack_top++) = value;
}

vm_value pop()
{
	return *(--vm.stack_top);
}

static inline interpret_result run()
{
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do { \
      double b = pop(); \
      double a = pop(); \
      push(a op b); \
    } while (false)

	while (true)
	{
#ifdef DEBUG_EXECUTION
		printf("          ");
		for (vm_value *slot = vm.stack; slot < vm.stack_top; slot++)
		{
			printf("[ ");
			vm_value_print(*slot);
			printf(" ]");
		}
		printf("\n");
		disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
		uint8_t instruction;
		switch (instruction = READ_BYTE())
		{
			case OP_RETURN:
				vm_value_print(pop());
				printf("\n");
				return INTERPRET_OK;
			case OP_NEGATE:
				push(-pop());
				break;
			case OP_ADD:
				BINARY_OP(+);
				break;
			case OP_SUB:
				BINARY_OP(-);
				break;
			case OP_MULT:
				BINARY_OP(*);
				break;
			case OP_DIV:
				BINARY_OP(/);
				break;
			case OP_CONSTANT:
				push(READ_CONSTANT());
				break;
			default:
				printf("Unknown instruction\n");
				return INTERPRET_RUNTIME_ERROR;
		}
	}
#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-stack-address"
interpret_result interpret(char *source)
{
	Chunk chunk;
	chunk_init(&chunk);
	if (!compile(source, &chunk))
	{
		chunk_free(&chunk);
		return INTERPRET_COMPILE_ERROR;
	}

	vm.chunk = &chunk;
	vm.ip = vm.chunk->code;
	interpret_result result = run();
	chunk_free(&chunk);
	return result;
}
#pragma clang diagnostic pop
