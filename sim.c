#include <math.h>
#include "universe.h"

static const double G = 1.4886546E-28;	// gravitational constant [AU^3 Gg^-1 day^-2]

#define veccastd(v) ((vec4d) {(v),(v),(v),(v)})
#define veccastf(v) ((vec4f) {(v),(v),(v),(v)})

static inline void compute_accelerations(struct	universe *universe) {
	// clear accelerations
	for(size_t planet = 0; planet < universe->num_planets; planet++)
		universe->planets[planet].acc = (vec4f) { 0., 0., 0., 0. };

	// accumulate accelerations
	for(size_t planet = 1; planet < universe->num_planets; planet++) {
		struct planet *p1 = &universe->planets[planet];

		for(size_t other_planet = 0; other_planet < planet; other_planet++) {
			struct planet *p2 = &universe->planets[other_planet];

			// Newton's law of universal gravitation
			vec4f d  = __builtin_ia32_cvtpd2ps256(p2->pos - p1->pos);
			vec4f d2 = d * d;
			vec4f dist2 = veccastf(d2[0]+d2[1]+d2[2]);
			vec4f ai = veccastf(G) / (dist2 * __builtin_ia32_sqrtps(dist2));
			vec4f da = ai * d;

			p1->acc += da * veccastf(p2->mass);
			p2->acc -= da * veccastf(p1->mass);
		}
	}
}

void universe_step(struct universe *universe, double dt) {
	compute_accelerations(universe);

	for(size_t planet = 0; planet < universe->num_planets; planet++) {
		struct planet *p = &universe->planets[planet];
		p->vel += __builtin_ia32_cvtps2pd256(p->acc) * veccastd(dt);
		p->pos += p->vel * veccastd(dt);
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
