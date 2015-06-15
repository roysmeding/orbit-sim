CC     := clang
CFLAGS += -Wall -Wextra -std=c99

all : tests

clean :
	rm -f test_universe

tests : test_universe

.PHONY: all clean tests

test_universe : test_universe.c universe.h universe.c
	${CC} ${CFLAGS} -o $@ test_universe.c universe.c
