#include <stdio.h>

#define EXIT_SUCC 0
#define EXIT_FAIL 1

void loop(void);
char *readline(void);
char **splitline(char*);

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
		line = readline();
		args = splitline(line);
		status = exec(args);

		free(line);
		free(args);
	} while (status);
}

#define BUFSIZ (1<<10)
char *readline(void)
{
	int i, c, bufsiz = BUFSIZ;
	char *buf = malloc(sizeof(char) * bufsiz);
	
	if (!buf) {
		fprintf(stderr, "osh: alloc error\n");
		exit(EXIT_FAIL);
	}

	i = 0;
	while (1) { /* basically 'getline' */
		if ((c = getchar()) == EOF || c == '\n')
			buf[i] = '\0'; return  buf;
		else
			buf[i] = c;
		i++;

		if (i >= bufsiz) {
			buf += BUFSIZE;
			buf = realloc(buf, bufsiz);
			if (!buf) {
				fprintf(stderr, "osh: alloc error\n");
				exit(EXIT_FAIL);
			}
		}
	}
}

#define TOK_BUFSIZ (1<<6)
#define TOK_DELIM " \t\r\n\a"
char **splitline(char *line)
{
	int bufsiz = TOK_BUFSIZ, i = 0;
	char **toks = malloc(sizeof(char*) * bufsiz);
	char *tok;

	if (!toks) {
		fprintf(stderr, "osh: alloc error\n");
		exit(EXIT_FAIL);
	}

	tok = strtok(line, TOK_DELIM);
	while (tok != NULL) {
		toks[i] = tok;
		i++;
		if (i >= bufsiz) {
			bufsiz += TOK_BUFSIZ;
			toks = realloc(toks, sizeof(char*) * bufsiz);
			if (!toks) {
				fprintf(stderr, "osh: alloc error\n");
				exit(EXIT_FAIL);
			}
		}
		tok = strtok(NULL, TOK_DELIM);
	}
	toks[i] = NULL;
	return toks;
}



