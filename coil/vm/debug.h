#pragma once
//
// Created by Christoffer Lernö on 2018-07-09.
// Copyright (c) 2018 Christoffer Lernö. All rights reserved.
//

#include "chunk.h"

void disassemble_chunk(Chunk *chunk, char *name);
int disassemble_instruction(Chunk* chunk, int sp);
