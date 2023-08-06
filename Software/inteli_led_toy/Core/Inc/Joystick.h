#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include "main.h"


class Joystick
{
public:
	Joystick(TIM_HandleTypeDef *polling_timer);

	TIM_HandleTypeDef *polling_timer;
	uint32_t rot_val[2];
	struct Rotation
	{
		uint32_t x_low = 250, y_low = 250;
		uint32_t x_high = 3800, y_high = 3800;
	} rot_lim;

	void on();
	void off();

	virtual ~Joystick();

};

#endif /* JOYSTICK_H_ */
