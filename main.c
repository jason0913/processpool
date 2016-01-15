/*
* @Author: jql
* @Date:   2016-01-14 21:42:49
* @Last Modified by:   jql
* @Last Modified time: 2016-01-14 22:40:24
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "sockopt.h"
#include "define.h"

int main(int argc, char const *argv[])
{
	char bind_addr[IPADDR_SIZE];

	int connectsock;
	int listensock;
	int result;

	memset(bind_addr,0,sizeof(bind_addr));

	listensock = socketServer(bind_addr,30000,PROCESSPOOL_ERROR_LOG_FILENAME);
	if (listensock <0)
	{
		printf("socketServer failed!\
			file:%s,line:%d\n",__FILE__,__LINE__);
		return 5;
	}

	return 0;
}