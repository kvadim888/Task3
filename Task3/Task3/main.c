#include <stdio.h>

#include "libwav.h"
#include "libfix.h"
#include "libdsp.h"

// av: file_input file_output file_FIR

int main(int ac, char **av)
{
	size_t		len = 0;

	if (ac != 4)
	{
		printf("Not enought input arguments\n");
		exit(1);
	}
	t_ringbuff *FIR = dsp_FIRimport(av[3]);
	t_wavbuffer *buffer = malloc(sizeof(t_wavbuffer));

	t_wavfile *input = wav_rdopen(av[1], buffer);
	wav_initbuff(input->buffer, &input->header, FIR->len);

	t_wavfile *output = wav_wropen(av[2], &input->header, input->buffer);

	t_ringbuff **ring_in = malloc(buffer->channels * sizeof(t_ringbuff*));
	for (size_t i = 0; i < buffer->channels; i++)
		ring_in[i] = dsp_newring(input->buffer->datalen * 2, input->buffer->datalen);

	t_ringbuff **ring_out = malloc(buffer->channels * sizeof(t_ringbuff*));
	for (size_t i = 0; i < buffer->channels; i++)
		ring_out[i] = dsp_newring(output->buffer->datalen * 2, output->buffer->datalen);

	while ((len = wav_read(input)) > 0)
	{
		for (size_t i = 0; i < buffer->channels; i++)
		{
			dsp_ringload(ring_in[i], input->buffer->data[i], input->buffer->datalen);
			dsp_ringproc(ring_in[i], ring_out[i], FIR, dsp_FIR);
			dsp_ringpull(output->buffer->data[i], ring_out[i], output->buffer->datalen);
		}
		len = wav_write(output, len);
	}

	wav_close(&input);
	wav_close(&output);
	return 0;
}
