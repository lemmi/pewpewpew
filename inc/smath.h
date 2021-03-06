#ifndef SMAT_H
#define SMAT_H

#include <math.h>
#include <stdint.h>
#include <game/Game.h>

//typedef long double scalar_type;
//#define sqrts sqrtl
//typedef double scalar_type;
//#define sqrts sqrt
typedef float scalar_type;
#define sqrts sqrtf

typedef scalar_type vector_t __attribute__ ((vector_size (2*sizeof(scalar_type))));

static inline vector_t vector(const scalar_type s1, const scalar_type s2) {
	return (vector_t) {s1, s2};
}

static inline scalar_type vqlen(const vector_t v) {
	vector_t t = v*v;
	return t[0] + t[1];
}

static inline scalar_type vlen(const vector_t v) {
	return sqrts(vqlen(v));
}

static inline scalar_type rands() {
	return (double) GetRandomInteger() / (double) UINT32_MAX;
}

static inline scalar_type randrange(scalar_type min, scalar_type max) {
	return min + (max - min) * rands();
}

static inline vector_t vrand() {
	return vector(rands(), rands());
}

static inline scalar_type deg_to_rad(scalar_type deg) {
	return deg * M_PI / (scalar_type) 180;
}

static inline scalar_type rad_to_deg(scalar_type rad) {
	return rad * (scalar_type) 180 / M_PI;
}

static inline vector_t vangle(scalar_type deg) {
	scalar_type rad = deg_to_rad(deg);
	return vector(sin(rad), cos(rad));
}

#endif /* end of include guard: SMAT_H */
