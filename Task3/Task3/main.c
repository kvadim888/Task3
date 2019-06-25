#include <stdio.h>

#include "libwav.h"
#include "libfix.h"
#include "libdsp.h"

// av: file_input file_output file_FIR

int main(int ac, char **av)
{
	size_t		len = 0;
	//int16_t		*buffer_in;
	//int16_t		*buffer_out;

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
	
	t_ringbuff *ring_in = dsp_newring(input->buffer->datalen, input->buffer->datalen);
	t_ringbuff *ring_out = dsp_newring(input->buffer->datalen, input->buffer->datalen);

	g_buffIO = wav_getbuffIO(input->header.block_align * input->buffer->datalen);
	while ((len = wav_read(input)) > 0)
	{
//		dsp_ringload(ring_in, input->buffer->data, input->buffer->datalen);
//		dsp_ringproc(ring_in, ring_out, FIR, dsp_FIR);
//		dsp_ringpull(output->buffer->data, ring_out, output->buffer->datalen)
		len = wav_write(output, len);
	}

	wav_close(&input);
	wav_close(&output);
	return 0;
}
