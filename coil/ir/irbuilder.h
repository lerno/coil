#pragma once

#include "compiler.h"

typedef void (*ir_call)();
typedef ir_call (*ir_call_patch)(struct _IrBuilder *);

typedef struct _IrBuilder
{
	void *context;
	ir_call right_shift;
	ir_call left_shift;
	ir_call right_shift_logic;
	ir_call bit_and;
	ir_call bit_or;
	ir_call bit_xor;
	ir_call plus;
	ir_call minus;
	ir_call mult;
	ir_call div;
	ir_call pow;
	ir_call negate;
	ir_call bit_not;
	ir_call not;
	ir_call_patch bne;
	Parser *parser;
} IrBuilder;
