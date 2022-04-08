// programming assignment #1
// Operating Systems
// Michael Angie

// the consumer

#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

// global incrementer
int i = 0;

// creating shared memory
int fileDescr = shm_open("/sharedMemory", O_RDWR, S_IRWXU);

// creating table (again? this should be in a header file)
struct table
{
    sem_t semPro;
    sem_t semCon;
    bool objects[2];
};

// Map the memory object
table *shmptr = (table *) mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, fileDescr, 0);

//pthread variable
pthread_t consumer;


void* consume(void *arg)
{
    while (i < 5)
    {   
        // wait for producer to produce
        sem_post(&shmptr->semPro);
        
        shmptr->objects[0] = false;
        std::cout << "I consumed one item, wait 2 seconds..." << std::endl;
        sleep(2);

        shmptr->objects[1] = false;
        std::cout << "I consumed the second item. Semaphore post() in 3..\n";
        sleep(1);
        std::cout << "2..\n";
        sleep(1);
        std::cout << "1..\n";
        sleep(1);
    
        ++i;
        std::cout << "Consumer ran  " << i << "/5 times." << std::endl;
        sem_post(&shmptr->semCon);
    }
    
    return NULL;
}


int main()
{
    //check for an error in the file descriptor
    if (fileDescr == -1)
    {
        std::cerr << "shm_open error" << std::endl;
        std::cout<< errno << std::endl;
        return 0;
    }

    // check for error in mapping
    if (shmptr == MAP_FAILED)
        std::cerr << "error when mapping shared memort to ptr" << std::endl;

    //put consumer on a thread
    pthread_create(&consumer, NULL, consume, NULL);
  
    //prod and consu go 5 times then quit
    while(i < 5)
        sleep(1);

    //exit cleanup
    sem_close(&shmptr->semCon);
    shm_unlink("/sharedMemory");
    return 0;
}

