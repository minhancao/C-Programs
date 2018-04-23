#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* the maximum time (in seconds) to sleep */
#define MAX_SLEEP_TIME 3
/* number of potential students */
#define NUM_OF_STUDENTS 4
#define NUM_OF_HELPS 2
/* number of available seats */
#define NUM_OF_SEATS 2

#define TRUE 1
#define FALSE 0

/* mutex declarations, a global variable */
pthread_mutex_t mutex_lock; /* protect the global variable waiting_student */

/* semaphore declarations, global variables */
sem_t students_sem; /* ta waits for a student to show up, student notifies ta his/her arrival*/
sem_t ta_sem;/* student waits for ta to help, ta notifies student he/she is ready to help*/

/* the number of waiting students, a global variable */
int waiting_students;

unsigned int seeds[6];

void *taFunc() {
	sem_wait(&ta_sem);
	while (TRUE) {
		while (waiting_students == 0) {
			sem_wait(&students_sem); //ta waits for a student to show up
		}

		//critical section
		pthread_mutex_lock(&mutex_lock);
		waiting_students--;
		pthread_mutex_unlock(&mutex_lock);
		seeds[5] = time(NULL);
		int progTime = (rand_r(&seeds[5]) % MAX_SLEEP_TIME) + 1;
		printf("\nHelping a student for %d seconds. # of waiting students = %d",
				progTime, waiting_students);
		sem_post(&ta_sem); //ta notifies student that he or she is ready to help
		sleep(progTime);

		if (waiting_students == 0) {
			seeds[4] = time(NULL);
			int progTime = (rand_r(&seeds[4]) % MAX_SLEEP_TIME) + 1;
			sleep(progTime);
		}

	}
}

void *studFunc(int i) {
	int numHelp = 0;
	while (numHelp < NUM_OF_HELPS) {
		seeds[i] = time(NULL) + i;
		int progTime = (rand_r(&seeds[i]) % MAX_SLEEP_TIME) + 1;
		printf("\n\tStudent %d programming for %d seconds", i, progTime);
		sleep(progTime);

		if (waiting_students >= NUM_OF_SEATS) {
			printf("\nStudent %d will try again later", i);
		}

		else {
			while (waiting_students >= NUM_OF_SEATS) {
				sem_wait(&students_sem);
			}

			if (waiting_students < NUM_OF_SEATS) {
				pthread_mutex_lock(&mutex_lock);
				waiting_students++;
				pthread_mutex_unlock(&mutex_lock);

				printf(
						"\n\t\tStudent %d takes a seat. # of waiting students %d",
						i, waiting_students);
				sem_post(&students_sem);

			}
			//signal ta the students arrival

			//wait for a ta to help the student

			sem_wait(&ta_sem); //student waits for ta to be able to help

			//critical section
			numHelp++;
			printf("\nStudent %d receiving help, # of times helped: %d", i,
					numHelp);
		}
	}
	pthread_exit(3);
}

int main() {
	printf("SleepingTA C Program\n");

	//mutex and semaphores initialization
	pthread_mutex_init(&mutex_lock, NULL);

	sem_init(&students_sem, 0, NUM_OF_SEATS); //two seats available
	sem_init(&ta_sem, 0, 1); //ta can only help 1 student at a time

	//pthreads creations
	pthread_t taThread;
	pthread_create(&taThread, NULL, taFunc, NULL);

	pthread_t studArray[NUM_OF_STUDENTS];

	for (int i = 0; i < NUM_OF_STUDENTS; i++) //student threads creation
			{
		pthread_create(&studArray[i], NULL, studFunc, i);
	}

	for (int j = 0; j < NUM_OF_STUDENTS; j++) //student threads creation
			{
		pthread_join(studArray[j], NULL); //wait for all students to get helped and finished
	}

	pthread_cancel(taThread); //cancels the TA thread when all students are helped and finished
	printf("\n"); //for output purposes
	sem_destroy(&ta_sem);
	sem_destroy(&students_sem);
	pthread_mutex_destroy(&mutex_lock);
	return 0;
}
