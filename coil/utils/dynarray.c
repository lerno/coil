//
//  dynarray.c
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include "dynarray.h"
#include "memory.h"

void dynarray_init(dynarray *array)
{
	array->capacity = 0;
	array->size = 0;
	array->data = NULL;
}

void dynarray_write(dynarray *array, uint8_t byte)
{
	if (array->size + 1 > array->capacity)
	{
		int old_capacity = array->capacity;
		array->capacity = GROW_CAPACITY(old_capacity);
		array->data = GROW_ARRAY(array->data, uint8_t, old_capacity, array->capacity);
	}
	array->data[array->size++] = byte;
}

void dynarray_free(dynarray *array)
{
	FREE_ARRAY(uint8_t, array->data, array->capacity);
	dynarray_init(array);
}
