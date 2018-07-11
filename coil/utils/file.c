//
//  file.c
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include <stdio.h>
#include "file.h"
#include <stdlib.h>

char *read_file(const char *path)
{
	FILE *file = fopen(path, "rb");

	if (file == NULL)
	{
		fprintf(stderr, "Could not open file \"%s\".\n", path);
		exit(74);
	}

	fseek(file, 0L, SEEK_END);
	size_t file_size = (size_t)ftell(file);

	rewind(file);

	char *buffer = (char *)malloc((size_t)file_size + 1);
	if (buffer == NULL)
	{
		fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
		exit(74);
	}

	size_t bytesRead = fread(buffer, sizeof(char), (size_t)file_size, file);
	if (bytesRead < file_size)
	{
		fprintf(stderr, "Failed to read file \"%s\".\n", path);
		exit(74);
	}

	buffer[bytesRead] = '\0';

	fclose(file);
	return buffer;
}
