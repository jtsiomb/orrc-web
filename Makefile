src = $(wildcard src/*.c)
obj = $(src:.c=.o)
dep = $(src:.c=.d)
bin = cgi-bin/orrc

CFLAGS = -std=gnu89 -pedantic -Wall -g -MMD
LDFLAGS = -static -ltreestore

$(bin): $(obj)
	$(CC) -o $@ $(obj) $(LDFLAGS)

-include $(dep)

.PHONY: clean
clean:
	rm -f $(obj) $(bin)

.PHONY: cleandep
cleandep:
	rm -f $(dep)

.PHONY: archive
archive:
	git archive -o orrc.tar.gz --prefix=orrc/ HEAD
