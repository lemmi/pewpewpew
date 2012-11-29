#ifndef PLANETS_H
#define PLANETS_H

#include <game/Game.h>
#include "objects.h"
#include "colors.h"

typedef struct {
	object_t *o;
} planet_t;

static inline planet_t *planet(vector_t pos, scalar_type r, scalar_type m) {
	planet_t *p = malloc(sizeof(planet_t));
	*p = (planet_t) { .o = object(pos, r, m) };
	return p;
}

static inline void draw_planet(const planet_t *p, Bitmap *b) {
	uint8_t colors[] = { WHITE, LGRAY, DGRAY };

	for (int i = 2; i >= 0; i--) {
		DrawFilledCircle(b, p->o->pos[1], p->o->pos[0], p->o->radius + i, colors[i]);
	}
}

static inline void free_planet(planet_t *p) {
	free(p->o);
	free(p);
}

#endif /* end of include guard: PLANETS_H */
