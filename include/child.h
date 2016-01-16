#ifndef __CHILD_H_
#define __CHILD_H_

#include "define.h"
extern processpool child_process[MAX_PROCESS_NUMBER];
extern int create_childs(int listenfd,int child_nums,socklen_t addrlen,const char *szLogFilePrefix);

#endif