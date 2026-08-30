#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cgi.h"
#include "dynarr.h"

#define ROOT_DIR		"/var/www/orrc"

extern const char html_top[], html_bot[];

static void debug_output(void);

int main(void)
{
	int existing_user = 0;
	const char *str;
	char userid[128];

	/* make sure we're in the correct working directory to open files */
	chdir(ROOT_DIR);

	if(cgi_init() == -1) {
		return 1;
	}
	if(cgi_read_input() == -1) {
		return 1;
	}

	if((str = cgi_find_cookie("ORRC_USER"))) {
		existing_user = 1;
		strcpy(userid, str);
	} else {
		strcpy(userid, "foobar");
		cgi_set_cookie("ORRC_USER", userid, CGI_COOKIE_SESSION);
	}

	cgi_begin_output();

	puts(html_top);
	puts("<h1><font color=\"#77ccff\">ORRC voting</font></h1><hr>\n");

	printf("<p><i>%s</i> user (%s)</p>\n", existing_user ? "existing" : "new", userid);

	debug_output();
	fputs(html_bot, stdout);
	return 0;
}

static void debug_output(void)
{
	int i;

	puts("<hr>\n<h2>Debug section</h2>\n");
	if(!dynarr_empty(cgi_input)) {
		printf("<h3>Query data</h3>\n<ul>\n");
		for(i=0; i<dynarr_size(cgi_input); i++) {
			printf("<li>\"%s\": \"%s\"</li>\n", cgi_input[i].name, cgi_input[i].value);
		}
		puts("</ul>\n");
	}

	if(!dynarr_empty(cgi_cookies)) {
		printf("<h3>Cookies</h3>\n<ul>\n");
		for(i=0; i<dynarr_size(cgi_cookies); i++) {
			printf("<li>\"%s\": \"%s\"</li>\n", cgi_cookies[i].name, cgi_cookies[i].value);
		}
		puts("</ul>\n");
	}
}
