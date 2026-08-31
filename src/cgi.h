#ifndef CGI_H_
#define CGI_H_

struct cgivar {
	char *name;
	char *value;
	unsigned int expires;	/* for new cookies */
};

enum {
	CGI_COOKIE_SESSION	= 0,
	CGI_COOKIE_DAY		= 24 * 60 * 60,
	CGI_COOKIE_MONTH	= 24 * 60 * 60 * 30
};

enum cgi_input_type {
	CGI_QUERY,
	CGI_COOKIE
};

enum cgi_request { CGI_GET, CGI_POST };

extern enum cgi_request cgi_req;
extern struct cgivar *cgi_input, *cgi_cookies;

int cgi_init(void);

int cgi_read_input(void);
int cgi_proc_input(const char *str, enum cgi_input_type type);

const char *cgi_find_input(const char *name);

void cgi_set_cookie(const char *name, const char *val, unsigned int expires);
const char *cgi_find_cookie(const char *name);

void cgi_begin_output(void);

#ifdef __GNUC__
__attribute__((noreturn))
#endif
void cgi_panic(const char *fmt, ...);


#endif	/* CGI_H_ */
