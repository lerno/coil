#pragma once
//
//  compiler.h
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include "chunk.h"
#include "common.h"
#include "irbuilder.h"
#include "lexer.h"

typedef struct
{
	Token current;
	Token previous;
	bool had_error;
	bool panic_mode;
} Parser;

bool compile(char *source, IrBuilder *chunk);
