#include "Joystick.h"

Joystick::Joystick(TIM_HandleTypeDef *polling_timer)
{
	this->polling_timer = polling_timer;
}

void Joystick::on()
{
	HAL_TIM_Base_Start_IT(polling_timer);
}

void Joystick::off()
{
	HAL_TIM_Base_Stop_IT(polling_timer);
}

Joystick::~Joystick()
{

}

