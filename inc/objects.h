#ifndef OBJECTS_H
#define OBJECTS_H

#include "smath.h"

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

#endif  /*OBJECTS_H*/
