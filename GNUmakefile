CFLAGS := -Wall -Wextra
#CFLAGS += -g
#CFLAGS += -Werror

gpsdate: gpsdate.c GNUmakefile
	gcc $(CFLAGS) gpsdate.c -o gpsdate -lgps -lm

.PHONY: install
install: gpsdate
	cp gpsdate /usr/local/bin/
