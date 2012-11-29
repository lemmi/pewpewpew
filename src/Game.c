#include <game/Game.h>
#include <game/Debug.h>

#include "objects.h"
#include "planets.h"
#include "bullets.h"
#include "explosion.h"
#include "players.h"
#include "list.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void Init(struct Gamestate*);
void OnEnter(struct Gamestate* state);
void OnLeave(struct Gamestate* state);
void Update(uint32_t);
void Draw(Bitmap *);

Gamestate InitState = { Init, OnEnter, OnLeave, Update, Draw };
Game* TheGame = &(Game) {&InitState};

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

static const scalar_type G = 0.5;
static scalar_type dt = 0.005;

#define NPLANETS 6
static list_t *g_planets;

#define NBULLETS 20
static list_t *g_bullets;

#define NEXPLOSIONS 20
static list_t *g_explosions;

#define NPLAYERS 2
static list_t *g_players;

#define NPLAYERCOLORS 2
static uint8_t player_colors[] = { RED, BLUE };

void Init(struct Gamestate* state) {
	g_planets = list(NPLANETS);
	g_bullets = list(NBULLETS);
	g_explosions = list(NEXPLOSIONS);
	g_players = list(NPLAYERS);

	for (int i = 0; i < NPLANETS; i++) {
		bool retry = true;
		while (retry) {
			retry = false;
			scalar_type radius = randrange(8,15);
			planet_t *new = planet(
					vector(randrange(radius + 1,SCREEN_HEIGHT-radius - 1), randrange(radius + 1,SCREEN_WIDTH-radius-1)),
					radius,
					randrange(1e5, 2e5)
					);
			for (int k = 0; k < i; k++) {
				planet_t *old = list_get(g_planets, k);
				if (collision_dist(old->o, new->o) <= 0) {
					free_planet(new);
					retry = true;
					break;
				}
			}

			list_add(g_planets, new);
		}
	}


	for (int i = 0;  i < NBULLETS; i++) {
		list_add(g_bullets, bullet(vector(50, 50 + i), vector(0, 0)));
	}

	for (int i = 0; i < NPLAYERS; i++) {
		list_add(g_players, player(list_get(g_planets, i), 0, player_colors[i % NPLAYERCOLORS]));
	}

	Delay(300);
}

void OnEnter(struct Gamestate* state) {
}

void OnLeave(struct Gamestate* state) {
}

void Step(list_t *planets, list_t *bullets) {
	bullet_t *b;
	for (int i = 0; i < bullets->size; i++) {
		b = list_get(bullets, i);
		vector_t a = vector(0, 0);

		for (int p = 0; p < planets->size; p++) {
			object_t *o = ((planet_t *) list_get(planets, p))->o;
			scalar_type qd = qdist(o, b->o);
			if (qd <= 25) { continue; }
			scalar_type f = o->mass / (qd * sqrtf(qd));
			a += (o->pos - b->o->pos) * f;
		}
		a = G * a;
		vector_t new_pos = 2 * b->o->pos - b->o->pos_old + dt*dt*a;
		b->o->pos_old = b->o->pos;
		b->o->pos = new_pos;
	}
}

void Update_collisions(list_t *planets, list_t *bullets, list_t *explosions) {
	for (int b = 0; b < bullets->size; b++) {
		bullet_t *bullet = list_get(bullets, b);

		for (int p = 0; p < planets->size; p++) {
			planet_t *planet = list_get(planets, p);

			if (collision_dist(planet->o, bullet->o) < 0) {
				list_mark_for_removal(bullets, b);

				if (explosions->size < explosions->max) {
					list_add(explosions, explosion(bullet->o->pos));
				}

				free_bullet(bullet);
			}
		}
	}

	list_clean_up(bullets);
}

void Update_explosions(list_t *explosions, list_t *players) {
	for (int i = 0; i < explosions->size; i++) {
		explosion_t *explosion = list_get(explosions, i);

		for (int p = 0; p < players->size; p++) {
			player_t *player = list_get(players, p);

			if (explosion_affects(explosion, player->o)) {
				hit_player(player);
			}
		}

		step_explosion(explosion);
		if (explosion_is_dead(explosion)) {
			list_mark_for_removal(explosions, i);
			free_explosion(explosion);
		}
	}

	list_clean_up(explosions);
}

void Update_players(list_t *players, list_t *bullets) {
	if (players->size >= 1) {
		player_t *player = list_get(players, 0);

		if (GetPushbuttonState().Right) { move_player(player, +0.3); }
		if (GetPushbuttonState().Left)  { move_player(player, -0.3); }
		if (GetPushbuttonState().Up)    { move_player_barrel(player, +0.3); }
		if (GetPushbuttonState().Down)  { move_player_barrel(player, -0.3); }
		if (GetPushbuttonState().A)     {
			if (!list_is_full(bullets)) {
				bullet_t * bullet = player_shoot(player);
				list_add(bullets, bullet);
			}
		}
	}
}

void Update(uint32_t a) {
	Step(g_planets, g_bullets);
	Update_explosions(g_explosions, g_players);
	Update_collisions(g_planets, g_bullets, g_explosions);
	Update_players(g_players, g_bullets);
}

void draw_planets(list_t *planets, Bitmap *b) {
	for (int i = 0; i < planets->size; i++) {
		draw_planet((planet_t *)list_get(planets, i), b);
	}
}

void draw_bullets(list_t *bullets, Bitmap *b) {
	for (int i = 0; i < bullets->size; i++) {
		draw_bullet((bullet_t *)list_get(bullets, i), b);
	}
}

void draw_explosions(list_t *explosions, Bitmap *b) {
	for (int i = 0; i < explosions->size; i++) {
		draw_explosion((explosion_t *)list_get(explosions, i), b);
	}
}

void draw_players(list_t *players, Bitmap *b) {
	for (int i = 0; i < players->size; i++) {
		draw_player((player_t *)list_get(players, i), b);
	}
}

void Draw(Bitmap *b) {
	ClearBitmap(b);
	draw_planets(g_planets, b);
	draw_bullets(g_bullets, b);
	draw_explosions(g_explosions, b);
	draw_players(g_players, b);
}
