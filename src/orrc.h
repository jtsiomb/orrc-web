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
#ifndef ORRC_H_
#define ORRC_H_

#include <stddef.h>

enum {
	ST_VOTE,
	ST_SUBMIT,
	ST_SHOW
};

struct entry {
	int id;
	char *user;
	char *title, *desc;
	char *img, *archive;
	/* derived */
	char *imgthumb, *imgprev;
};

#define ROOT_DIR		"/var/www/orrc"

extern struct entry *entries;
extern char *theme;
extern const char *round_dir;
extern const char *round_date;

void op_vote(void);
void op_submit(void);
void op_show(void);
void vote_reject_msg(void);

struct entry *find_entry(int id);

char *strdup_nf(const char *s);

const char *filesize_str(size_t sz);

#endif	/* ORRC_H_ */
