//
//  main.c
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include "common.h"
#include <stdio.h>
#include "dynarray.h"
int main(int argc, const char * argv[])
{
	// insert code here...
	printf("Hello, World!\n");
	dynarray array;
	dynarray_init(&array);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	printf("Count %d %d\n", array.size, array.capacity);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 2);
	dynarray_write(&array, 5);
	dynarray_write(&array, 1);
	printf("Count %d %d\n", array.size, array.capacity);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 1);
	dynarray_write(&array, 2);
	dynarray_write(&array, 5);
	dynarray_write(&array, 1);
	printf("Count %d %d\n", array.size, array.capacity);
	return 0;
}
