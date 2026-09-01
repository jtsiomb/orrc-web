#include <stdio.h>
#include "orrc.h"
#include "cgi.h"
#include "html.h"


void op_submit(void)
{
	html_heading(1, "ORRC voting");
	html_sep();

	html_heading(2, "Votes submitted");

	puts("<p>Thank you for voting!</p>");
}
