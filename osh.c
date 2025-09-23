#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h> /* waitpid */

void loop();

int main(int argc, char **argv)
{
	/*
	 * TODO:
	 * pipes pubs.opengroup.org/onlinepubs/9799919799/functions/pipe.html
	 * config loader
	 * more included routines
	 */

	loop();

	return EXIT_SUCCESS;
}

/*
 * loop:
 * read from stdin
 * parse input
 * execute command
 */


char *readline();
char **splitline(char*);
int _exec(char **);

void loop(void)
{
	int status;
	char *line;
	char **args;

	do {
		printf("$ ");
		line = readline();
		args = splitline(line);
		status = _exec(args);

		free(line);
		free(args);
	} while (status);
}


#define BUFSIZE (1<<10)
char *readline(void)
{
	int i, c, bufsize = BUFSIZE;
	char *buf = malloc(sizeof(char) * bufsize);
	
	if (!buf) {
		fprintf(stderr, "osh: alloc error\n");
		exit(EXIT_FAILURE);
	}

	i = 0;
	while (1) { /* basically 'getline' */
		if ((c = getchar()) == EOF || c == '\n') {
			buf[i] = '\0';
			return  buf;
		}	
		else
			buf[i] = c;
		i++;

		if (i >= bufsize) {
			bufsize += BUFSIZE;
			buf = realloc(buf, bufsize);
			if (!buf) {
				fprintf(stderr, "osh: alloc error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}


#define TOK_BUFSIZE (1<<6)
#define TOK_DELIMS " \t\r\n\a"
char **splitline(char *line)
{
	int bufsize = TOK_BUFSIZE, i = 0;
	char **toks = malloc(sizeof(char*) * bufsize);
	char *tok;

	if (!toks) {
		fprintf(stderr, "osh: alloc error\n");
		exit(EXIT_FAILURE);
	}

	tok = strtok(line, TOK_DELIMS);
	while (tok != NULL) {
		toks[i] = tok;
		i++;
		if (i >= bufsize) {
			bufsize += TOK_BUFSIZE;
			toks = realloc(toks, sizeof(char*) * bufsize);
			if (!toks) {
				fprintf(stderr, "osh: alloc error\n");
				exit(EXIT_FAILURE);
			}
		}
		tok = strtok(NULL, TOK_DELIMS);
	}
	toks[i] = NULL;
	return toks;
}

int _run(char **args)
{
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) {
		/* child */
		if (execvp(args[0], args) == -1)
			perror("osh");
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
		perror("osh");
	else
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	return 1;
}


int _cd(char **);
int _help(char **);
int shexit(char **);

char *builtins[] = {"cd", "help", "exit"};

int (*rptr[]) (char**) = {&_cd, &_help, &shexit};

/* static */
static inline int num_builtins()
{
	return sizeof(builtins) / sizeof(char *);
}

/* implementations */
int _cd(char **args)
{
	if (args[1] == NULL)
		fprintf(stderr, "osh: expected argument to \"cd\"\n");
	else
		if (chdir(args[1]) != 0)
			perror("osh");
	return 1;
}

int _help(char **args)
{
	int i;

	printf("osh: enter program and arguments to execute\n");
	printf("built in:\n");
	for (i = 0; i < num_builtins(); i++)
		printf("\t%s\n", builtins[i]);
	return 1;
}

int shexit(char **args)
{
	return 0;
}

int _exec(char **args)
{
	int i;

	if (args[0] == NULL) return 1; /* empty cmd */

	for (i = 0; i < num_builtins(); i++)
		if (strcmp(args[0], builtins[i]) == 0)
			return (*rptr[i])(args);
	return _run(args);
}
