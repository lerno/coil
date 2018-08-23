#pragma once
//
// Created by Christoffer Lernö on 2018-07-09.
// Copyright (c) 2018 Christoffer Lernö. All rights reserved.
//

#include "common.h"

typedef enum
{
	OP_RETURN,
	OP_CONSTANT,
	OP_NEGATE,
	OP_ADD,
	OP_SUB,
	OP_MULT,
	OP_DIV,
	OP_NOT,
	OP_TRUE,
	OP_FALSE,
	OP_NIL,
	OP_BIT_NOT,
	OP_BIT_AND,
	OP_BIT_OR,
	OP_BIT_XOR,
	OP_LEFT_SHIFT,
	OP_RIGHT_SHIFT,
	OP_RIGHT_SHIFT_LOGIC,
	OP_POW,
	OP_OR,
	OP_AND,
	OP_ASSIGN,
	OP_EQUAL,
	OP_GREATER,
	OP_LESS,
} OpCode;

