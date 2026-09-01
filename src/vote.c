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
	const char *thumbimg;
	struct entry *ent;

	html_heading(1, "ORRC voting");
	html_sep();

	if(dynarr_empty(entries)) {
		puts("<p>There is no current voting round.</p>");
		return;
	}

	printf("<p><b>Theme</b>: %s</p>\n", theme);

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
