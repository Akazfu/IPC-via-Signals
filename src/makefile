.PHONY: clean

covertsigs.double: covertsigs.c
	gcc -g -Wall -std=c99 -D_GNU_SOURCE -o covertsigs covertsigs.c

covertsigs.single: covertsigs.c
	gcc -g -Wall -std=c99 -D_GNU_SOURCE -D SINGLE -o covertsigs covertsigs.c

clean:
	$(RM) covertsigs