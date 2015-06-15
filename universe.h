#ifndef _ORBIT_H
#define _ORBIT_H

#include <stdint.h>
#include <time.h>

struct planet {
	char *name;
	double x, y, z;
	double vx, vy, vz;
	double mass;
	double radius;
} ;

void planet_init(struct planet *planet, char *name, double mass, double radius, double x, double y, double z, double vx, double vy, double vz);

struct universe {
	time_t time;

	size_t num_planets;
	struct planet *planets;
} ;

struct universe *universe_new(size_t num_planets, time_t time);
void universe_free(struct universe *universe);

#endif
