#ifndef _ORBIT_H
#define _ORBIT_H

#include <stdint.h>
#include <stdio.h>

typedef double vec4d __attribute__ ((vector_size (32))) __attribute__ ((aligned (32)));
typedef float vec4f __attribute__ ((vector_size (16))) __attribute__ ((aligned (16)));

static const size_t BODY_ALIGN = 32;

/* Basic data management */
struct body {
	vec4d pos;	// position
	vec4d vel;	// velocity
	vec4f acc;	// acceleration
	char *name;
	double mass;
	double radius;
} __attribute__ ((aligned (32)));

void body_init(struct body *body, char *name, double mass, double radius, double x, double y, double z, double vx, double vy, double vz);

struct universe {
	double time;

	size_t num_bodies;
	struct body *bodies;
} ;

struct universe *universe_new(size_t num_bodies, double time);
void universe_free(struct universe *universe);

struct universe *universe_load(const char* filename);
void universe_dump(FILE *dest, struct universe *universe);


/* Simulation */
void universe_step(struct universe *universe, double dt);

#endif
