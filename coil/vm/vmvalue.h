#pragma once


typedef double vm_value;

typedef struct
{
	int size;
	int capacity;
	vm_value *values;
} VmValueArray;

void vm_value_array_init(VmValueArray *value_array);
void vm_value_write(VmValueArray *value_array, vm_value value);
void vm_value_free(VmValueArray *value_array);
void vm_value_print(vm_value value);
