#include <iostream.h>
#include <math.h>
#include <complex.h>

void FFT(float *X, float *Y)
{
   int k, n;						// integers for the loops
   complex mult;					// a complex number
   float prod;						// a floating point number for the real component
   complex i=complex(0, 1);	// the square root of -1

	for(k=0; k<512; k++)				// loops through each sub harmonic
   {
		for(n=0; n<1024; n++)		// loops through each sample
      {
      	mult=exp((-2*M_PI*i*k*(n+1))/1024);	// calculates the equation
         prod=real(mult);							// gets the real component
         Y[k]=+X[n]*prod;							// multiplies the real component
      }
   }
}