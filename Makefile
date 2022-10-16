# uwudev - scrunklificator
# Copyright (C) 2022 ArcNyxx
# see LICENCE file for licensing information

.POSIX:

include config.mk

SRC = uwudev.c
OBJ = $(SRC:.c=.o)

all: uwudev

$(OBJ): config.mk

.c.o:
	$(CC) $(CFLAGS) -c $<

uwudev: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f uwudev $(OBJ) uwudev-$(VERSION).tar.gz

dist: clean
	mkdir -p uwudev-$(VERSION)
	cp -R README LICENCE.GPL-2.0 LICENCE-MIT Makefile config.mk uwudev.1 \
		$(SRC) uwudev-$(VERSION)
	tar -cf - uwudev-$(VERSION) | gzip -c > uwudev-$(VERSION).tar.gz
	rm -rf uwudev-$(VERSION)

install: all
	mkdir -p $(PREFIX)/bin $(MANPREFIX)/man1
	cp -f uwudev $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/uwudev
	sed 's/VERSION/$(VERSION)/g' < uwudev.1 > $(MANPREFIX)/man1/uwudev.1
	chmod 644 $(MANPREFIX)/man1/uwudev.1

uninstall:
	rm -f $(PREFIX)/bin/uwudev $(MANPREFIX)/man1/uwudev.1

.PHONY: all clean dist install uninstall
