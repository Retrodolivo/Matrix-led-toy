#include "Game_food.h"


Game_food::Game_food(Led_plane *plane)
{
	RTC_GetDateTime(&Time, &Date);
	srand(RTC_ToEpoch(&Time, &Date));
	/*Determinate occupied points in a matrix field*/
	/*Mark unavailable points for object via false*/
	bool available_points[plane->matrix_rgb.size()];
	for (uint32_t i = 0; i < plane->matrix_rgb.size(); i++)
	{
		if (plane->matrix_rgb[i] > 0)
			available_points[i] = false;
		else
			available_points[i] = true;
	}
	/*Choose randomly one of the available points*/
	uint32_t obj_point = 0;
	uint32_t rand_point = 0;
	while(1)
	{
		rand_point = rand() % plane->matrix_rgb.size();
		if (available_points[rand_point] == true)
		{
			obj_point = rand_point;
			break;
		}
	}
	/*Find coordinates of the point*/
	uint16_t obj_y = obj_point / plane->Cols;
	uint16_t obj_x;
	if (obj_y % 2 == 0)
		obj_x = obj_y * plane->Cols + (plane->Cols - 1) - obj_point;
	else
		obj_x = obj_point - obj_y * plane->Cols;

	/*Pick color randomly*/
	std::array<uint32_t, 3> c;
	for (uint8_t i = 0; i < c.size(); i++)
		c[i] = rand() % 0x08;
	rgb_color = c.at(0) << 16 | c.at(1) << 8 | c.at(2);

	plane->fill_point(obj_x, obj_y, rgb_color);

	grb_color = (rgb_color & 0x0000FF00) << 8 | (rgb_color & 0x00FF0000) >> 8 | (rgb_color & 0x000000FF);
}

Game_food::~Game_food()
{

}

