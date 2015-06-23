#include <math.h>

#include "universe.h"

static const double G = 1.4886546E-28;	// gravitational constant [AU^3 Gg^-1 day^-2]

#define veccast(v) ((vec4) {v,v,v,v})

static inline void compute_accelerations(struct	universe *universe) {
	// clear accelerations
	for(size_t planet = 0; planet < universe->num_planets; planet++)
		universe->planets[planet].acc = (vec4) { 0., 0., 0., 0. };

	// accumulate accelerations
	for(size_t planet = 1; planet < universe->num_planets; planet++) {
		struct planet *p1 = &universe->planets[planet];

		for(size_t other_planet = 0; other_planet < planet; other_planet++) {
			struct planet *p2 = &universe->planets[other_planet];

			// Newton's law of universal gravitation
			vec4 d  = p2->pos - p1->pos;
			vec4 d2 = d*d;
			double dist2 = d2[0]+d2[1]+d2[2];
			double ai = G / (dist2*sqrt(dist2));
			vec4 da = veccast(ai) * d;

			p1->acc += da * veccast(p2->mass);
			p2->acc -= da * veccast(p1->mass);
		}
	}
}

void universe_step(struct universe *universe, double dt) {
	compute_accelerations(universe);

	for(size_t planet = 0; planet < universe->num_planets; planet++) {
		struct planet *p = &universe->planets[planet];
		p->vel += p->acc * veccast(dt);
		p->pos += p->vel * veccast(dt);
	}
/*
	// normalize everything so the first planet is in the middle again
	{
		vec4 firstpos = universe->planets[0].pos;
		vec4 firstvel = universe->planets[0].vel;
		for(size_t planet = 0; planet < universe->num_planets; planet++) {
			struct planet *p = &universe->planets[planet];
			p->pos -= firstpos;
			p->vel -= firstvel;
		}
	}
*/
	// update time
	universe->time += dt;
}
