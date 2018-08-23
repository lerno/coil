#pragma once

#include "chunk.h"

#define STACK_MAX 100
#define VAR_MAX 256
typedef struct
{
	Chunk *chunk;
	uint8_t *ip;
	Value stack[STACK_MAX];
	Value *stack_top;
	Value locals[VAR_MAX];
} VM;


typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} interpret_result;


void init_vm(void);
void free_vm(void);
interpret_result interpret(char *source);
