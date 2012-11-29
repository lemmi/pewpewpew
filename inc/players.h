#ifndef PLAYERS_H
#define PLAYERS_H

#include <math.h>
#include <game/Game.h>

#include "smath.h"
#include "planets.h"
#include "objects.h"
#include "bullets.h"

typedef struct {
	planet_t *p;
	object_t *o;
	scalar_type deg;
	scalar_type barrel_deg_offset;
	scalar_type bullet_energy;
	uint8_t color;
	bool is_dead;
} player_t;

static inline void update_player_object(player_t *p) {
	object_t *o = p->o;
	object_t *po = p->p->o;

	o->pos[0] = po->pos[0] + sin(p->deg/(M_PI*2.0)) * po->radius;
	o->pos[1] = po->pos[1] + cos(p->deg/(M_PI*2.0)) * po->radius;
}

static inline player_t *player(planet_t *pl, int deg, uint8_t color) {
	player_t *p = malloc(sizeof(player_t));
	p->p = pl;
	p->o = object(pl->o->pos, 3, 0);
	p->deg = deg;
	p->barrel_deg_offset = 0;
	p->bullet_energy = 1;
	p->color = color;
	p->is_dead = false;

	update_player_object(p);

	return p;
}

static inline void draw_player(player_t *p, Bitmap *b) {
	if (p->is_dead) { return; }

	DrawFilledCircle(b, p->o->pos[1], p->o->pos[0], p->o->radius, p->color);

	int barrel_end_x = p->o->pos[1] + p->o->radius * 3 * cos((p->deg + p->barrel_deg_offset)/(2.0 * M_PI));
	int barrel_end_y = p->o->pos[0] + p->o->radius * 3 * sin((p->deg + p->barrel_deg_offset)/(2.0 * M_PI));
	DrawLine(b, p->o->pos[1], p->o->pos[0], barrel_end_x, barrel_end_y, p->color);
}

static inline void free_player(player_t *p) {
	free(p->o);
	free(p);
}

static inline void move_player(player_t *p, scalar_type deg) {
	if (p->is_dead) { return; }

	p->deg += deg;
	update_player_object(p);
}

static inline void move_player_barrel(player_t *p, scalar_type deg) {
	if (p->is_dead) { return; }

	p->barrel_deg_offset += deg;
	if (p->barrel_deg_offset > 90)  { p->barrel_deg_offset = 90;  }
	if (p->barrel_deg_offset < -90) { p->barrel_deg_offset = -90; }
}

static inline void hit_player(player_t *p) {
	p->is_dead = true;
}

static inline bullet_t *player_shoot(player_t *p) {
	if (p->is_dead) { return NULL; }

	int barrel_end_x = p->o->pos[1] + p->o->radius * 3 * cos((p->deg + p->barrel_deg_offset)/(2.0 * M_PI));
	int barrel_end_y = p->o->pos[0] + p->o->radius * 3 * sin((p->deg + p->barrel_deg_offset)/(2.0 * M_PI));

	scalar_type bullet_vel = sqrts((scalar_type) 2 * p->bullet_energy / BULLET_MASS) * dt;

	return bullet(
			vector(barrel_end_y, barrel_end_x),
			vector(
				bullet_vel * sin((p->deg + p->barrel_deg_offset)/(2.0 * M_PI)),
				bullet_vel * cos((p->deg + p->barrel_deg_offset)/(2.0 * M_PI))
			      ));
}

#endif /* end of include guard: PLAYERS_H */
