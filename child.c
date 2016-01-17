/* 
* @Author: jql
* @Date:   2016-01-15 02:53:17
* @Last Modified by:   jql
* @Last Modified time: 2016-01-16 22:21:50
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "logger.h"
#include "define.h"
#include "global.c"

static void split_client_message(char *str,char *cmd,char *opt_file)
{
	int i =0;
	char para[2][32]={{0},{0}};
	char *token;
	token = strtok(str,"|");

	while(NULL != token)
	{

		printf("%s\n", token);
		strcpy(para[i],token);
		i++;
		token = strtok(NULL,"|");
	}

	char *start = para[0];
	char *pos = strchr(start,'=');
	strncpy(cmd,pos+1,strlen(start)-(pos -start));

	start = para[1];
	pos = strchr(start,'=');
	strncpy(opt_file,pos+1,strlen(start)-(pos -start));
}

void do_service(int listenfd,socklen_t addrlen,int pipe)
{
	int connfd;
	socklen_t clilen;
	struct sockaddr *cliaddr;
	int res;

	char buf[256];
	char cmd[32];
	char opt_file[128];

	memset(buf,0,sizeof(buf));
	memset(cmd,0,sizeof(cmd));
	memset(opt_file,0,sizeof(opt_file));

	cliaddr = malloc(addrlen);
	printf("wait for main message,pid = %d\n",getpid());
	while(1)
	{

		res = read(pipe,buf,sizeof(buf));
		if (res <= 0)
		{
			continue;
		}
#ifdef __DEBUG__
		printf("child %d  recv mess from pipe :%s\n", getpid(),buf);
#endif
		clilen = addrlen;
		connfd = accept(listenfd,cliaddr,&clilen);

		memset(buf,0,sizeof(buf));
		res = read(connfd,buf,sizeof(buf));
#ifdef __DEBUG__
		printf("child read message from client:%s\n", buf);
#endif
		split_client_message(buf,cmd,opt_file);

#ifdef __DEBUG__
		printf("%s\n", cmd);
		printf("%s\n", opt_file);
#endif
		if (0 == strcmp(cmd,"fdfs_upload_file"))
		{
			close(1);
			dup(connfd);
			close(connfd);
			// printf("fdfs_upload_file\n");
			if (execl("./fdfs_upload_file","fdfs_upload_file","/etc/fdfs/client.conf",opt_file,NULL) <0)
			{
				logErrorEx(PROCESSPOOL_ERROR_LOG_FILENAME,"file: "__FILE__",line:%d,"\
					"errno info:%s",__LINE__,strerror(errno));
				return ;
			}
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%s,pid=%d","child_done",getpid());
			write(pipe,buf,sizeof(buf));

		}
		else if (0 == strcmp(cmd,"fdfs_download_file"))
		{
			close(1);
			dup(connfd);
			close(connfd);
			if (execl("./fdfs_download_file","fdfs_download_file","/etc/fdfs/client.conf",opt_file,NULL) <0)
			{
				logErrorEx(PROCESSPOOL_ERROR_LOG_FILENAME,"file: "__FILE__",line:%d,"\
					"errno info:%s",__LINE__,strerror(errno));
				return ;
			}
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%s,pid=%d","child_done",getpid());
			write(pipe,buf,sizeof(buf));
		}
		else if (0 == strcmp(cmd,"fdfs_delete_file"))
		{

			if (execl("./fdfs_delete_file","fdfs_delete_file","/etc/fdfs/client.conf",opt_file,NULL) <0)
			{
				logErrorEx(PROCESSPOOL_ERROR_LOG_FILENAME,"file: "__FILE__",line:%d,"\
					"errno info:%s",__LINE__,strerror(errno));
				return ;
			}
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%s,pid=%d","child_done",getpid());
			write(pipe,buf,sizeof(buf));
		}
		else if (0 == strcmp(cmd,"fdfs_file_info"))
		{
			close(1);
			dup(connfd);
			close(connfd);
			if (execl("./fdfs_file_info","fdfs_file_info","/etc/fdfs/client.conf",opt_file,NULL) <0)
			{
				logErrorEx(PROCESSPOOL_ERROR_LOG_FILENAME,"file: "__FILE__",line:%d,"\
					"errno info:%s",__LINE__,strerror(errno));
				return ;
			}
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%s,pid=%d","child_done",getpid());
			write(pipe,buf,sizeof(buf));
		}
		else
		{

		}
		close(connfd);
	}
}

int create_childs(int listenfd,int child_nums, socklen_t addrlen, const char *szLogFilePrefix)
{
	if (child_nums > MAX_PROCESS_NUMBER)
	{
		logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d,to many child_nums"\
			,__LINE__);
		return -1;
	}
	int i;
	int ret;
	int pid;

	for (i = 0; i < child_nums; ++i)
	{
		ret = socketpair( PF_UNIX, SOCK_STREAM, 0, child_process[i].pipefd );
		if (ret < 0)
		{
			logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d,to many child_nums"\
			,__LINE__);
			return -1;
		}
		pid = fork();
		if (pid < 0)
		{
			logErrorEx(szLogFilePrefix,"file: "__FILE__",line:%d"\
				"fork error,errinfo = %s\n",__LINE__,strerror(errno));
			return -1;
		}
		switch(pid)
		{
			case 0:
				/*close(0);
				close(1);
				dup(child_process[i].pipefd[1]);*/
				close(child_process[i].pipefd[0]);
				do_service(listenfd,addrlen,child_process[i].pipefd[1]);
				exit(0);

			default:
				close(child_process[i].pipefd[1]);
				child_process[i].child_pid = pid;
				child_process[i].status = IDLE_STATUS;
				break;
		}
	}

	return 0;
}