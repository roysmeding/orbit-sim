#include <math.h>
#include <stdio.h>
#include "immintrin.h"

#include "universe.h"

static const double G = 1.4886546E-28;	// gravitational constant [AU^3 Gg^-1 day^-2]

#define veccastd(v) ((vec4d) {(v),(v),(v),(v)})
#define veccastf(v) ((vec4f) {(v),(v),(v),(v)})

typedef float vec8f __attribute__ ((vector_size (32)));

#define veccastf8(v)     ((vec8f) { (v),(v),(v),(v),(v),(v),(v),(v) })
#define veccastf8_2(a,b) ((vec8f) { (a),(a),(a),(a),(b),(b),(b),(b) })

// convert two 4double vectors into one 8float vector
static inline vec8f vec_4d_8f(vec4d a, vec4d b) {
	return _mm256_permute2f128_ps(_mm256_castps128_ps256(_mm256_cvtpd_ps(a)),
	                              _mm256_castps128_ps256(_mm256_cvtpd_ps(b)),
				      0x20);
}

static inline void newton_2x4(struct universe *u, size_t row, size_t col) {
	struct planet *a0 = &u->planets[row];
	struct planet *a1 = &u->planets[row+1];
	struct planet *b0 = &u->planets[col];
	struct planet *b1 = &u->planets[col+1];
	struct planet *b2 = &u->planets[col+2];
	struct planet *b3 = &u->planets[col+3];

	// put these eight 4double vectors into four 8float vectors
	vec8f d0 = vec_4d_8f(b0->pos - a0->pos, b0->pos - a1->pos);
	vec8f d1 = vec_4d_8f(b1->pos - a0->pos, b1->pos - a1->pos);
	vec8f d2 = vec_4d_8f(b2->pos - a0->pos, b2->pos - a1->pos);
	vec8f d3 = vec_4d_8f(b3->pos - a0->pos, b3->pos - a1->pos);

	// compute horizontal sums to yield magnitude-squared for each of the 8 vectors
	vec8f distsquared = _mm256_hadd_ps(
			_mm256_hadd_ps(d0*d0, d1*d1),
			_mm256_hadd_ps(d2*d2, d3*d3)
		);

	// calculate the acceleration magnitude
	vec8f dist = _mm256_sqrt_ps(distsquared);
	vec8f ai = veccastf8(G) / (dist * distsquared);

	// shuffle ai such that each 4float in the result consists of four repeated values
	d0 = d0 * veccastf8_2(ai[0], ai[4]);
	d1 = d1 * veccastf8_2(ai[1], ai[5]);
	d2 = d2 * veccastf8_2(ai[2], ai[6]);
	d3 = d3 * veccastf8_2(ai[3], ai[7]);

	// multiply each vector with the correct mass
	vec8f aacc;
	aacc  = d0 * veccastf8(b0->mass);
	aacc += d1 * veccastf8(b1->mass);
	aacc += d2 * veccastf8(b2->mass);
	aacc += d3 * veccastf8(b3->mass);

	vec8f a0m = veccastf8(a0->mass), a1m = veccastf8(a1->mass);
	vec8f b01acc = _mm256_permute2f128_ps(d0, d1, 0x20) * a0m + _mm256_permute2f128_ps(d0, d1, 0x31) * a1m;
	vec8f b23acc = _mm256_permute2f128_ps(d2, d3, 0x20) * a0m + _mm256_permute2f128_ps(d2, d3, 0x31) * a1m;

	// update all the total accelerations
	a0->acc += _mm256_extractf128_ps(aacc, 0);
	a1->acc += _mm256_extractf128_ps(aacc, 1);

	b0->acc -= _mm256_extractf128_ps(b01acc, 0);
	b1->acc -= _mm256_extractf128_ps(b01acc, 1);
	b2->acc -= _mm256_extractf128_ps(b23acc, 0);
	b3->acc -= _mm256_extractf128_ps(b23acc, 1);
}

static inline void newton_1x4(struct universe *u, size_t row, size_t column) {
	struct planet *a  = &u->planets[row];
	struct planet *b0 = &u->planets[column];
	struct planet *b1 = &u->planets[column+1];
	struct planet *b2 = &u->planets[column+2];
	struct planet *b3 = &u->planets[column+3];

	// put these eight 4double vectors into four 8float vectors
	vec8f d0 = vec_4d_8f(b0->pos - a->pos, b1->pos - a->pos);
	vec8f d1 = vec_4d_8f(b2->pos - a->pos, b3->pos - a->pos);

	// compute horizontal sums to yield magnitude-squared for each of the 8 vectors

	vec8f _tmp = _mm256_hadd_ps(d0*d0, d1*d1);
	vec8f distsquared = _mm256_hadd_ps(_tmp,_tmp);

	// calculate the acceleration magnitude
	vec8f dist = _mm256_sqrt_ps(distsquared);
	vec8f ai = veccastf8(G) / (dist * distsquared);

	// shuffle ai such that each 4float in the result consists of four repeated values
	vec8f ai0 = veccastf8_2(ai[0], ai[4]);
	vec8f ai1 = veccastf8_2(ai[1], ai[5]);

	// multiply all the vectors to obtain acceleration values
	d0 = d0 * ai0;
	d1 = d1 * ai1;

	// multiply each vector by either mass
	vec8f aacc;
	aacc  = d0 * veccastf8_2(b0->mass, b1->mass);
	aacc += d1 * veccastf8_2(b2->mass, b3->mass);

	vec8f am = veccastf8(a->mass);
	d0 = d0 * am;
	d1 = d1 * am;

	// update all the total accelerations
	a->acc  += _mm256_extractf128_ps(aacc, 0) + _mm256_extractf128_ps(aacc, 1);
	b0->acc -= _mm256_extractf128_ps(d0, 0);
	b1->acc -= _mm256_extractf128_ps(d0, 1);
	b2->acc -= _mm256_extractf128_ps(d1, 0);
	b3->acc -= _mm256_extractf128_ps(d1, 1);
}



static inline void compute_accelerations(struct	universe *universe) {
	// clear accelerations
	for(size_t planet = 0; planet < universe->num_planets; planet++)
		universe->planets[planet].acc = (vec4f) { 0., 0., 0., 0. };

	// accumulate accelerations
	for(size_t row = 1; row < universe->num_planets; row++) {
		struct planet *p1 = &universe->planets[row];
		size_t column;
		column = 0;
		if(row > 3) {
			for(; column < row-3; column += 4) {
				if(row%2 == 0) {
					newton_2x4(universe, row, column);
				}
			}

			for(; column < row-3; column += 4) {
				newton_1x4(universe, row, column);
			}
		}

		for(; column < row; column++) {
			struct planet *p2 = &universe->planets[column];

			// Newton's law of universal gravitation
			vec4f d  = _mm256_cvtpd_ps(p2->pos - p1->pos);
			vec4f d2 = d * d;
			vec4f dist2 = veccastf(d2[0]+d2[1]+d2[2]);
			vec4f ai = veccastf(G) / (dist2 * _mm_sqrt_ps(dist2));
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
		p->vel += _mm256_cvtps_pd(p->acc) * veccastd(dt);
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
