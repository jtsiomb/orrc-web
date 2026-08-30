src = $(wildcard src/*.c)
obj = $(src:.c=.o)
dep = $(src:.c=.d)
bin = cgi-bin/voting

CFLAGS = -std=gnu89 -pedantic -Wall -g -MMD

$(bin): $(obj)
	$(CC) -o $@ $(obj) $(LDFLAGS)

-include $(dep)

.PHONY: clean
clean:
	rm -f $(obj) $(bin)

.PHONY: cleandep
cleandep:
	rm -f $(dep)
