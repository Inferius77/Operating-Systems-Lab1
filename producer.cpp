// programming assignment #1
// Operating Systems
// Michael Angie

// the producer

#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>

// global incrementer
int i = 0;

// creating shared memory
int fileDescr = shm_open("/sharedMemory", O_CREAT | O_RDWR, S_IRWXU);

// creating table
struct table
{
    sem_t semPro;
    sem_t semCon;
    bool objects[2];
};

// set memory object to proper size
int errcheck = ftruncate(fileDescr, sizeof(table));

// Map the memory object
table *shmptr = (table *) mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, fileDescr, 0);

// pthread variable
pthread_t producer;


void* produce(void *arg)
{
    while (i < 5)
    {
        //wait for consumer to consume 
        sem_wait(&shmptr->semCon);

        shmptr->objects[0] = true;
        std::cout << "I placed one item, wait 2 seconds..." << std::endl;
        sleep(2);

        shmptr->objects[1] = true;
        std::cout << "I placed the second item. Semaphore post() in 3..\n";
        sleep(1);
        std::cout << "2..\n";
        sleep(1);
        std::cout << "1..\n";
        sleep(1);

        ++i;
        std::cout << "Producer ran  " << i << "/5 times." << std::endl;
        sem_post(&shmptr->semPro);
    }

    return NULL;
}


int main()
{
    //check for an error in the file descriptor
    if (fileDescr == -1)
    {
        std::cerr << "shm_open error" << std::endl;
        return 0;
    }

    //check for an error in the truncation
    if (errcheck == -1)
    {
        std::cerr << "ftruncator error" << std::endl;
        return 0;
    }

    // initalize the objects on the table to exist
    shmptr->objects[0] = true;
    shmptr->objects[1] = true;

    // initialize semas to 0
    sem_init(&shmptr->semPro, 1, 0);
    sem_init(&shmptr->semCon, 1, 0);

    //put producer on a thread
    pthread_create(&producer, NULL, produce, NULL);
  
    //prod and consu go 5 times then quit
    while(i < 5)
        sleep(1);

    sem_close(&shmptr->semPro);
    shm_unlink("/sharedMemory");
    return 0;
}
