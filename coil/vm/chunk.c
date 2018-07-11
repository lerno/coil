//
//  dynarray.c
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include "chunk.h"
#include "memory.h"
#include "vmvalue.h"


void chunk_init(Chunk *chunk)
{
	chunk->capacity = 0;
	chunk->size = 0;
	chunk->code = NULL;
	chunk->lines = NULL;
	vm_value_array_init(&chunk->constants);
}

void chunk_write(Chunk *chunk, uint8_t byte, int line)
{
	if (chunk->size + 1 > chunk->capacity)
	{
		int old_capacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(old_capacity);
		chunk->code = GROW_ARRAY(chunk->code, uint8_t, old_capacity, chunk->capacity);
		chunk->lines = GROW_ARRAY(chunk->lines, int, old_capacity, chunk->capacity);
	}
	chunk->lines[chunk->size] = line;
	chunk->code[chunk->size++] = byte;
}

void chunk_free(Chunk *chunk)
{
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	FREE_ARRAY(int, chunk->lines, chunk->capacity);
	vm_value_free(&chunk->constants);
	chunk_init(chunk);
}


int chunk_add_constant(Chunk *chunk, vm_value value)
{
	vm_value_write(&chunk->constants, value);
	return chunk->constants.size - 1;
}


