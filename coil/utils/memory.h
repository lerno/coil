#pragma once
//
//  memory.h
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include <stdlib.h>

#define GROW_CAPACITY(c) ((c) < 8 ? 8 : (c) * 2)

#define GROW_ARRAY(previous, type, old_size, new_size) \
	(type *)reallocate(previous, sizeof(type) * old_size, sizeof(type) * new_size)

#define FREE_ARRAY(type, pointer, old_count) \
	reallocate(pointer, sizeof(type) * (old_count), 0)


#define ALLOCATE(type, count) (type *)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

void *reallocate(void *previous, size_t old_size, size_t new_size);
void free_objects();
