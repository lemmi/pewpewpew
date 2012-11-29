#ifndef PLAYERS_H
#define PLAYERS_H

#include <math.h>
#include <game/Game.h>

#include "smath.h"
#include "planets.h"
#include "objects.h"

typedef struct {
	planet_t *p;
	object_t *o;
	scalar_type deg;
	uint8_t color;
} player_t;

static inline void update_player_object(player_t *p) {
	object_t *o = p->o;
	object_t *po = p->p->o;

	o->pos[0] = po->pos[0] + cos(p->deg/(M_PI*2.0)) * po->radius;
	o->pos[1] = po->pos[1] + sin(p->deg/(M_PI*2.0)) * po->radius;
}

static inline player_t *player(planet_t *pl, int deg, uint8_t color) {
	player_t *p = malloc(sizeof(player_t));
	p->p = pl;
	p->o = object(pl->o->pos, 3, 0);
	p->deg = deg;
	p->color = color;

	update_player_object(p);

	return p;
}

static inline void draw_player(player_t *p, Bitmap *b) {
	DrawFilledCircle(b, p->o->pos[1], p->o->pos[0], p->o->radius, p->color);
}

static inline void free_player(player_t *p) {
	free(p->o);
	free(p);
}

static inline void move_player(player_t *p, scalar_type deg) {
	p->deg += deg;
	update_player_object(p);
}

#endif /* end of include guard: PLAYERS_H */
