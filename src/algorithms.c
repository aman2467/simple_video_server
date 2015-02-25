
extern unsigned int g_framesize;
extern int g_algo_bw;
extern int g_algo_cartoon;

void apply_algo(char *frame, int enable)
{
	char *ptr;
	int cnt,pix_cnt;

	if(enable == 0) return;
	
	if(g_algo_bw) {
		ptr = frame;
		cnt = 0;
		while(cnt < g_framesize) {
			ptr++;
			*(ptr) = (0xfe)>>1;
			ptr += 1;
			cnt+=2;
		}
	}
	if(g_algo_cartoon) {
		ptr = frame;
		pix_cnt = 0;
		while(pix_cnt < g_framesize) {
			pix_cnt++;
			*(ptr+pix_cnt) &= 0xab;
		}
	}
}
