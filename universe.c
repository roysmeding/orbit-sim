#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "universe.h"

void init(struct body *body, char *name, double mass, double radius, double x, double y, double z, double vx, double vy, double vz) {
	body->name   = name;
	body->mass   = mass;
	body->radius = radius;

	body->pos[0] =  x, body->pos[1] =  y, body->pos[2] =  z, body->pos[3] = 0.;
	body->vel[0] = vx, body->vel[1] = vy, body->vel[2] = vz, body->vel[3] = 0.;
}

struct universe *universe_new(size_t num_bodies, double time) {
	struct universe *universe = aligned_alloc(BODY_ALIGN, sizeof(struct universe));
	universe->time = time;
	universe->num_bodies = num_bodies;
	universe->bodies = aligned_alloc(BODY_ALIGN, num_bodies * sizeof(struct body));

	return universe;
}

void universe_free(struct universe *universe) {
	for(size_t body = 0; body < universe->num_bodies; body++)
		free(universe->bodies[body].name);
	free(universe->bodies);
	free(universe);
}

struct universe *universe_load(const char* filename) {
	size_t num_bodies;
	struct universe *universe;
	struct tm tm;

	FILE *infile = fopen(filename, "r");
	fscanf(infile, "%d %d %d %d %d %d",
			&tm.tm_year, &tm.tm_mon, &tm.tm_mday,
			&tm.tm_hour, &tm.tm_min, &tm.tm_sec
		);

	fscanf(infile, "%zu\n", &num_bodies);

	universe = universe_new(num_bodies, (double) mktime(&tm));

	for(size_t body = 0; body < num_bodies; body++) {
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

		init(&universe->bodies[body],
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
	for(size_t body = 0; body < universe->num_bodies; body++) {
		struct body *p = &universe->bodies[body];
		fprintf(dest, "\t%2zd." " %-10s"
				"\t%12.6le"  "\t%12.6le"
				"\t% 22.15le" "\t% 22.15le" "\t% 22.15le"
				"\t% 22.15le" "\t% 22.15le" "\t% 22.15le" "\n",
				body+1, p->name,
				p->mass, p->radius,
				p->pos[0], p->pos[1], p->pos[2],
				p->vel[0], p->vel[1], p->vel[2]
			);
	}
}
