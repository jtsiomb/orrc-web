#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "html.h"

static char *page_title;
static char *page_stylesheet;
static char *page_font;
static int page_font_size = 4;
static unsigned int colors[NUM_HTML_ELEMENTS] = {
	0xffffff, 0,
	0x0000ff, 0x000088,
	0xffffff, 0xffffff, 0xffffff
};

void html_set_title(const char *title)
{
	free(page_title);
	page_title = strdup(title);
}

void html_set_css(const char *css)
{
	free(page_stylesheet);
	page_stylesheet = strdup(css);
}

void html_set_color(enum html_element elem, unsigned int color)
{
	colors[elem] = color;
}

void html_set_font(const char *name, int sz)
{
	free(page_font);
	page_font = strdup(name);
	page_font_size = sz;
}

void html_begin(void)
{
	puts("<html>");
	puts("<head>");
	if(page_title) {
		printf("  <title>%s</title>\n", page_title);
	}
	puts("  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	if(page_stylesheet) {
		printf("  <link rel=\"stylesheet\" type=\"text/css\" href=\"%s\">\n", page_stylesheet);
	}
	puts("</head>");

	printf("<body bgcolor=\"#%06x\" text=\"#%06x\" vlink=\"#%06x\" link=\"#%06x\">\n",
			colors[HTML_BG], colors[HTML_TEXT], colors[HTML_VLINK], colors[HTML_LINK]);
	if(page_font) {
		printf("<font face=\"%s\" size=\"%d\">\n", page_font, page_font_size);
	}
}

void html_end(void)
{
	if(page_font) {
		puts("</font>");
	}
	puts("</body></html>");
}

void html_heading(int lvl, const char *text)
{
	printf("<h%d><font color=\"#%06x\">%s</font></h%d>\n", lvl, colors[HTML_H1 + lvl - 1],
			text, lvl);
}

void html_sep(void)
{
	puts("<hr>");
}
