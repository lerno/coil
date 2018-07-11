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
	OP_BIT_NOT,
} OpCode;

