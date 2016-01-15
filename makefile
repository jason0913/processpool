.PHONY:clean

CC = gcc
RM = rm
RMFLAG = -rf
CFLAG = -Wall -DOS_LINUX -D__DEBUG__ -g

EXE = processpool
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))
INCLUDE = ./include/

$(EXE):$(OBJS)
	$(CC) $^ -o $@ $(CFLAG) $(CFLAG)
%.o:%.c
	$(CC) -c $^ -I $(INCLUDE) -o $@ $(CFLAG)

clean:
	$(RM) $(RMFLAG) $(EXE) $(OBJS)