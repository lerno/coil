//
//  memory.c
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include "memory.h"
#include "vm.h"
#include "object.h"


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

static void free_object(Obj *object)
{
	switch (object->type)
	{
		case OBJ_STRING:
		{
			ObjString *string = (ObjString *)object;
			FREE_ARRAY(char, string->chars, string->length + 1);
			FREE(OBJ_STRING, object);
			break;
		}

	}
}

void free_objects()
{
	Obj *object = vm.objects;
	while (object)
	{
		Obj *next = object->next;
		free_object(object);
		object = next;
	}
}
