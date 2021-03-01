#ifndef MAP_H_
#define MAP_H_

#define LCD_MAP_START_X		06
#define LCD_MAP_START_Y		45

#define LCD_CLR_START_X		150
#define LCD_CLR_START_Y		270
#define LCD_CLR_LENGTH		38
#define LCD_CLR_WIDTH			75
#define LCD_CLR_END_X			(LCD_CLR_START_X + LCD_CLR_WIDTH)
#define LCD_CLR_END_Y			(LCD_CLR_START_Y + LCD_CLR_LENGTH)

#define LCD_START_START_X		10
#define LCD_START_START_Y		270
#define LCD_START_LENGTH		38
#define LCD_START_WIDTH			75
#define LCD_START_END_X			(LCD_START_START_X + LCD_START_WIDTH)
#define LCD_START_END_Y			(LCD_START_START_Y + LCD_START_LENGTH)

#define WITHIN_INTERVAL(val, a, b) (((val) >= (a)) && ((val) <= (b)))

#define LCD_POSCOL(col) (LCD_MAP_START_X + ((col)*15))
#define LCD_POSROW(row) (LCD_MAP_START_Y + ((row)*15))

#define LCD_GAMEROW(cgame) (LCD_POSROW(cgame.robot.curr_pos.row - 1))
#define LCD_GAMECOL(cgame) (LCD_POSCOL(cgame.robot.curr_pos.col - 1))

#define LCD_MAP_BKCOLOR		Black

#endif
