#include <stdint.h>
#include <stdio.h>

#include <sys/timeb.h>

#include "universe.h"

const char UNIVERSE_FILE[] = "universe.txt";

const double DT = 0.1;	// 1 day
const unsigned int N_ITERATIONS = 3650000;
const unsigned int OUTPUT_INTERVAL = 1;
const unsigned int REPORT_INTERVAL = 1000;

int main(__attribute__((unused)) int argc, __attribute__((unused))char **argv) {
	// read data file
	fprintf(stderr, "Loading universe file %s...\n", UNIVERSE_FILE);
	struct universe *universe = universe_load(UNIVERSE_FILE);
	fprintf(stderr, "Loaded %zd planets:\n", universe->num_planets);

	fprintf(stderr, "Initial state:\n");
	universe_dump(stderr, universe);

	struct timeb start;
	ftime(&start);

	for(unsigned int i = 0; i < N_ITERATIONS; i++) {
		universe_step(universe, DT);

		if(i%OUTPUT_INTERVAL==0) {
			if(i%REPORT_INTERVAL == 0) fprintf(stderr, "Iteration %d of %d\n", i, N_ITERATIONS);

			for(size_t planet = 0; planet < universe->num_planets; planet++) {
				struct planet *p = &universe->planets[planet];
				printf("%22.15le"
						"\t%2zd" "\t%-10s"
						"\t% 37.30le" "\t% 37.30le" "\t% 37.30le"
						"\t% 37.30le" "\t% 37.30le" "\t% 37.30le" "\n",
						universe->time,
						planet+1, p->name,
						p->x,  p->y,  p->z,
						p->vx, p->vy, p->vz
				      );
			}
		}
	}

	struct timeb end;
	ftime(&end);

	double dt = (end.time - start.time) + (end.millitm - start.millitm)*1e-3;

	fprintf(stderr, "%d iterations in %lf s = %lf it/s\n", N_ITERATIONS, dt, N_ITERATIONS/dt);

	universe_free(universe);

	return 0;
}
