#pragma once

#if __STDC_VERSION__ <= 201710L
# include <stdbool.h>
#endif

struct option
{
	char *longopt;
	char shortopt;
	char *about;
	int argc;
	void (*callback)(struct option, char **argv);
	void *userdata;
} __attribute__((packed));

struct action
{
	char *name;
	char *about;
	int argc;
	void (*callback)(struct action, char **argv);
	void *userdata;
} __attribute__((packed));

struct colors
{
	char *prog;
	char *usage;
	char *about;
	char *epilog;
	char *header;
	char *action;
	char *action_about;
	char *option;
	char *option_about;
} __attribute__((packed));

struct argparser
{
	struct argparser* (*new)(void);

	/* The name of the program
	   (default: basename(argv[0])) */
	void (*prog)(char *);

	/* The string describing the program usage
	   (default: generated from arguments added to parser) */
	void (*usage)(char *);

	/* Text to display before the argument help
	   (default: no text) */
	void (*about)(char *);

	/* Text to display after the argument help
	   (default: no text) */
	void (*epilog)(char *);

	/* Set custom error function */
	void (*error_fn)(void (*)(char *format, ...));

	/* Set custom help function */
	void (*help_fn)(void (*)(void));

	/* Set custom usage function,
	   @param: is exit needed */
	void (*usage_fn)(void (*)(bool));

	/* Should exit at error
	   (default: false ) */
	void (*exit)(bool);

	/* Options array must end with {0} */
	void (*options)(struct option *);

	/* Actions array must end with {0} */
	void (*actions)(struct action *);

	/* Set colors to default output
	   functions like help
	   NOTE: colors will be disabled
	   when output is not tty */
	void (*colors)(struct colors);

	/* Run parser, returns malloced error string
	   when error_fn is not set */
	char *(*parse)(int argc, char **argv);
};

extern struct argparser argparser;
