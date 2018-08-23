//
// Created by Christoffer Lernö on 2018-07-09.
// Copyright (c) 2018 Christoffer Lernö. All rights reserved.
//

#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include "vm.h"
#include "opcodes.h"
#include "debug.h"
#include "compiler.h"
#include "vmvalue.h"

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

void push(Value value)
{
	*(vm.stack_top++) = value;
}

Value pop()
{
	return *(--vm.stack_top);
}

static Value peek(int distance)
{
  return vm.stack_top[-1 - distance];
}

static inline double fast_double_exp(double value, int64_t exp)
{
	double result = 1;
	if (exp == 0) return 1;
	if (exp == 1) return value;
	double mult = value * value;
	while (exp > 0)
	{
		if (exp & 1)
		{
			result *= value;
			exp--;
		}
		else
		{
			result *= mult;
			exp -= 2;
		}
	}
	return result;
}

static inline int64_t fast_int_exp(int64_t value, int64_t exp)
{
	int64_t result = 1;
	if (exp == 0) return 1;
	if (exp == 1) return value;
	int64_t mult = value * value;
	while (exp > 0)
	{
		if (exp & 1)
		{
			result *= value;
			exp--;
		}
		else
		{
			result *= mult;
			exp -= 2;
		}
	}
	return result;
}

static Value fast_value_exp(Value *value, int64_t exp)
{
	if (IS_FLOAT(*value))
	{
		return FLOAT_VAL(fast_double_exp(AS_FLOAT(*value), exp));
	}
	if (IS_INT(*value))
	{
		return INT_VAL(fast_int_exp(AS_INT(*value), exp));
	}
	if (IS_BOOL(*value))
	{
		return INT_VAL(AS_BOOL(*value) ? 1 : 0);
	}
	return INT_VAL(0);
}

static void runtimeError(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = vm.ip - vm.chunk->code;
	fprintf(stderr, "[line %d] in script\n", vm.chunk->lines[instruction]);

	reset_stack();
}


static inline interpret_result run()
{
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do { \
	  Value a = pop(); \
	  Value b = pop(); \
	  if (IS_NIL(a) || IS_NIL(b)) { \
        runtimeError("Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
	  } \
	  if (IS_FLOAT(a) || IS_FLOAT(b)) { \
         FLOAT_CAST(a); FLOAT_CAST(b); \
      } else { \
	  	 INT_CAST(a); INT_CAST(b); \
	  } \
	  if (IS_INT(a)) { \
	     push(INT_VAL(AS_INT(b) op AS_INT(a))); \
	  } else { \
	     push(FLOAT_VAL(AS_FLOAT(b) op AS_FLOAT(a))); \
	  } \
    } while (false)

#define BINARY_OP_TO_BOOL(op) \
    do { \
	  Value a = pop(); \
	  Value b = pop(); \
	  if (IS_NIL(a) || IS_NIL(b)) { \
        runtimeError("Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
	  } \
	  if (IS_FLOAT(a) || IS_FLOAT(b)) { \
         FLOAT_CAST(a); FLOAT_CAST(b); \
      } else { \
	  	 INT_CAST(a); INT_CAST(b); \
	  } \
	  if (IS_INT(a)) { \
	     push(BOOL_VAL(AS_INT(b) op AS_INT(a))); \
	  } else { \
	     push(BOOL_VAL(AS_FLOAT(b) op AS_FLOAT(a))); \
	  } \
    } while (false)

#define BINARY_OP_INT(op) \
    do { \
      Value a = pop(); \
      Value b = pop(); \
	  INT_CAST(a); INT_CAST(b); \
      push(INT_VAL(AS_INT(b) op AS_INT(a))); \
    } while (false)

#define BINARY_OP_BOOL(op) \
    do { \
      Value a = pop(); \
      Value b = pop(); \
	  BOOL_CAST(a); BOOL_CAST(b); \
      push(BOOL_VAL(AS_BOOL(b) op AS_BOOL(a))); \
    } while (false)

	while (true)
	{
#ifdef DEBUG_TRACE_EXECUTION
		printf("          ");
		for (Value *slot = vm.stack; slot < vm.stack_top; slot++)
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
			// DISCUSS MOD / REM positive negative
			case OP_RETURN:
				vm_value_print(pop());
				printf("\n");
				return INTERPRET_OK;
			case OP_FALSE:
				push(BOOL_VAL(false));
				break;
			case OP_TRUE:
				push(BOOL_VAL(true));
				break;
			case OP_NIL:
				push(NIL_VAL);
				break;
			case OP_NEGATE:
			{
				Value value1 = pop();
				switch (value1.type)
				{
					case VAL_FLOAT:
						push(FLOAT_VAL(-AS_FLOAT(value1)));
						break;
					case VAL_BOOL:
						INT_CAST(value1);
						// Fall through
					case VAL_INT:
						push(INT_VAL(-AS_INT(value1)));
						break;
					default:
						runtimeError("Attempted to negate nil");
						return INTERPRET_RUNTIME_ERROR;
				}
				break;
			}
			case OP_NOT:
			{
				Value value1 = pop();
				BOOL_CAST(value1);
				push(BOOL_VAL(!AS_BOOL(value1)));
				break;
			}
			case OP_EQUAL:
				BINARY_OP_TO_BOOL(==);
				break;
			case OP_GREATER:
				BINARY_OP_TO_BOOL(>);
				break;
			case OP_LESS:
				BINARY_OP_TO_BOOL(<);
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
			case OP_LEFT_SHIFT:
				BINARY_OP_INT(<<);
				break;
			case OP_RIGHT_SHIFT:
				BINARY_OP_INT(>>);
				break;
			case OP_OR:
			{
				Value or_value = pop();
				BOOL_CAST(or_value);
				int8_t jump = READ_BYTE();
				if (AS_BOOL(or_value))
				{
					vm.ip += jump;
					push(BOOL_VAL(true));
				}
				break;
			}
			case OP_AND:
			{
				Value and_value = pop();
				BOOL_CAST(and_value);
				int8_t jump = READ_BYTE();
				if (!AS_BOOL(and_value))
				{
					vm.ip += jump;
					push(BOOL_VAL(false));
				}
				break;
			}
			case OP_RIGHT_SHIFT_LOGIC:
			{
				Value a = pop();
				Value b = pop();
				INT_CAST(a);
				INT_CAST(b);
				push(INT_VAL((int64_t)((uint64_t)AS_INT(b) >> AS_INT(a))));
				break;
			}
			case OP_POW:
			{
				Value y = pop();
				Value x = pop();
				if (IS_INT(y) && AS_INT(y) >= 0)
				{
					push(fast_value_exp(&x, AS_INT(y)));
					break;
				}
				bool should_be_int = IS_INT(x) && IS_INT(y) && AS_INT(y) > 0;
				FLOAT_CAST(x);
				FLOAT_CAST(y);
				double result = pow(AS_FLOAT(x), AS_FLOAT(y));
				push(should_be_int ? INT_VAL((int64_t)result) : FLOAT_VAL(result));
				break;
			}
			case OP_ASSIGN:
			{
				Value value = pop();
				uint8_t local_id = READ_BYTE();
				vm.locals[local_id] = value;
				push(BOOL_VAL(true));
				break;
			}

			case OP_BIT_OR:
				BINARY_OP_INT(|);
				break;
			case OP_BIT_AND:
				BINARY_OP_INT(&);
				break;
			case OP_BIT_XOR:
				BINARY_OP_INT(^);
				break;
			case OP_BIT_NOT:
			{
				Value x = pop();
				INT_CAST(x);
				push(INT_VAL(~AS_INT(x)));
			}
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
#undef BINARY_OP_INT
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
