#include "GLCD/GLCD.h"
#include "stdint.h"
#include "blind_labyrinth.h"
#include "lab_map.h"

#define GET_BKCOLOR(pixel, bk, row) 	((pixel == White && ((row) < 7 || (row) > 13)) ? bk : pixel)

#define gray_low 	 		RGB565CONVERT(169, 170, 170)
#define gray_verylow 	RGB565CONVERT(240, 240, 240)
#define gray_high  		RGB565CONVERT(118, 115, 115)
#define black_high 		RGB565CONVERT(10, 10, 10)
#define black_low  		RGB565CONVERT(41, 42, 42)
#define bordeaux_low	RGB565CONVERT(88, 57, 61)
#define bordeaux_high	RGB565CONVERT(60, 22, 27)
#define red_low				RGB565CONVERT(141, 9, 9)
#define red_high			RGB565CONVERT(119, 0, 0)
#define red_clear			RGB565CONVERT(255, 0, 9)
#define cyan_low			RGB565CONVERT(199, 240, 240)
#define cyan_middle		RGB565CONVERT(116, 183, 228)
#define cyan_high			RGB565CONVERT(87, 126, 126)
#define pink_high			RGB565CONVERT(142, 97, 102)
#define pink_low			RGB565CONVERT(250, 167, 178)
#define blue_ok				RGB565CONVERT(63, 97, 123)
#define green_truelow	RGB565CONVERT(170, 213, 170)
#define green_clear		RGB565CONVERT(18, 247, 18)
#define green_high		RGB565CONVERT(0, 156, 0)
#define green_veryhig RGB565CONVERT(0, 76, 0)

static const uint16_t spaceship_pixels[16][16] = {
	{ White, White, White, White, White, White, White, White, White, White, White, White, White, White, White, White },
	{ White, White, White, White, White, White, gray_low, gray_high, gray_high, gray_low, White, White, White, White, White, White},
	{ White, White, White, White, White, White, gray_low, gray_high, gray_high, gray_low, White, White, White, White, White, White},
	{ White, White, White, White, White, gray_low, black_high, gray_low, gray_low, black_low, gray_verylow, White, White, White, White, White},
	{ White, White, White, White, White, gray_high, black_high, White, White, gray_high, gray_low, White, White, White, White, White},
	{ White, White, gray_verylow, bordeaux_low, bordeaux_high, gray_low, gray_high, gray_verylow, gray_verylow, gray_high, gray_low, bordeaux_high, bordeaux_low, gray_verylow, White, White},
	{ White, White, cyan_low, red_low, red_high, cyan_high, gray_low, White, White, gray_low, cyan_high, red_high, red_low, cyan_low, White, White},
	{ gray_verylow, bordeaux_low, black_high, pink_high, pink_high, black_high, gray_high, White, White, gray_high, black_high, pink_high, pink_high, black_high, bordeaux_low, White},
	{ gray_low, red_high, black_high, cyan_high, gray_low, black_high, gray_high, White, White, gray_high, black_high, gray_low, cyan_high, black_high, red_high, gray_low},
	{ gray_low, red_low, bordeaux_high, cyan_low, gray_low, gray_low, White, pink_high, pink_high, White, gray_low, gray_low, cyan_low, bordeaux_high, red_low, gray_low},
	{ gray_low, gray_verylow, black_low, bordeaux_low, cyan_middle, cyan_low, pink_low, red_clear, red_clear, pink_low, cyan_low, cyan_high, bordeaux_low, bordeaux_low, gray_verylow, gray_low},
	{ gray_low, gray_verylow, gray_high, blue_ok, cyan_low, White, pink_high, red_low, red_low, pink_low, White, cyan_low, blue_ok, gray_high, gray_verylow, gray_low},
	{ gray_low, gray_high, gray_verylow, White, White, pink_high, pink_low, White, White, pink_low, pink_high, White, White, gray_verylow, gray_high, gray_low},
	{ gray_verylow, gray_high, gray_low, cyan_high, bordeaux_high, red_clear, gray_high, cyan_low, cyan_low, red_clear, bordeaux_high, cyan_high, gray_low, gray_high, gray_verylow},
	{ White, White, White, cyan_low, bordeaux_low, bordeaux_high, black_high, gray_high, gray_high, black_high, bordeaux_high, bordeaux_low, cyan_low, White, White, White},
	{ White, White, White, White, cyan_low, cyan_middle, gray_low, gray_verylow, gray_verylow, gray_low, cyan_middle, cyan_low, White, White, White, White}	
};

static const uint16_t monster_pixels[16][8] = {
	{ White, White, White, White, White, White, White, White},
	{ White, White, White, green_truelow, White, White, White, White},
	{ White, White, green_truelow, green_clear, green_truelow, White, White, White},
	{ White, White, green_truelow, green_clear, green_clear, green_truelow, White, White},
	{ White, White, White, White, green_clear, green_clear, green_truelow, White},
	{ White, White, White, green_truelow, green_clear, green_clear, green_clear, green_truelow},
	{ White, White, green_truelow, green_clear, green_high, green_high, green_clear, green_clear},
	{ White, White, green_truelow, green_clear, green_veryhig, green_veryhig, green_clear, green_clear},
	{ green_truelow, green_clear, green_clear, green_clear, green_clear, green_clear, green_clear, green_clear},
	{ green_clear, green_truelow, green_truelow, green_clear, green_clear, green_clear, green_clear, green_clear},
	{ green_clear, green_truelow, green_truelow, green_truelow, green_clear, green_truelow, green_truelow, green_clear},
	{ green_truelow, White, White, White, green_truelow, green_clear, green_truelow, White},
	{ White, White, White, White, green_truelow, green_clear, green_truelow, White},
	{ White, White, White, White, White, green_truelow, White, White},
	{ White, White, White, White, White, White, White, White},
	{ White, White, White, White, White, White, White, White}
};

static __attribute__((always_inline)) void LCD_SetStartingPoint(uint16_t Xpos, uint16_t Ypos) {
		LCD_SetCursor(Xpos, Ypos);
		LCD_WriteIndexNoInline(0x0022);
}

static __attribute__((always_inline)) uint16_t map_monster_pixel(uint16_t pixel, uint8_t mode) {
	
	switch(mode) {
		case 0:
		default:	
			return pixel;
		
		case 1:
			if (pixel == green_clear) {
				return red_clear;
			} else if (pixel == green_high) {
				return red_high;
			} else if (pixel == green_veryhig) {
				return bordeaux_high;
			} else if (pixel == green_truelow) {
				return red_low;
			}
			
	}

	return pixel;
}

void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t heigth, uint16_t width, uint16_t bkColor) {
	/*uint16_t i;
	
	for (i = 0; i < heigth; i++) {
		LCD_DrawLine(Xpos, Ypos + i, Xpos + width - 1, Ypos + i, bkColor);
	}*/
	
	uint16_t i, j;
	
	for (i = 0; i < heigth; i++) {
		LCD_SetCursor(Xpos, Ypos + i); 	// SetCursor sets the right start address (the row address) into the GRAM (address 0x0022) 
		LCD_WriteIndexNoInline(0x0022);	// Sets writing into GRAM
		
		for (j = 0; j < width; j++) {
			LCD_WriteDataNoInline(bkColor);	// Starting from last SetCursor, this writes data into GRAM (writes columns)
		}
	}
	
}

void LCD_DrawSpaceship(uint16_t LeftTopXpos, uint16_t LeftTopYpos, uint8_t direction, uint16_t bkColor) {
	uint16_t i, j;
	
	switch(direction) {
		case NORTH:
				for (i = 0; i < 16; i++) {
					LCD_SetStartingPoint(LeftTopXpos, LeftTopYpos + i);
					for (j = 0; j < 16; j++) {
						//LCD_SetPoint(LeftTopXpos + j, LeftTopYpos + i, (spaceship_pixels[i][j] == White) ? bkColor : spaceship_pixels[i][j]);
						LCD_WriteDataNoInline(GET_BKCOLOR(spaceship_pixels[i][j], bkColor, i));
					}
				}
				break;
		case EAST:
				/*for (i = 0; i < 16; i++) {
					for (j = 0; j < 16; j++) {
						LCD_SetPoint(LeftTopXpos + 15 - i, LeftTopYpos + j, (spaceship_pixels[i][j] == White) ? bkColor : spaceship_pixels[i][j]);
					}
				}*/
				for (j = 0; j < 16; j++) {
					LCD_SetStartingPoint(LeftTopXpos, LeftTopYpos + j);
					for (i = 0; i < 16; i++) {
						LCD_WriteDataNoInline(GET_BKCOLOR(spaceship_pixels[15 - i][j], bkColor, 15 - i));
					}
				}
			break;
		case SOUTH:
				for (i = 0; i < 16; i++) {
					LCD_SetStartingPoint(LeftTopXpos, LeftTopYpos + 15 - i);
					for (j = 0; j < 16; j++) {
						//LCD_SetPoint(LeftTopXpos + 15 - j, LeftTopYpos + 15 - i, (spaceship_pixels[i][j] == White) ? bkColor : spaceship_pixels[i][j]);
						LCD_WriteDataNoInline(GET_BKCOLOR(spaceship_pixels[i][15 - j], bkColor, i));
					}
				}
				break;
		case WEST:
				/*for (i = 0; i < 16; i++) {
					for (j = 0; j < 16; j++) {
						LCD_SetPoint(LeftTopXpos + i, LeftTopYpos + 15 - j, (spaceship_pixels[i][j] == White) ? bkColor : spaceship_pixels[i][j]);
					}
				}*/
				for (j = 0; j < 16; j++) {
					LCD_SetStartingPoint(LeftTopXpos, LeftTopYpos + 15 - j);
					for (i = 0; i < 16; i++) {
						LCD_WriteDataNoInline(GET_BKCOLOR(spaceship_pixels[i][15 - j], bkColor, i));
					}
				}
				break;
	}
	
}

void LCD_DrawSpacemonster(uint16_t LeftTopXpos, uint16_t LeftTopYpos, uint8_t mode) {
	uint16_t i, j;
	
	for (i = 0; i < 16; i++) {
		for (j = 0; j < 16; j++) {
			LCD_SetPoint(LeftTopXpos + j, LeftTopYpos + i, GET_BKCOLOR(map_monster_pixel(monster_pixels[i][(j < 8) ? j : (7 - (j % 8))], mode), LCD_MAP_BKCOLOR, 0));
		}
	}	
}
