#include <stdio.h>
#include <stdlib.h>
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
	const char *str, *img;
	char *endp;
	struct entry *ent;
	char buf[128];
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
	ent = entries + id;

	sprintf(buf, "current/entry%02d/preview.jpg", id);
	if(access(buf, R_OK) == -1) {
		img = "img/none.gif";
	} else {
		img = strdup_nf(buf);
	}

	html_heading(1, "ORRC Entry");
	html_sep();
	html_heading(2, ent->title);

	html_img(img, ent->title, ent->img);

	printf("<p>by <b>%s</b></p>\n", ent->user);

	if(ent->desc) {
		html_heading(3, "Description");
		printf("<p>%s</p>\n", ent->desc);
	}


	sprintf(buf, "current/entry%02d/%s", id, ent->archive);
	if(stat(buf, &st) != -1) {
		fsize = st.st_size;
	}

	html_heading(3, "Download");
	printf("<p><a href=\"%s\">%s</a> (%s)</p>\n", ent->archive, ent->archive,
			fsize > 0 ? filesize_str(fsize) : "??");
}

static void gallery(void)
{
}
