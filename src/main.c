#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "orrc.h"
#include "cgi.h"
#include "dynarr.h"
#include "html.h"
#include "treestor.h"


struct entry *entries;
char *theme;

static int state;

static int load_entries(void);
static void debug_output(void);


int main(void)
{
	const char *str;

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

	state = ST_VOTE;
	if((str = cgi_find_input("cmd"))) {
		if(strcmp(str, "show") == 0) {
			state = ST_SHOW;
		} else if(strcmp(str, "submit") == 0) {
			state = ST_SUBMIT;
		}
	}

	if(state == ST_VOTE || state == ST_SHOW) {
		if(load_entries() == -1) {
			cgi_panic("failed to load competition entries\n");
		}
	}

	cgi_begin_output();
	html_begin();

	switch(state) {
	case ST_VOTE:
		op_vote();
		break;

	case ST_SUBMIT:
		op_submit();
		break;

	case ST_SHOW:
		op_show();
		break;

	default:
		cgi_panic("unknown state");
	}

	debug_output();
	html_end();
	return 0;
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
	const char *str, *title, *user, *desc, *img, *archive;
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
			if(!(archive = ts_get_attr_str(node, "archive", 0))) goto next;
			desc = ts_get_attr_str(node, "desc", 0);
			img = ts_get_attr_str(node, "image", 0);

			memset(&entry, 0, sizeof entry);
			entry.id = num;
			entry.title = strdup_nf(title);
			entry.user = strdup_nf(user);
			if(desc) entry.desc = strdup_nf(desc);
			if(img) entry.img = strdup_nf(img);
			entry.archive = strdup_nf(archive);

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

const char *filesize_str(size_t sz)
{
	static const char *suffix[] = {"bytes", "kb", "mb", "gb", "tb", 0};
	static char buf[32];
	int idx;

	idx = 0;
	while(sz >= 1024 && suffix[idx + 1]) {
		sz >>= 10;
		idx++;
	}

	sprintf(buf, "%lu %s", sz, suffix[idx]);
	return buf;
}
