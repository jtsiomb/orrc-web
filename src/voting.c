#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cgi.h"
#include "dynarr.h"
#include "html.h"

struct entry {
	char *name, *desc;
	char *thumbimg;
};

#define ROOT_DIR		"/var/www/orrc"

extern const char html_top[], html_bot[];

static int load_entries(int compo_id);
static void debug_output(void);

int main(void)
{
	/* make sure we're in the correct working directory to open files */
	chdir(ROOT_DIR);

	html_set_title("ORRC voting");
	html_set_css("orrc.css");
	html_set_color(HTML_TEXT, 0xdddddd);
	html_set_color(HTML_BG, 0x181828);
	html_set_color(HTML_LINK, 0x66bbff);
	html_set_color(HTML_VLINK, 0x4488ee);
	html_set_color(HTML_H1, 0x77ccff);
	html_set_color(HTML_H2, 0xaaeecc);
	html_set_color(HTML_H3, 0xeeccee);
	html_set_font("sans-serif", 4);

	if(cgi_init() == -1) {
		return 1;
	}
	if(cgi_read_input() == -1) {
		return 1;
	}

	if(load_entries(0) == -1) {
		cgi_panic("failed to load competition entries\n");
	}

	cgi_begin_output();
	html_begin();

	html_heading(1, "ORRC voting");
	html_sep();

	html_heading(2, "Entries");

	debug_output();
	html_end();
	return 0;
}

static void debug_output(void)
{
	int i;

	html_sep();
	html_heading(2, "Debug section");
	if(!dynarr_empty(cgi_input)) {
		html_heading(3, "Query data");
		printf("<ul>\n");
		for(i=0; i<dynarr_size(cgi_input); i++) {
			printf("<li>\"%s\": \"%s\"</li>\n", cgi_input[i].name, cgi_input[i].value);
		}
		puts("</ul>\n");
	}

	if(!dynarr_empty(cgi_cookies)) {
		html_heading(3, "Cookies");
		printf("<ul>\n");
		for(i=0; i<dynarr_size(cgi_cookies); i++) {
			printf("<li>\"%s\": \"%s\"</li>\n", cgi_cookies[i].name, cgi_cookies[i].value);
		}
		puts("</ul>\n");
	}
}

static int load_entries(int compo_id)
{
	return 0;
}
