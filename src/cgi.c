#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "cgi.h"
#include "dynarr.h"

extern const char html_top[], html_bot[];

struct cgivar *cgi_input, *cgi_cookies;
static int cgivars_sorted;

static struct cgivar *new_cookies;
static int outstate;	/* 0: before headers, 1: body */

static void conv_special(char *s, enum cgi_input_type type);
static void sort_cgivars(void);
static int cgivar_cmp(const void *a, const void *b);

int cgi_init(void)
{
	if(!(cgi_input = dynarr_alloc(0, sizeof *cgi_input))) {
		return -1;
	}
	if(!(cgi_cookies = dynarr_alloc(0, sizeof *cgi_cookies))) {
		return -1;
	}
	if(!(new_cookies = dynarr_alloc(0, sizeof *new_cookies))) {
		return -1;
	}
	return 0;
}

int cgi_read_input(void)
{
	char *env;
	int contlen, sz;
	char *query, *end;

	if((env = getenv("CONTENT_LENGTH")) && (contlen = atoi(env)) > 0) {
		if(!(query = malloc(contlen + 1))) {
			cgi_panic("failed to allocate %d bytes for query buffer\n", contlen + 1);
		}
		end = query;
		do {
			if((sz = fread(end, 1, contlen, stdin)) <= 0) {
				cgi_panic("unexpected EOF while reading POST query\n");
			}
			contlen -= sz;
			end += sz;
		} while(contlen > 0);
		*end = 0;

		cgi_proc_input(query, CGI_QUERY);
		free(query);
	}

	if((env = getenv("QUERY_STRING"))) {
		cgi_proc_input(env, CGI_QUERY);
	}

	if((env = getenv("HTTP_COOKIE"))) {
		cgi_proc_input(env, CGI_COOKIE);
	}

	return 0;
}

int cgi_proc_input(const char *str, enum cgi_input_type type)
{
	const char *end, *sep, *name;
	struct cgivar var;
	int len;
	char sepsym = type == CGI_QUERY ? '&' : ';';

	while(*str) {
		while(*str && isspace((unsigned char)*str)) str++;
		if(!*str) break;

		end = str;
		sep = 0;
		while(*end && *end != sepsym) {
			if(*end == '=' && !sep) sep = end;
			end++;
		}

		name = str;
		str = *end ? end + 1 : end;
		if(!sep) continue;

		if((len = sep - name) <= 0) {
			continue;
		}

		memset(&var, 0, sizeof var);
		if(!(var.name = malloc(len + 1))) {
			cgi_panic("failed to allocate query variable name buffer\n");
		}
		memcpy(var.name, name, len); var.name[len] = 0;

		if((len = end - ++sep) <= 0) {
			free(var.name);
			continue;
		}
		if(!(var.value = malloc(len + 1))) {
			cgi_panic("failed to allocate memory for query value buffer\n");
			free(var.name);
		}
		memcpy(var.value, sep, len); var.value[len] = 0;

		conv_special(var.name, type);
		conv_special(var.value, type);

		if(type == CGI_COOKIE) {
			dynarr_push_nf(cgi_cookies, &var);
		} else {
			dynarr_push_nf(cgi_input, &var);
		}
	}
	return 0;
}

const char *cgi_find_input(const char *name)
{
	struct cgivar *var, key;

	if(!cgivars_sorted) sort_cgivars();

	key.name = (char*)name;
	if((var = bsearch(&key, cgi_input, dynarr_size(cgi_input), sizeof *cgi_input,
					cgivar_cmp))) {
		return var->value;
	}
	return 0;
}

void cgi_set_cookie(const char *name, const char *val, unsigned int expires)
{
	struct cgivar newvar;

	if(!(newvar.name = strdup(name)) || !(newvar.value = strdup(val))) {
		free(newvar.name);
		cgi_panic("Failed to allocate new cookie strings\n");
	}
	newvar.expires = expires;

	dynarr_push_nf(new_cookies, &newvar);
}

const char *cgi_find_cookie(const char *name)
{
	struct cgivar *var, key;

	if(!cgivars_sorted) sort_cgivars();

	key.name = (char*)name;
	if((var = bsearch(&key, cgi_cookies, dynarr_size(cgi_cookies),
					sizeof *cgi_cookies, cgivar_cmp))) {
		return var->value;
	}
	return 0;
}

void cgi_begin_output(void)
{
	int i;
	puts("Content-Type: text/html");

	for(i=0; i<dynarr_size(new_cookies); i++) {
		printf("Set-Cookie: %s=%s; Path=/", new_cookies[i].name, new_cookies[i].value);
		if(new_cookies[i].expires) {
			printf("; Max-Age=%u\n", new_cookies[i].expires);
		} else {
			putchar('\n');
		}
	}
	putchar('\n');
	outstate = 1;
}

void cgi_panic(const char *fmt, ...)
{
	va_list ap;

	if(!outstate) {
		cgi_begin_output();
		puts(html_top);
	}

	puts("<h1><font color=\"#ff0000\">ORRC Panic!</font></h1><hr><p>");
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	puts("</p>");
	puts(html_bot);
	exit(1);
}

static void conv_special(char *s, enum cgi_input_type type)
{
	char *out = s, *endp;
	char hexbuf[3] = {0};
	unsigned int val;

	while(*s) {
		char c = *s++;

		switch(c) {
		case '+':
			*out++ = type == CGI_QUERY ? ' ' : '+';
			break;

		case '%':
			if(!(hexbuf[0] = *s++)) goto end;
			if(!(hexbuf[1] = *s++)) goto end;

			val = strtoul(hexbuf, &endp, 16);
			if(val > 0 && endp != hexbuf) {
				*out++ = val;
			}
			break;

		default:
			*out++ = c;
			break;
		}
	}
end:
	*out = 0;
}

static void sort_cgivars(void)
{
	qsort(cgi_input, dynarr_size(cgi_input), sizeof *cgi_input, cgivar_cmp);
	qsort(cgi_cookies, dynarr_size(cgi_cookies), sizeof *cgi_cookies, cgivar_cmp);
	cgivars_sorted = 1;
}

static int cgivar_cmp(const void *a, const void *b)
{
	return strcmp(((struct cgivar*)a)->name, ((struct cgivar*)b)->name);
}
