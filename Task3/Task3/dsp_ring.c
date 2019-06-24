#include "libdsp.h"

t_ringbuff *dsp_newring(size_t len, size_t offset)
{
	t_ringbuff *ring;

	ring = malloc(sizeof(t_ringbuff));
	ring->len = len;
	ring->buff = calloc(ring->len, sizeof(uint32_t));
	ring->ptr = ring->buff + offset;
	return ring;
}

t_ringbuff *dsp_ringinit(size_t len, uint32_t *buff, size_t offset)
{
	t_ringbuff *ring;

	ring = malloc(sizeof(t_ringbuff));
	ring->len = len;
	ring->buff = buff;
	ring->ptr = ring->buff + offset;
	return ring;
}

void dsp_ringload(t_ringbuff *ring, uint32_t *buff, size_t len)
{
	if (ring->len <= len)
	{
		printf("ERROR: Total ring overwriting\n");
		return;
	}
	size_t size = ring->len - (size_t)(ring->ptr - ring->buff);
	if (size >= len)
		memcpy(ring->ptr, buff, len * sizeof(*buff));
	else
	{
		memcpy(ring->ptr, buff, size * sizeof(*buff));
		buff += size;
		memcpy(ring->buff, buff, (len - size) * sizeof(*buff));
	}
}

void dsp_ringpull(uint32_t *buff, t_ringbuff *ring, size_t len)
{
	if (ring->len <= len)
	{
		printf("ERROR: Repeating content in buff\n");
		return;
	}
	size_t size = ring->len - (size_t)(ring->ptr - ring->buff);
	if (size >= len)
		memcpy(buff, ring->ptr, len * sizeof(*buff));
	else
	{
		memcpy(buff, ring->ptr, size * sizeof(*buff));
		buff += size;
		memcpy(buff, ring->buff, (len - size) * sizeof(*buff));
	}
}

void dsp_ringproc(t_ringbuff * input, t_ringbuff * output,
				t_ringbuff * coeffs, uint32_t(*func)(t_ringbuff *, t_ringbuff *))
{
	if (input->len != output->len)
	{
		printf("input->len != output->len\n");
		return ;
	}
	if (coeffs->len > input->len)
	{
		printf("coeffs->len > input->len\n");
		return ;
	}
	for (int i = 0; i < input->len; i++)
	{
		output->buff[i] = func(input, coeffs);
	}
}

