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
	Pixel color;
	bool is_dead;
	int next_shoot_timer;
} player_t;

static inline void update_player_object(player_t *p) {
	p->o->pos = p->p->o->pos + vangle(p->deg) * p->p->o->radius;
}

static inline player_t *player(planet_t *pl, int deg, Pixel color) {
	player_t *p = malloc(sizeof(player_t));
	p->p = pl;
	p->o = object(pl->o->pos, 3, 0);
	p->deg = deg;
	p->barrel_deg_offset = 0;
	p->bullet_energy = 4000;
	p->color = color;
	p->is_dead = false;
	p->next_shoot_timer = 0;

	update_player_object(p);

	return p;
}

static inline vector_t calc_barrel_end(player_t *p) {
	return p->o->pos + p->o->radius * 3 * vangle(p->deg + p->barrel_deg_offset);
}

static inline void draw_player(player_t *p, Bitmap *b) {
	if (p->is_dead) { return; }

	DrawFilledCircle(b, p->o->pos[1], p->o->pos[0], p->o->radius, p->color);

	vector_t barrel_end = calc_barrel_end(p);
	DrawLine(b, p->o->pos[1], p->o->pos[0], barrel_end[1], barrel_end[0], p->color);
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

static inline void set_bullet_energy(player_t *p, scalar_type delta) {
	p->bullet_energy += delta;
	p->bullet_energy = (p->bullet_energy < 0) ? 0 : p->bullet_energy;
}

static inline void hit_player(player_t *p) {
	p->is_dead = true;
}

static inline bullet_t *player_shoot(player_t *p) {
	if (p->is_dead) { return NULL; }
	if (p->next_shoot_timer > 0) { return NULL; }
	p->next_shoot_timer=100;

	vector_t barrel_end = calc_barrel_end(p);

	scalar_type bullet_vel = sqrts((scalar_type) 2 * p->bullet_energy / BULLET_MASS) * dt;

	return bullet(barrel_end, bullet_vel * vangle(p->deg + p->barrel_deg_offset), p->color);
}

static inline bullet_t *player_shoot_blanket(player_t *p) {
	int old_timer = p->next_shoot_timer;
	p->next_shoot_timer = 0;
	bullet_t *bullet = player_shoot(p);
	p->next_shoot_timer = old_timer;
	return bullet;
}

static inline void step_player(player_t *p) {
	if (p->next_shoot_timer > 0) {
		p->next_shoot_timer--;
	}
}

#endif /* end of include guard: PLAYERS_H */
