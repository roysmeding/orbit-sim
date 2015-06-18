#include <math.h>

#include "universe.h"

static const double G = 1.4886546E-28;	// gravitational constant [AU^3 Gg^-1 day^-2]

static inline void compute_accelerations(struct universe *universe) {
	// clear accelerations
	for(size_t planet = 0; planet < universe->num_planets; planet++)
		universe->planets[planet].ax = universe->planets[planet].ay = universe->planets[planet].az = 0.;

	// accumulate accelerations
	for(size_t planet = 1; planet < universe->num_planets; planet++) {
		struct planet *p1 = &universe->planets[planet];

		for(size_t other_planet = 0; other_planet < planet; other_planet++) {
			struct planet *p2 = &universe->planets[other_planet];

			// Newton's law of universal gravitation
			double dx = p2->x - p1->x, dy = p2->y - p1->y, dz = p2->z - p1->z;
			double dist2 = dx*dx + dy*dy + dz*dz;
			double ai = G * p2->mass / (dist2*sqrt(dist2));

			p1->ax += ai*dx;
			p1->ay += ai*dy;
			p1->az += ai*dz;

			p2->ax -= ai*dx;
			p2->ay -= ai*dy;
			p2->az -= ai*dz;
		}
	}
}

void universe_step(struct universe *universe, double dt) {
	compute_accelerations(universe);

	// integrate velocities
	for(size_t planet = 1; planet < universe->num_planets; planet++) {
		struct planet *p = &universe->planets[planet];
		p->vx += p->ax * dt;
		p->vy += p->ay * dt;
		p->vz += p->az * dt;
	}

	compute_accelerations(universe);

	// integrate positions
	for(size_t planet = 1; planet < universe->num_planets; planet++) {
		struct planet *p = &universe->planets[planet];
		p->x += p->vx * dt;
		p->y += p->vy * dt;
		p->z += p->vz * dt;
	}

	// update time
	universe->time += dt;
}
