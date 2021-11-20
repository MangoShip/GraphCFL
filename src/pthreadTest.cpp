#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>

using namespace std;

#define NUM_THREADS 2

struct threadArg {
    int threadId;
    vector<int> *testData;
};

void* threadFunction(void *arg) {
    struct threadArg *threadArg;
    threadArg = (struct threadArg *) arg;

    // Retrieve variables from threadArg
    
    int threadId = threadArg->threadId;
    //vector<int> testData = *threadArg->testData;
    vector<int>* testData = threadArg->testData;

    (*testData).push_back(threadId);

    /*cout << "IN THREAD: " << endl;
    for(int i = 0; i < (*testData).size(); i++) {
        cout << testData[i] << endl;
    }*/

    pthread_exit(NULL);
}

int main() {

    vector<int> testData;

    pthread_t threads[NUM_THREADS];
    pthread_attr_t threadAttr;

    int threadResult;
    void *status; // For debugging
    struct threadArg threadArgs[NUM_THREADS];

    // Initialize and set thread joinable
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < NUM_THREADS; i++) {
        // Initialize thread argument

        threadArgs[i].threadId = i;
        threadArgs[i].testData = &testData;

        // Call thread function
        threadResult = pthread_create(&threads[i], &threadAttr, threadFunction, &threadArgs[i]);
        if (threadResult != 0) {
            cout << "Error in creating thread, " << threads[i] << endl;
            exit(-1);
        }
    }

    // Thread function has been completed
    pthread_attr_destroy(&threadAttr);

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        threadResult = pthread_join(threads[i], &status);
        if (threadResult != 0) {
            cout << "Error in joining thread, " << threads[i] << endl;
            exit(-1);
        }
    }

    cout << "AFTER JOINING: " << endl;

    //vector<int> result = testData;
    for(int i = 0; i < testData.size(); i++) {
        cout << testData[i] << endl;
    }

    return 0;
};