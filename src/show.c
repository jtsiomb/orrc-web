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
#include <unistd.h>
#include <sys/stat.h>
#include "orrc.h"
#include "cgi.h"
#include "html.h"
#include "dynarr.h"

static void gallery(void);

void op_show(void)
{
	int id;
	const char *str;
	char *endp;
	struct entry *ent;
	char buf[256];
	struct stat st;
	size_t fsize = 0;

	if(!(str = cgi_find_input("entry"))) {
		gallery();
		return;
	}
	id = strtol(str, &endp, 10);
	if(endp == str || id < 0 || id >= dynarr_size(entries)) {
		cgi_panic("invalid entry: \"%s\"", str);
	}
	ent = find_entry(id);


	cgi_begin_output();
	html_begin();

	html_heading(1, "ORRC Entry");
	html_sep();
	html_heading(2, ent->title);

	printf("<p>by <b>%s</b></p>\n", ent->user);
	html_img(ent->imgprev, ent->title, ent->img);

	if(ent->desc) {
		html_heading(3, "Description");
		printf("<p>%s</p>\n", ent->desc);
	}

	snprintf(buf, sizeof buf, "%s/entry%02d/%s", round_dir, id, ent->archive);
	if(stat(buf, &st) != -1) {
		fsize = st.st_size;
	}

	html_heading(3, "Download");
	printf("<p><a href=\"%s\">%s</a> (%s)</p>\n", ent->archive, ent->archive,
			fsize > 0 ? filesize_str(fsize) : "??");
}

#define NCOLS	4

static void gallery(void)
{
	int i, col;
	struct entry *ent;


	cgi_begin_output();
	html_begin();

	html_heading(1, "ORRC Gallery");
	html_sep();

	html_heading(2, "Round");

	if(round_date) {
		printf("<p>%s</p>\n", round_date);
	}
	if(theme) {
		printf("<p><b>Theme</b>: <i>%s</i></p>\n", theme);
	}

	if(!cgi_find_cookie("voted") && strcmp(round_dir, "current") == 0) {
		puts("<p><a href=\"orrc?cmd=vote\">Vote for your favourite entries!</a></p>");
	}

	html_heading(2, "Entries");

	if(dynarr_empty(entries)) return;

	puts("<table cellspacing=\"15\">");
	col = 0;
	ent = entries;
	for(i=0; i<dynarr_size(entries); i++) {
		if(!col) puts(i ? "</tr><tr>" : "<tr>");

		printf("<td><center><a href=\"orrc?cmd=show&dir=%s&entry=%d\">", round_dir, ent->id);
		html_img(ent->imgthumb, ent->title, 0);
		printf("<br><i>%s</i></a><br>by %s</center></td>\n", ent->title, ent->user);

		if(++col >= NCOLS) col = 0;
		ent++;
	}
	puts("</tr></table>");
}
