#include <iostream.h>
#include <math.h>
#include <complex.h>
#include <fstream.h>

void main(void)
{
   int k, n;						// integers for the loops
   complex mult;					// a complex number
   float prod;						// a floating point number for the real component
   complex i=complex(0, 1);	// the square root of -1
   fstream file;					// a file to store the generated source code in

   file.open("FFT.C", ios::out);	// opens a text file
	for(k=0; k<512; k++)				// loops through each sub harmonic
   {
		for(n=0; n<1024; n++)		// loops through each sample
      {
      	mult=exp((-2*M_PI*i*k*(n+1))/1024);	// calculates the equation
         prod=real(mult);							// gets the real component
      	if(prod!=0)									// eliminates useless lines of code
      	{
            if(prod!=1)	// only multiplies if the product isn't 1
            {
            	file<<"   Net.Input["<<k<<"]=+X["<<n<<"]"<<"*"<<prod<<";\n";
            }
            else			// prints only the variable because the product is 1
            {
               file<<"   Net.Input["<<k<<"]=+X["<<n<<"];\n";
            }
         }
      }
   }
   file.close();	// closes the file
}