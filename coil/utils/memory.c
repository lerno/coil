//
//  memory.c
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include "memory.h"

void *reallocate(void *previous, size_t old_size, size_t new_size)
{
	if (new_size == 0)
	{
		free(previous);
		return NULL;
	}
	
	if (new_size == old_size) return previous;
	
	return realloc(previous, new_size);
	
}
