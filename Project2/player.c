#include "local.h"


struct Player player;


int distance = 20;
int workSignal = 1;
void playerOne(int,int,sem_t *,struct memory *);
void playerTwoOrThree(int,int,int,int,sem_t *,struct memory *);
void playerFour(int,sem_t *,struct memory *);


int main(int argc, char *argv[]){
sem_t *sem1;
sem_t *sem2;
srand(getpid());
if (argc != 3) // number of arguments
    {
        printf("ERROR");
        fflush(stdout);
        exit(-3);
    }
void changeWorkSignal(int );
if (sigset(SIGUSR1, changeWorkSignal) == SIG_ERR) // to toggle working
    {
        perror("Sigset can not set SIGQUSR1");
        exit(SIGQUIT);
    }


int shmID;
int ppid = getppid();
//accessing the shared memory
struct memory *sharedData;
  if((shmID = shmget(ppid ,sizeof(struct memory)*6, 0666 | IPC_CREAT)) < 0){
  perror("shared memory access");
  exit(1);
  }
sharedData = (struct memory *)shmat(shmID,NULL,0);
if(sharedData < 0){
perror("shmat");
  exit(1);
}
// accress the semaphores
sem1 = sem_open("mysem1",O_RDWR);
sem2 = sem_open("mysem2",O_RDWR);


player.pid = getpid();
player.team = atoi(argv[1]);
player.player = atoi(argv[2]);


printf("I am player=%d of Team #%d and my PID %d \n",player.player, player.team,getpid());
fflush(stdout);


if(  player.player == 1){
if(player.team == 1){

playerOne(0,1,sem1,sharedData);
}else {
playerOne(3,2,sem2,sharedData);
}



	
}

if( player.player == 4){

if(player.team == 1){

playerFour(0,sem1,sharedData);
}else {
playerFour(3,sem2,sharedData);
}

}


if(player.player == 2 || player.player== 3){
if(player.team == 2 && player.player == 2){
playerTwoOrThree(0,1,2,2,sem1,sharedData);
}else if(player.team == 2 && player.player == 3){
playerTwoOrThree(0,2,3,2,sem1,sharedData);
}else if(player.team == 1 && player.player == 2){
playerTwoOrThree(3,4,2,1,sem2,sharedData);
}else{
playerTwoOrThree(3,5,3,1,sem2,sharedData);
}



	

}



return 0;
}

//player num 2 and 3 , those who steal the water from number 1
void playerTwoOrThree (int n,int np,int playerNumber,int team,sem_t *sem,struct memory *sharedData){
int timeToGoBack;

while(1){
if(workSignal == 0){// here when the parent is checking who win the round we do nothing

}else{
sem_wait(sem);//waiting to access the shared memory

if(sharedData[n].mainTank == 0 && sharedData[n].bag <= 0 ){//if empty exit
sem_post(sem);
break;
}
	//stealing from the bag by generating a random number
	if(sharedData[n].bag > 0){
	
	int randomnumber = (rand() % (10-2+1))+2 - sharedData[np].sandBag;
	if(randomnumber < 0){
	randomnumber = 0;
	}
	if(randomnumber > sharedData[n].bag){
	randomnumber = sharedData[n].bag;
	
	}
	
	sharedData[n].bag -= randomnumber ;
	sharedData[n].mainTank +=randomnumber;//put the stolen water back to the tank
	#ifdef _D_
	printf(" Player %d from Team %d  stole %d from Player 4\n",playerNumber,team,randomnumber);
	fflush(stdout);
	#endif
	
	
	
	}

timeToGoBack = sharedData[n].distance/4;//going back to the main tank and then to steal again
if(timeToGoBack < 1){
timeToGoBack = 1;
}
if(sharedData[n].bag == 0){
	timeToGoBack=0;
	}
sem_post(sem);//release the semaphoe
sleep(timeToGoBack);

}


}

}

//putting sand in player 2 and 3
void playerFour(int n,sem_t *sem,struct memory *sharedData){
while(1){
if(workSignal == 0){

}else{
sem_wait(sem);
if(sharedData[n].mainTank == 0 && sharedData[n].bag <= 0 ){
sem_post(sem);
break;
}
int randomPush = (rand() % 2)+1  ;
int sandBagRandom;
if(randomPush == 1){//pushing player 4
sandBagRandom=0;
}else{
int chooseRandomPlayer = (rand() % 2)+1  ;
sandBagRandom = (rand() % 3) + 1 ;
if(chooseRandomPlayer == 1){// putting sand by choosing the player and the amount randomly

sharedData[n+1].sandBag = sandBagRandom ; 
}else{

sharedData[n+2].sandBag = sandBagRandom ;
}
}

sem_post(sem);
sleep(sharedData[n].distance/4);
}
}
}


void playerOne(int n,int team,sem_t *sem,struct memory *sharedData){
	//initial values
	int bagCapacity = 50;
	sharedData[n].targetTank = 0;
	sharedData[n].mainTank = 250;
	while(sharedData[n].mainTank > 0 ){
	
	if(workSignal == 0){
	
	}else{
	if(sharedData[n].mainTank >bagCapacity ){//put water in the bag
	sharedData[n].mainTank -= bagCapacity;
	sharedData[n].bag = bagCapacity;
	}else{
	sharedData[n].bag = sharedData[n].mainTank;
	sharedData[n].mainTank = 0;
	}
	
	sem_post(sem);// we release here so all players are waiting till player one finishes filling the bag
	
	int d;
	for(d = 0; d < distance;d++){
	sleep(0.1);
	sharedData[n].distance = d;
	if(d%4 == 0 ){//losing water from walking and falling down logic
	
	int fallingRandom = (rand() % 30) + 1;
	if(fallingRandom == 9){//player 1 fall down
	sem_wait(sem);
	printf("i fell down \n");
	fflush(stdout);
	sharedData[n].bag = 0;
	sem_post(sem);
	}
	
	
	
	if(sharedData[n].bag >= (bagCapacity)/2){//lose water from walking
	sem_wait(sem);
	
	int randomnumber = rand() % 3 ;
	sharedData[n].bag -= randomnumber ;
	#ifdef _D_
	printf("lost from walking %d\n",randomnumber);
	fflush(stdout);
	#endif
	sem_post(sem);
	}
	
	
	}
	
	if(sharedData[n].bag <= 0){//if the bag is empty during walking go back
	sem_wait(sem);
	sharedData[n].bag = 0;
	sleep(d/4);
	break;
	}
	
	}
	if(sharedData[n].bag > 0){//reaching target tank so we drop the water to it
	sem_wait(sem);
	sleep(0.1);
	#ifdef _D_
	printf("Player Bag Now has: %d\n",sharedData[n].bag);
	fflush(stdout);
	#endif
	sharedData[n].targetTank = sharedData[n].targetTank + sharedData[n].bag;
	printf("team #%d main tank is %d and targetTank is %d\n",team,sharedData[n].mainTank,sharedData[n].targetTank);
	fflush(stdout);
	
	if(sharedData[n].mainTank ==0 ){//signal to the parent to start comparing the tanks
	
	kill(getppid(),SIGUSR1);
	sleep(10);
	
	}
	
	sleep(d/4);
	}
	
	}
	
	
	}
	#ifdef _D_
	printf("team %d exit\n",team);
	fflush(stdout);
	#endif
	
}



void changeWorkSignal(int n){// toggle when the players are playing or not (recevied from the parent)
if(workSignal == 0){
workSignal = 1;
}else{
workSignal = 0;
}
}

