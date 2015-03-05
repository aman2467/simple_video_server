#include <osd_thread.h>
#include <time.h>
#include <sys/time.h>
#include <common.h>

extern unsigned int g_framesize;
extern int g_algo_bw;
extern int g_algo_thermal;

void getcurrenttime(DATE_TIME *cur)
{
	time_t timep;
	struct tm *p;

	tzset();
	time(&timep);
	p=localtime(&timep);

	cur->hour = p->tm_hour;
	cur->min = p->tm_min;
	cur->sec = p->tm_sec;
	cur->year = 1900+p->tm_year;
	cur->mon = p->tm_mon+1;
	cur->day = p->tm_mday;
}

void update_osd_window(int window)
{


}

