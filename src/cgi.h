#ifndef CGI_H_
#define CGI_H_

struct cgivar {
	char *name;
	char *value;
};

extern struct cgivar *cgi_input, *cgi_cookies;

int cgi_init(void);

int cgi_read_input(void);
int cgi_proc_query(const char *qstr);

void cgi_set_cookie(const char *name, const char *val);

void cgi_begin_output(void);

void cgi_panic(const char *fmt, ...);


#endif	/* CGI_H_ */
