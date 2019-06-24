#ifndef LIBWAV_H
#define LIBWAV_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#pragma pack(push, 1)
// WAVE file header format
typedef struct s_wavheader
{
	char		riff[4];				// "RIFF"
	uint32_t	overall_size;			// filesize
	char		wave[4];				// "WAVE"
	char		fmt_chunk_marker[4];	// "fmt "string with trailing null char
	uint32_t	length_of_fmt;			// length of the format data
	uint16_t	format_type;			// format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	uint16_t	channels;				// channels' amount
	uint32_t	sample_rate;			// sampling rate (blocks per second)
	uint32_t	byterate;				// SampleRate * NumChannels * BitsPerSample/8
	uint16_t	block_align;			// NumChannels * BitsPerSample/8
	uint16_t	bits_per_sample;		// bits per sample, 8- 8bits, 16- 16 bits etc
	char		data_chunk_header[4];	// "DATA" or "FLLR"
	uint32_t	data_size;				// NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
}				t_wavheader;

typedef struct
{
	uint8_t		*data;
	size_t		datalen;
	size_t		samplen;
}				t_wavbuffer;

typedef	struct
{
	FILE		*fs;
	t_wavheader header;
	t_wavbuffer	*buffer;
}		t_wavfile;

#pragma pack(pop)

t_wavfile	*wav_rdopen(const char *path, t_wavbuffer *buffer);
t_wavfile	*wav_wropen(const char *path, t_wavheader *header, t_wavbuffer *buffer);

size_t		wav_read(t_wavfile *file);
size_t		wav_write(t_wavfile *file);

void		wav_buffclear(t_wavfile *file);

void		wav_info(const char *filename, t_wavheader *header);
void		wav_close(t_wavfile **wavfile);

uint32_t	wav_getsample(t_wavbuffer *buff, size_t pos);

/* byte little endian to big and vise a versa */

uint16_t swap_uint16(uint16_t val);
int16_t swap_int16(int16_t val);
uint32_t swap_uint32(uint32_t val);
int32_t swap_int32(int32_t val);

#endif