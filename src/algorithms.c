#include <common.h>

void apply_algo(char *frame, int enable)
{
	char *ptr;
	int pix_cnt;
	SERVER_CONFIG *serverConfig = GetServerConfig();

	if(enable == 0) return;

	switch(serverConfig->algo_type) {
		case ALGO_BW:
			ptr = frame;
			pix_cnt = 1;
			while(pix_cnt < serverConfig->capture.framesize) {
				*(ptr+pix_cnt) = (0xfe)>>1;
				pix_cnt+=2;
			}
			break;
		case ALGO_CARTOON:
			ptr = frame;
			pix_cnt = 0;
			while(pix_cnt < serverConfig->capture.framesize) {
				pix_cnt++;
				*(ptr+pix_cnt) &= 0xab;
			}
			break;
		case ALGO_NONE:
			break;
	}
}
