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
