#include <stdio.h>
#include "vmvalue.h"
#include "memory.h"

void vm_value_array_init(VmValueArray *value_array)
{
	value_array->size = 0;
	value_array->capacity = 0;
	value_array->values = NULL;
}

void vm_value_write(VmValueArray *value_array, vm_value value)
{
	if (value_array->size + 1 > value_array->capacity)
	{
		int old_capacity = value_array->capacity;
		value_array->capacity = GROW_CAPACITY(old_capacity);
		value_array->values = GROW_ARRAY(value_array->values, vm_value, old_capacity, value_array->capacity);
	}
	value_array->values[value_array->size++] = value;
}

void vm_value_free(VmValueArray *value_array)
{
	FREE_ARRAY(uint8_t, value_array->values, value_array->capacity);
	vm_value_array_init(value_array);
}


void vm_value_print(vm_value value)
{
	printf("%g", value);
}
