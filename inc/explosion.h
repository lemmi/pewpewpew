#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <game/Game.h>
#include "objects.h"

typedef struct {
	object_t *o;
} explosion_t;

static inline explosion_t *explosion(vector_t pos) {
	explosion_t *e = malloc(sizeof(explosion_t));
	e->o = object(pos, 10, 0);

	return e;
}

static inline void free_explosion(explosion_t *e) {
	free(e->o);
	free(e);
}

static inline void step_explosion(explosion_t *e) {
	e->o->radius -= (e->o->radius > 0);
}

static inline void draw_explosion(explosion_t *e, Bitmap *b) {
	DrawFilledCircle(b, e->o->pos[1], e->o->pos[0], e->o->radius, YELLOW);
}

static inline bool explosion_is_dead(explosion_t *e) {
	return e->o->radius == 0;
}

#endif /* end of include guard: EXPLOSION_H */
