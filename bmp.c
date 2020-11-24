//Toma Andrei 311CB	
#include "bmp_header.h"
#include <stdio.h>
#include <stdlib.h>

#define PIXEL_ALB 255
#define NUL 0

/*functie pentru scrierea matricei de pixeli*/
void Write_Pixels(TPixels **image, int height, int width, int pad, FILE *out)
{
	int i, j, q;
	for (i=height-1;i>=0;i--)
	{
		for (j=0;j<width;j++)
		{
			fwrite(&image[i][j].R, sizeof(unsigned char), 1, out);
			fwrite(&image[i][j].G, sizeof(unsigned char), 1, out);
			fwrite(&image[i][j].B, sizeof(unsigned char), 1, out);
		}
		for (q=0;q<pad;q++)
			fputc(NUL, out);
	}
}

void Write_Image(unsigned char pix1, unsigned char pix2, unsigned char pix3, 
	FILE *out)
{
	/*functie pentru scrierea a 3 pixeli*/
	fwrite(&pix1, sizeof(unsigned char), 1, out);
	fwrite(&pix2, sizeof(unsigned char), 1, out);
	fwrite(&pix3, sizeof(unsigned char), 1, out);
}

void Pixel_Alb(FILE *out)
{
	/*scriere in fisier a pixelilor albi*/
	fputc(PIXEL_ALB, out);
	fputc(PIXEL_ALB, out);
	fputc(PIXEL_ALB, out);
}

void WriteHeaders(bmp_fileheader *bitmapFileHeader, bmp_infoheader *inf,
	char *outfile)
{
	/*scrierea a structurilor bmp_fileheader bmp_infoheader*/
	FILE *out=fopen(outfile, "wb");
	if (out==NULL)
	{	
		printf("EROARE FISIERUL NU EXISTA\n");
		return;
	}
	fwrite(&bitmapFileHeader->fileMarker1, sizeof(char), 1, out);
	fwrite(&bitmapFileHeader->fileMarker2, sizeof(char), 1, out);
	fwrite(&bitmapFileHeader->bfSize, sizeof(int), 1, out);
	fwrite(&bitmapFileHeader->unused1, sizeof(short), 1, out);
	fwrite(&bitmapFileHeader->unused2, sizeof(short), 1, out);
	fwrite(&bitmapFileHeader->imageDataOffset, sizeof(int), 1, out);
	fwrite(&inf->biSize, sizeof(int), 1, out);
	fwrite(&inf->width, sizeof(int), 1, out);
	fwrite(&inf->height, sizeof(int), 1, out);
	fwrite(&inf->planes, sizeof(short), 1, out);
	fwrite(&inf->bitPix, sizeof(short), 1, out);
	fwrite(&inf->biCompression, sizeof(int), 1, out);
	fwrite(&inf->biSizeImage, sizeof(int), 1, out);
	fwrite(&inf->biXPelsPerMeter, sizeof(int), 1, out);
	fwrite(&inf->biYPelsPerMeter, sizeof(int), 1, out);
	fwrite(&inf->biClrUsed, sizeof(int), 1, out);
	fwrite(&inf->biClrImportant, sizeof(int), 1, out);
	return ;
}

TPixels **ReadBMP(char *infile, bmp_infoheader *inf, 
	bmp_fileheader *bitmapFileHeader, TPixels **image)
{
	/*citire imagini*/
	FILE *in=fopen(infile, "rb");
	if (in==NULL)
		printf("EROARE FISIERUL NU EXISTA!\n");
	fread(&bitmapFileHeader->fileMarker1, sizeof(char), 1, in);
	fread(&bitmapFileHeader->fileMarker2, sizeof(char), 1, in);
	fread(&bitmapFileHeader->bfSize, sizeof(int), 1, in);
	fread(&bitmapFileHeader->unused1, sizeof(short), 1, in);
	fread(&bitmapFileHeader->unused2, sizeof(short), 1, in);
	fread(&bitmapFileHeader->imageDataOffset, sizeof(int), 1, in);
	fread(&inf->biSize, sizeof(int), 1, in);
	fread(&inf->width, sizeof(int), 1, in);
	fread(&inf->height, sizeof(int), 1, in);
	fread(&inf->planes, sizeof(short), 1, in);
	fread(&inf->bitPix, sizeof(short), 1, in);
	fread(&inf->biCompression, sizeof(int), 1, in);
	fread(&inf->biSizeImage, sizeof(int), 1, in);
	fread(&inf->biXPelsPerMeter, sizeof(int), 1, in);
	fread(&inf->biYPelsPerMeter, sizeof(int), 1, in);
	fread(&inf->biClrUsed, sizeof(int), 1, in);
	fread(&inf->biClrImportant, sizeof(int), 1, in);
	fseek(in, bitmapFileHeader->imageDataOffset, SEEK_SET);
	int pix=3*inf->width, pad=0;
	/*aflare padding*/
	while (pix%4!=0)
	{
		pad++;
		pix++;
	}
	/*alocare a matricei de pixeli*/
	image=(TPixels **)malloc(sizeof(TPixels *)*inf->height);
	if (image==NULL)
	{
		printf("%s\n", "EROARE");
		return NULL;
	}
	int i, j;
	for (i=0;i<inf->height;i++)
	{
		image[i]=(TPixels *)malloc(sizeof(TPixels)*inf->width);
		if (image[i]==NULL)
		{
			printf("%s\n", "EROARE");
			return NULL;
		}
	}
	for(i=inf->height-1;i>=0;i--)
	{
		for(j=0;j<inf->width;j++)
		{
			fread(&image[i][j].R, sizeof(unsigned char), 1, in);
			fread(&image[i][j].G, sizeof(unsigned char), 1, in);
			fread(&image[i][j].B, sizeof(unsigned char), 1, in);
		}
		/*trecere peste padding*/
		fseek(in, pad, SEEK_CUR);
	}
	fclose(in);
	return image;
}

/*Task Black and White*/
void BlackAndWhite(TPixels **image, char *out_black_white, 
	bmp_fileheader *bitmapFileHeader, bmp_infoheader *inf)
{
	FILE *out=fopen(out_black_white,"wb");
	if (out==NULL)
		printf("EROARE!\n");
	/*scriere headere*/
	WriteHeaders(bitmapFileHeader, inf, out_black_white);
	/*mutare pointer din fisier pentru a scrie pixelii*/
	fseek(out, bitmapFileHeader->imageDataOffset, SEEK_SET);
	int i, j, BlackWhite, q,pix=3*inf->width, pad=0;
	/*aflare padding*/
	while (pix%4!=0)
	{
		pix++;
		pad++;
	}
	for (i=inf->height-1;i>=0;i--)
	{
		for (j=0;j<inf->width;j++)
		{
			/*calcul pixel pentru a transforma imaginea alb-negru*/
			BlackWhite=(image[i][j].R+image[i][j].G+image[i][j].B)/3;
			/*afisare pixeli imagine schombati pentru ca imaginea sa fie
			afisata alb-negru*/
			Write_Image(BlackWhite, BlackWhite, BlackWhite, out);
		}
		/*punere padding*/
		for (q=0;q<pad;q++)
			fputc(NUL, out);
	}
	return;
}

/*Task NoCrop*/
void NoCrop(TPixels **image, char *out_no_crop, 
	bmp_fileheader *bitmapFileHeader, bmp_infoheader *info)
{
	FILE *out=fopen(out_no_crop, "wb");
	if (out==NULL)
		printf("EROARE FISIER NU S-A PUTUT DESCHIDE\n");
	int i, j, pix=3*info->width, pad=0;
	while(pix%4!=0)
	{
		pix++;
		pad++;
	}
	if (info->width==info->height)
	{	/*daca matricea de pixeli este patratica afisez imaginea*/
		WriteHeaders(bitmapFileHeader, info, out_no_crop);
		fseek(out, bitmapFileHeader->imageDataOffset, SEEK_SET);
		Write_Pixels(image, info->height, info->width, 0, out);
	}
	else
	{
		if (info->width>info->height)
		{
			if (abs(info->width-info->height)%2==0) /*diferenta este para*/
			{
				int aux=info->height, q;
				info->height=info->width;
				WriteHeaders(bitmapFileHeader, info, out_no_crop);
				fseek(out, bitmapFileHeader->imageDataOffset, SEEK_SET);
				for (i=0;i<(info->width-aux)/2;i++)
				{
					for (j=info->width-1;j>=0;j--)
						Pixel_Alb(out);/*adaugare pixeli albi*/
					for (q=0;q<pad;q++)
						fputc(NUL, out);
				}
				Write_Pixels(image, aux, info->width, pad, out);
				for (i=0;i<(info->width-aux)/2;i++)
				{	
					for(j=info->width-1;j>=0;j--)
						Pixel_Alb(out);
					for (q=0;q<pad;q++)
						fputc(NUL, out);
				}
				fclose(out);
			}
			else
			{
				int aux=info->height, q;
				info->height=info->width;
				WriteHeaders(bitmapFileHeader, info, out_no_crop);
				fseek(out, bitmapFileHeader->imageDataOffset, SEEK_SET);
				for(i=0;i<((info->width-aux)/2)+1;i++)
				{
					for (j=info->width-1;j>=0;j--)
						Pixel_Alb(out);/*adaugare pixeli albi*/
					for (q=0;q<pad;q++)
						fputc(NUL, out);
				}
				Write_Pixels(image, aux, info->width, pad, out);			
				for (i=0;i<(info->width-aux)/2;i++)
				{
					for (j=info->width-1;j>=0;j--)
						Pixel_Alb(out);
					for (q=0;q<pad;q++)
						fputc(NUL,out);
				}
				fclose(out);
			}
		}
		else /*cazul in care diferenta dintre width si height e impara*/
		{
			int aux=info->width;
			info->width=info->height;
			WriteHeaders(bitmapFileHeader, info, out_no_crop);
			fseek(out, bitmapFileHeader->imageDataOffset, SEEK_SET);
			for (j=info->height-1;j>=0;j--)
			{
				for (i=0;i<(info->height-aux)/2;i++)
					Pixel_Alb(out);
				for (i=0;i<aux;i++)
					Write_Image(image[j][i].R, image[j][i].G, image[j][i].B,
						out);
				for (i=0;i<(info->height-aux)/2;i++)
					Pixel_Alb(out);
			}
		}
	}
}

void freeMatrix(int **mat, int n)
{
	int i;
	for (i=0;i<n;i++)
		free(mat[i]);
	free(mat);
}

/*Task Convolutional Layers*/
void Filter(TPixels **image, bmp_fileheader *bitmapHeader,
	bmp_infoheader *inf, char *outname, char *input)
{
	FILE *out=fopen(outname, "wb"), *in=fopen(input, "rt");
	if (out==NULL || in==NULL)
		printf("EROARE FISIERUL NU S-A PUTUT DESCHIDE\n");
	WriteHeaders(bitmapHeader, inf, outname);
	fseek(out, bitmapHeader->imageDataOffset, SEEK_SET);
	int dim, i, j, q, k;
	fscanf(in, "%d", &dim);
	/*alocare matrice de filter*/
	int **fil=(int **)malloc(sizeof(int *)*dim);
	if (fil==NULL)
	{
		printf("EROARE ALOCAREA NU S-A PUTUT FACE!\n");
		return;
	}
	for (i=0;i<dim;i++)
	{
		fil[i]=(int *)malloc(sizeof(int)*dim);
		if (fil[i]==NULL)
		{
			printf("EORARE ALOCAREA NU S-A EFECTUAT\n");
			return;
		}
	}
	/*citire matrice de filter*/
	for (i=0;i<dim;i++)
		for(j=0;j<dim;j++)
			fscanf(in, "%d", &fil[i][j]);
	int pix=3*inf->width, pad=0;
	while (pix%4!=0)
	{
		pad++;
		pix++;
	}
	for (i=inf->height-1;i>=0;i--)
	{
		for (j=0;j<inf->width;j++)
		{
			int sumR=0, sumG=0, sumB=0	;
			for (q=i-dim/2;q<=i+dim/2;q++)
			{
				for (k=j-dim/2;k<=j+dim/2;k++)
				{/*veriicare daca pixelul se afla in matricea de pixeli*/
					if (q>=0 && k>=0 && q<inf->height && k<inf->width)
					{	/*calculare pixeli aplicand matricea de filter*/
						sumR=sumR+image[q][k].R*fil[q-i+dim/2][k-j+dim/2];
						sumG=sumG+image[q][k].G*fil[q-i+dim/2][k-j+dim/2];
						sumB=sumB+image[q][k].B*fil[q-i+dim/2][k-j+dim/2];
					}
				}
			}
			if (sumR>255)
				sumR=255;
			if (sumR<0)
				sumR=0;
			if (sumG>255)
				sumG=255;
			if (sumG<0)
				sumG=0;
			if (sumB>255)
				sumB=255;
			if (sumB<0)
				sumB=0;
			Write_Image(sumR, sumG, sumB, out);/*scriere pixeli modificati*/	
		}
		for (q=0;q<pad;q++)
			fputc(NUL, out);
	}
	freeMatrix(fil, dim);
	fclose(in);
	fclose(out);
}

/*Task Min/Max Pooling*/
void Pooling (TPixels **image, bmp_fileheader *bitmapHeader, 
	bmp_infoheader *inf, char *outfile, char *input)
{
	FILE *in=fopen(input, "rt"), *out=fopen(outfile, "wb");
	int dim, i, j, q, k;
	char min_max;
	fscanf(in, "%c", &min_max);
	fscanf(in, "%d", &dim);
	int pad=0, pix=3*inf->width;
	WriteHeaders(bitmapHeader, inf, outfile);
	fseek(out, bitmapHeader->imageDataOffset, SEEK_SET);
	while (pix%4!=0)
	{
		pad++;
		pix++;
	}
	if (min_max=='M') /*Max Pooling*/
	{
		for (i=inf->height-1;i>=0;i--)
		{
			for (j=0;j<inf->width;j++)
			{
				unsigned char maxR=0, maxG=0, maxB=0;
				for (q=i-dim/2;q<=i+dim/2;q++)
				{
					for (k=j-dim/2;k<=j+dim/2;k++)
					{
						if (q>=0 && k>=0 && q<inf->height && k<inf->width)
						{
							if (image[q][k].R>maxR)
								maxR=image[q][k].R;
							if (image[q][k].G>maxG)
								maxG=image[q][k].G;
							if (image[q][k].B>maxB)
								maxB=image[q][k].B;
						}
					}
				}
				Write_Image(maxR, maxG, maxB, out);/*scriere pixeli noi*/
			}
			for (q=0;q<pad;q++)
				fputc(NUL, out);
		}
	}
	else /*Min Pooling*/
	{
		for (i=inf->height-1;i>=0;i--)
		{
			for (j=0;j<inf->width;j++)
			{
				unsigned char minR=255, minG=255, minB=255;
				for (q=i-dim/2;q<=i+dim/2;q++)
				{
					for (k=j-dim/2;k<=j+dim/2;k++)
					{
						if (q>=0 && k>=0 && q<inf->height && k<inf->width)
						{
							if (image[q][k].R<minR)
								minR=image[q][k].R;
							if (image[q][k].G<minG)
								minG=image[q][k].G;
							if (image[q][k].B<minB)
								minB=image[q][k].B;
						}
						else
						{
							minR=0;
							minG=0;
							minB=0;
						}
					}
				}
				Write_Image(minR, minG, minB, out);/*scriere pixeli noi*/
			}
			for (q=0;q<pad;q++)
				fputc(NUL, out);
		}
	}
	fclose(in);
	fclose(out);
}

int Cond_Cluster1 (TPixels **image, int i, int j, int t)
{
	if (abs(image[i][j].R-image[i+1][j].R)+abs(image[i][j].G-image[i+1][j].G)
		+abs(image[i][j].B-image[i+1][j].B)<=t)
		return 1;
	else
		return 0;
}

int Cond_Cluster2 (TPixels **image, int i, int j, int t)
{
	if (abs(image[i][j].R-image[i][j+1].R)+abs(image[i][j].G-image[i][j+1].G)
		+abs(image[i][j].B-image[i][j+1].B)<=t)
		return 1;
	else
		return 0;
}

TPixels **Clustering(TPixels **image, int i , int j, int **zona, int t, int zo,
	int height, int width)
{
	zona[i][j]=zo;
	if (i>=0 && i<height && j>=0 && j<width && zona[i+1][j]==0 && 
		Cond_Cluster1(image, i, j, t)==1 && i+1>=0 && i+1<height)
	{
		zona[i+1][j]=zo;
		Clustering(image, i+1, j, zona, t, zo, height, width);
	}
	if (i>=0 && i<height && j>=0 && j<width && zona[i][j+1]==0 && 
		Cond_Cluster2(image, i, j, t)==1 && j+1>=0 && j+1<width)
	{
		zona[i][j+1]=zo;
		Clustering(image, i, j+1, zona, t, zo, height, width);
	}		
	return image;
}

void Write_Clustering(char *outfile, bmp_fileheader *head, bmp_infoheader *inf)
{
	WriteHeaders(head, inf, outfile);
} 

/*functie pentru apelarea fiecarui task pentru fiecare imagine*/
void APEL(TPixels **photoPixels, bmp_infoheader *info, 
	bmp_fileheader *bmpHeader, char *input, char *output_black_white, 
	char *output_nocrop, char *output_filter, char *input_filter, 
	char *output_pooling, char *input_pooling, char *input_clustering,
	char *output_clustering, int **zona)
{
	int threshold, i, zo=1;
	photoPixels=ReadBMP(input, info, bmpHeader, photoPixels);
	zona=(int **)calloc(sizeof(int *), info->height);
	if (zona==NULL)
		return;
	for (i=0;i<info->height;i++)
	{
		zona[i]=(int *)calloc(sizeof(int), info->width);
		if (zona[i]==NULL)
			return;
	}	
	BlackAndWhite(photoPixels, output_black_white, bmpHeader, info);
	bmp_infoheader cinfo=*info;
	NoCrop(photoPixels, output_nocrop, bmpHeader, &cinfo);
	Filter(photoPixels, bmpHeader, info, output_filter, input_filter);
	Pooling(photoPixels, bmpHeader, info, output_pooling, input_pooling);
	FILE *in_clustering=fopen(input_clustering, "rt");
	fscanf(in_clustering, "%d", &threshold);
	printf("%d\n", threshold);
		Clustering(photoPixels, 0, 0, zona, threshold, zo, info->height, 
		info->width);
}

int main()
{
	TPixels **photoPixels0=NULL, **photoPixels1=NULL, **photoPixels2=NULL, 
	**photoPixels3=NULL, **photoPixels4=NULL, **photoPixels5=NULL,
	**photoPixels6=NULL, **photoPixels7=NULL, **photoPixels8=NULL, 
	**photoPixels9=NULL;
	bmp_infoheader bitmapHeader0, bitmapHeader1, bitmapHeader2, bitmapHeader3,
	bitmapHeader4, bitmapHeader5, bitmapHeader6, bitmapHeader7, bitmapHeader8,
	bitmapHeader9;
	bmp_fileheader bitmapInfo0, bitmapInfo1, bitmapInfo2, bitmapInfo3,
	bitmapInfo4, bitmapInfo5, bitmapInfo6, bitmapInfo7, bitmapInfo8, 
	bitmapInfo9;
	int **zona0=NULL, **zona1=NULL, **zona2=NULL, **zona3=NULL, **zona4=NULL, 
	**zona5=NULL, **zona6=NULL, **zona7=NULL, **zona8=NULL, **zona9=NULL;    
	/*apelul pentru fiecare functie*/
	APEL(photoPixels0, &bitmapHeader0, &bitmapInfo0, "input/images/test0.bmp", 
		"test0_black_white.bmp", "test0_nocrop.bmp", "test0_filter.bmp", 
		"input/filters/filter0.txt", "test0_pooling.bmp", 
		"input/pooling/pooling0.txt", "input/clustering/cluster0.txt", 
		"test0_clustered.bmp", zona0);
	APEL(photoPixels1, &bitmapHeader1, &bitmapInfo1, "input/images/test1.bmp",
		"test1_black_white.bmp", "test1_nocrop.bmp", "test1_filter.bmp", 
		"input/filters/filter1.txt", "test1_pooling.bmp", 
		"input/pooling/pooling1.txt", "input/clustering/cluster1.txt", 
		"test1_clustered.bmp", zona1);
	APEL(photoPixels2, &bitmapHeader2, &bitmapInfo2, "input/images/test2.bmp",
		"test2_black_white.bmp", "test2_nocrop.bmp", "test2_filter.bmp", 
		"input/filters/filter2.txt", "test2_pooling.bmp", 
		"input/pooling/pooling2.txt", "input/clustering/cluster2.txt", 
		"test2_clustered.bmp", zona2);
	APEL(photoPixels3, &bitmapHeader3, &bitmapInfo3, "input/images/test3.bmp",
		"test3_black_white.bmp", "test3_nocrop.bmp", "test3_filter.bmp", 
		"input/filters/filter3.txt", "test3_pooling.bmp", 
		"input/pooling/pooling3.txt", "input/clustering/cluster3.txt", 
		"test3_clustered.bmp", zona3);
	APEL(photoPixels4, &bitmapHeader4, &bitmapInfo4, "input/images/test4.bmp",
		"test4_black_white.bmp", "test4_nocrop.bmp","test4_filter.bmp",
		"input/filters/filter4.txt", "test4_pooling.bmp", 
		"input/pooling/pooling4.txt", "input/clustering/cluster4.txt", 
		"test4_clustered.bmp", zona4);
	APEL(photoPixels5, &bitmapHeader5, &bitmapInfo5, "input/images/test5.bmp",
		"test5_black_white.bmp", "test5_nocrop.bmp", "test5_filter.bmp",
		"input/filters/filter5.txt", "test5_pooling.bmp", 
		"input/pooling/pooling5.txt", "input/clustering/cluster5.txt", 
		"test5_clustered.bmp", zona5);
	APEL(photoPixels6, &bitmapHeader6, &bitmapInfo6, "input/images/test6.bmp",
		"test6_black_white.bmp", "test6_nocrop.bmp","test6_filter.bmp", 
		"input/filters/filter6.txt", "test6_pooling.bmp", 
		"input/pooling/pooling6.txt", "input/clustering/cluster6.txt", 
		"test6_clustered.bmp", zona6);
	APEL(photoPixels7, &bitmapHeader7, &bitmapInfo7, "input/images/test7.bmp",
		"test7_black_white.bmp", "test7_nocrop.bmp","test7_filter.bmp", 
		"input/filters/filter7.txt", "test7_pooling.bmp", 
		"input/pooling/pooling7.txt", "input/clustering/cluster7.txt", 
		"test7_clustered.bmp", zona7);
	APEL(photoPixels8, &bitmapHeader8, &bitmapInfo8, "input/images/test8.bmp",
		"test8_black_white.bmp", "test8_nocrop.bmp", "test8_filter.bmp", 
		"input/filters/filter8.txt", "test8_pooling.bmp", 
		"input/pooling/pooling8.txt", "input/clustering/cluster8.txt", 
		"test8_clustered.bmp", zona8);
	APEL(photoPixels9, &bitmapHeader9, &bitmapInfo9, "input/images/test9.bmp",
		"test9_black_white.bmp", "test9_nocrop.bmp", "test9_filter.bmp", 
		"input/filters/filter9.txt", "test9_pooling.bmp", 
		"input/pooling/pooling9.txt", "input/clustering/cluster9.txt", 
		"test9_clustered.bmp", zona9);
	return 0;
}