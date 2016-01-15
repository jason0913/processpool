/* 
* @Author: jql
* @Date:   2016-01-14 21:44:20
* @Last Modified by:   jql
* @Last Modified time: 2016-01-14 22:32:48
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "logger.h"

int socketServer(const char *bind_ipaddr, const int port, \
		const char *szLogFilePrefix)
{

	struct sockaddr_in bindaddr;
	int sock;
	int result;

	sock = socket(AF_INET,SOCK_STREAM,0);
	if (sock <0)
	{
		logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d"\
			"socket creat file,errno= %d,err info= %s\n",\
			__LINE__,errno,strerror(errno));
		return -1;
	}

	result = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &result, sizeof(int));
	if (result <0)
	{
		logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d"\
			"setsockopt failed,errno= %d,err info= %s\n",\
			__LINE__,errno,strerror(errno));
		close(sock);
		return -2;
	}

	bindaddr.sin_family = AF_INET;
	bindaddr.sin_port = htons(port);
	if (NULL == bind_ipaddr || '\0' == bind_ipaddr[0])
	{
		bindaddr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		if (0 == inet_aton(bind_ipaddr,&bindaddr.sin_addr))
		{
			logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d,"\
			"invalid ip %s,errno = %d ,err info = %s\n",\
			__LINE__,bind_ipaddr,errno,strerror(errno));

			close(sock);
			return -3;
		}
	}

	result = bind(sock,(struct sockaddr*)&bindaddr, sizeof(bindaddr));
	if (result <0)
	{
		logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d,"\
			"bind port fail,errno = %d,err info = %s\n",\
			__LINE__,errno,strerror(errno));

		close(sock);
		return -4;
	}

	result = listen(sock,5);
	if (result <0)
	{
		logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d,"\
			"listen port fail,errno = %d,err info = %s\n",\
			__LINE__,errno,strerror(errno));

		close(sock);
		return -5;
	}

#ifdef __DEBUG__
	printf("listen...!,socketServer done! file :%s,line:%d\n",\
			__FILE__,__LINE__);
#endif

	return sock;
}