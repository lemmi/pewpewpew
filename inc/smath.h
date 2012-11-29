#ifndef SMAT_H
#define SMAT_H

#include <math.h>

//typedef long double scalar_type;
//typedef double scalar_type;
typedef float scalar_type;

typedef scalar_type vector_t __attribute__ ((vector_size (2*sizeof(scalar_type))));

static inline vector_t vector(const scalar_type s1, const scalar_type s2) {
	return (vector_t) {s1, s2};
}

static inline scalar_type vqlen(const vector_t v) {
	vector_t t = v*v;
	return t[0] + t[1];
}

static inline scalar_type vlen(const vector_t v) {
	return sqrtf(vqlen(v));
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


#endif /* end of include guard: SMAT_H */
