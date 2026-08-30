#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgi.h"
#include "dynarr.h"

extern const char html_top[], html_bot[];


int main(void)
{
	int i;

	if(cgi_init() == -1) {
		return 1;
	}
	if(cgi_read_input() == -1) {
		return 1;
	}

	cgi_begin_output();

	puts(html_top);
	puts("<h1><font color=\"#77ccff\">ORRC voting</font></h1><hr>\n");

	if(!dynarr_empty(cgi_input)) {
		printf("<p>query data:</p>\n<ul>\n");
		for(i=0; i<dynarr_size(cgi_input); i++) {
			printf("<li>\"%s\": \"%s\"</li>\n", cgi_input[i].name, cgi_input[i].value);
		}
		puts("</ul>\n");
	}

	if(!dynarr_empty(cgi_cookies)) {
		printf("<p>Cookies:</p>\n<ul>\n");
		for(i=0; i<dynarr_size(cgi_cookies); i++) {
			printf("<li>\"%s\": \"%s\"</li>\n", cgi_cookies[i].name, cgi_cookies[i].value);
		}
		puts("</ul>\n");
	}

	fputs(html_bot, stdout);
	return 0;
}
