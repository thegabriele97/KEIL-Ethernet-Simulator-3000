/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

#include "lpc17xx.h"
#include "RIT.h"
#include "stdbool.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../blind_labyrinth.h"
#include "../timer/timer.h"
#include "../lab_map.h"
#include "stdint.h"

#define IS_JOYSEL_PRESSED() ((LPC_GPIO1->FIOPIN & (1 << 25)) == 0)
#define IS_JOYUP_PRESSED() ((LPC_GPIO1->FIOPIN & (1 << 29)) == 0)
#define IS_JOYDW_PRESSED() ((LPC_GPIO1->FIOPIN & (1 << 26)) == 0)
#define IS_JOYLT_PRESSED() ((LPC_GPIO1->FIOPIN & (1 << 27)) == 0)
#define IS_JOYRT_PRESSED() ((LPC_GPIO1->FIOPIN & (1 << 28)) == 0)

typedef union {
	struct {
		unsigned is_down_pressed 	: 1;
		unsigned is_left_pressed 	: 1;
		unsigned is_right_pressed : 1;
		unsigned is_up_pressed 	 	: 1;
	} flags;
	uint8_t byte;
} flags_t;

typedef enum {
	ROTATE = Yellow, 
	MOVE 	 = LCD_MAP_BKCOLOR
} mode_t;

static void joystick_EvtHandler(mode_t game_mode, uint8_t new_dir, flags_t *flags, uint8_t field_n, bool is_set);
static void LCD_DrawGrid(void);

game_t game;

void RIT_IRQHandler (void) {					
	static mode_t mode;
	
	static bool is_started = false;
	static bool is_select_pressed = false;
	static flags_t flags;
	static pos_t last_obstacled_drawed;
	
	Coordinate lcd;
	uint8_t i, j;
	
	getDisplayPoint(&lcd, Read_Ads7846(), &matrix);
	if (WITHIN_INTERVAL(lcd.x, LCD_START_START_X, LCD_START_END_X) && WITHIN_INTERVAL(lcd.y, LCD_START_START_Y, LCD_START_END_Y)) {
		is_started = false;

		LCD_DrawRect(LCD_MAP_START_X, LCD_MAP_START_Y, 13*15 + 1, 15*15 + 1, LCD_MAP_BKCOLOR);
		GUI_Text(60, 110, (uint8_t *)"Touch to Start", Red, LCD_MAP_BKCOLOR);
		GUI_Text(76, 125, (uint8_t *)"a New Game", Red, LCD_MAP_BKCOLOR);
		
	} else if (!is_started && WITHIN_INTERVAL(lcd.x, LCD_MAP_START_X, LCD_MAP_START_X + 15*15) && WITHIN_INTERVAL(lcd.y, LCD_MAP_START_Y, LCD_MAP_START_Y + 13*15)) {
		
		LCD_DrawRect(60, 110, 50, 120, LCD_MAP_BKCOLOR); // Clears words on the map area
		
		game = new_game(new_robot());
		mode = MOVE;
		is_started = true;
		flags.byte = 0x00;
		
		//LCD_DrawGrid();
		//LCD_DrawRect(LCD_POSCOL(7), LCD_POSROW(7), 16, 16, Red);
		//GUI_Text(LCD_GAMECOL(game), LCD_GAMEROW(game), (uint8_t *)"E", Black, White);
	} else if (is_started) {
		
		if (!is_select_pressed && IS_JOYSEL_PRESSED()) {
				mode = (mode == ROTATE) ? MOVE : ROTATE;
		}
		
		is_select_pressed = IS_JOYSEL_PRESSED();
		
		if (!is_select_pressed) {
			if (!flags.flags.is_up_pressed && IS_JOYUP_PRESSED()) {
				joystick_EvtHandler(mode, NORTH, &flags, 0x3, true);
			} else if (!flags.flags.is_down_pressed && IS_JOYDW_PRESSED()) {
				joystick_EvtHandler(mode, SOUTH, &flags, 0x0, true);
			} else if (!flags.flags.is_right_pressed && IS_JOYRT_PRESSED()) {
				joystick_EvtHandler(mode, EAST, &flags, 0x2, true);
			} else if (!flags.flags.is_left_pressed && IS_JOYLT_PRESSED()) {
				joystick_EvtHandler(mode, WEST, &flags, 0x1, true);
			} else if ((flags.byte & (0xf << 0)) && (LPC_GPIO1->FIOPIN & (flags.byte << 26))) { // Checking if there is some flag at 1 and one of the button released (at 1)
				joystick_EvtHandler(mode, 0, &flags, 0, false);   // A smart thing: flags.byte contains only a bit at 1 (the current pressed).
			}                                                   // This byte can be used as mask to obtain the status of the relative joystick btn pressed!!
                                                          // In fact, the bits in the flags.byte are ordered with the same order of btns!!!
		}

		LCD_DrawSpaceship(LCD_GAMECOL(game), LCD_GAMEROW(game), game.robot.dir_obstacle.direction, mode);
		
		if (is_exit_reached(game)) {
			disable_timer(0);
			is_started = false;
			
			GUI_Text(20, 80,  (uint8_t *)"!!!!!!!!!!!!!!!!!!!!!!!!!", Red, LCD_MAP_BKCOLOR);
			GUI_Text(20, 155, (uint8_t *)"!!!!!!!!!!!!!!!!!!!!!!!!!", Red, LCD_MAP_BKCOLOR);
			GUI_Text(100, 110, (uint8_t *)"YOU", Red, LCD_MAP_BKCOLOR);
			GUI_Text(95, 125, (uint8_t *)"WON!!", Red, LCD_MAP_BKCOLOR);
		}
		
		if (game.robot.dir_obstacle.is_obstacle && WITHIN_INTERVAL(game.robot.obstacle_pos.col, 1, NCOLS-2) && WITHIN_INTERVAL(game.robot.obstacle_pos.row, 1, NROWS-2)) {
			if (!pos_eq(last_obstacled_drawed, game.robot.obstacle_pos)) {
				LCD_DrawSpacemonster(LCD_POSCOL(game.robot.obstacle_pos.col - 1), LCD_POSROW(game.robot.obstacle_pos.row - 1), (LPC_TIM0->TC >> 0) & 0x1);
				last_obstacled_drawed = game.robot.obstacle_pos;
			}
		}
		
		if (WITHIN_INTERVAL(lcd.x, LCD_CLR_START_X, LCD_CLR_END_X) && WITHIN_INTERVAL(lcd.y, LCD_CLR_START_Y, LCD_CLR_END_Y)) {
			for (i = 1; i < NROWS - 1; i++) {
				for (j = 1; j < NCOLS - 1; j++) {
					if (game.map[i][j] == OBSTACLE && !(j == game.robot.obstacle_pos.col && i == game.robot.obstacle_pos.row)) {
						LCD_DrawRect(LCD_POSCOL(j-1), LCD_POSROW(i-1), 16, 16, LCD_MAP_BKCOLOR);
					}
				}
			}				
		}
		
  }
	
	reset_RIT();
	LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
  return;
}

static void joystick_EvtHandler(mode_t game_mode, uint8_t new_dir, flags_t *flags, uint8_t field_n, bool is_set) {
	
	flags->byte = is_set ? (flags->byte | (1 << field_n)) : 0x00;
	
	if (is_set) {
		set_dir(&game, new_dir);
		
		if (game_mode == MOVE && !(LPC_TIM0->TCR & 0x1)) { // Timer enabled once
			enable_timer(0);
		}
	} else if (!is_set) {
		disable_timer(0);
		//reset_timer(0);
	}
	
}

static void LCD_DrawGrid() {
	uint16_t col;
	
	for (col = 0; col <= 15; col++) {
			LCD_DrawLine(LCD_MAP_START_X + col*15, LCD_MAP_START_Y, LCD_MAP_START_X + col*15, LCD_MAP_START_Y + 13*15, Green);
	}

	for (col = 0; col <= 13; col++) {
			LCD_DrawLine(LCD_MAP_START_X, LCD_MAP_START_Y + col*15, LCD_MAP_START_X + 15*15, LCD_MAP_START_Y + col*15, Green);
	}
	
}
