#ifndef SRC_LEDPLANE_H_
#define SRC_LEDPLANE_H_

#include "main.h"
#include <array>

using std::array;

class Led_plane
{
public:
	Led_plane(TIM_HandleTypeDef *tim, uint32_t channel);

	enum SIZE {ROWS, COLS, TOTAL} s;
	enum CHANNEL {CH1 = 0x00000000U, CH2 = 0x00000004U,
				  CH3 = 0x00000008U, CH4 = 0x0000000CU} ch;
	enum MATRIX {PWM, RGB} m;

	uint32_t size(enum MATRIX m, enum SIZE s);
	void fill_full(uint32_t rgb_code);
	void fill_point(uint32_t x, uint32_t y, uint32_t rgb_code);
	void fill_rect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t rgb_code);
	void light_on();
	void light_off();
	void *get_matrix(enum MATRIX m);
	static const uint8_t reset_bits = 60;
	static const uint32_t Rows = 16;
	static const uint32_t Cols = 16;
	array<uint32_t, Rows * Cols> matrix_rgb;

	virtual ~Led_plane();
private:
	uint32_t pwm_channel;
	TIM_HandleTypeDef *htim;
	uint8_t high_bit = 60;
	uint8_t low_bit = 30;
	array<uint8_t, Rows * Cols * 24 + reset_bits> matrix_pwm;
};

#endif /* SRC_LEDPLANE_H_ */
