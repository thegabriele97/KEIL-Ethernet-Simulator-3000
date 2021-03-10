
#ifndef BLIND_LAB_H_
#define BLIND_LAB_H_

#include <stdint.h>
#include "stdbool.h"

#define __NROWS		0xd
#define __NCOLS		0xf

#define NROWS			(__NROWS + 2)
#define NCOLS			(__NCOLS + 2)

#define NORTH			0x3
#define SOUTH			0x1
#define EAST			0x2
#define WEST			0x0

#define LED_RUN		0x5
#define LED_WEST	0x0
#define LED_SOUTH 0x1
#define LED_EAST	0x2
#define LED_NORTH	0x3

typedef enum {
	NORMAL   = (uint8_t)0x0,
	OBSTACLE = (uint8_t)0x1,
	AWW_EXIT = (uint8_t)0x2,
} slot_t;

typedef struct {
	uint8_t row;
	uint8_t col;
} pos_t;

typedef struct {
	pos_t curr_pos;
	pos_t obstacle_pos;
	
	struct {
			unsigned direction 			: 2;
			unsigned is_obstacle		: 1;
			unsigned obst_dist			: 3;
	} dir_obstacle;
	
} robot_t;

typedef struct {
	const slot_t (*map)[NCOLS];
	robot_t robot;
} game_t;

extern const slot_t default_map[NROWS][NCOLS];

robot_t new_robot(void);
game_t new_game(robot_t robot);
void set_dir(game_t *game, uint8_t dir);
bool go_on(game_t *game);
bool is_exit_reached(game_t game);
bool pos_eq(pos_t pos1, pos_t pos2);

#endif
