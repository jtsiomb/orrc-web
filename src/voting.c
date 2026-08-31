#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cgi.h"
#include "dynarr.h"
#include "html.h"
#include "treestor.h"

enum {
	ST_VOTE,
	ST_PROC
};

struct entry {
	int id;
	char *user;
	char *title, *desc;
};

#define ROOT_DIR		"/var/www/orrc"

static int state;
static char *theme;
static struct entry *entries;

static void scr_voting(void);
static void scr_procvote(void);
static int load_entries(void);
static void debug_output(void);


char *strdup_nf(const char *s);


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

	if(cgi_req == CGI_POST) {
		state = ST_PROC;
	} else {
		state = ST_VOTE;
	}

	if(state == ST_VOTE) {
		if(load_entries() == -1) {
			cgi_panic("failed to load competition entries\n");
		}
	}

	cgi_begin_output();
	html_begin();

	html_heading(1, "ORRC voting");
	html_sep();

	switch(state) {
	case ST_VOTE:
		scr_voting();
		break;

	case ST_PROC:
		scr_procvote();
		break;

	default:
		cgi_panic("unknown state");
	}

	debug_output();
	html_end();
	return 0;
}

static void scr_voting(void)
{
	int i, j;
	char buf[256];
	const char *thumbimg;
	struct entry *ent;

	if(dynarr_empty(entries)) {
		puts("<p>There is no current voting round.</p>");
		return;
	}

	printf("<p><b>Theme</b>: %s</p>\n", theme);

	html_heading(2, "Entries");

	puts("<form action=\"voting\" method=\"post\">\n");
	puts("<table width=\"100%\" border=\"1\">");
	for(i=0; i<dynarr_size(entries); i++) {
		ent = entries + i;
		sprintf(buf, "current/entry%02d/", ent->id);
		if(access(buf, X_OK) == -1) continue;
		printf("<tr><td width=\"200\"><a href=\"%s\">", buf);

		sprintf(buf, "current/entry%02d/thumb.jpg", ent->id);
		thumbimg = access(buf, R_OK) == -1 ? "img/none.gif" : buf;
		printf("<img src=\"%s\" alt=\"%s\">", thumbimg, ent->title);

		printf("<br><center><b>%s</b></center></a></td>", ent->title);
		printf("<td>%s</td><td>%s</td>\n", ent->user, ent->desc);

		printf("<td><select name=\"score%02d\">\n", ent->id);
		for(j=0; j<10; j++) {
			printf("  <option value=\"%d\">%d</option>\n", j, j);
		}
		puts("</select></td></tr>");
	}
	puts("</table><br>");
	puts("<center><input type=\"submit\" value=\"Submit\"></center></form>");
}

static void scr_procvote(void)
{
	html_heading(2, "Votes submitted");

	puts("<p>Thank you for voting!</p>");
}

static void debug_output(void)
{
	int i;

	html_sep();
	html_heading(2, "Debug section");

	printf("<p>HTTP method: %s</p>\n", cgi_req == CGI_POST ? "POST" : "GET");

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

static int load_entries(void)
{
	struct ts_node *root, *node;
	struct entry entry;
	const char *str, *title, *user, *desc;
	int num;

	entries = dynarr_alloc_nf(0, sizeof *entries);

	if(!(root = ts_load("current/entries")) || strcmp(root->name, "orrc") != 0) {
		return 0;
	}
	if((str = ts_lookup_str(root, "orrc.theme", 0))) {
		theme = strdup_nf(str);
	}

	node = root->child_list;
	while(node) {
		if(strcmp(node->name, "entry") == 0) {
			if((num = ts_get_attr_int(node, "id", -1)) < 0) goto next;
			if(!(title = ts_get_attr_str(node, "title", 0))) goto next;
			if(!(user = ts_get_attr_str(node, "user", 0))) goto next;
			desc = ts_get_attr_str(node, "desc", 0);

			memset(&entry, 0, sizeof entry);
			entry.id = num;
			entry.title = strdup_nf(title);
			entry.user = strdup_nf(user);
			if(desc) entry.desc = strdup_nf(desc);

			dynarr_push_nf(entries, &entry);
		}
next:	node = node->next;
	}

	ts_free_tree(root);
	return 0;
}

char *strdup_nf(const char *s)
{
	char *res;
	if(!(res = strdup(s))) {
		cgi_panic("failed to allocate string");
	}
	return res;
}
