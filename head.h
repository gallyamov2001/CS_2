#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <sys/shm.h>
#include "fcntl.h"

#define PAGE_SIZE 4096
const char SIZEOFINT = 4;


void Init_sops(int n, struct sembuf *sops, short num, short op, short flg){
    sops[n].sem_num = num;
    sops[n].sem_op = op;
    sops[n].sem_flg = flg;
}

int Init_sem(int semId, int semNum, int initVal){
    return semctl(semId, semNum, SETVAL, initVal);
}
