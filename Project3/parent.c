#include "local.h"
  

    int totalNumberOfDeteregnets = 0;
    int truckAsup = 0;
    int truckBsup = 0;
    int availableTrucks = 5;
    struct Storage_s1 storage_s1;
    
    struct Line line[5];
    int main(){
    
    ReadValuesFromUserInputFile(); // reading all the values from inputs.txt file.
    int i;
    for(i = 0 ; i < 5;i++){ //initiate each line material A and B
        line[i].mat_A = 0;
         line[i].mat_B = 0;
    }

	storage_s1.mat_A = 0;
    storage_s1.mat_B = 0;

    //trucks threads
    int truckAid = 0;
    int truckBid = 1;

    pthread_create(&truckA,NULL, (void *) supplierTruck, &truckAid);  
    pthread_create(&truckB,NULL, (void *) supplierTruck, &truckBid);  

    // create the loading emplyees threads  from Factory A & B to Storage Area 1
    for(i=0; i<numberOfLoadingEmployees;i++ ){
     int id = i;
        if(pthread_create(&l1_emp_threads[i],NULL, (void *) LoadingEmp, &id) ){
    	    perror("thread creation failed");
    	    exit(1);
        }
     }

    //create the loading emplyees threads from storage area 1 to the  5 lines 

    for(i=0; i<numberOfLoadingEmployees;i++ ){
        int id = i;
        if(pthread_create(&l2_emp_threads[i],NULL, (void *) L2Employees, &id) ){
    	    perror("thread creation failed");
    	    exit(1);
        }
    }

        int j;
        // create the 10 techinical emplyees threads
    for(i=0;i < 5 ; i++){
        for(j=0;j<10;j++){
    	    Line_emp *threadParams = malloc(sizeof(threadParams));
             if(threadParams == NULL){
    	         perror("error in malloc");
    	         exit(1);
             }
        threadParams->line = i;
        threadParams->number=j;
             if(pthread_create(&line_emp[i][j],NULL, (void *) lineEmployee, (void *) (threadParams) )){
     	        perror("thread creation failed");
    	        exit(1);
            }
        }
    }

    //create the delevring trucks threads
    for(i=0; i<5;i++ ){
        int id = i;
        if(pthread_create(&storage2ToTrucks_emp_threads[i],NULL, (void *) deleviringTrucksfunc, &id) ){
     	    perror("thread creation failed");
     	    exit(1);
         }
    }
    // create the loading emplyees threads from storage area 2 to the trucks
    for(i=0; i<numberOfLoadingEmployees;i++ ){
        int id = i;
        if(pthread_create(&deleivering_trucks[i],NULL, (void *) fromStorageToTrucksEmp, &id) ){
    	     perror("thread creation failed");
             exit(1);
            }
    }
        // initiate the mutexes
  

    for (int i = 0; i < 5; i++) {
        if (pthread_mutex_init(&cartonLocks[i], NULL)) {
            perror("Initiate mutex");
            exit(1);
        }
    }
     for (int i = 0; i < 5; i++) {
        if (pthread_mutex_init(&deleviringTrucksLocks[i], NULL)) {
            perror("Initiate mutex");
            exit(1);
        }
    }
    
 


  
    
    
     // create the storage employees threads.
    
      for (int i = 0; i < numberOfStorageEmployees; i++) {
        int j = i;
        if (pthread_create(&storageEmps[i], NULL, (void *)storageEmpWork,
                           (void *)&j)) {
            perror("Create thread");
            exit(1);
        }
    }


      for (i = 0; i < numberOfLoadingEmployees; i++) {
          pthread_join(l1_emp_threads[i], NULL);
        }
            
    

    sleep(number_of_minutes*60);
    exitProgram(); //free resources

    return 0;
    }

    int enoughTruckB = 1;
    void supplierTruck (int *idp){ // supply the detergent factor of Material A and B
      while(1){
        if(*idp == 0){ // Material A
       	 pthread_mutex_lock(&supplier_truck_A);

       	 truckAsup = truck_capacity;
       	 #ifdef _D_
   		 printf(" %d  of mat A is  loaded \n",truckAsup);
       	 #endif
       	 pthread_mutex_unlock(&supplier_truck_A);
       	 }
        else if(enoughTruckB ==1){ //   If the ratio between material  A and B is still less than 2 Supply Material B 
       	 pthread_mutex_lock(&supplier_truck_B);
	
       	 truckBsup = truck_capacity;
       	 #ifdef _D_
   		 printf(" %d of mat b is loaded  \n",truckBsup);
         	 #endif
         	 pthread_mutex_unlock(&supplier_truck_B);
        }
        sleep(5);
      }
    }

    void LoadingEmp(int *idp){
        sleep(1);
        storage_s1.mat_A = 0;
    	storage_s1.mat_B = 0;
        while(1){
            while((storage_s1.mat_A+storage_s1.mat_B) == room_s1 ){// if storage room is full don't load any new material

            }
            while(truckAsup == 0 && truckBsup == 0 );
             pthread_mutex_lock(&supplier_truck_A);
             pthread_mutex_lock(&supplier_truck_B);

            if(truckAsup > 0){
              pthread_mutex_lock(&storage1_mutex);
              storage_s1.mat_A++;

            if(storage_s1.mat_B > (2*storage_s1.mat_A)){ //If the ratio between material  A and B is still less than 2  stop 	loading Material B 
             enoughTruckB = 0;
             }
            else{
             enoughTruckB = 1;
             }
             pthread_mutex_unlock(&storage1_mutex);
             truckAsup--;

            }
            if(truckBsup > 0){
                pthread_mutex_lock(&storage1_mutex);
                storage_s1.mat_B++;
            if(storage_s1.mat_B > (2*storage_s1.mat_A)){
                enoughTruckB = 0;
            }else{
                enoughTruckB = 1;
            }
            pthread_mutex_unlock(&storage1_mutex);
            truckBsup--;
            }
            pthread_mutex_unlock(&supplier_truck_A);
            pthread_mutex_unlock(&supplier_truck_B);
       #ifdef _D_
            printf("storage1:mat A is %d and mat b is %d and trucks: a is %d b is %d\n",storage_s1.mat_A,storage_s1.mat_B,truckAsup,truckBsup);
     #endif
            sleep(5);
            }


    }

    
    int lineCounter = 0;
    void L2Employees(int *idp){

    while(1){
        while(storage_s1.mat_A == 0 && storage_s1.mat_B == 0);
            pthread_mutex_lock(&storage1_mutex);

            pthread_mutex_lock(&linesLocksStart[lineCounter % 5]);// load the material A and B on the 5 lines
        if(storage_s1.mat_A > 0 && storage_s1.mat_B > 0){
            storage_s1.mat_A--;
            storage_s1.mat_B--;
            line[lineCounter % 5].mat_A += cartonBox_capacity;
            line[lineCounter % 5].mat_B += cartonBox_capacity;

        }else if(storage_s1.mat_A > 0 ){
            storage_s1.mat_A--;
            line[lineCounter % 5].mat_A += cartonBox_capacity;

        }else if(storage_s1.mat_B > 0){
            storage_s1.mat_B--;
            line[lineCounter % 5].mat_B += cartonBox_capacity;
        }
        printf("line %d has a %d and b %d\n",lineCounter%5,line[lineCounter % 5].mat_A,line[lineCounter % 5].mat_B);
        fflush(stdout);
        lineCounter++;


        pthread_mutex_unlock(&linesLocksStart[(lineCounter-1) % 5]);
        pthread_mutex_unlock(&storage1_mutex);
        sleep(5);
    	}
     }


void *lineEmployee(void *arg){


    // line employee from 1-10 will work in order.
   
    Line_emp param = *(( Line_emp *)arg);;
    int lineNumber = param.line;
    int empNumber = param.number;
    int prevEmpNumber = empNumber - 1;
    int detergentsNum = 0;

    while (1) {
        //sleep(1);
  
        if (empNumber == 0 && empLineCounter[lineNumber] ==0) { // first technical employee of the line
            
            pthread_mutex_lock(&linesLocksStart[lineNumber]);
            if (line[lineNumber].mat_A >=2 && line[lineNumber].mat_B >=1){// process 2 products from material A and 1 from 		material B
     	       line[lineNumber].mat_A-=2;
     	       line[lineNumber].mat_B-=1;
     	       empLineCounter[lineNumber]++; //increment thr number os steps in manufacturing the detergent
            }
           pthread_mutex_unlock(&linesLocksStart[lineNumber]);
           sleep(0.1);
            

        }
        if (empNumber==1 && empLineCounter[lineNumber] ==1) {
        
        empLineCounter[lineNumber]++;
        
            
        }else if(empNumber==2 && empLineCounter[lineNumber] ==2){
     		empLineCounter[lineNumber]++;
       
        }else if(empNumber==3 && empLineCounter[lineNumber] ==3){
        	empLineCounter[lineNumber]++;
       
        }else if(empNumber==4 && empLineCounter[lineNumber] ==4){
        	empLineCounter[lineNumber]++;
       
        }
        else if(empNumber==5 && empLineCounter[lineNumber] ==5){
        	empLineCounter[lineNumber]++;
       
        }
        else if(empNumber==6 && empLineCounter[lineNumber] ==6){
        	empLineCounter[lineNumber]++;
      
        }
        else if(empNumber==7 && empLineCounter[lineNumber] ==7){
        	empLineCounter[lineNumber]++;
       
        }
        else if(empNumber==8 && empLineCounter[lineNumber] ==8){
        	empLineCounter[lineNumber]++;
       
        }
        else if (empNumber == 9 && empLineCounter[lineNumber] ==9) { // last technical worker 
        
          
            
                    detergentsNum++; // a detergent product is finished 
                  
                    pthread_mutex_lock(&totalNumberOfDeteregnetsMutex);
                    totalNumberOfDeteregnets++; 
                    
                    printf("total number of Detergents is %d\n",totalNumberOfDeteregnets);
                    fflush(stdout);
                    if(totalNumberOfDeteregnets >= manufactured_detergent){
                    	exitProgram();
                    	exit(1);
                    }
                    pthread_mutex_unlock(&totalNumberOfDeteregnetsMutex);
                    if (detergentsNum == cartonBox_capacity) {// N number of products are ready to be in a carton
                        if (pthread_mutex_lock(&cartonLocks[lineNumber])) {
                            perror("Error");
                            exit(1);
                        }
                        cartonNum[lineNumber]++; // number of cartoons for this line increases 
                        printf("number of cortons in line %d is %d\n",lineNumber,cartonNum[lineNumber]);
                        fflush(stdout);
                        detergentsNum = 0;
                        if (pthread_mutex_unlock(&cartonLocks[lineNumber])) {
                            perror("Error");
                            exit(1);
                        }
                    }
                   
			        empLineCounter[lineNumber]=0;
                }
                 sleep(2);
            }


        
       
    
   
   }
   
   
   




    void *storageEmpWork(void *arg) {   //we have 5  that are responsible of moving carttons from lines to Storage room_2 
    // each storage employee will be responsible for one carton of detergents.
    int empId = *(int *)arg;
    while (1) {
        if (pthread_mutex_lock(&addToStorage)) {
            perror("Error");
            exit(1);
        }
        if (isAddToStorage && storageCount >= room_s2_max) {// if the space  in storage area 2 exceeds the maximum 
            isAddToStorage = 0;
            printf("Storage room is full !!\n\n");
            fflush(stdout);
        } else if (!isAddToStorage && storageCount < room_s2_min) {  // if the space  in storage area 2 goes under minimum  
            isAddToStorage = 1;
            printf("Storage goes down again !!\n\n");
            fflush(stdout);
        }
        if (isAddToStorage == 1) { // if there still exists space in  storage area 2
            if (pthread_mutex_lock(&cartonLocks[empId])) {
                perror("Error");
                exit(1);
            }
            if (cartonNum[empId] > 0) {
                if (pthread_mutex_lock(&storage2Lock)) {
                    perror("Error");
                    exit(1);
                }
                cartonNum[empId]--; // unfill the cartons
                storageCount++;
                printf("New Detergent Cartoon is added to the storage room from line %d\n",
                       empId);
                fflush(stdout);
                printf("Total number of Detergent Cartoons in storage room ==> %d\n\n",
                       storageCount);
                fflush(stdout);
                
                if (pthread_mutex_unlock(&storage2Lock)) {
                    perror("Error");
                    exit(1);
                }
                sleep((rand() % (time_storage_employee - 1)) + 1); // storage employee will be absent for random time
            }
            if (pthread_mutex_unlock(&cartonLocks[empId])) {
                perror("Error");
                exit(1);
            }
        }
        if (pthread_mutex_unlock(&addToStorage)) {
            perror("Error");
            exit(1);
        }
        sleep(0.1);
    }
}

  
    void fromStorageToTrucksEmp(int *idp){ // distribute the finished product 
    while(1){
    	sleep(1);
    	pthread_mutex_lock(&trucksAvailabeMutex);
	pthread_mutex_lock(&deleviringTrucksLocks[availableTrucks]);
    	pthread_mutex_lock(&storage2Lock);
    	while(availableTrucks > 0 && storageCount > 0 ){ // if theres available trucks and the storage area 2 isn't empty
    		storageCount--;
    		deleviringTrucks[availableTrucks] ++;
   		if(deleviringTrucks[availableTrucks] == truck_capacity){ // if the truck is full it leaves
    			availableTrucks--;
    			pthread_mutex_unlock(&trucksAvailabeMutex);
    			break;
    		}
    	}
    pthread_mutex_unlock(&deleviringTrucksLocks[availableTrucks+1]);
    pthread_mutex_unlock(&storage2Lock);

    }



    }


    void deleviringTrucksfunc (int *idp){
    sleep(3);
    while(1){
    	pthread_mutex_lock(&trucksAvailabeMutex);
    	pthread_mutex_lock(&deleviringTrucksLocks[availableTrucks]);
    	if(deleviringTrucks[availableTrucks] == truck_capacity){
    		pthread_mutex_lock(&totalNumberOfDeteregnetsMutex); 
    	if(totalNumberOfDeteregnets == manufactured_detergent){// if total number of produced products exceeds the threshold trucks 	  should stop leaving the factory and simulation ends
    //free  resources and exit
    		exitProgram();
    		exit(1);
    }
    pthread_mutex_unlock(&totalNumberOfDeteregnetsMutex); 
    pthread_mutex_unlock(&trucksAvailabeMutex);
     #ifdef _D_    
    printf("Truck %d is full and has left\n\n", (5-availableTrucks));
    fflush(stdout); 
    #endif 
    sleep(truck_absant);// the truck will be unavilable when it leaves the factory      
    pthread_mutex_lock(&trucksAvailabeMutex);   
    availableTrucks++;// the truck is back a the factory 
    deleviringTrucks[availableTrucks]=0;
    }
    pthread_mutex_unlock(&trucksAvailabeMutex);
    pthread_mutex_unlock(&deleviringTrucksLocks[availableTrucks]);
    
    sleep(1);
    }




    }


