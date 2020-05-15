#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <pthread.h>
#include <time.h>

using namespace std;

static long RESULT = 0;

typedef struct ARG {
    int ID;
    long PART;
    double TIME;
    pthread_mutex_t *MUTEX;
} ARG;

void* fooo(void *arg);

int main(int argc, char **argv, char **env) {
    //timer
    struct timespec BEGIN, END; //-lrt key is needed -> time_t tv_sec and long tv_nsec
    double TIMER;

    clock_gettime(CLOCK_REALTIME, &BEGIN);

    //check input argument
    if(argc != 3) {
        cout << "Invalid input!\n";
        exit(1);
    }

    for(int i = 0; i * sizeof(char) < strlen(argv[1]); i++) {
        if(!isdigit(argv[1][i])) {
            cout << "Invalid input: number of threads is needed!\n";
            exit(1);
        }
    }

    for(int i = 0; i * sizeof(char) < strlen(argv[2]); i++) {
        if(!isdigit(argv[2][i])) {
            cout << "Invalid input: number of elements is needed!\n";
            exit(1);
        }
    }

    int NUMBER = atoi(argv[1]);
    long N = atol(argv[2]);

    //ID = [0, 1, 2, ..., NUMBER - 1]
    int *ID = (int*) malloc(sizeof(int) * NUMBER);
    for(int i = 0; i < NUMBER; i++) {
        ID[i] = i;
    }

    pthread_t *thread = (pthread_t*) malloc(sizeof(pthread_t) * NUMBER);
    pthread_attr_t attr;

    //initialize and set the thread attributes
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    ARG *ARGUMENTS = (ARG*) malloc(sizeof(ARG) * NUMBER); //arguments which sent to each thread

    for(long i = N - (N % NUMBER) + 1; i <= N; i++) { //sum of the rest of members
        RESULT += i;
    }

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    //creating threads
    for(int i = 0; i < NUMBER; i++) {
        ARGUMENTS[i].ID = i; //ID of each thread
        ARGUMENTS[i].PART = N / NUMBER; //number of members for each thread to sum
        ARGUMENTS[i].MUTEX = &mutex; //mutex which will help to avoid collisions
        int temp = pthread_create(&thread[i], &attr, fooo, (void*) &ARGUMENTS[i]);

        if(temp != 0) {
            cout << "Creating thread " << i << " failed!" << endl;
            return 1;
        }
    }

    //joining threads
    for(int i = 0; i < NUMBER; i++) {
        int temp = pthread_join(thread[i], NULL);

        if(temp != 0) {
            cout << "Joining thread " << i << " failed" << endl;
            return 1;
        }
    }

    pthread_mutex_destroy(&mutex);

    clock_gettime(CLOCK_REALTIME, &END);

    TIMER = END.tv_sec - BEGIN.tv_sec; //seconds
    TIMER += (END.tv_nsec - BEGIN.tv_nsec) / 1000000000.0; //nanoseconds

    cout << "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n";
    for(int i = 0; i < NUMBER; i++) {
        cout << "░░ Time spent in thread #" << setw(2) << setfill('0') << i << " equals to " << fixed << setprecision(10) << ARGUMENTS[i].TIME << " seconds\n";
    }
    cout << "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n";

    cout << "░░ Total spent time = " << fixed << setprecision(10) << TIMER << " seconds\n";

    cout << "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n";
    cout << "░░ Sum of " << N << " numbers equals to " << RESULT << "\n";
    cout << "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n";

    free(ID);
    free(thread);

    return 0;
}

void* fooo(void *arg) {
    //timer
    struct timespec BEGIN, END; //-lrt key is needed -> time_t tv_sec and long tv_nsec
    double TIMER;

    clock_gettime(CLOCK_REALTIME, &BEGIN);

    ARG *ARGUMENTS = (ARG*) arg;

    long LOCAL_RESULT = 0;

    for(long i = ARGUMENTS->ID * ARGUMENTS->PART + 1; i <= (ARGUMENTS->ID + 1) * ARGUMENTS->PART; i++) {
        LOCAL_RESULT += i;
    }

    pthread_mutex_t mutex = (pthread_mutex_t) *(ARGUMENTS->MUTEX);

    pthread_mutex_lock(&mutex); //collisions avoidance

    RESULT += LOCAL_RESULT;

    pthread_mutex_unlock(&mutex); //collisions avoidance

    clock_gettime(CLOCK_REALTIME, &END);

    TIMER = END.tv_sec - BEGIN.tv_sec; //seconds
    TIMER += (END.tv_nsec - BEGIN.tv_nsec) / 1000000000.0; //nanoseconds

    ARGUMENTS->TIME = TIMER;

    pthread_exit(NULL);
}
