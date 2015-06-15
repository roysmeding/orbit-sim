#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "universe.h"

const char UNIVERSE_FILE[] = "universe.txt";

int main(int argc, char **argv) {
	// read data file
	fprintf(stderr, "Loading universe file %s...\n", UNIVERSE_FILE);
	struct universe *universe = universe_load(UNIVERSE_FILE);
	fprintf(stderr, "Loaded %zd planets:\n", universe->num_planets);
	fprintf(stderr, "\t%2s." " %-10s"
			"\t%-12s"  "\t%-12s"
			"\t %-21s" "\t %-21s" "\t %-21s"
			"\t%-22s" "\t%-22s" "\t%-22s" "\n",
			"no", "name",
			"mass [kg]", "radius [km]",
			 "x [AU]",    "y [AU]",    "z [AU]",
			"vx [AU/d]", "vy [AU/d]", "vz [AU/d]"
	       );
	for(size_t planet = 0; planet < universe->num_planets; planet++) {
		struct planet *p = &universe->planets[planet];
		fprintf(stderr, "\t%2zd." " %-10s"
				"\t%12.6le"  "\t%12.6le"
				"\t% 22.15le" "\t% 22.15le" "\t% 22.15le"
				"\t% 22.15le" "\t% 22.15le" "\t% 22.15le" "\n",
				planet+1, p->name,
				p->mass, p->radius,
				p->x,  p->y,  p->z,
				p->vx, p->vy, p->vz
			);
	}

	universe_free(universe);

	return 0;
}
