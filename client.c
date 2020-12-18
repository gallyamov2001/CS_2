#include "head.h"


void File_Translator(char* filename, int sem_id, void* addr){

    struct sembuf sops[4];

    int file_in;
    if ((file_in = open(filename, O_RDONLY)) < 0) {
        perror("Couldn't open the file for client.");
        exit(-1);
    }

    Init_sops(0, sops, 0, 0, 0);
    Init_sops(1, sops, 0, 1, SEM_UNDO);
    if((semop(sem_id, sops, 2)) < 0){
        printf("Semop error");
        exit(-1);
    }

    Init_sops(0, sops, 4, 0, 0);
    if((semop(sem_id, sops, 1)) < 0){
        printf("Semop error");
        exit(-1);
    }

    Init_sem(sem_id, 3, 1);

    Init_sops(0, sops, 0, 1, SEM_UNDO);
    Init_sops(1, sops, 3, -1, SEM_UNDO);
    if(semop(sem_id, sops, 2) < 0){
        printf("Semop error");
        exit(-1);
    }

    Init_sops(0, sops, 1, -2, 0);
    Init_sops(1, sops, 1, 2, 0);
    Init_sops(2, sops, 4, 1, SEM_UNDO);
    if(semop(sem_id, sops, 3) < 0){
        printf("Semop error");
        exit(-1);
    }


    int reallength = PAGE_SIZE - SIZEOFINT;

    while(reallength == PAGE_SIZE - SIZEOFINT){
        Init_sops(0, sops, 2, -1, 0);
        if(semop(sem_id, sops, 1) < 0){
            printf("Semop error");
            exit(-1);
        }



        if(semctl(sem_id, 4, GETVAL) != 2){
            break;
        }
        reallength = read(file_in, addr + SIZEOFINT, PAGE_SIZE - SIZEOFINT);
        *(int*)addr = reallength;

        Init_sops(0, sops, 3, 1, 0);
        if(semop(sem_id, sops, 1) < 0){
            printf("Semop error");
            exit(-1);
        }
    }
}



int main(int argc, char* argv[]){
    if (argc > 2 || argc < 1){
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

    File_Translator(argv[1], sem_id, addr);

    return 0;
}
