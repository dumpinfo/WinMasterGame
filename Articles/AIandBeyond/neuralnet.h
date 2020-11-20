#include <math.h>
#include <stdlib.h>
#include <fstream.h>
              
struct NEURALNET
{
	float Input[512];	// the input data
  	float Hidden[64];	// the hidden layer neurons
 	float Output[8];	// the output data
	float Weight1[512][64];	// the first set of weights
  	float Weight2[64][8];	// the second weight set
   float Target[8];

   float dk[8];
 	float dj[64];
};


void Net_Recognize(NEURALNET Net)
{
   int i, j, k;
   for(j=0; j<64; j++)
   {
   	float sum=0;
      for(i=0; i<512; i++)
      {
      	sum=+(Net.Input[i])*(Net.Weight1[i][j]);
      }
      Net.Hidden[j]=1/(1+exp(-sum));
   }
   for(k=0; k<8; k++)
   {
   	float sum=0;
      for(j=0; j<64; j++)
      {
      	sum=+(Net.Hidden[j])*(Net.Weight2[j][k]);
      }
      Net.Output[k]=1/(1+exp(-sum));
   }
}

void Net_Learn(NEURALNET Net)
{
   int i, j, k;
	Net_Recognize(Net);
	for(k=0; k<8; k++)
   {
   	Net.dk[k]=(Net.Target[k]-Net.Output[k])*(Net.Output[k])*(1-Net.Output[k]);
   }
   for(j=0; j<64; j++)
   {
   	for(k=0; k<8; k++)
      {
      	Net.Weight2[j][k]=+(Net.dk[k])*(Net.Hidden[k]);
      }
   }
   for(j=0; j<64; j++)
   {
   	float sum=0;
      for(k=0; k<8; k++)
      {
      	sum=+(Net.dk[k])*(Net.Weight2[j][k]);
      }
      Net.dj[j]=(Net.Hidden[j])*(1-Net.Hidden[j])*sum;
   }
   for(i=0; i<512; i++)
   {
   	for(j=0; j<64; j++)
      {
      	Net.Weight1[i][j]=+(Net.dj[j])*(Net.Input[i]);
      }
   }
}

void Net_Init(NEURALNET Net)
{
   int i, j, k;
   fstream file;
   file.open("VOICE.NET", ios::binary | ios::out);
   for(i=0; i<512; i++)
   {
   	for(j=0; j<64; j++)
      {
      	Net.Weight1[i][j]=random(1000000)*0.000001;
         file<<Net.Weight1[i][j]<<"\n";
      }
   }
   for(j=0; j<64; j++)
   {
   	for(k=0; k<8; k++)
      {
      	Net.Weight2[j][k]=random(1000000)*0.000001;
         file<<Net.Weight2[j][k]<<"\n";
      }
   }
   file.close();
}

void Net_Load(NEURALNET Net)
{
   int i, j, k;
   fstream file;
   file.open("VOICE.NET", ios::binary | ios::in);
   if(file==0)
   {
   	Net_Init(Net);
      return;
   }
   for(i=0; i<512; i++)
   {
   	for(j=0; j<64; j++)
		{
         file>>Net.Weight1[i][j];
         cout<<Net.Weight1[i][j]<<"\n";
      }
   }
   for(j=0; j<64; j++)
   {
   	for(k=0; k<8; k++)
      {
         file>>Net.Weight2[j][k];
         cout<<Net.Weight2[j][k]<<"\n";
      }
   }
   file.close();
}

void Net_Save(NEURALNET Net)
{
   int i, j, k;
   fstream file;
   file.open("VOICE.NET", ios::binary | ios::out);
   for(i=0; i<512; i++)
   {
   	for(j=0; j<64; j++)
      {
         file<<Net.Weight1[i][j]<<"\n";
         cout<<Net.Weight1[i][j]<<"\n";
      }
   }
   for(j=0; j<64; j++)
   {
   	for(k=0; k<8; k++)
      {
         file<<Net.Weight2[j][k]<<"\n";
         cout<<Net.Weight2[j][k]<<"\n";
      }
   }
   file.close();
}
