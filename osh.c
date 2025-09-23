#include <stdio.h>

#define EXIT_SUCC 0
#define EXIT_FAIL 1

void loop(void);


int main(int argc, char **argv)
{
	/* TODO: config file loader */

	loop();

	return EXIT_SUCC;
}

/*
 * loop:
 * read from stdin
 * parse input
 * execute command
 */
void loop(void)
{
	int status;
	char *line;
	char **args;

	do {
		printf("$ ");
		line = _readline();
		args = splitline(line);
		status = exec(args);

		free(line);
		free(args);
	} while (status);
}


