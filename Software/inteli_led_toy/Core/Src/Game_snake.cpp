#include "Game_snake.h"

Game_snake::Game_snake(TIM_HandleTypeDef *fps_timer, Led_plane *plane, uint32_t init_len, uint32_t rgb_head, uint32_t rgb_body)
{
//	RTC_GetDateTime(&Time, &Date);
//	srand(RTC_ToEpoch(&Time, &Date));

	this->fps_timer = fps_timer;
	snake_plane = plane;
	len = init_len;
	head_color = head_color_buff = rgb_head;
	body_color = body_color_buff = rgb_body;
	grb_body_color = (rgb_body & 0x0000FF00) << 8 | (rgb_body & 0x00FF0000) >> 8 | (rgb_body & 0x000000FF);
	/*first segment is a snake head always*/
	snake_segment[0].x = 7;
	snake_segment[0].y = 7;
	snake_plane->fill_point(snake_segment[0].x, snake_segment[0].y, head_color);

	/*Choose snake initial direction randomly*/
	dir = static_cast<DIR>(rand() % 3);
	switch (dir)
	{
		case UP:
			for (uint8_t i = 1; i < len; i++)
			{
				snake_segment[i].x = snake_segment[0].x;
				snake_segment[i].y = snake_segment[0].y + i;
				snake_plane->fill_point(snake_segment[i].x, snake_segment[i].y, body_color);
			}
			break;
		case DOWN:
			for (uint8_t i = 1; i < len; i++)
			{
				snake_segment[i].x = snake_segment[0].x;
				snake_segment[i].y = snake_segment[0].y - i;
				snake_plane->fill_point(snake_segment[i].x, snake_segment[i].y, body_color);
			}
			break;
		case LEFT:
			for (uint8_t i = 1; i < len; i++)
			{
				snake_segment[i].x = snake_segment[0].x + i;
				snake_segment[i].y = snake_segment[0].y;
				snake_plane->fill_point(snake_segment[i].x, snake_segment[i].y, body_color);
			}
			break;
		case RIGHT:
			for (uint8_t i = 1; i < len; i++)
			{
				snake_segment[i].x = snake_segment[0].x - i;
				snake_segment[i].y = snake_segment[0].y;
				snake_plane->fill_point(snake_segment[i].x, snake_segment[i].y, body_color);
			}
			break;
	}
}

void Game_snake::start_move()
{
	set_speed(sr);
	HAL_TIM_Base_Start_IT(fps_timer);
	is_moving = true;
}

void Game_snake::stop_move()
{
	HAL_TIM_Base_Stop_IT(fps_timer);
	is_moving = false;
}

void Game_snake::set_speed(enum SPEED_RATE sr)
{
	switch (sr)
	{
		case ONE:
			fps_timer->Instance->CNT = 0;
			fps_timer->Instance->ARR = 7200;
			break;
		case TWO:
			fps_timer->Instance->CNT = 0;
			fps_timer->Instance->ARR = 3600;
			break;
		case THREE:
			fps_timer->Instance->CNT = 0;
			fps_timer->Instance->ARR = 1800;
			break;
		case FOUR:
			fps_timer->Instance->CNT = 0;
			fps_timer->Instance->ARR = 900;
			break;
		case FIVE:
			fps_timer->Instance->CNT = 0;
			fps_timer->Instance->ARR = 450;
			break;
		case SIX:
			fps_timer->Instance->CNT = 0;
			fps_timer->Instance->ARR = 225;
			break;
	}
}

void Game_snake::make_step()
{
	/*First, get rid of previous state of a snake*/
	for (uint8_t i = 0; i < len; i++)
		snake_plane->fill_point(snake_segment[i].x, snake_segment[i].y, 0);
	/*segment got coord of its neighbor*/
	for (int16_t i = len - 2; i >= 0; i--)
	{
		snake_segment[i + 1].x = snake_segment[i].x;
		snake_segment[i + 1].y = snake_segment[i].y;
	}
	/*Then change position of snake according to its direction*/
	switch (dir)
	{
		case UP:
			/*change position of the head*/
			if (snake_segment[0].y > 0)
				snake_segment[0].y -= 1;
			else
				snake_segment[0].y = Led_plane::Cols - 1;
			to_light_snake();
			break;

		case DOWN:
			/*change position of the head*/
			if (snake_segment[0].y < Led_plane::Cols - 1)
				snake_segment[0].y += 1;
			else
				snake_segment[0].y = 0;
			to_light_snake();
			break;

		case LEFT:
			/*change position of the head*/
			if (snake_segment[0].x > 0)
				snake_segment[0].x -= 1;
			else
				snake_segment[0].x = Led_plane::Rows - 1;
			to_light_snake();
			break;

		case RIGHT:
			/*change position of the head*/
			if (snake_segment[0].x < Led_plane::Rows - 1)
				snake_segment[0].x += 1;
			else
				snake_segment[0].x = 0;
			to_light_snake();
			break;
	}
	/*if snake ate food*/
	if(!is_stretch)
	{
		if(steps_till_stretch != 0)
			steps_till_stretch--;
		else
		{
			snake_segment[len].x = snake_segment[len - 1].x;
			snake_segment[len].y = snake_segment[len - 1].y;
		}
		is_stretch = true;
	}
	/********************/
	move_done = true;
}

void Game_snake::turn(enum DIR dir)
{
	/*Take care of 180 degrees turnings. We don't need them*/
	switch (dir)
	{
		case UP:
			if (move_done && this->dir != DOWN)
			{
				this->dir = UP;
				move_done = false;
			}
			break;
		case DOWN:
			if (move_done && this->dir != UP)
			{
				this->dir = DOWN;
				move_done = false;
			}
			break;
		case LEFT:
			if (move_done && this->dir != RIGHT)
			{
				this->dir = LEFT;
				move_done = false;
			}
			break;
		case RIGHT:
			if (move_done && this->dir != LEFT)
			{
				this->dir = RIGHT;
				move_done = false;
			}
			break;
	}
}

void Game_snake::set_brightness(float brightness)
{
	if (brightness < 0.05)
		brightness = 0.05;
	uint8_t head_8t, body_8t = 0;
	uint32_t head_32t, body_32t = 0;

	for(uint8_t i = 0; i < 3; i++)
	{
		head_8t = ((head_color_buff >> i * 8) & 0xFF) * brightness;
		head_32t |= static_cast<uint32_t>(head_8t) << i * 8;

		body_8t = ((body_color_buff >> i * 8) & 0xFF) * brightness;
		body_32t |= static_cast<uint32_t>(body_8t) << i * 8;
	}
	head_color = head_32t;
	body_color = body_32t;
	grb_body_color = (body_color & 0x0000FF00) << 8 | (body_color & 0x00FF0000) >> 8 | (body_color & 0x000000FF);
}

/*
 * Check if obstacle one pixel ahead of a snake
 * Obstacles determine by color
 * */
enum OBJ_TYPES Game_snake::get_obj_ahead(Game_food *food)
{
	uint32_t pix_ahead = 0;
	uint32_t next_head_pos_x = 0;
	uint32_t next_head_pos_y = 0;

	/*if snake head is at border of led field*/
	if(snake_segment[0].y == 0 && dir == UP)
		next_head_pos_y = snake_plane->Rows - 1;
	else if(snake_segment[0].y == snake_plane->Rows - 1 && dir == DOWN)
		next_head_pos_y = 0;
	else if(dir == UP)
		next_head_pos_y = snake_segment[0].y - 1;
	else if(dir == DOWN)
		next_head_pos_y = snake_segment[0].y + 1;

	if(snake_segment[0].x == 0 && dir == LEFT)
		next_head_pos_x = snake_plane->Cols - 1;
	else if(snake_segment[0].x == snake_plane->Cols - 1 && dir == RIGHT)
		next_head_pos_x = 0;
	else if(dir == LEFT)
		next_head_pos_x = snake_segment[0].x - 1;
	else if(dir == RIGHT)
		next_head_pos_x = snake_segment[0].x + 1;


	switch (dir)
	{
		case UP:
			if (snake_segment[0].y % 2 == 0)
				pix_ahead = next_head_pos_y * snake_plane->Cols + snake_segment[0].x;
			else
				pix_ahead = next_head_pos_y * snake_plane->Cols + (snake_plane->Cols - 1) - snake_segment[0].x;
			break;
		case DOWN:
			if (snake_segment[0].y % 2 == 0)
				pix_ahead = next_head_pos_y * snake_plane->Cols + snake_segment[0].x;
			else
				pix_ahead = next_head_pos_y * snake_plane->Cols + (snake_plane->Cols - 1) - snake_segment[0].x;
			break;
		case LEFT:
			if (snake_segment[0].y % 2 == 0)
				pix_ahead = snake_segment[0].y * snake_plane->Cols + (snake_plane->Cols - 1) - next_head_pos_x;
			else
				pix_ahead = snake_segment[0].y * snake_plane->Cols + next_head_pos_x;
			break;
		case RIGHT:
			if (snake_segment[0].y % 2 == 0)
				pix_ahead = snake_segment[0].y * snake_plane->Cols + (snake_plane->Cols - 1) - next_head_pos_x;
			else
				pix_ahead = snake_segment[0].y * snake_plane->Cols + next_head_pos_x;
			break;
	}

	if(snake_plane->matrix_rgb[pix_ahead] == food->grb_color)
		return FOOD;
	else if(snake_plane->matrix_rgb[pix_ahead] == grb_body_color)
		return OBSTACLE;
	else
		return NONE;
}

void Game_snake::devour(Game_food *food)
{
	is_stretch = false;
	steps_till_stretch = len;
	len += 1;

	delete food;
}

void Game_snake::change_color(uint32_t rgb_color)
{
	head_color = body_color = rgb_color;
	grb_body_color = (rgb_color & 0x0000FF00) << 8 | (rgb_color & 0x00FF0000) >> 8 | (rgb_color & 0x000000FF);

	snake_plane->fill_point(snake_segment[0].x, snake_segment[0].y, rgb_color);
	for (uint8_t i = 1; i < len; i++)
		snake_plane->fill_point(snake_segment[i].x, snake_segment[i].y, rgb_color);
}

void Game_snake::to_light_snake()
{
	/*light up snake at the field*/
	snake_plane->fill_point(snake_segment[0].x, snake_segment[0].y, head_color);
	for (uint8_t i = 1; i < len; i++)
		snake_plane->fill_point(snake_segment[i].x, snake_segment[i].y, body_color);
}

Game_snake::~Game_snake()
{

}

