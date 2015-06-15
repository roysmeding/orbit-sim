#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "universe.h"

void planet_init(struct planet *planet, char *name, double mass, double radius, double x, double y, double z, double vx, double vy, double vz) {
	planet->name   = name;
	planet->mass   = mass;
	planet->radius = radius;

	planet->x  =  x, planet->y  =  y, planet->z  =  z;
	planet->vx = vx, planet->vy = vy, planet->vz = vz;
}

struct universe *universe_new(size_t num_planets, time_t time) {
	struct universe *universe = malloc(sizeof(struct universe));
	universe->time = time;
	universe->num_planets = num_planets;
	universe->planets = malloc(num_planets * sizeof(struct planet));

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
