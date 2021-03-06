#include <game/Game.h>
#include <game/Debug.h>

#include "config.h"
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

extern Gamestate WinScreen;
Gamestate InitState = { Init, OnEnter, OnLeave, Update, Draw };
Game* TheGame = &(Game) {&InitState};

static list_t *g_planets;
static list_t *g_bullets;
static list_t *g_explosions;
static list_t *g_players;

static bool is_initalized;
static int32_t next_update_timer;

void self_init() {
	is_initalized = true;
	next_update_timer = 0;

	g_planets = list(NPLANETS);
	g_bullets = list(NBULLETS);
	g_explosions = list(NEXPLOSIONS);
	g_players = list(NPLAYERS);

	for (int i = 0; i < NPLANETS; i++) {
		scalar_type radius;
RETRY:
		radius = randrange(8,15);
		planet_t *new = planet(
				vector(randrange(radius + 1,SCREEN_HEIGHT-radius - 1), randrange(radius + 1,SCREEN_WIDTH-radius-1)),
				radius,
				randrange(1e5, 2e5)
				);
		for (int k = 0; k < i; k++) {
			planet_t *old = list_get(g_planets, k);
			if (collision_dist(old->o, new->o) <= 0) {
				free_planet(new);
				goto RETRY;
			}
		}

		list_add(g_planets, new);
	}

	for (int i = 0; i < NPLAYERS; i++) {
		list_add(g_players, player(list_get(g_planets, i), 0, player_colors[i % NPLAYERCOLORS]));
	}

	Delay(30);
}

void Init(struct Gamestate* state) {
	self_init();
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

				if (!list_is_full(explosions)) {
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
	if (players->size != 2) { return; }

	snes_button_state_t state[2] = { GetControllerState1(), GetControllerState2() };

	for (int i = 0; i < 2; i++) {
		player_t *player = list_get(players, i);
		step_player(player);
		snes_button_state_t s = state[i];

		if (s.buttons.Up)    { set_bullet_energy(player, +10.0); }
		if (s.buttons.Down)  { set_bullet_energy(player, -10.0); }
		if (s.buttons.Right) { move_player(player, +0.3); }
		if (s.buttons.Left)  { move_player(player, -0.3); }
		if (s.buttons.R)     { move_player_barrel(player, +0.3); }
		if (s.buttons.L)     { move_player_barrel(player, -0.3); }
		if (s.buttons.A)     {
			if (!list_is_full(bullets)) {
				bullet_t * bullet = player_shoot(player);
				if (bullet != NULL) {
					list_add(bullets, bullet);
				}
			}
		}

	}
}

bool at_most_one_player_alive(list_t *players) {
	int players_alive = 0;

	for (int i = 0; i < players->size; i++) {
		player_t *player = list_get(players, i);

		if (!player->is_dead) {
			players_alive++;
		}
	}

	return players_alive < 2;
}

void destroy_everything(list_t *planets, list_t *bullets, list_t *explosions, list_t *players) {
	for (int i = 0; i < planets->size; i++) {
		planet_t *planet = list_get(planets, i);
		free_planet(planet);
	}
	free_list(planets);

	for (int i = 0; i < bullets->size; i++) {
		bullet_t *bullet = list_get(bullets, i);
		free_bullet(bullet);
	}
	free_list(bullets);

	for (int i = 0; i < explosions->size; i++) {
		explosion_t *explosion = list_get(explosions, i);
		free_explosion(explosion);
	}
	free_list(explosions);

	for (int i = 0; i < players->size; i++) {
		player_t *player = list_get(players, i);
		free_player(player);
	}
	free_list(players);
}

void Do_update() {
	if (!is_initalized) { return; }

	Step(g_planets, g_bullets);
	Update_explosions(g_explosions, g_players);
	Update_collisions(g_planets, g_bullets, g_explosions);
	Update_players(g_players, g_bullets);

	if (at_most_one_player_alive(g_players)) {
		ChangeState(&WinScreen);
		destroy_everything(g_planets, g_bullets, g_explosions, g_players);
		is_initalized = false;
	}
}
void Update(uint32_t a) {
	if (!is_initalized) {
		self_init();
	}

	next_update_timer += a;
	if (a > 100) { SetLEDs(0x02); }

	for (; next_update_timer > 0; next_update_timer -= 1) {
		Do_update();
	}
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

void draw_scopes(list_t *players, Bitmap *b) {
	list_t *bulletlist = list(1);
	for (int i = 0; i < players->size; i++) {
		player_t *player = list_get(players, i);
		bullet_t *bullet = player_shoot_blanket(player);
		bullet->color &= 0x6D;
		list_add(bulletlist, bullet);
		for (int l = 0; l < SCOPEDOTS; l++) {
			for (int s = 0; s < SCOPESTEPS; s++) {
				Step(g_planets, bulletlist);
			}
			draw_bullets(bulletlist, b);
		}
		list_remove(bulletlist, 0);
		free_bullet(bullet);

	}
	free_list(bulletlist);
}

void Draw(Bitmap *b) {
	if (!is_initalized) { return; }
	ClearBitmap(b);
	draw_explosions(g_explosions, b);
	draw_players(g_players, b);
	draw_planets(g_planets, b);
	draw_scopes(g_players, b);
	draw_bullets(g_bullets, b);
}
