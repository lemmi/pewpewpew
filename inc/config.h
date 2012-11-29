#ifndef CONFIG_H
#define CONFIG_H

#include "smath.h"

/* screen */
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

/* physics */
extern scalar_type dt;
extern const scalar_type G;
extern const scalar_type BULLET_MASS;

/* world */
#define NPLANETS 6
#define NPLAYERS 2
#define NBULLETS 20
#define NEXPLOSIONS 20
#define SCOPEDOTS 20
#define SCOPESTEPS 10

/* graphics */
#define NPLAYERCOLORS 2
extern uint8_t player_colors[];

#endif  /*CONFIG_H*/
