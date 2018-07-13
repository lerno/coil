//
//  compiler.c
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include <stdio.h>
#include "compiler.h"
#include "lexer.h"
#include "opcodes.h"
#include "string.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#include "chunk.h"

#endif

typedef struct
{
	Token current;
	Token previous;
	bool had_error;
	bool panic_mode;
} Parser;


typedef void (*ParseFn)(void);

typedef enum
{
	PREC_NONE,
	PREC_ASSIGNMENT,  // =
	PREC_OR,          // or
	PREC_AND,         // and
	PREC_COMPARISON,  // < > <= >= == !=
	PREC_TERM,        // + -
	PREC_BITWISE,     // ^ | &
	PREC_SHIFT,       // << >> >>>
	PREC_FACTOR,      // * / %
	PREC_POW,         // **
	PREC_UNARY,       // ! - + ~
	PREC_CALL,        // . () []
	PREC_PRIMARY
} precedence;

typedef struct
{
	ParseFn prefix;
	ParseFn infix;
	precedence precedence;
} ParseRule;


static ParseRule rules[TOKEN_EOF + 1];

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
#ifdef DEBUG_PRINT_CODE
	if (!parser.had_error)
	{
		disassemble_chunk(current_chunk(), "code");
	}
#endif
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


static inline ParseRule *get_rule(token_type type)
{
	return &rules[type];
}


static void parse_precedence(precedence precedence)
{
	advance();

	// Get the rule for the previous token.
	ParseFn prefix_rule = get_rule(parser.previous.type)->prefix;
	if (prefix_rule == NULL)
	{
		error("Expected expression.");
		return;
	}

	prefix_rule();

	while (precedence <= get_rule(parser.current.type)->precedence)
	{
		advance();
		ParseFn infix_rule = get_rule(parser.previous.type)->infix;
		infix_rule();
	}
}


static void expression()
{
	parse_precedence(PREC_ASSIGNMENT);
}


static void binary()
{
	// Remember the operator.
	token_type operator_type = parser.previous.type;

	// Compile the right operand.
	ParseRule *rule = get_rule(operator_type);
	parse_precedence((precedence)(rule->precedence + 1));
	// Right hand side now loaded.

	// Emit the operator instruction.
	switch (operator_type)
	{
		case TOKEN_RIGHT_SHIFT:
			emit_byte(OP_RIGHT_SHIFT);
			break;
		case TOKEN_LEFT_SHIFT:
			emit_byte(OP_LEFT_SHIFT);
			break;
		case TOKEN_RIGHT_SHIFT_LOGIC:
			emit_byte(OP_RIGHT_SHIFT_LOGIC);
			break;
		case TOKEN_BIT_AND:
			emit_byte(OP_BIT_AND);
			break;
		case TOKEN_BIT_OR:
			emit_byte(OP_BIT_OR);
			break;
		case TOKEN_BIT_XOR:
			emit_byte(OP_BIT_XOR);
			break;
		case TOKEN_PLUS:
			emit_byte(OP_ADD);
			break;
		case TOKEN_MINUS:
			emit_byte(OP_SUB);
			break;
		case TOKEN_MULT:
			emit_byte(OP_MULT);
			break;
		case TOKEN_DIV:
			emit_byte(OP_DIV);
			break;
		case TOKEN_POW:
			emit_byte(OP_POW);
			break;
		case TOKEN_AND:
			printf("Not implemented\n");
			break;
		default:
			printf("Invalid binary operator %d\n", operator_type);
			return; // Unreachable.
	}
}


// Right associative
static void binary_right()
{
	// Remember the operator.
	token_type operator_type = parser.previous.type;

	// Compile the right operand.
	ParseRule *rule = get_rule(operator_type);
	parse_precedence((precedence)(rule->precedence));
	// Right hand side now loaded.

	// Emit the operator instruction.
	switch (operator_type)
	{
		case TOKEN_POW:
			emit_byte(OP_POW);
			break;
		default:
			printf("Invalid binary-right operator %d\n", operator_type);
			return; // Unreachable.
	}
}

static void grouping()
{
	expression();
	consume(TOKEN_PAREN_R, "Expected ')' after expression.");
}

static void or()
{
	// Skip the right argument if the left is true.
	emit_byte(OP_OR);
	int jump = current_chunk()->size;
	emit_byte(0xFF);
	parse_precedence(PREC_OR);
	current_chunk()->code[jump] = (uint8_t)(current_chunk()->size - jump - 1);
}

static void and()
{
	// Skip the right argument if the left is true.
	emit_byte(OP_AND);
	int jump = current_chunk()->size;
	emit_byte(0xFF);
	parse_precedence(PREC_AND);
	current_chunk()->code[jump] = (uint8_t)(current_chunk()->size - jump - 1);
}

static void unary()
{
	token_type operatorType = parser.previous.type;

	// Compile the operand.
	parse_precedence(PREC_UNARY);

	// Emit the operator instruction.
	switch (operatorType)
	{
		case TOKEN_MINUS:
			emit_byte(OP_NEGATE);
			break;
		case TOKEN_NOT:
			emit_byte(OP_NOT);
			break;
		case TOKEN_BIT_NOT:
			emit_byte(OP_BIT_NOT);
			break;
		default:
			return; // Unreachable.
	}
}


static void set_parse_rule(token_type type, ParseFn prefix, ParseFn infix, precedence rule_precedence)
{
	rules[type].prefix = prefix;
	rules[type].precedence = rule_precedence;
	rules[type].prefix = prefix;
	rules[type].infix = infix;
};


static void setup_parse_rules()
{
	static bool parse_rules_done = false;
	if (parse_rules_done) return;
	set_parse_rule(TOKEN_PAREN_L, grouping, NULL, PREC_CALL);
	set_parse_rule(TOKEN_DOT, NULL, NULL, PREC_CALL);
	set_parse_rule(TOKEN_MINUS, unary, binary, PREC_TERM);
	set_parse_rule(TOKEN_PLUS, NULL, binary, PREC_TERM);
	set_parse_rule(TOKEN_DIV, NULL, binary, PREC_FACTOR);
	set_parse_rule(TOKEN_MULT, NULL, binary, PREC_FACTOR);
	set_parse_rule(TOKEN_DOT, NULL, NULL, PREC_CALL);
	set_parse_rule(TOKEN_NOT, unary, NULL, PREC_NONE);
    set_parse_rule(TOKEN_BIT_NOT, unary, NULL, PREC_NONE);
	set_parse_rule(TOKEN_BIT_XOR, NULL, binary, PREC_BITWISE);
	set_parse_rule(TOKEN_BIT_OR, NULL, binary, PREC_BITWISE);
	set_parse_rule(TOKEN_BIT_AND, NULL, binary, PREC_BITWISE);
    set_parse_rule(TOKEN_EQUAL, NULL, NULL, PREC_COMPARISON);
	set_parse_rule(TOKEN_NOT_EQUAL, NULL, NULL, PREC_COMPARISON);
	set_parse_rule(TOKEN_GREATER, NULL, NULL, PREC_COMPARISON);
	set_parse_rule(TOKEN_GREATER_EQUAL, NULL, NULL, PREC_COMPARISON);
	set_parse_rule(TOKEN_LESS, NULL, NULL, PREC_COMPARISON);
	set_parse_rule(TOKEN_LESS_EQUAL, NULL, NULL, PREC_COMPARISON);
	set_parse_rule(TOKEN_LESS, NULL, NULL, PREC_COMPARISON);
	set_parse_rule(TOKEN_LESS_EQUAL, NULL, NULL, PREC_COMPARISON);
	set_parse_rule(TOKEN_LEFT_SHIFT, NULL, binary, PREC_SHIFT);
	set_parse_rule(TOKEN_RIGHT_SHIFT, NULL, binary, PREC_SHIFT);
	set_parse_rule(TOKEN_RIGHT_SHIFT_LOGIC, NULL, binary, PREC_SHIFT);
	set_parse_rule(TOKEN_INTEGER, integer_number, NULL, PREC_NONE);
	set_parse_rule(TOKEN_POW, NULL, binary_right, PREC_POW);
	set_parse_rule(TOKEN_FLOAT, double_number, NULL, PREC_NONE);
	set_parse_rule(TOKEN_OR, NULL, or, PREC_OR);
	set_parse_rule(TOKEN_AND, NULL, and, PREC_OR);
    parse_rules_done = true;
}


bool compile(char *source, Chunk *chunk)
{
	setup_parse_rules();
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
