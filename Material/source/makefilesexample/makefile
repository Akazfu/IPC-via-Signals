# hello_world: hello_world.c
#     gcc -o hello_world hello_world.c

###################################################################
# .PHONY: clean
# hello_world: hello_world.c
#     gcc -o hello_world hello_world.c

# clean:
#     $(RM) hello_world
#
###################################################################
# .PHONY: clean
# hello_world: hello_world.c

# clean:
#     $(RM) hello_world
###################################################################
CC = gcc
CFLAGS = -Wall -pedantic -Werror
LDFLAGS =

.PHONY: clean

hello_world2: hello_world2.c worlds.o
worlds.o: worlds.c worlds.h

clean:
	$(RM) hello_world2 *.o
