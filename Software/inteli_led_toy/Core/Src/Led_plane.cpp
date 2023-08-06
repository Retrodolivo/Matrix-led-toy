#include "Led_plane.h"

Led_plane::Led_plane(TIM_HandleTypeDef *tim, uint32_t channel)
{
	if (channel == CH1 || channel == CH2 || channel == CH3 || channel == CH4)
	{
		pwm_channel = channel;
		htim = tim;
	}
	this->fill_full(0x000000);
	/*fill last matrix elements with reset_bits*/
	for (uint8_t k = 0; k < reset_bits; k++)
		matrix_pwm.at(matrix_pwm.size() - reset_bits + k) = 0;
}

void *Led_plane::get_matrix(enum MATRIX m)
{
	if (m == PWM)
		return (uint8_t *)matrix_pwm.data();
	else
		return (uint32_t *)matrix_rgb.data();
}

void Led_plane::fill_point(uint32_t x, uint32_t y, uint32_t rgb_code)
{
	/*WS2812 data input format is Green->Red->Blue(grb)*/
	/*grb = g | r | b*/
	uint32_t grb = (rgb_code & 0x0000FF00) << 8 | (rgb_code & 0x00FF0000) >> 8 | (rgb_code & 0x000000FF);
	/*
		Should take in account zig-zag matrix connection
	 	So different equations were made for odd rows
	*/
	uint32_t point = 0;
	if(y % 2 == 0)
		point = y * Cols + (Cols - 1) - x;
	else
		point = y * Cols + x;
	/*24 = 8 * 3 colors bits*/
	for (int8_t j = 23; j >= 0; j--)
	{
		if (grb & (1<<j))
			matrix_pwm.at(24 * point + (23 - j)) = high_bit;
		else
			matrix_pwm.at(24 * point + (23 - j)) = low_bit;
	}
	/*save color and its coordinates*/
	matrix_rgb.at(point) = grb;
}

void Led_plane::fill_full(uint32_t rgb_code)
{
	for (uint32_t i = 0; i < Rows; i++)
	  for (uint32_t j = 0; j < Cols; j++)
		  fill_point(i, j, rgb_code);
}

void Led_plane::fill_rect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t rgb_code)
{
	if (y1 * Cols + x1 < y2 * Cols + x2)
	{
		for (uint32_t i = y1; i <= y2; i++)
			for (uint32_t j = x1; j <= x2; j++)
				fill_point(j, i, rgb_code);
	}
}

void Led_plane::light_on()
{
	HAL_TIM_PWM_Start_DMA(htim, pwm_channel, (uint32_t *)this->get_matrix(Led_plane::PWM), this->size(Led_plane::PWM, Led_plane::TOTAL));
}
/*TODO true light off*/
void Led_plane::light_off()
{
	HAL_TIM_PWM_Stop_DMA(htim, pwm_channel);
}

uint32_t Led_plane::size(enum MATRIX m, enum SIZE s)
{
	if (m == PWM)
	{
		switch (s)
		{
			case ROWS:
				return matrix_pwm.size() / Cols;
			case COLS:
				return matrix_pwm.size() / Rows;
			case TOTAL:
				return matrix_pwm.size();
			default:
				return 0;
		}
	}
	else
		switch (s)
		{
			case ROWS:
				return matrix_rgb.size() / Cols;
			case COLS:
				return matrix_rgb.size() / Rows;
			case TOTAL:
				return matrix_rgb.size();
			default:
				return 0;
		}
}

Led_plane::~Led_plane()
{

}

