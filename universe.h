#ifndef _ORBIT_H
#define _ORBIT_H

#include <stdint.h>
#include <stdio.h>

typedef double vec4 __attribute__ ((vector_size (32))) __attribute__ ((aligned (32)));

static const size_t PLANET_ALIGN = 32;

/* Basic data management */
struct planet {
	vec4 pos;	// position
	vec4 vel;	// velocity
	vec4 acc;	// acceleration
	char *name;
	double mass;
	double radius;
} __attribute__ ((aligned (32)));

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
