
#include "irbuilder.h"
#include "chunk.h"
#include "opcodes.h"


static void emit_byte(IrBuilder *builder, uint8_t byte)
{
	chunk_write((Chunk *)builder->context, byte, builder->parser->previous.line);
}


static void emit_bytes(IrBuilder *builder, uint8_t byte1, uint8_t byte2)
{
	emit_byte(builder, byte1);
	emit_byte(builder, byte2);
}


static void left_shift(IrBuilder *builder)
{
	emit_byte(builder, OP_LEFT_SHIFT);
}


static void right_shift(IrBuilder *builder)
{
	emit_byte(builder, OP_RIGHT_SHIFT);
}


static void right_shift_logic(IrBuilder *builder)
{
	emit_byte(builder, OP_RIGHT_SHIFT_LOGIC);
}


static void bit_and(IrBuilder *builder)
{
	emit_byte(builder, OP_BIT_AND);
}


static void bit_or(IrBuilder *builder)
{
	emit_byte(builder, OP_BIT_OR);
}


static void bit_xor(IrBuilder *builder)
{
	emit_byte(builder, OP_BIT_XOR);
}


static void plus(IrBuilder *builder)
{
	emit_byte(builder, OP_ADD);
}


static void minus(IrBuilder *builder)
{
	emit_byte(builder, OP_SUB);
}


static void mult(IrBuilder *builder)
{
	emit_byte(builder, OP_MULT);
}


static void divide(IrBuilder *builder)
{
	emit_byte(builder, OP_DIV);
}


static void pow(IrBuilder *builder)
{
	emit_byte(builder, OP_POW);
}


static void negate(IrBuilder *builder)
{
	emit_byte(builder, OP_NEGATE);
}


static void bit_not(IrBuilder *builder)
{
	emit_byte(builder, OP_BIT_NOT);
}


static void not(IrBuilder *builder)
{
	emit_byte(builder, OP_NOT);
}

static ir_call bne(IrBuilder *builder)
{

}

IrBuilder create_vm_irbuilder(Chunk *chunk)
{
	return {
		.context = chunk,
		.right_shift = right_shift,
		.left_shift = left_shift,
		.right_shift_logic = right_shift_logic,
		.bit_and = bit_and,
		.bit_or = bit_or,
		.bit_xor = bit_xor,
		.div = divide,
		.plus = plus,
		.minus = minus,
		.mult = mult,
		.pow = pow,
		.not = not,
		.bit_not = bit_not,
		.negate = negate,
		.bne = bne,
	};
}
