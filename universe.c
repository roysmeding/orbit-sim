#include <stdint.h>
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
