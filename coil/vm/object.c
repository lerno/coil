#include <string.h>
#include <stdio.h>
#include "object.h"
#include "memory.h"
#include "vm.h"

static Obj *allocate_object(size_t size, ObjType type)
{
	Obj *object = (Obj *)reallocate(NULL, 0, size);
	object->type = type;
	object->next = vm.objects;
	vm.objects = object;
	return object;
}

#define ALLOCATE_OBJ(type, objectType) (type *)allocate_object(sizeof(type), objectType)

static ObjString *allocate_string(char *chars, int length)
{
	ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
	string->length = length;
	string->chars = chars;
	return string;
}

ObjString *copy_string(const char *start, int length)
{
	char *chars = ALLOCATE(char, length + 1);
	memcpy(chars, start, length);
	chars[length] = '\0';
	return allocate_string(chars, length);
}

ObjString *take_string(char *chars, int length)
{
	return allocate_string(chars, length);
}
void print_object(Value value)
{
	switch (OBJ_TYPE(value))
	{
		case OBJ_STRING:
			printf("%s", AS_CSTRING(value));
			break;
	}
}
