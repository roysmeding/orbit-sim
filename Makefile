CC     := gcc
CFLAGS += -Wall -Wextra -std=c99 -O3 -fopt-info-all=opts

all : orbit

clean :
	rm -f orbit test_universe

tests : test_universe

.PHONY: all clean tests

orbit : orbit.c universe.h universe.c sim.c
	${CC} ${CFLAGS} -o $@ orbit.c universe.c sim.c -lm

test_universe : test_universe.c universe.h universe.c sim.c
	${CC} ${CFLAGS} -o $@ test_universe.c universe.c sim.c -lm
