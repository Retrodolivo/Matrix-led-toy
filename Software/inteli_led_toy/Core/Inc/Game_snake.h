#ifndef GAME_SNAKE_H_
#define GAME_SNAKE_H_

#include "main.h"
#include "Led_plane.h"
#include "Game_food.h"


extern "C"
{
	#include "rtc.h"
}

extern RTC_TimeTypeDef Time;
extern RTC_DateTypeDef Date;


class Game_snake
{
public:
	Game_snake(TIM_HandleTypeDef *fps_timer, Led_plane *plane, uint32_t init_len, uint32_t rgb_head, uint32_t rgb_body);

	enum SPEED_RATE {ONE, TWO, THREE, FOUR, FIVE, SIX} sr;/*pixels per second*/
	enum DIR {UP, DOWN, LEFT, RIGHT} dir;
	enum OBJ_TYPES get_obj_ahead(Game_food *food);
	TIM_HandleTypeDef *fps_timer = nullptr;
	bool is_moving = false;
	bool is_crush = false;

	void start_move();
	void stop_move();
	void set_speed(enum SPEED_RATE sr);
	void make_step();
	void turn(enum DIR dir);
	void devour(Game_food *food);
	void change_color(uint32_t rgb_color);
	void set_brightness(float brightness);


	virtual ~Game_snake();
private:
	uint32_t len;
	uint32_t head_color;
	uint32_t body_color;
	uint32_t head_color_buff;
	uint32_t body_color_buff;
	uint32_t grb_body_color;
	Led_plane *snake_plane = nullptr;
	uint8_t steps_till_stretch = 0;
	bool is_stretch = true;
	bool move_done = true;
	/*Max len of snake is ROWS * COLS of led field*/
	struct Snake_segment
	{
		uint8_t x = 0;
		uint8_t y = 0;
	} snake_segment[Led_plane::Rows * Led_plane::Cols];
//	std::map<struct Snake_segment, uint32_t> snake_body;
	void to_light_snake();

};

#endif /* GAME_SNAKE_H_ */
