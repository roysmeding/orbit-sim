#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "universe.h"

struct universe *create_universe(const char* filename) {
	size_t num_planets;
	struct universe *universe;
	struct tm tm;

	FILE *infile = fopen(filename, "r");
	fscanf(infile, "%d %d %d %d %d %d",
			&tm.tm_year, &tm.tm_mon, &tm.tm_mday,
			&tm.tm_hour, &tm.tm_min, &tm.tm_sec
		);

	fscanf(infile, "%zu\n", &num_planets);

	universe = universe_new(num_planets, mktime(&tm));

	for(size_t planet = 0; planet < num_planets; planet++) {
		int c;
		char *name;
		double mass, radius;
		double x, y, z, vx, vy, vz;

		// check comment
		c = fgetc(infile);
		while(c == '#') {
			while(c != '\n')
				c = fgetc(infile);
			c = fgetc(infile);
		}

		ungetc(c, infile);

		fscanf(infile, "%ms %lf %lf %lf %lf %lf %lf %lf %lf\n",
				&name,
				&mass, &radius,
				&x, &y, &z,
				&vx, &vy, &vz
			);

		planet_init(&universe->planets[planet],
				name,
				mass, radius,
				x, y, z,
				vx, vy, vz
			);
	}

	fclose(infile);

	return universe;
}

const char UNIVERSE_FILE[] = "universe.txt";

int main(int argc, char **argv) {
	// read data file
	fprintf(stderr, "Loading universe file %s...\n", UNIVERSE_FILE);
	struct universe *universe = create_universe(UNIVERSE_FILE);
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
