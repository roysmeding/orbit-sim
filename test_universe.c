#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "universe.h"

const char UNIVERSE_FILE[] = "universe.txt";

int main(__attribute__((unused)) int argc, __attribute__((unused))char **argv) {
	// read data file
	fprintf(stderr, "Loading universe file %s...\n", UNIVERSE_FILE);
	struct universe *universe = universe_load(UNIVERSE_FILE);
	fprintf(stderr, "Loaded %zd planets:\n", universe->num_planets);
	universe_dump(stderr, universe);
	universe_free(universe);

	return 0;
}
