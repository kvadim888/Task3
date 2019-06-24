#include "libdsp.h"

int32_t		dsp_db2gain(double gain)
{
	double scale = pow(10, gain / 20);
	return float_to_fix(scale);
}

uint32_t	dsp_Gain(t_ringbuff *input, t_ringbuff *coeffs)
{
	return fix_mul(input->ptr, coeffs->buff[0]);
}
