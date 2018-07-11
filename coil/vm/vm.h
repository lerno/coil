#pragma once

#include "chunk.h"

#define STACK_MAX 100
typedef struct
{
	Chunk *chunk;
	uint8_t *ip;
	vm_value stack[STACK_MAX];
	vm_value *stack_top;
} VM;


typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} interpret_result;


void init_vm(void);
void free_vm(void);
interpret_result interpret(char *source);
