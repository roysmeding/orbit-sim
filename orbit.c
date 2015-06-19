#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/timeb.h>

#include "universe.h"

static const char UNIVERSE_FILE[] = "universe.txt";
static const char RESULT_FILE[]   = "results.txt";

static const double DT = 0.1;	// 1 day
static const unsigned int NUM_ITERATIONS = 3650000;
static const unsigned int OUTPUT_INTERVAL = 10;

struct pstate {
	double time;
	size_t index;
	double x, y, z;
};

int main(__attribute__((unused)) int argc, __attribute__((unused))char **argv) {
	// read data file
	fprintf(stderr, "Loading universe file %s...\n", UNIVERSE_FILE);
	struct universe *universe = universe_load(UNIVERSE_FILE);
	fprintf(stderr, "Loaded %zd planets:\n", universe->num_planets);

	fprintf(stderr, "Initial state:\n");
	universe_dump(stderr, universe);

	// data buffer
	fprintf(stderr, "Allocating data buffer...\n");
	size_t num_pstate = universe->num_planets * NUM_ITERATIONS/OUTPUT_INTERVAL;
	struct pstate *history = malloc(num_pstate * sizeof(struct pstate));

	fprintf(stderr, "Starting run...\n");
	
	struct timeb start;
	ftime(&start);

	for(unsigned int i = 0; i < NUM_ITERATIONS; i++) {
		universe_step(universe, DT);

		if(i%OUTPUT_INTERVAL==0) {
			for(size_t planet = 0; planet < universe->num_planets; planet++) {
				struct planet *p = &universe->planets[planet];
				size_t idx = (i/OUTPUT_INTERVAL)*universe->num_planets + planet;
				struct pstate *curstate = &history[idx];

				curstate->time  = universe->time;
				curstate->index = planet;
				curstate->x     = p->x;
				curstate->y     = p->y;
				curstate->z     = p->z;
			}
		}
	}

	{
		struct timeb end;
		ftime(&end);

		double time_taken = (end.time - start.time) + (end.millitm - start.millitm)*1e-3;

		fprintf(stderr, "%d iterations (%.2lf days) in %lf s = %lf it/s\n", NUM_ITERATIONS, NUM_ITERATIONS*DT, time_taken, NUM_ITERATIONS/time_taken);
	}

	// output
	{
		FILE *outfile = fopen(RESULT_FILE, "w");

		for(size_t i=0; i<num_pstate; i++) {
			fprintf(outfile, "%le" "\t%zd" "\t%le\t%le\t%le" "\n",
					history[i].time, history[i].index,
					history[i].x, history[i].y, history[i].z
				);
		}

		fclose(outfile);
	}

	universe_free(universe);
	free(history);

	return 0;
}
