    #ifndef __HEADER_H_
    #define __HEADER_H_

    #include <stdio.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <string.h>
    #include <pthread.h>
    #include <signal.h>
    #define numberOfLoadingEmployees 3
    #define numberOfStorageEmployees 5


    struct Storage_s1 {
    int mat_A;
    int mat_B;
    };
    struct Line {
    int mat_A;
    int mat_B;
    };
    typedef struct  {
    int line;
    int number;
    }Line_emp;


    typedef struct {
        int finishedSteps;
        int id;
    } Detergent;

    typedef struct Node Node;
    struct Node {
       Detergent *detergent;
        Node *next;
    };

    void exitProgram();//exiting the program
    void LoadingEmp(int*);//loading employee from Factory A & B to Storage Area 1
    void supplierTruck(int*);// Supplier Truck  of A and B materials
    void* lineEmployee(void*);// techinical emplyees working on raw materials
    void L2Employees(int*);//loading employess from storage area 1 to the  5 lines 
    void* storageEmpWork(void* arg);//storage emplyees moving materials from lines tom storage area 2
    void fromStorageToTrucksEmp(int* arg);//loading emplyees from s2 to trucks to distribute 
    void deleviringTrucksfunc(int* arg);//delevring trucks 

    pthread_t l1_emp_threads[numberOfLoadingEmployees];// loading employees from trucks to s1
    pthread_t truckA;//trucks A threads
    pthread_t truckB;//trucks B threads
    pthread_t l2_emp_threads[numberOfLoadingEmployees];// loading employees from s1 to lines
    pthread_t line_emp[5][10];
    pthread_t storageEmps[10];
    pthread_t storage2ToTrucks_emp_threads[numberOfLoadingEmployees];// loading employees from s2 to trucks
    pthread_t deleivering_trucks[5];

    pthread_mutex_t supplier_truck_A = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t supplier_truck_B = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t storage1_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t storage2Lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t addToStorage = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t linesLocksStart[5];
    pthread_mutex_t trucksAvailabeMutex;
    pthread_mutex_t totalNumberOfDeteregnetsMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t deleviringTrucksLocks[5];
    pthread_mutex_t cartonLocks[5];
   
	

    int detergentIDS = 0;
    int cartonNum[5] = {0, 0, 0, 0, 0};
    int deleviringTrucks [5] = {0,0,0,0,0}; 
    int empLineCounter[5] = {0, 0, 0, 0, 0};
    int truck_capacity=0;
    int room_s1 = 0;
	
    int cartonBox_capacity = 0;
    int time_storage_employee = 0;
    int manufactured_detergent = 0;
    int number_of_minutes = 0;
    int truck_absant = 0;

    int room_s2_min = 0;
    int room_s2_max = 0;
    int storageEmpMinDelay = 0;
    int storageEmpMaxDelay = 0;
    int storageCount = 0;
    int isAddToStorage = 1;


    FILE *fPtr;

    void exitProgram() { /* terminate the program. */
    // destroy all mutexes.
    
        for (int i = 0; i < 5; i++) {
            if (pthread_mutex_destroy(&cartonLocks[i])) {
                perror("Destroy mutex");
                exit(1);
            }
        }
        for (int i = 0; i < 5; i++) {
            if (pthread_mutex_destroy(&deleviringTrucksLocks[i])) {
                perror("Destroy mutex");
                exit(1);
            }
        }

        for (int i = 0; i < 5; i++) {
            if (pthread_mutex_destroy(&linesLocksStart[i])) {
                perror("Destroy mutex");
                exit(1);
            }
        }

    
      if (pthread_mutex_destroy(&supplier_truck_A) ||
         pthread_mutex_destroy(&supplier_truck_B) ||
         pthread_mutex_destroy(&storage1_mutex) ||
         pthread_mutex_destroy(&trucksAvailabeMutex) ||
         pthread_mutex_destroy(&totalNumberOfDeteregnetsMutex) || pthread_mutex_destroy(&storage2Lock)|| pthread_mutex_destroy(&addToStorage)) {
          perror("Destroy mutex");
          exit(1);
      } 
      exit(0);
    }



    int getValue (){
    char oneLine[100];
    if(fscanf(fPtr,"%s",oneLine)==EOF || fscanf(fPtr,"%s",oneLine)==EOF){
    perror("error reading from file");
    exit(1);
    }

    return atoi(oneLine);

    }

    void ReadValuesFromUserInputFile(){
    char fileName[] = "userInput.txt";
    fPtr = fopen(fileName,"r");
    if(fPtr == NULL){
    perror("file not found");
    exit(1);
    }

    truck_capacity=getValue();
    room_s1 = getValue();
    room_s2_min = getValue();
    room_s2_max = getValue();
    cartonBox_capacity = getValue();
    time_storage_employee = getValue();
    manufactured_detergent =getValue();
    number_of_minutes = getValue();
    truck_absant = getValue();

    if(fclose(fPtr)){
    perror("error closing file");
    exit(1);
    }

    }



#endif
