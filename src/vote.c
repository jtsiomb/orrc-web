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
#include <unistd.h>
#include "orrc.h"
#include "cgi.h"
#include "html.h"
#include "dynarr.h"


void op_vote(void)
{
	int i, j;
	char buf[256];
	struct entry *ent;

	cgi_begin_output();
	html_begin();

	html_heading(1, "ORRC voting");
	html_sep();

	if(dynarr_empty(entries)) {
		puts("<p>There is no current voting round.</p>");
		return;
	}

	if(cgi_find_cookie("voted")) {
		vote_reject_msg();
		return;
	}

	printf("<p><big><b>Theme</b>: %s</big></p>\n", theme);

	html_heading(2, "Entries");

	puts("<form action=\"orrc\" method=\"post\">");
	puts("<input type=\"hidden\" name=\"cmd\" value=\"submit\">");
	puts("<table width=\"100%\" border=\"1\">");
	for(i=0; i<dynarr_size(entries); i++) {
		ent = entries + i;

		sprintf(buf, "current/entry%02d/", ent->id);
		if(access(buf, X_OK) == -1) continue;

		printf("<tr><td width=\"200\"><a href=\"orrc?cmd=show&dir=current&entry=%d\">",
				ent->id);

		printf("<img src=\"%s\" alt=\"%s\">", ent->imgthumb, ent->title);

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

static const char *rejstr = "<p>It appears you have already voted for this round.</p>\n"
	"<p>If you think this is a mistake, please contact "
	"<a href=\"mailto:orrc@mutantstargoat.com\">orrc@mutantstargoat.com</a></p>\n"
	"<p>Feel free to visit the <a href=\"orrc?cmd=show\">gallery</a> to see all the entries</p>\n";

void vote_reject_msg(void)
{
	html_heading(2, "Vote rejected");
	puts(rejstr);
}
