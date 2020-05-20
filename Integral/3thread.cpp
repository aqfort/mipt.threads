#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cmath>
#include <pthread.h>
#include <time.h>

#define NUMBER 30 //number of threads

using namespace std;

static double RESULT = 0; //result of integration

//integrate from A to B with aertain GRID
static double A = 0;
static double B = 0;
static long GRID = 0;

typedef struct ARG {
    int ID;
    double TIME;
    pthread_mutex_t *MUTEX;
} ARG;

void* fooo(void *arg); //function for each thread
double equation(const double &x); //function that will be integrated

int main(int argc, char **argv, char **env) {
    //check input argument
    if(argc != 1) {
        cout << "Invalid input: no argument is needed!\n";
        exit(1);
    }

    cout << "\n░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░";
    cout << "\n░░ Hello, this program calculates integral";
    cout << "\n░░ of certain function from A to B (A < B)";
    cout << "\n░░ Enter A = ";
    cin >> A;
    cout << "░░ Enter B = ";
    cin >> B;
    cout << "░░ Enter number of nodes (GRID > 1) = ";
    cin >> GRID;
    cout << "░░ Ok, wait, processing..." << endl;

    if((A >= B) || (GRID <= 1)) {
        cout << "Invalid numbers (input)\n";
        exit(1);
    }

    //timer
    struct timespec BEGIN, END; //-lrt key is needed -> time_t tv_sec and long tv_nsec
    double TIMER;

    clock_gettime(CLOCK_REALTIME, &BEGIN);

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

    //mutex to avoid collisions
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    //creating threads
    for(int i = 0; i < NUMBER; i++) {
        ARGUMENTS[i].ID = i; //ID of each thread
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
    cout << "░░ Integral equals to " << RESULT << "\n";
    cout << "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n";

    free(ID);
    free(thread);
    free(ARGUMENTS);

    return 0;
}

void* fooo(void *arg) {
    //timer
    struct timespec BEGIN, END; //-lrt key is needed -> time_t tv_sec and long tv_nsec
    double TIMER;

    clock_gettime(CLOCK_REALTIME, &BEGIN);

    ARG *ARGUMENTS = (ARG*) arg;

    double LOCAL_RESULT = 0;
    long LOCAL_RANGE = (GRID - 1) / NUMBER;

    double h = (B - A) / ((double) (GRID - 1)); //step of integration

    for(long i = 0; i < LOCAL_RANGE; i++) {
        LOCAL_RESULT += (equation(A + (LOCAL_RANGE * (ARGUMENTS->ID) + i) * h) +
                         equation(A + (LOCAL_RANGE * (ARGUMENTS->ID) + (i + 1)) * h)) * 0.5 * h;
    }

    if((ARGUMENTS->ID) == 0) {
        for(long i = 0; i + NUMBER * LOCAL_RANGE + 1 < GRID; i++) {
            LOCAL_RESULT += (equation(B - i * h) +
                             equation(B - (i + 1) * h)) * 0.5 * h;
        }
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

//any function f(x) can be used
double equation(const double &x) {
    return sin(2 * M_PI * x) + x;
}