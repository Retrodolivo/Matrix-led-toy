#ifndef INC_GAME_FOOD_H_
#define INC_GAME_FOOD_H_

#include "main.h"
#include "Led_plane.h"
#include <random>
#include <stdlib.h>

extern "C"
{
	#include "rtc.h"
}


extern RTC_TimeTypeDef Time;
extern RTC_DateTypeDef Date;


class Game_food
{
public:
	Game_food(Led_plane *plane);
	uint32_t rgb_color;
	uint32_t grb_color;

	virtual ~Game_food();

};




#endif /* INC_GAME_FOOD_H_ */
