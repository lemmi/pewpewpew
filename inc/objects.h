#ifndef OBJECTS_H
#define OBJECTS_H

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

typedef struct {
	vector_t pos;
	vector_t pos_old;      // for static objects this is always equal to pos
	scalar_type radius;
	scalar_type mass;
} object_t;

object_t *object(vector_t pos, scalar_type radius, scalar_type mass) {
	object_t *o = malloc(sizeof(object_t));
	*o = (object_t) {.pos = pos, .pos_old = pos, .radius = radius, .mass = mass};
	return o;
}

static inline scalar_type qdist(const object_t *o1, const object_t *o2) {
	return vqlen(o1->pos - o2->pos);
}

static inline scalar_type dist(const object_t *o1, const object_t *o2) {
	return vlen(o1->pos - o2->pos);
}

static inline scalar_type collision_dist(const object_t *o1, const object_t *o2) {
	return vlen(o1->pos - o2->pos) - o1->radius - o2->radius;
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

#endif  /*OBJECTS_H*/
