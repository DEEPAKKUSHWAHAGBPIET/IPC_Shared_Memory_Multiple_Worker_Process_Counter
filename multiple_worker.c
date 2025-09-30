#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdatomic.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>

#define WORKERS 4
#define OPERATATIONS 100

typedef struct shmPayload{
     atomic_int counter[WORKERS];
}shmPayload;

int die(char *msg){
     perror(msg);
     exit(0);
}
int main(){
     int shmid = shmget(IPC_PRIVATE, sizeof(shmPayload), IPC_CREAT | 0600);
     if(shmid<0)
     {
          die("shmget error");
     }

     shmPayload *shm_ptr = (shmPayload *)shmat(shmid, NULL, 0);
     if(shm_ptr == (void *)-1)
     {
          die("shmat error");
     }

     for(int i=0; i<WORKERS; i++)
     {
          atomic_store(&shm_ptr->counter[i], 0);
     }

     for(int i=0; i<WORKERS; i++)
     {
          int pid = fork();
          if(pid==-1)
          {
               die("fork issue");
          }
          if(pid==0)
          {
               shmPayload *child_shm_ptr = (shmPayload *)shmat(shmid, NULL, 0);
               if(child_shm_ptr== (void *)-1)
               {
                    die("child shmptr");
               }

               for(int j=0; j<OPERATATIONS; j++)
               {
                    atomic_fetch_add(&child_shm_ptr->counter[i], 1);
               }

               shmdt(child_shm_ptr);
               _exit(0);
          }
     }

     //wait for each worker child to completee the increment of counter
     for(int i=0; i<WORKERS; i++)
     {
          wait(NULL);
     }

     int total_increment = 0;

     for(int i=0; i<WORKERS; i++)
     {
          int count = atomic_load(&shm_ptr->counter[i]);
          printf("worker : %d, its counter value : %d\n", i, count);
          total_increment += count;
     }
     printf("all workers finished..\n");
     printf("final sum of counter values : %d\n (expected total counter value : %d)\n", total_increment, WORKERS*OPERATATIONS);

     shmdt(shm_ptr);
     shmctl(shmid, IPC_RMID, NULL);
     
     return 0;
}