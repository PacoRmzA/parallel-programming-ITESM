//Francisco José Ramírez Aldaco A01634262
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#define NUM_THREADS 16
#define NUM_POINTS 1000000

int thread_counts[NUM_THREADS];

void* estimatePi(void* threadid) {
    int count = 0;
    double x, y;
    double rmax = RAND_MAX;
    for (int i=0; i<NUM_POINTS/NUM_THREADS; i++) {
        x = rand() / rmax;
        y = rand() / rmax;
        if (x*x + y*y <= 1) count++;
    }
    thread_counts[(long)threadid] = count;
    pthread_exit(NULL);
}

int main() {
    clock_t start, end;
    pthread_t threads[NUM_THREADS];
    int rc;
    int totalCount = 0;
    double totalTime, piEstimate;
    srand(time(NULL));
    start = clock();
    for(long t=0;t<NUM_THREADS;t++){
        rc = pthread_create(&threads[t], NULL, estimatePi, (void *)t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        pthread_join(threads[t], NULL);
        totalCount += thread_counts[t];
    }
    piEstimate = 4.0*totalCount/NUM_POINTS;
    end = clock();
    totalTime = ((double) (end-start)) / CLOCKS_PER_SEC;
    printf("Estimation with %d threads done. Took %f seconds. Result: %f\n", NUM_THREADS, totalTime, piEstimate);
    return 0;
}