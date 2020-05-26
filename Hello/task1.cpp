#include <iostream>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <pthread.h>

using namespace std;

void * fooo(void *arg);

int main(int argc, char **argv, char **env) {
    //check input argument
    if(argc != 2) {
        cout << "Give me number of threads!\n";
        exit(1);
    }

    for(int i = 0; i * sizeof(char) < strlen(argv[1]); i++) {
        if(!isdigit(argv[1][i])) {
            cout << "Give me NUMBER of threads!\n";
            exit(1);
        }
    }

    int N = atoi(argv[1]);
    int *arg = new int[N];

    for(int i = 0; i < N; i++) {
        arg[i] = i;
    }

    //pointers to threads
    pthread_t *thread = new pthread_t[N];

    //initialize and set the thread attributes
    //they could be not initialized
    //in pthread_create (&attr) -> (NULL)
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    cout << "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n";

    //creating threads
    for(int i = 0; i < N; i++) {
        int temp = pthread_create(&thread[i], &attr, fooo, (void *) &arg[i]);

        if(temp != 0) {
            cout << "Creating thread " << i << " failed!" << endl;
            return 1;
        }
    }

    pthread_attr_destroy(&attr);

    //joining threads
    for(int i = 0; i < N; i++) {
        int temp = pthread_join(thread[i], NULL);

        if(temp != 0) {
            cout << "Joining thread " << i << " failed" << endl;
            return 1;
        }
    }

    cout << "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n";

    delete[] arg;
    delete[] thread;

    return 0;
}

void * fooo(void *arg) {
    int ID;

    ID = *((int *) arg);

    printf("░░ Hello from thread %02d!\n", ID);

    //string line = "hello from thread " + to_string(ID) + '\n'; //can't see to_string
    //cout << line;

    //cout << "hello from thread " << ID << endl; //random order like word1 + word2 + number1 + number2

    pthread_exit(NULL);
}




