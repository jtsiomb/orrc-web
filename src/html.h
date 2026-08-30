#ifndef HTML_H_
#define HTML_H_

enum html_element {
	HTML_TEXT,
	HTML_BG,
	HTML_LINK,
	HTML_VLINK,
	HTML_H1,
	HTML_H2,
	HTML_H3,
	NUM_HTML_ELEMENTS
};

void html_set_title(const char *title);
void html_set_css(const char *css);
void html_set_color(enum html_element elem, unsigned int color);
void html_set_font(const char *name, int sz);

void html_begin(void);
void html_end(void);

void html_heading(int lvl, const char *text);
void html_sep(void);

#endif	/* HTML_H_ */
