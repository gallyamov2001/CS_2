#include "head.h"

void File_Receiver(int sem_id, void* addr){

    struct sembuf sops[4];

    Init_sops(0, sops, 1, 0, 0);
    Init_sops(1, sops, 1, 1, SEM_UNDO);
    if((semop(sem_id, sops, 2)) < 0){
        printf("Semop error");
        exit(-1);
    }

    Init_sops(0, sops, 4, 0, 0);
    if((semop(sem_id, sops, 1)) < 0){
        printf("Semop error");
        exit(-1);
    }

    Init_sem(sem_id, 2, 2);

    Init_sops(0, sops, 1, 1, SEM_UNDO);
    Init_sops(1, sops, 2, -1, SEM_UNDO);
    if(semop(sem_id, sops, 2) < 0){
        printf("Semop error");
        exit(-1);
    }

    Init_sops(0, sops, 0, -2, 0);
    Init_sops(1, sops, 0, 2, 0);
    Init_sops(2, sops, 4, 1, SEM_UNDO);
    if(semop(sem_id, sops, 3) < 0){
        printf("Semop error");
        exit(-1);
    }


    int indicator = PAGE_SIZE - SIZEOFINT;

    while(indicator == PAGE_SIZE - SIZEOFINT) {

        Init_sops(0, sops, 3, -1, 0);
        if(semop(sem_id, sops, 1) < 0){
            printf("Semop error");
            exit(-1);
        }

        if(semctl(sem_id, 4, GETVAL) != 2){
            if (*(int*)addr == PAGE_SIZE - SIZEOFINT)
                break;
        }

        indicator = *(int*)addr;
        write(STDOUT_FILENO, addr + SIZEOFINT , indicator);

        Init_sops(0, sops, 2, 1, 0);
        if(semop(sem_id, sops, 1) < 0){
            printf("Semop error");
            exit(-1);
        }

    }



}


int main(int argc, char* argv[]){
    if (argc > 1){
        printf("Invalid argc\n");
        return 0;
    }

    int sem_id = semget(ftok("server.c", 10), 5, IPC_CREAT | 0666);
    if( sem_id == -1){
        perror("Invalid sem_id");
        exit(1);
    }
    //0-file translator
    //1-file receiver
    //2-shmwrite
    //3-shmread
    //4-synchronyze

    int shm_id = shmget(ftok("server.c", 10), PAGE_SIZE, IPC_CREAT | 0666);
    if( shm_id == -1){
        perror("Invalid shm_id");
        exit(1);
    }
    void* addr = shmat(shm_id, NULL, 0);


    File_Receiver(sem_id, addr);


    return 0;
}
