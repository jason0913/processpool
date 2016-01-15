/* 
* @Author: jql
* @Date:   2016-01-14 21:53:54
* @Last Modified by:   jql
* @Last Modified time: 2016-01-14 22:32:33
*/

#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "define.h"


char g_error_file_prefix[64] = {'\0'};


static void doLog(const char *prefix,const char *text)
{
	time_t t;
	struct tm *pCurrentTime;
	char datebuffer[32];
	char logfile[MAX_PATH_SIZE];
	FILE *fp;
	int fd;
	struct flock lock;

	t = time(NULL);
	pCurrentTime = localtime(&t);
	strftime(datebuffer,sizeof(datebuffer),"[%Y-%m-%d %X]",pCurrentTime);

	if (0 != prefix)
	{
		snprintf(logfile,sizeof(logfile),"./logs/%s.log",prefix);
		umask(0);
		if (NULL == fopen(logfile,"a"))
		{
			fp = stderr;
		}
	}
	else
		fp = stderr;

	fd = fileno(fp);

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 10;

	if (0 == fcntl(fd,F_SETLKW, &lock))
	{
		fprintf(fp, "%s %s\n", datebuffer,text);
	}

	lock.l_type = F_UNLCK;
	fcntl(fd,F_SETLKW, &lock);
	if (fp != stderr)
	{
		fclose(fp);
	}
}

void logError(const char *format,...)
{
	char logBuffer[LINE_MAX];
	va_list ap;
	va_start(ap,format);
	vsnprintf(logBuffer,sizeof(logBuffer),format,ap);
	doLog(g_error_file_prefix,logBuffer);
	va_end(ap);
}

void logErrorEx(const char* prefix, const char* format, ...)
{
	char logBuffer[LINE_MAX];
	va_list ap;
	va_start(ap,format);
	vsnprintf(logBuffer,sizeof(logBuffer),format,ap);
	doLog(prefix,logBuffer);
	va_end(ap);
}

void logInfo(const char* prefix, const char* format, ...)
{
	char logBuffer[LINE_MAX];
	va_list ap;
	va_start(ap,format);
	vsnprintf(logBuffer,sizeof(logBuffer),format,ap);
	doLog(prefix,logBuffer);
	va_end(ap);
}