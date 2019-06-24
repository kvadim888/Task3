#include "libdsp.h"

uint32_t dsp_FIR(t_ringbuff *input, t_ringbuff *coeffs)
{
	uint32_t	result = 0;
		
	for (int i = 0; i < coeffs->len; i++)
	{
		result = fix_mac(result,
			input->ptr[(input->len - i) % input->len], coeffs->buff[i]);
	}
	return result;
}

t_ringbuff * dsp_FIRimport(const char * path)
{
	FILE		*file;
	t_ringbuff	*buff;
	char		str[100];

	if (fopen_s(&file, path, "r"))
	{
		printf("%s: Unable to open file for reading\n", path);
		exit(1);
	}
	printf("\nfilename: %s\n", path);
	buff = malloc(sizeof(t_ringbuff));
	if (buff == NULL)
		return NULL;
	while (fgets(str, 90, file))
	{
		if (str[0] == '%')
			printf("%s", str);
		if (strncmp(str, "% Filter Length", 15) == 0)
			buff->len = atoi(str+22);
		if (strncmp(str, "Numerator:", 10) == 0)
			break;
		memset(str, 0, 100);
	}
	memset(str, 0, 100);
	buff->buff = malloc(buff->len * sizeof(uint32_t));
	for (int i = 0; i < buff->len; i++)
	{
		fgets(str, 90, file);
		buff->buff[i] = float_to_fix(atof(str));
		memset(str, 0, 100);
	}
	fclose(file);
	return buff;
}
