//Sally Ayoub 1181172
//Mohammad Awadallah 1180142
#include "local.h"
int team1Pids[4];
  int team2Pids[4];
  int shmID;
  int pauseKill=0;
struct memory *sharedData;
int team1Counter=0 , team2Counter=0;
int main(int argc, char *argv[]){

  if (argc != 2) // number of arguments
    {
        printf("please enter round time\n");
        fflush(stdout);
        exit(-3);
    }
    int roundTime = atoi(argv[1]);
    if(roundTime < 15 ){
    printf("please enter round time greater than 15 \n");
        fflush(stdout);
        exit(-3);
    }
  int ppid = getpid();
  
  sem_t *sem1;
  sem_t *sem2;
  /*shmem creation */
  
  if((shmID = shmget(ppid , sizeof(struct memory)*6, 0666 | IPC_CREAT)) < 0){
  perror("shared memory creation");
  exit(1);
  }
void gameWinner(int);

if (sigset(SIGUSR1, gameWinner) == SIG_ERR)
    {
        perror("Sigset can not set SIGQUSR1");
        exit(SIGQUIT);
    }
sharedData = (struct memory *) shmat(shmID,NULL,0);
if(sharedData < 0){
perror("shmatp");
  exit(1);
}


/*Semaphore*/
if((sem1 = sem_open("mysem1",O_CREAT,0644,1))==SEM_FAILED){
perror("Semaphore init");
  exit(1);
} 
if((sem2 = sem_open("mysem2",O_CREAT,0644,1))==SEM_FAILED){
perror("Semaphore init");
  exit(1);
} 
// initalizing the 2 semaphores to be zero
sem_init(sem1,1,0);
sem_init(sem2,1,0);

int pid;

int j;
for(j=1;j<5;j++){//creating team 1
 pid = fork();
 
 if(pid == 0 ){
 char playerNumber[5];
 sprintf(playerNumber, "%d", j);
 execl("./player", "player","1" , playerNumber,  (char *)NULL);
 }else{
 team1Pids[j-1]=pid;
 }
}



 char team1pid[5];
 sprintf(team1pid, "%d", pid);
 int i;
 for(i = 1 ; i<5; i++){//creating team 2
 pid = fork();
 if(pid == 0){
 	char playerNumber[5];
      

  sprintf(playerNumber, "%d", i);
  execl("./player", "player","2" , playerNumber ,  (char *)NULL);
 }else{
 team2Pids[i-1]=pid;
 }
 }
 



while(1){ // if round time finishes finish the round
sleep(roundTime);
if(pauseKill == 0){
kill(getpid(),SIGUSR1);
}

}
}

void gameWinner(int n){
pauseKill = 1;
int i;
for(i = 0 ; i < 4 ; i++ ){
kill(team1Pids[i],SIGUSR1);
kill(team2Pids[i],SIGUSR1);

}


 #ifdef _D_
 printf("Working in the parent\n");
 fflush(stdout);
 #endif
sleep(1);
 if(sharedData[0].targetTank > sharedData[3].targetTank){//checking which team has more water
 team1Counter++;
 team2Counter=0;
 printf("\033[1;31m");
 printf("team 1 won %d round in a row !!\n",team1Counter);
 printf("\033[0m");
 fflush(stdout);
 }else{
 team2Counter++;
 team1Counter=0;
  printf("\033[1;31m");
  printf("team 2 won %d round in a row !!\n",team2Counter);
  printf("\033[0m");

  fflush(stdout);
 }
 printf("\033[0m");
sharedData[0].mainTank = 250;//reseting for the next round
sharedData[3].mainTank = 250;
sharedData[0].targetTank = 0;
sharedData[3].targetTank = 0;
sleep(1);
for(i = 0 ; i < 4 ; i++ ){
kill(team1Pids[i],SIGUSR1);
kill(team2Pids[i],SIGUSR1);

}
pauseKill = 0;
#ifdef _D_
 printf("mid in the parent\n");
 fflush(stdout);
 #endif

 if(team1Counter == 3 || team2Counter==3){//if one team win 3 games in a row
 if(team1Counter == 3){
  printf("\033[5;1;33m");//blinking yellow
  printf("team 1 won the game !!! \n");
  fflush(stdout);
 }else{
  printf("\033[5;1;33m");//blinking yellow
  printf("team 2 won the game !!! \n");
  fflush(stdout);
 }
 for(i = 0 ; i < 4 ; i++ ){//killing all players
kill(team1Pids[i],SIGQUIT);
kill(team2Pids[i],SIGQUIT);


}
// free the resources
  shmdt(sharedData);
  shmctl(shmID, IPC_RMID, (struct shmid_ds *) 0);
int sem1return =sem_unlink("mysem1");
int sem2return =sem_unlink("mysem2");

if(sem1return !=0 || sem2return!=0){
perror("Semaphore unlinking error");
  exit(4);
}

 printf("\033[0m");
 printf("Sources are free and exiting the parent\n");
 fflush(stdout);
 exit(1);

 }
 

 

 
 





}





