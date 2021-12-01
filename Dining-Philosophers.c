/* 
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* gcc Dining-Philosophers.c -o Dining-Philosophers -lpthread  *
* (Control C(^C) to terminate program)                        *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
 
#define THINKING 2
#define HUNGRY 1
#define EATING 0
 
int condition[5];
int phil[5] = {0, 1, 2, 3, 4};
sem_t lock[5];
sem_t mutex;


// Check to see if neighbors are eating if selected philosopher is hungry
void check(int philnum) {
    if (condition[philnum] == HUNGRY
        && condition[(philnum + 1) % 5] != EATING // Right neighbor not eating
        && condition[(philnum + 4) % 5] != EATING) { // Left neighbor not eating

        // Selected philosopher is allowed to eat. Change state
        condition[philnum] = EATING;
 
        sleep((rand() % 3) + 1);
 
        printf("Philosopher %d is eating\n", philnum);
 
        // Semaphore used to wake up sleeping philosopher -- unlock
        sem_post(&lock[philnum]);
    }
}
 
// pick up chopsticks
void Pickup_Forks(int philnum) {
 
    // decrement semaphore
    sem_wait(&mutex);
 
    // change condition to hungry
    condition[philnum] = HUNGRY;
 
    // check to see if neighbors are eating
    check(philnum);
    sem_post(&mutex);
 
    // Unable to eat, wait until able to eat
    sem_wait(&lock[philnum]);
    sleep((rand() % 3) + 1); // Random number between 1 and 3
}
 
// put chopsticks down
void Return_Forks(int philnum) {
    sem_wait(&mutex);
 
    // change condition to thinking and announce to user
    condition[philnum] = THINKING;
    printf("Philosopher %d is thinking\n", philnum);
 
    // Check neighbors
    check((philnum + 4) % 5);
    check((philnum + 1) % 5);
 
    sem_post(&mutex);
}

void* philosopher(void* number) {
    while (HUNGRY) {
        int* j = number;
        sleep((rand() % 3) + 1); 
        Pickup_Forks(*j);
        sleep((rand() % 3) + 1);
        Return_Forks(*j);
    }
}
 
int main() {
    // Seed reset for randomized sleep timer
    srand(time(NULL));

    pthread_t tid[5];
 
    // initialize semaphores
    int i;
    sem_init(&mutex, 0, 1);
    for (i = 0; i < 5; i++) {
        sem_init(&lock[i], 0, 0);
    }
 
    for (i = 0; i < 5; i++) {
        // philosopher initialization
        pthread_create(&tid[i], NULL, philosopher, &phil[i]);
    }
    

    for (i = 0; i < 5; i++) {
        pthread_join(tid[i], NULL);
    }
}