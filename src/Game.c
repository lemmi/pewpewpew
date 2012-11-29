#include <game/Game.h>
#include <game/Debug.h>

#include "objects.h"
#include "planets.h"
#include "bullets.h"

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

#define NPLANETS 6
#define NBULLETS 20
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

static const scalar_type G = 0.5;

static planet_t *g_planets[NPLANETS];
static bullet_t *g_bullets[NBULLETS];

static scalar_type dt = 0.005;
static int t = 0;
void Init(struct Gamestate* state) {
	for (int i = 0; i < NPLANETS; i++) {
		bool retry = true;
		while (retry) {
			SetLEDs(t++ & 0xf);
			retry = false;
			scalar_type radius = randrange(8,15);
			g_planets[i] = planet(
					vector(randrange(radius + 1,SCREEN_HEIGHT-radius - 1), randrange(radius + 1,SCREEN_WIDTH-radius-1)),
					radius,
					randrange(1e5, 2e5)
					);
			for (int k = 0; k < i; k++) {
				if (collision_dist(g_planets[k]->o, g_planets[i]->o) <= 0) {
					free_planet(g_planets[i]);
					g_planets[i] = NULL;
					retry = true;
					break;
				}
			}
		}
	}

	for (int i = 0;  i < NBULLETS; i++) {
		g_bullets[i] = bullet(vector(50, 50 + i));
	}
	Delay(300);
}

void OnEnter(struct Gamestate* state) {
}

void OnLeave(struct Gamestate* state) {
}

void Step(planet_t *planets[], const int np, bullet_t *bullets[], const int nb) {
	bullet_t *b;
	for (int i = 0; i < nb; i++) {
		b = bullets[i];
		vector_t a = vector(0, 0);
		for (int p = 0; p < np; p++) {
			object_t *o = planets[p]->o;
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

void Update(uint32_t a) {
	Step(g_planets, NPLANETS, g_bullets, NBULLETS);
}

void Draw(Bitmap *b) {
	ClearBitmap(b);
	for (int i = 0; i < NPLANETS; i++) {
		draw_planet(g_planets[i], b);
	}

	for (int i = 0; i < NBULLETS; i++) {
		draw_bullet(g_bullets[i], b);
	}
}
