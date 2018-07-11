//
//  compiler.c
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include "compiler.h"
#include "lexer.h"
#include "opcodes.h"
#include "string.h"

typedef struct
{
	Token current;
	Token previous;
	bool had_error;
	bool panic_mode;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! - +
  PREC_CALL,        // . () []
  PREC_PRIMARY
} precedence;

Parser parser;


static void error_at(Token *token, const char *message)
{
	if (parser.panic_mode) return;
	parser.panic_mode = true;

	fprintf(stderr, "[line %d] Error", token->line);

	if (token->type == TOKEN_EOF)
	{
		fprintf(stderr, " at end");
	}
	else if (token->type == TOKEN_ERROR)
	{
		// Nothing.
	}
	else
	{
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	}

	fprintf(stderr, ": %s\n", message);
	parser.had_error = true;
}


static void error_at_current(const char *message)
{
	error_at(&parser.current, message);
}


static void error(const char *message)
{
	error_at(&parser.previous, message);
}


void advance(void)
{
	parser.previous = parser.current;

	while (1)
	{
		parser.current = scan_token();
		if (parser.current.type != TOKEN_ERROR) break;
		error_at_current(parser.current.start);
	}
}


static void consume(token_type type, const char *message)
{
	if (parser.current.type == type)
	{
		advance();
		return;
	}

	error_at_current(message);
}


Chunk *compiling_chunk;


static inline Chunk *current_chunk()
{
	return compiling_chunk;
}


static void emit_byte(uint8_t byte)
{
	chunk_write(current_chunk(), byte, parser.previous.line);
}


static void emit_bytes(uint8_t byte1, uint8_t byte2)
{
	emit_byte(byte1);
	emit_byte(byte2);
}


static void end_compiler()
{
	emit_byte(OP_RETURN);
}

static uint8_t make_constant(vm_value value)
{
	int constant = chunk_add_constant(current_chunk(), value);
	if (constant > UINT8_MAX)
	{
		error("Too many constants in one chunk.");
		return 0;
	}

	return (uint8_t)constant;
}

static void emit_constant(vm_value value)
{
	emit_bytes(OP_CONSTANT, make_constant(value));
}

static void integer_number(void)
{
	uint64_t number = parse_uint64(parser.previous.start, parser.previous.length);
	emit_constant((double)number);
}

static void double_number(void)
{
	// Rewrite to be faster and take the underscores!
	double value = strtod(parser.previous.start, NULL);
	emit_constant(value);
}


static void grouping()
{
	expression();
	consume(TOKEN_PAREN_R, "Expected ')' after expression.");
}


static void parse_precedence(precedence precedence)
{
  // What goes here?
}

static void unary()
{
	token_type operatorType = parser.previous.type;

	// Compile the operand.
	expression();

	// Emit the operator instruction.
	switch (operatorType)
	{
		case TOKEN_MINUS:
			emit_byte(OP_NEGATE);
			break;
		case TOKEN_NOT:
			emit_byte(OP_NOT);
			break;
		default:
			return; // Unreachable.
	}
}

bool compile(char *source, Chunk *chunk)
{
	init_lexer(source);
	compiling_chunk = chunk;
	parser.had_error = false;
	parser.panic_mode = false;
	advance();
	expression();
	consume(TOKEN_EOF, "Expect end of expression.");
	end_compiler();
	return !parser.had_error;
}
