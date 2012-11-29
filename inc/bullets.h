#ifndef BULLETS_H
#define BULLETS_H

#include <game/Game.h>
#include "objects.h"
#include "config.h"

typedef struct {
	object_t *o;
} bullet_t;

static inline void draw_bullet(const bullet_t *bu, Bitmap *bi) {
	DrawFilledCircle(bi, bu->o->pos[1], bu->o->pos[0], bu->o->radius, BLUE);
}

static inline bullet_t *bullet(vector_t pos, vector_t v) {
	bullet_t *b = malloc(sizeof(bullet_t));
	*b = (bullet_t) { .o = object(pos, 1, BULLET_MASS) };
	b->o->pos_old -= v;
	return b;
}

static inline void free_bullet(bullet_t *b) {
	free(b->o);
	free(b);
}

#endif /* end of include guard: BULLETS_H */
