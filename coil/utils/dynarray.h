#pragma once
//
//  dynarray.h
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include <stdlib.h>

typedef struct
{
	int size;
	int capacity;
	uint8_t *data;
} dynarray;

void dynarray_init(dynarray *array);
void dynarray_write(dynarray *array, uint8_t byte);
void dynarray_free(dynarray *array);
