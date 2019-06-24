#include <stdio.h>
#include <stdlib.h>
#include "libwav.h"
#include "libfix.h"
#include "libdsp.h"

// av: file_input file_output file_FIR

int main(int ac, char **av)
{
	size_t		len = 0;
	int16_t		*buffer_in;
	int16_t		*buffer_out;

	if (ac != 4)
	{
		printf("Not enought input arguments\n");
		exit(1);
	}
	t_ringbuff *FIR = dsp_FIRimport(av[3]);
	t_wavbuffer *buffer = malloc(sizeof(t_wavbuffer));
	t_wavfile *input = wav_rdopen(av[1], buffer);
	buffer->datalen = FIR->len;
	buffer->data = calloc(buffer->datalen,
		(input->header.channels * input->header.bits_per_sample) / 8);
	buffer->samplen = input->header.bits_per_sample / 8;
	buffer->datalen = 100 * input->header.channels;
	t_wavfile *output = wav_wropen(av[2], &input->header, input->buffer);
	
	t_ringbuff *ring_in = dsp_newring(input->buffer->datalen * 2, input->buffer->datalen);
	t_ringbuff *ring_out = dsp_newring(input->buffer->datalen * 2, input->buffer->datalen);

	while (wav_read(input) > 0)
	{
		dsp_ringload(ring_in, input->buffer->data, input->buffer->datalen);
		dsp_ringproc(ring_in, ring_out, FIR, dsp_FIR);
		dsp_ringpull(output->buffer->data, ring_out, output->buffer->datalen);
		wav_write(output);
		wav_buffclear(output);
	}

	wav_close(&input);
	wav_close(&output);
	return 0;
}
