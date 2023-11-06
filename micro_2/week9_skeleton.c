#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <math.h>
#include <stdio.h>
#include <float.h>


void downsampling (unsigned char* in, int const height, int const width, int const channel, unsigned char* out); 
void upsampling (unsigned char* in, int const height, int const width, int const channel, unsigned char* out); 
void bilinear (unsigned char* in, int const height, int const width, int const channel, unsigned char* out); 
double imagePSNR(unsigned char* frame1, unsigned char* frame2, unsigned int size);

int main()
{
 
	int height;
	int width;
	int channel;

    	float PSNR_up,PSNR_bilinear;

	char command;

 	printf("Take a picture? (y/n)\n");
 	scanf("%c", &command);

 	if(command == 'n')
 		exit(1);


 	printf("Cheeze !\r\n");
	system("libcamera-still --width 960 --height 720 -o src.bmp");

 	unsigned char* imgIn = stbi_load("src.bmp", &width, &height, &channel, 3);
	unsigned char* imgOut_down0 = (unsigned char*) malloc (sizeof(unsigned char)*3*320*240);
	unsigned char* imgOut_up0 = (unsigned char*) malloc (sizeof(unsigned char)*3*960*720);
	unsigned char* imgOut_up1 = (unsigned char*) malloc (sizeof(unsigned char)*3*960*720);
 	
	downsampling(imgIn, height, width, channel, imgOut_down0);
	upsampling(imgOut_down0, height, width, channel, imgOut_up0);
	bilinear(imgOut_down0, height, width, channel, imgOut_up1);
	PSNR_up = imagePSNR(imgIn,imgOut_up0, 3*960*720);
	PSNR_bilinear = imagePSNR(imgIn,imgOut_up1, 3*960*720);
    	printf("======PSNR RESULT======\n");
	printf("%f, %f\n",PSNR_up,PSNR_bilinear);
	printf("=======================\n");
				
	stbi_write_bmp("image_down.bmp", width/3, height/3, channel, imgOut_down0);
	stbi_write_bmp("image_up.bmp", width, height, channel, imgOut_up0);
	stbi_write_bmp("image_bi.bmp", width, height, channel, imgOut_up1);
	
	stbi_image_free(imgIn);
	free(imgOut_down0);
	free(imgOut_up0);
	free(imgOut_up1);
 

	return 0;
}

void downsampling (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {  
	for (int h = 0; h < height / 3; h++)
	{
		for (int w = 0; w < width / 3; w++)
		{
			for (int ch = 0; ch < channel; ch++)
			{
				out[h * 320 * 3 + w * 3 + ch] = in[3 * h * 960 * 3 + 3 * w * 3 + ch];
			}
		}
	}
}

void upsampling (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) { 
	for (int h = 0; h < height / 3; h++)
	{
		for (int w = 0; w < width / 3; w++)
		{
			for (int ch = 0; ch < channel; ch++)
			{
				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						out[(3 * h + i) * 960 * 3 + (3 * w + j) * 3 + ch] = in[h * 320 * 3 + w * 3 + ch];
					}
				}

			}
		}
	}
}

void bilinear (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {  
	for (int h = 0; h < height / 3 - 1; h++)
	{
		for (int w = 0; w < width / 3 - 1; w++)
		{
			for (int ch = 0; ch < channel; ch++)
			{
				for (int i = 0; i < 6; i++)
				{
					for (int j = 0; j < 6; j++)
					{

						out[(3 * h + i) * 960 * 3 + (3 * w + j) * 3 + ch] =
							(1 - (double)i / 5) * ((1 - (double)j / 5) * in[h * 320 * 3 + w * 3 + ch]
								+ ((double)j / 5) * in[h * 320 * 3 + (w + 1) * 3 + ch])
							+ ((double)i / 5) * ((1 - (double)j / 5) * in[(h + 1) * 320 * 3 + w * 3 + ch]
								+ ((double)j / 5) * in[(h + 1) * 320 * 3 + (w + 1) * 3 + ch]);
					}
				}
			}
		}
	}
}

//Calculates image PSNR value
double imagePSNR(unsigned char* frame1, unsigned char* frame2, unsigned int size){   
	double MAX = 255.0;
	double MSE = 0.0;
	double PSNR = 0.0;
	double n = size;

	for (int i = 0; i < n; i++)
	{
		MSE = MSE + (1 / n) * (frame1[i] - frame2[i]) * (frame1[i] - frame2[i]);
	}
	PSNR = 20 * log10(MAX) - 10 * log10(MSE);
	return PSNR;
}
