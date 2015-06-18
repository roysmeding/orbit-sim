#ifndef _ORBIT_H
#define _ORBIT_H

#include <stdint.h>
#include <stdio.h>

/* Basic data management */
struct planet {
	char *name;
	double  x,  y,  z;	// position
	double vx, vy, vz;	// velocity
	double ax, ay, az;	// acceleration
	double mass;
	double radius;
} ;

void planet_init(struct planet *planet, char *name, double mass, double radius, double x, double y, double z, double vx, double vy, double vz);

struct universe {
	double time;

	size_t num_planets;
	struct planet *planets;
} ;

struct universe *universe_new(size_t num_planets, double time);
void universe_free(struct universe *universe);

struct universe *universe_load(const char* filename);
void universe_dump(FILE *dest, struct universe *universe);


/* Simulation */
void universe_step(struct universe *universe, double dt);

#endif
