//
//  main.c
//  coil
//
//  Created by Christoffer Lernö on 2018-07-09.
//  Copyright © 2018 Christoffer Lernö. All rights reserved.
//

#include "common.h"
#include <stdio.h>
#include "chunk.h"
#include "opcodes.h"
#include "debug.h"
#include "vm.h"
#include "file.h"


static void repl()
{
	char line[1024];
	while (true)
	{
		printf("coil> ");
		if (!fgets(line, sizeof(line), stdin))
		{
			printf("\n");
			break;
		}
		interpret(line);
	}
}

static void runFile(const char *file)
{
	char *source = read_file(file);
	interpret_result result = interpret(source);
	free(source);

	if (result == INTERPRET_COMPILE_ERROR) exit(65);
	if (result == INTERPRET_RUNTIME_ERROR) exit(70);

}

int main(int argc, const char *argv[])
{
	init_vm();

	switch (argc)
	{
		case 1:
			repl();
			break;
		case 2:
			runFile(argv[1]);
			break;
		default:
			fprintf(stderr, "Usage: coil [path]\n");
			exit(64);
	}
	free_vm();
	return 0;
}
