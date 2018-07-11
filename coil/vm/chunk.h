#pragma once
//
//  dynarray.h
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include <stdlib.h>
#include "vmvalue.h"

typedef struct
{
	int size;
	int capacity;
	uint8_t *code;
	int *lines;
	VmValueArray constants;
} Chunk;

void chunk_init(Chunk *chunk);
void chunk_write(Chunk *chunk, uint8_t byte, int line);
void chunk_free(Chunk *chunk);
int chunk_add_constant(Chunk *chunk, vm_value value);
