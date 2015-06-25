#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "universe.h"

void planet_init(struct planet *planet, char *name, double mass, double radius, double x, double y, double z, double vx, double vy, double vz) {
	planet->name   = name;
	planet->mass   = mass;
	planet->radius = radius;

	planet->pos[0] =  x, planet->pos[1] =  y, planet->pos[2] =  z, planet->pos[3] = 0.;
	planet->vel[0] = vx, planet->vel[1] = vy, planet->vel[2] = vz, planet->vel[3] = 0.;
}

struct universe *universe_new(size_t num_planets, double time) {
	struct universe *universe = aligned_alloc(PLANET_ALIGN, sizeof(struct universe));
	universe->time = time;
	universe->num_planets = num_planets;
	universe->planets = aligned_alloc(PLANET_ALIGN, num_planets * sizeof(struct planet));

	return universe;
}

void universe_free(struct universe *universe) {
	for(size_t planet = 0; planet < universe->num_planets; planet++)
		free(universe->planets[planet].name);
	free(universe->planets);
	free(universe);
}

struct universe *universe_load(const char* filename) {
	size_t num_planets;
	struct universe *universe;
	struct tm tm;

	FILE *infile = fopen(filename, "r");
	fscanf(infile, "%d %d %d %d %d %d",
			&tm.tm_year, &tm.tm_mon, &tm.tm_mday,
			&tm.tm_hour, &tm.tm_min, &tm.tm_sec
		);

	fscanf(infile, "%zu\n", &num_planets);

	universe = universe_new(num_planets, (double) mktime(&tm));

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

void universe_dump(FILE *dest, struct universe *universe) {
	fprintf(dest, "\t%2s." " %-10s"
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
		fprintf(dest, "\t%2zd." " %-10s"
				"\t%12.6le"  "\t%12.6le"
				"\t% 22.15le" "\t% 22.15le" "\t% 22.15le"
				"\t% 22.15le" "\t% 22.15le" "\t% 22.15le" "\n",
				planet+1, p->name,
				p->mass, p->radius,
				p->pos[0], p->pos[1], p->pos[2],
				p->vel[0], p->vel[1], p->vel[2]
			);
	}
}
