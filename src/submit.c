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
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include "orrc.h"
#include "cgi.h"
#include "html.h"
#include "dynarr.h"

#define SCOREFILE	"current/score"

static int *scores;
static int num_entries;

static void write_scores();

void op_submit(void)
{
	int i, id, val, count, multivote = 0;
	struct cgivar *var;
	char *endp;

	if(cgi_find_cookie("voted")) {
		multivote = 1;
		goto skip_vote;
	}

	num_entries = dynarr_size(entries);
	if(!(scores = calloc(num_entries, sizeof *scores))) {
		cgi_panic("failed to allocate scoring buffer");
	}

	count = 0;
	for(i=0; i<dynarr_size(cgi_input); i++) {
		var = cgi_input + i;
		if(sscanf(var->name, "score%d", &id) != 1) {
			continue;
		}
		if(id < 0 || id >= num_entries) {
			continue;
		}
		val = strtol(var->value, &endp, 10);
		if(endp == var->value || val <= 0) continue;
		if(val > 10) val = 10;

		if(scores[id] < val) scores[id] = val;
		count++;
	}
	if(count) {
		write_scores();
		cgi_set_cookie("voted", "yes", CGI_COOKIE_MONTH);
	}

skip_vote:
	cgi_begin_output();
	html_begin();

	html_heading(1, "ORRC voting");
	html_sep();

	if(multivote) {
		vote_reject_msg();
	} else {
		html_heading(2, "Votes submitted");

		puts("<p>Thank you for voting!</p>");
	}
}

static void write_scores()
{
	int i, fd;
	struct flock flk;
	char line[256];

	if((fd = open(SCOREFILE, O_WRONLY | O_CREAT | O_APPEND, 0664)) == -1) {
		cgi_panic("failed to open score file, try again later: %s", strerror(errno));
	}

	flk.l_type = F_WRLCK;
	flk.l_start = flk.l_len = 0;
	flk.l_whence = SEEK_SET;

	if(fcntl(fd, F_SETLK, &flk) == -1) {
		cgi_panic("failed to lock score file");
	}

	for(i=0; i<num_entries; i++) {
		if(!scores[i]) continue;

		sprintf(line, "%s id%d %d\n", cgi_remote_addr, i, scores[i]);
		write(fd, line, strlen(line));
	}

	flk.l_type = F_UNLCK;
	flk.l_start = flk.l_len = 0;
	flk.l_whence = SEEK_SET;
	fcntl(fd, F_SETLK, &flk);

	close(fd);
}
