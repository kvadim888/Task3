#ifndef LIBDSP_H
#define LIBDSP_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "libfix.h"

/*==============================  RING BUFFER  ================ dsp_ring.c ===*/

typedef struct	s_ringbuff
{
	size_t		len;
	uint32_t	*buff;
	uint16_t	*ptr;
}				t_ringbuff;

/* Create ring buffer with length = len */ 
t_ringbuff *dsp_newring(size_t len, size_t offset);

/* Initialize ring buffer with length = len */ 
t_ringbuff *dsp_ringinit(size_t len, uint32_t *data, size_t offset);

/* Loading buff data to ring by (*ptr) location */
void dsp_ringload(t_ringbuff *ring, uint32_t *buff, size_t len);

/* Pulling ring data to buff by (*ptr) location */
void dsp_ringpull(uint32_t *buff, t_ringbuff *ring, size_t len);

/* Ring buffer processing iterator */
void dsp_ringproc(t_ringbuff * input, t_ringbuff * output,
	t_ringbuff * coeffs, uint32_t(*func)(t_ringbuff *, t_ringbuff *));

/*============================================================================*/

/*============================  FILTER FUNCTIONS  ============== dsp_FIR.c ===*/

/* FIR filter function */
uint32_t dsp_FIR(t_ringbuff *input, t_ringbuff *coeffs);

/* Imprort coefficients from MATLAB (Signal Processing Toolbox) file */
t_ringbuff *dsp_FIRimport(const char *file);

/*============================================================================*/


/*===========================  FIXPOINT UTILITIES  ================libdsp.c===*/

/*
**	Convert gain entered in dB to scale coeficient.
**	The gain can be negative (in dB) only.
*/

int32_t		dsp_db2gain(double gain);

/*============================================================================*/







#endif // LIBDSP_H
