#ifndef __HEADER_H_
#define __HEADER_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <wait.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>

struct memory {
	int distance;
	int bag;
	int mainTank;
	int sandBag;
	int targetTank;
};

struct Player{
    int team;
    int player;
    int pid;
};

enum {AVAIL_SLOTS, TO_CONSUME};

#endif
