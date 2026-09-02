/*
ORRC - Own-Renderer Render Competition web backend
Copyright (C) 2026 John Tsiombikas <nuclear@mutantstargoat.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include "orrc.h"
#include "cgi.h"
#include "dynarr.h"
#include "html.h"
#include "treestor.h"


struct entry *entries;
char *theme;
const char *round_dir;
const char *round_date;
static char round_date_buf[128];

static int state;

static void debug_output(void);
static int load_entries(void);
static int parse_date(struct tm *tm, const char *dstr);
static char *locate_image(const char *dir, const char *basename);
static void shuffle(struct entry *arr, int count);
static int valid_dirname(const char *s);


int main(void)
{
	const char *str;

	/* make sure we're in the correct working directory to open files */
	chdir(ROOT_DIR);

	html_set_title("ORRC - Own-Renderer Render Competition");
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

	state = ST_SHOW;
	if((str = cgi_find_input("cmd"))) {
		if(strcmp(str, "vote") == 0) {
			state = ST_VOTE;
		} else if(strcmp(str, "submit") == 0) {
			state = ST_SUBMIT;
		}
	}

	/* default to current, and also force current if voting */
	if(state == ST_VOTE || state == ST_SUBMIT || !(round_dir = cgi_find_input("dir"))) {
		round_dir = "current";
	} else {
		if(!valid_dirname(round_dir)) {
			cgi_panic("invalid \"dir\" parameter");
		}
	}

	if(load_entries() == -1) {
		cgi_panic("failed to load competition entries");
	}

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

	html_sep();
	puts("<p><a href=\"/\">Back to the main page</a></p>");
	puts("<p><small>ORRC backend is free software, <a href=\"http://www.gnu.org/licenses/agpl\">AGPLv3</a> or later.");
	puts("- <a href=\"orrc.tar.gz\">source code</a>");
	puts("- <a href=\"https://github.com/jtsiomb/orrc-web\">github project</a></small></p>");

	if((str = cgi_find_input("debug")) && strcmp(str, "false") != 0 && strcmp(str, "0") != 0) {
		debug_output();
	}
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
			printf("<li>\"%s\": \"%s\"</li>\n", html_droptags(cgi_input[i].name),
					html_droptags(cgi_input[i].value));
		}
		puts("</ul>\n");
	}

	if(!dynarr_empty(cgi_cookies)) {
		html_heading(3, "Cookies");
		printf("<ul>\n");
		for(i=0; i<dynarr_size(cgi_cookies); i++) {
			printf("<li>\"%s\": \"%s\"</li>\n", html_droptags(cgi_cookies[i].name),
					html_droptags(cgi_cookies[i].value));
		}
		puts("</ul>\n");
	}
}

static const char *monthstr[] = {0, "January", "February", "March", "April", "May",
	"June", "July", "August", "September", "October", "November", "December"};

static int load_entries(void)
{
	struct ts_node *root, *node;
	struct entry entry;
	const char *str, *title, *user, *desc, *img, *archive, *start, *end;
	int num;
	char path[256];
	struct tm tm0, tm1;

	entries = dynarr_alloc_nf(0, sizeof *entries);

	snprintf(path, sizeof path, "%s/entries", round_dir);
	if(!(root = ts_load(path)) || strcmp(root->name, "orrc") != 0) {
		return 0;
	}
	if((str = ts_get_attr_str(root, "theme", 0))) {
		theme = strdup_nf(str);
	}

	start = ts_get_attr_str(root, "start", 0);
	end = ts_get_attr_str(root, "end", 0);
	if(parse_date(&tm0, start) != -1 && parse_date(&tm1, end) != -1) {
		round_date = (const char*)round_date_buf;
		if(tm0.tm_year != tm1.tm_year) {
			sprintf(round_date_buf, "%d %s %d - %d %s %d", tm0.tm_mday, monthstr[tm0.tm_mon],
					tm0.tm_year, tm1.tm_mday, monthstr[tm1.tm_mon], tm1.tm_year);
		} else if(tm0.tm_mon == tm1.tm_mon) {
			sprintf(round_date_buf, "%d - %d %s %d", tm0.tm_mday, tm1.tm_mday,
					monthstr[tm0.tm_mon], tm0.tm_year);
		} else {
			sprintf(round_date_buf, "%d %s - %d %s %d", tm0.tm_mday, monthstr[tm0.tm_mon],
					tm1.tm_mday, monthstr[tm1.tm_mon], tm0.tm_year);
		}
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

			/* populate derived fields */
			snprintf(path, sizeof path, "%s/entry%02d", round_dir, entry.id);
			entry.imgthumb = locate_image(path, "thumb");
			entry.imgprev = locate_image(path, "preview");

			dynarr_push_nf(entries, &entry);
		}
next:	node = node->next;
	}

	shuffle(entries, dynarr_size(entries));

	ts_free_tree(root);
	return 0;
}

static int parse_date(struct tm *tm, const char *dstr)
{
	char buf[5] = {0};

	if(!dstr || strlen(dstr) < 8) return -1;

	memcpy(buf, dstr, 4);
	if((tm->tm_year = atoi(buf)) < 1900) {
		return -1;
	}

	memcpy(buf, dstr + 4, 2);
	buf[2] = 0;
	if((tm->tm_mon = atoi(buf)) < 1 || tm->tm_mon > 12) {
		return -1;
	}
	if((tm->tm_mday = atoi(dstr + 6)) < 1 || tm->tm_mday > 31) {
		return -1;
	}
	return 0;
}

struct entry *find_entry(int id)
{
	int i;

	for(i=0; i<dynarr_size(entries); i++) {
		if(entries[i].id == id) {
			return entries + i;
		}
	}
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
	size_t frac = 0;

	idx = 0;
	while(sz >= 1024 && suffix[idx + 1]) {
		frac = sz & 0x3ff;
		sz >>= 10;
		idx++;
	}

	if(frac) {
		sprintf(buf, "%lu.%lu %s", sz, frac * 10 >> 10, suffix[idx]);
	} else {
		sprintf(buf, "%lu %s", sz, suffix[idx]);
	}
	return buf;
}

static char *locate_image(const char *dir, const char *basename)
{
	static const char *suffixes[] = {".png", ".jpg", ".jpeg", ".gif", 0};
	int i;
	char buf[256];

	for(i=0; suffixes[i]; i++) {
		sprintf(buf, "%s/%s%s", dir, basename, suffixes[i]);
		if(access(buf, R_OK) == 0) {
			return strdup_nf(buf);
		}
	}
	return strdup_nf("img/none.gif");
}

static void shuffle(struct entry *arr, int count)
{
	int i, r;
	struct entry tmp;

	srand(time(0));
	for(i=1; i<count; i++) {
		r = rand() % (i + 1);
		if(i != r) {
			tmp = arr[i];
			arr[i] = arr[r];
			arr[r] = tmp;
		}
	}
}

static int valid_dirname(const char *s)
{
	if(strlen(s) > 32) return 0;

	while(*s) {
		if(!isalnum((unsigned char)*s++)) {
			return 0;
		}
	}
	return 1;
}
