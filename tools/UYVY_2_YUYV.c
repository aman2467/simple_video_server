#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main(int argc, char **argv)
{
	FILE *fps, *fpd;
	short int pixel, temp;
	char source[30] = {0},dest[30] = {0};
	char ch, ch2;
	if(argc != 3) {
		printf("Usage : %s <source_file.uyuv> <dest_file.yuyv>\n",argv[0]);
		exit(0);
	}
	strcpy(source,argv[1]);
	strcpy(dest,argv[2]);

	fps = fopen(source,"rb");
	fpd = fopen(dest,"wb");
	while((ch=fgetc(fps)) != EOF) {
		pixel = 0x0000;
		ch2 = fgetc(fps);
		pixel = ch;
		pixel = (pixel << 8);
		pixel |= ch2;
//		pixel |= 0x7f;
		fwrite(&pixel,2,1, fpd);
	}
	fclose(fps);
	fclose(fpd);
}
