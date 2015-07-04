CC     := clang
CFLAGS += -Wall -Wextra -std=gnu11 -O3
CFLAGS += -g
CFLAGS += -march=native
#CFLAGS += -Wall -Wextra -std=gnu11 -g -march=sandybridge -mtune=sandybridge -O3
#CFLAGS += -ffast-math -funroll-loops -fprefetch-loop-arrays  -fvariable-expansion-in-unroller -ftree-loop-distribution -floop-nest-optimize -fselective-scheduling -fsel-sched-pipelining -fsel-sched-pipelining-outer-loops

all : orbit

clean :
	rm -f orbit

.PHONY: all clean

orbit : orbit.c universe.h universe.c sim.c
	${CC} ${CFLAGS} -o $@ orbit.c universe.c sim.c -lm
