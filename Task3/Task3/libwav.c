#include "libwav.h"

uint8_t		*wav_getrwbuff(size_t len)
{
	if (g_rwbuff == NULL)
		g_rwbuff = calloc(len, sizeof(uint8_t));
	return (g_rwbuff);
}

int			wav_initbuff(t_wavbuffer * buffer, t_wavheader * header, size_t datalen)
{
	if (buffer == NULL || header == NULL)
		return 1;
	buffer->datalen = datalen;
	buffer->channels = header->channels;
	buffer->samplen = header->bits_per_sample / 8;
	buffer->data = malloc(buffer->channels * sizeof(uint8_t*));
	for (int i = 0; i < buffer->channels; i++)
		buffer->data[i] = calloc(buffer->datalen, buffer->samplen);
	return 0;
}

t_wavfile	*wav_rdopen(const char *path, t_wavbuffer *buffer)
{
	t_wavfile *wavfile;

	wavfile = malloc(sizeof(t_wavfile));
	if (fopen_s(&wavfile->fs, path, "rb"))
	{
		printf("%s: Unable to open file for reading\n", path);
		exit(1);
	}
	if (fread(&wavfile->header, sizeof(uint8_t), 44, wavfile->fs) < 44)
	{
		printf("%s: Invalid file\n", path);
		exit(1);
	}
	wav_info(path, &wavfile->header);
	if (buffer == NULL)
	{
		wavfile->buffer = malloc(sizeof(t_wavbuffer));
		wav_initbuff(wavfile->buffer, &wavfile->header, 100);
	}
	else
		wavfile->buffer = buffer;
	return (wavfile);
}

t_wavfile	*wav_wropen(const char *path, t_wavheader *header, t_wavbuffer *buffer)
{
	t_wavfile *wavfile;

	wavfile = malloc(sizeof(t_wavfile));
	if (fopen_s(&wavfile->fs, path, "wb"))
	{
		printf("%s: Unable to open file for writing\n", path);
		exit(1);
	}
	memcpy(&wavfile->header, header, sizeof(t_wavheader));
	if (fwrite(&wavfile->header, sizeof(uint8_t), 44, wavfile->fs) < 44)
	{
		printf("%s: Writing error\n", path);
		exit(1);
	}
	wav_info(path, &wavfile->header);
	if (buffer == NULL)
	{
		wavfile->buffer = malloc(sizeof(t_wavbuffer));
		wav_initbuff(wavfile->buffer, &wavfile->header, 100);
	}
	else
		wavfile->buffer = buffer;
	return (wavfile);
}

size_t		wav_read(t_wavfile * file)
{
	if (file == NULL || file->fs == NULL)
		return 0;
	g_rwbuff = wav_getrwbuff(file->buffer->samplen * file->buffer->channels * file->buffer->datalen);
	size_t len = fread(g_rwbuff, file->buffer->samplen, file->buffer->datalen, file->fs);
	size_t datalen = len / (file->buffer->samplen * file->buffer->channels);
	assert(len % datalen);
	t_sample	sample;
//	for (int i = 0; i < datalen; i++)
//	{
//		for (int j = 0; j < file->buffer->channels; j++)
//		{
//			if (file->buffer->samplen == 1)
//				sample.int8[7] = (uint8_t) *(g_rwbuff + i * file->buffer->samplen * file->buffer->channels + j * file->buffer->samplen);
//			else if (file->buffer->samplen == 2)
//				sample.int16[3] = (uint16_t) *(g_rwbuff + i * file->header.block_align + j * file->buffer->samplen);
//			else if (file->buffer->samplen == 4)
//				sample.int32[1] = (uint32_t) *(g_rwbuff + i * file->header.block_align + j * file->buffer->samplen);
//			file->buffer->data[j][i] = sample.int32[1];
//		}
//	}
	memset(g_rwbuff, 0, len);
	return datalen;
}

size_t		wav_write(t_wavfile * file)
{
	t_sample	sample;
	if (file == NULL || file->fs == NULL)
		return 0;
	g_rwbuff = wav_getrwbuff(file->header.block_align * file->buffer->datalen);
	memset(g_rwbuff, 0, file->header.block_align * file->buffer->datalen);
//	for (size_t i = 0; i < file->buffer->datalen; i++)
//	{
//		for (size_t j = 0; j < file->buffer->channels; j++)
//			memcpy(g_rwbuff + i * file->header.block_align + j * file->buffer->samplen,
//				file->buffer->data[j][i], file->buffer->samplen);
//	}
	size_t len = fwrite(g_rwbuff, file->header.block_align, file->buffer->datalen, file->fs);
	return len / file->header.block_align;
}

void		wav_close(t_wavfile **wavfile)
{
	if (!wavfile && !*wavfile)
		return;
	fclose((*wavfile)->fs);
	printf("file closing\n");
	free(*wavfile);
	*wavfile = NULL;
}

void		wav_info(const char *filename, t_wavheader *header)
{
	if (filename == NULL || header == NULL)
		return;
	printf("\nfilename: %s\n", filename);

	printf("riff:\t%.4s \n", header->riff); 
	printf("overall_size:\t%u b (%u Kb)\n", header->overall_size, header->overall_size / 1024); 
	printf("wave:\t%.4s \n", header->wave); 
	printf("fmt_chunk_marker:\t%.4s \n", header->fmt_chunk_marker); 
	printf("length_of_fmt:\t%u\n", header->length_of_fmt); 
	printf("format_type:\t%u\n", header->format_type); 
	printf("channels:\t%u\n", header->channels); 
	printf("sample_rate:\t%u\n", header->sample_rate); 
	printf("byterate:\t%u\n", header->byterate); 
	printf("block_align:\t%u\n", header->block_align); 
	printf("bits_per_sample:\t%u\n", header->bits_per_sample); 
	printf("data_chunk_header:\t%.4s \n", header->data_chunk_header); 
	printf("data_size:\t%u\n", header->data_size); 

	long num_samples = (8 * header->data_size) / (header->channels * header->bits_per_sample);
	printf("Number of samples:\t%lu \n", num_samples);

	long size_of_each_sample = (header->channels * header->bits_per_sample) / 8;
	printf("Size of each sample:\t%ld bytes\n", size_of_each_sample);

	float duration_in_seconds = (float) header->overall_size / header->byterate;
	printf("duration = %f sec\n", duration_in_seconds);
}

uint16_t	swap_uint16(uint16_t val)
{
    return (val << 8) | (val >> 8 );
}

int16_t		swap_int16(int16_t val) 
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

uint32_t	swap_uint32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | (val >> 16);
}

int32_t		swap_int32(int32_t val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | ((val >> 16) & 0xFFFF);
}
