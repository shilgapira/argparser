#define _GNU_SOURCE

#include <argparser.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>

struct data
{
	char *prog;
	char *usage;
	char *about;
	char *epilog;
	bool exit;

	struct option *options;
	struct action *actions;
	struct colors colors;

	void (*error_fn)(char *, ...);
	void (*help_fn)();
	void (*usage_fn)(bool exit);

	int argc;
	char **argv;

	char *error;
};

typedef enum {
	PROG,
	USAGE,
	ABOUT,
	EPILOG,
	ERROR_FN,
	HELP_FN,
	USAGE_FN,
	EXIT,
	OPTIONS,
	ACTIONS
} data_t;

struct data_field
{
	data_t type;

	union {
		char *string;
		bool boolean;
		struct option *options;
		struct action *actions;
		void (*function)();
	};
};

static struct data *data_global()
{
	static struct data self = {0};
	return &self;
}

static struct argparser *argparser_global()
{
	static struct argparser self = {0};
	return &self;
}

static void argparser_set(struct data_field field)
{
	struct data *data = data_global();

	switch (field.type) {
        case USAGE:
        	data->usage = field.string;
                break;
        case PROG:
        	data->prog = field.string;
        case ABOUT:
        	data->about = field.string;
        case EPILOG:
        	data->epilog = field.string;
        case EXIT:
        	data->exit = field.boolean;
                break;
        case OPTIONS:
        	data->options = field.options;
                break;
        case ACTIONS:
        	data->actions = field.actions;
                break;
        case ERROR_FN:
        	data->error_fn = (void *) field.function;
        	break;
        case HELP_FN:
        	data->help_fn = field.function;
        case USAGE_FN:
        	data->usage_fn = (void *) field.function;
                break;
        }
}

static void argparser_prog(char *string)
{
	argparser_set((struct data_field) {
		.type = PROG,
	       	.string = string
	});
}

static void argparser_usage(char *string)
{
	argparser_set((struct data_field) {
		.type = USAGE,
	       	.string = string
       	});
}

static void argparser_about(char *string)
{
	argparser_set((struct data_field) {
		.type = ABOUT,
	       	.string = string
       	});
}

static void argparser_epilog(char *string)
{
	argparser_set((struct data_field) {
		.type = EPILOG,
	       	.string = string
	});
}

static void argparser_error_fn(void (*function)(char *, ...))
{
	argparser_set((struct data_field) {
		.type = ERROR_FN,
	       	.function = (void *) function
	});
}

static void argparser_help_fn(void (*function)())
{
	argparser_set((struct data_field) {
		.type = HELP_FN,
	       	.function = (void *) function
	});
}

static void argparser_usage_fn(void (*function)(bool))
{
	argparser_set((struct data_field) {
		.type = USAGE_FN,
	       	.function = (void *) function
	});
}

static void argparser_exit(bool boolean)
{
	argparser_set((struct data_field) {
		.type = EXIT,
		.boolean = boolean
	});
}

static void argparser_options(struct option *options)
{
	argparser_set((struct data_field) {
		.type = OPTIONS,
	       	.options = options
	});
}

static void argparser_actions(struct action *actions)
{
	argparser_set((struct data_field) {
		.type = ACTIONS,
	       	.actions = actions
	});
}

static char *color(char *color)
{
	if (isatty(STDOUT_FILENO) && color)
		return color;
	return "";
}

static char *reset()
{
	return (isatty(STDOUT_FILENO)) ? "\033[0m" : "";
}

static void default_usage(bool exit)
{
	struct data *self = data_global();

	printf("usage: %s%s%s %s%s%s\n",
		color(self->colors.prog), self->prog, reset(),
		color(self->colors.usage), self->usage, reset()
	);
}

static void default_help()
{
	struct data *self = data_global();
	int longest;

	if (self->usage_fn)
		self->usage_fn(false);
	else
		default_usage(false);

	longest = 0;

	if (self->options) {
		for (int i = 0; self->options[i].longopt && self->options[i].shortopt; i++) {
			int buf = 0;
			if (self->options[i].longopt)
				buf += strlen(self->options[i].longopt);

			if (self->options[i].shortopt != 0)
				buf += 3;

			if (buf > longest)
				longest = buf;
		}
	}

	if (self->actions) {
		for (int i = 0; self->actions[i].name; i++) {
			int length = strlen(self->actions[i].name);

			if (length > longest)
				longest = length;
		}

		printf("\n%sactions%s:\n", color(self->colors.header), reset());

		for (int i = 0; self->actions[i].name; i++) {
			printf("\t%s%s%s", color(self->colors.action),
	  		                 self->actions[i].name, reset());

			if (self->actions[i].about) {
				int offset = longest - strlen(self->actions[i].name) + 8;

				while (offset--)
					fputc(' ', stdout);

				printf("%s%s%s", color(self->colors.action_about),
						 self->actions[i].about, reset());
			}

			fputc('\n', stdout);
		}

		fputc('\n', stdout);
	}

	if (self->options) {
		printf("%soptions%s:\n", color(self->colors.header), reset());

		for (int i = 0; self->options[i].longopt && self->options[i].shortopt; i++) {
			int lenght = 0;

			fputc('\t', stdout);

			if (self->options[i].longopt) {
				lenght += strlen(self->options[i].longopt) + 2;

				printf("%s--%s%s", color(self->colors.option),
	   			                   self->options[i].longopt, reset());
			}

			if (self->options[i].shortopt) {
				lenght += 4;
				printf(", %s-%c%s", color(self->colors.option),
				                    self->options[i].shortopt, reset());
			}

			if (self->options[i].about) {
				int offset = longest - lenght + 8;

				while (offset--)
					fputc(' ', stdout);

				printf("%s%s%s", color(self->colors.option_about),
				                 self->options[i].about, reset());
			}

			fputc('\n', stdout);
		}
	}

	if (self->epilog)
		printf("\n%s%s%s\n", color(self->colors.epilog), self->epilog, reset());

	exit(EXIT_SUCCESS);
}

static void default_error(char *fmt, ...)
{
	struct data *self = data_global();

	va_list args;
	va_start(args, fmt);

	if (self->error_fn) {
		self->error_fn(fmt, args);
	} else if (self->exit) {
		fprintf(stderr, "argparser: %serror%s: ", color("\033[31m"), reset());
		vfprintf(stderr, fmt, args);
		fputc('\n', stderr);
		va_end(args);
		exit(EXIT_FAILURE);
	} else {
		asprintf(&self->error, fmt, args);
	}

	va_end(args);
}

static int find_option(int at)
{
	struct data *self = data_global();

	for (int i = 0; self->options[i].longopt || self->options[i].shortopt; i++) {
		if (!strcmp(self->argv[at] + 2, self->options[i].longopt)
		 || self->argv[at][1] == self->options[i].shortopt)
		 	return i;
	}

	return -1;
}

static int parse_option(int at)
{
	struct data *self = data_global();
	struct option option;

	int pos = find_option(at);

	if (pos == -1)
		default_error("unknown option: '%s'", self->argv[at]);

	option = self->options[pos];

	switch (option.argc) {
	case 0:
		option.callback(option, NULL);
		break;
	default:
		if (self->argc - (at + 1) < option.argc)
			default_error("option `%s` expects %i argumant(s) after, got only %i",
				      option.longopt, option.argc, self->argc - (at + 1));
		option.callback(option, self->argv + (at + 1));
		return option.argc;
	}

	return 0;
}

static int find_action(int at)
{
	struct data *self = data_global();

	for (int i = 0; self->actions[i].name; i++) {
		if (!strcmp(self->argv[at], self->actions[i].name))
			return i;
	}

	return -1;
}

static int parse_action(int at)
{
	struct data *self = data_global();
	struct action action;

	int pos = find_action(at);

	if (pos == -1)
		default_error("unknown action: '%s'", self->argv[at]);

	action = self->actions[pos];

	switch (action.argc) {
	case 0:
		action.callback(action, NULL);
		break;
	default:
		if (self->argc - (at + 1) < action.argc)
			default_error("action `%s` expects %i argumant(s) after, got only %i",
				      action.name, action.argc, self->argc - (at + 1));
		action.callback(action, self->argv + (at + 1));
		return action.argc;
	}

	return 0;
}

static char *argparser_parse(int argc, char **argv)
{
	struct data *self = data_global();

	if (!self->prog)
		self->prog = basename(argv[0]);

	self->argc = --argc;
	self->argv = ++argv;

	if (argc == 0) {
		if (self->usage_fn)
			self->usage_fn(true);
		else
			default_usage(true);
	}

	for (int i = 0; i < self->argc; i++) {
		if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
			if (self->help_fn)
				self->help_fn();
			else
				default_help();
		}

		if (*self->argv[i] == '-' && self->options)
			i += parse_option(i);
		else if (self->actions)
			i += parse_action(i);
		else
			default_error("don't know what to do with `%s`", self->argv[i]);
	}

	return self->error;
}

struct argparser *__argparser_new()
{
	struct argparser *self = argparser_global();

	self->prog = argparser_prog;
	self->usage = argparser_usage;
	self->about = argparser_about;
	self->epilog = argparser_epilog;
	self->error_fn = argparser_error_fn;
	self->help_fn = argparser_help_fn;
	self->usage_fn = argparser_usage_fn;
	self->exit = argparser_exit;
	self->options = argparser_options;
	self->actions = argparser_actions;
	self->parse = argparser_parse;

	return self;
}

struct argparser argparser = {
	.new = __argparser_new
};
