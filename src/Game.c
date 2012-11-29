#include <game/Game.h>
#include <game/Debug.h>
#include "objects.h"

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

#define MAXOBJECTS 50
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

static const scalar_type G = 0.5;

static int planets = 6;
static object_t *static_object[MAXOBJECTS + 1];
static int projectiles = 0;
static object_t *dynamic_object[MAXOBJECTS + 1];

static scalar_type dt = 0.005;
static int t = 0;
void Init(struct Gamestate* state) {
	for (int i = 0; i < planets; i++) {
		bool retry = true;
		while (retry) {
			SetLEDs(t++ & 0xf);
			retry = false;
			scalar_type radius = randrange(8,15);
			static_object[i] = object(
					vector(randrange(radius + 1,SCREEN_HEIGHT-radius - 1), randrange(radius + 1,SCREEN_WIDTH-radius-1)),
					radius,
					randrange(1e5, 2e5)
					);
			for (int k = 0; k < i; k++) {
				if (collision_dist(static_object[k], static_object[i]) <= 0) {
					free(static_object[i]);
					static_object[i] = NULL;
					retry = true;
					break;
				}
			}
		}
	}

	for (int i = planets; i < MAXOBJECTS + 1; i++) {
		static_object[i] = NULL;
	}

	for (int i = 0;  i < MAXOBJECTS; i++, projectiles++) {
		dynamic_object[i] = object(vector(50, 50 + i), 1, 1);
	}
	Delay(300);
}

void OnEnter(struct Gamestate* state) {
}

void OnLeave(struct Gamestate* state) {
}

void Step(object_t *statics[], const int ns, object_t *dynamics[], const int nd) {
	object_t *c;
	for (int i = 0; i < nd; i++) {
		c = dynamics[i];
		vector_t a = vector(0, 0);
		for (int p = 0; p < ns; p++) {
			object_t *o = statics[p];
			scalar_type qd = qdist(o, c);
			if (qd <= 25) { continue; }
			scalar_type f = o->mass / (qd * sqrtf(qd));
			a += (o->pos - c->pos) * f;
		}
		a = G * a;
		vector_t new_pos = 2 * c->pos - c->pos_old + dt*dt*a;
		c->pos_old = c->pos;
		c->pos = new_pos;
	}
}

void Update(uint32_t a) {
	Step(static_object, planets, dynamic_object, projectiles);
}

void Draw(Bitmap *b) {
	ClearBitmap(b);
	for (int i = 0; i < planets; i++) {
		object_t *o = static_object[i];
		DrawFilledCircle(b, o->pos[1], o->pos[0], o->radius + 2, RGB(64,64,64));
		DrawFilledCircle(b, o->pos[1], o->pos[0], o->radius + 1, RGB(128,128,128));
		DrawFilledCircle(b, o->pos[1], o->pos[0], o->radius, RGB(255,255,255));
	}

	for (int i = 0; i < projectiles; i++) {
		object_t *o = dynamic_object[i];
		DrawFilledCircle(b, o->pos[1], o->pos[0], o->radius, RGB(0,0,255));
	}
}
