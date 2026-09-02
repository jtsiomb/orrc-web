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

void html_img(const char *img, const char *desc, const char *link);

void html_parbegin(void);
void html_parend(void);
void html_newline(void);

#endif	/* HTML_H_ */
