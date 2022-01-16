#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <chrono>
#include <pthread.h>
#include "Barrier.h"

using namespace std;

#define NUM_THREADS 2

//barrier_object B;

// Global variable to indicate if a new variable has been added
bool newEdgeAdded = true;

// Global variable to keep tracking how many edges have been added
int numTotalEdgesAdded = 0;

// Variable that stores information of graph edge
struct graphEdge {
    string edgeLabel;
    int sourceVertex;
    int destVertex;
};

// Argument to be passed to traverse thread function
struct traverseThreadArg {
    int threadId;
    vector<graphEdge> *graphData;
    vector<graphEdge> *firstSymbolEdge;
    vector<graphEdge> *secondSymbolEdge;
    string currGrammarRule;
}; 

// Argument to be passed to create thread function
struct createThreadArg {
    int threadId;
    int numEdgesAdded;
    vector<graphEdge> *graphData;
    vector<graphEdge> *newGraphData;
    vector<graphEdge> *firstSymbolEdge;
    vector<graphEdge> *secondSymbolEdge;
    string currGrammarRule;
};

// Helper function for checking if edge exists in graphData
bool checkEdgeExists(vector<graphEdge> graphData, graphEdge newEdge) {
    for (graphEdge edge : graphData) {
        if (edge.edgeLabel == newEdge.edgeLabel &&
            edge.sourceVertex == newEdge.sourceVertex &&
            edge.destVertex == newEdge.destVertex) {
                return true;
            }
    }
    return false;
}

// Thread function for traversing graph
void* traverseGraph(void *arg) {
    struct traverseThreadArg *threadArg;
    threadArg = (struct traverseThreadArg *) arg;

    // Retrieve variables from threadArg
    int threadId = threadArg->threadId;
    vector<graphEdge> *graphData = threadArg->graphData;
    vector<graphEdge> *firstSymbolEdge = threadArg->firstSymbolEdge;
    vector<graphEdge> *secondSymbolEdge = threadArg->secondSymbolEdge;
    string currGrammarRule = threadArg->currGrammarRule;

    // Variables for assigning chunk of graphData for each thread
    int chunkSize = ((*graphData).size() + (NUM_THREADS - 1)) / NUM_THREADS;
    int startIndex = chunkSize * threadId;
    int endIndex = min(startIndex + chunkSize, (int)(*graphData).size());

    // NUM_THREADS is greater than size of graphData
    // Current thread won't be used
    if (startIndex >= endIndex) { 
        cout << "TRAVERSE THREAD" << threadId << ": " << startIndex << " " << endIndex << endl; 
        pthread_exit(NULL);
    }

    for (int i = startIndex; i < endIndex; i++) {

        string edgeLabel = (*graphData)[i].edgeLabel;
        string leftHandSymbol = currGrammarRule.substr(0, 1);
        string rightHandFirstSymbol = currGrammarRule.substr(1, 1);
        string rightHandSecondSymbol = currGrammarRule.substr(2, 1);

        if(edgeLabel == rightHandFirstSymbol) {
            (*firstSymbolEdge).push_back((*graphData)[i]);
        }
        if(edgeLabel == rightHandSecondSymbol) {
            (*secondSymbolEdge).push_back((*graphData)[i]);
        }

    }

    // This thread has finished traversing through its chunk
    // Now wait for other threads to finish (Barrier)
    //B.barrier(threadId);

    pthread_exit(NULL);
}

// Thread function for creating a new edge
void* createNewEdge (void *arg) {
    struct createThreadArg *threadArg;
    threadArg = (struct createThreadArg *) arg;

    // Retrieve variables from threadArg
    int threadId = threadArg->threadId;
    vector<graphEdge> *graphData = threadArg->graphData;
    vector<graphEdge> *newGraphData = threadArg->newGraphData;
    vector<graphEdge> *firstSymbolEdge = threadArg->firstSymbolEdge;
    vector<graphEdge> *secondSymbolEdge = threadArg->secondSymbolEdge;
    string currGrammarRule = threadArg->currGrammarRule;

    // Variables for assigning chunk of graphData for each thread
    int chunkSize = ((*firstSymbolEdge).size() + (NUM_THREADS - 1)) / NUM_THREADS;
    int startIndex = chunkSize * threadId;
    int endIndex = min(startIndex + chunkSize, (int)(*firstSymbolEdge).size());

    // NUM_THREADS is greater than size of graphData
    // Current thread won't be used
    if (startIndex >= endIndex) { 
        cout << "CREATE THREAD" << threadId << ": " << startIndex << " " << endIndex << endl; 
        pthread_exit(NULL);
    }

    //cout << "Thread " << threadId << ": " << startIndex << " " << endIndex << endl;

    for (int i = startIndex; i < endIndex; i++) {
        for (int j = 0; j < (*secondSymbolEdge).size(); j++) {
            if ((*firstSymbolEdge)[i].destVertex == (*secondSymbolEdge)[j].sourceVertex) {
                graphEdge newEdge;
                newEdge.edgeLabel = currGrammarRule.substr(0, 1);
                newEdge.sourceVertex = (*firstSymbolEdge)[i].sourceVertex;
                newEdge.destVertex = (*secondSymbolEdge)[j].destVertex;

                if(!checkEdgeExists((*graphData), newEdge)) {
                    (*newGraphData).push_back(newEdge);
                    newEdgeAdded = true;
                    threadArg->numEdgesAdded++;
                };
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    // Check initial argument 
    if (argc != 3) {
        cout << "ERROR: Invalid Input Command" << endl;
        cout << "Run with following format: ./main [graph_file] [grammar_file]" << endl;
        return 1;
    }

    // Check input graph file exists
    ifstream graphFile;
    string graphName = argv[1];
    graphFile.open("../graph/" + graphName);
    if(!graphFile.is_open()) {
        cout << "ERROR: Invalid Input Graph" << endl;
        cout << "The file " << graphName << " does not exist in graph folder." << endl;
        return 1;
    }

    // Check input grammar file exists
    ifstream grammarFile;
    string grammarName = argv[2];
    grammarFile.open("../grammar/" + grammarName);
    if(!grammarFile.is_open()) {
        cout << "ERROR: Invalid Input Grammar" << endl;
        cout << "The file " << grammarName << " does not exist in grammar folder." << endl;
        return 1;
    }

    // Graph Data that stores all vertices and edges
    vector<graphEdge> graphData;
    
    // Read through graph file then insert to graphData
    string inputLine;
    string inputWord;
    while(getline(graphFile, inputLine)) { // Get a line from file
        if (inputLine.length() == 0) {
            continue;
        }    
        stringstream lineStream(inputLine);
        vector<string> lineWords;

        //cout << lineStream.str() << endl;
        
        while(getline(lineStream, inputWord, '\t')) { // Transfer each word of line to vector
            // lineWords[0] = Source Vertex
            // lineWords[1] = Destination Vertex
            // lineWords[2] = Edge label
            lineWords.push_back(inputWord);
        }

        graphEdge newGraphEdge;
        newGraphEdge.sourceVertex = stoi(lineWords[0]);
        newGraphEdge.destVertex = stoi(lineWords[1]);

        string edgeLabel = lineWords[2];
        // Remove '\n' from string
        if(edgeLabel.length() > 1) {
            edgeLabel = edgeLabel.substr(0, 1);
        } 
        newGraphEdge.edgeLabel = edgeLabel;

        graphData.push_back(newGraphEdge);
    }

    // Grammar Data that stores all grammar rules
    vector<string> grammarData;

    // Read through grammar file then insert to grammarData
    while(getline(grammarFile, inputLine)) { // Get a line from file
        if (inputLine.length() == 0) {
            continue;
        }
        stringstream lineStream(inputLine);
        string lineWords = "";
        
        while(getline(lineStream, inputWord, '\t')) { // Transfer each word of line to vector
            // lineWords[0] = Left hand symbol
            // lineWords[1] = Right hand first symbol
            // lineWords[2] = Right hand second symbol
            if (inputWord.length() > 1) {
                inputWord = inputWord.substr(0, 1);
            }
            lineWords += inputWord;
        }

        grammarData.push_back(lineWords);
    }

    // Create threads and attribute varaibles
    pthread_t threads[NUM_THREADS];
    pthread_attr_t threadAttr;

    int threadResult;
    void *status; // For debugging
    struct traverseThreadArg traverseThreadArgs[NUM_THREADS];
    struct createThreadArg createThreadArgs[NUM_THREADS];

    // Initialize and set thread joinable
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);

    // Initialize barrier object
    //B.init(NUM_THREADS);
    
    // Measure time for performance result
    auto startTime = chrono::system_clock::now(); 

    /* NOTES
    1. Easy to paralleize
    2. Avoid redundancy (Adding check mechanism)
    3. Locality (Accessing elements that are close to each other is faster than being separated away)
    */
    int loopCount = 0;
    
    while (newEdgeAdded) {
        cout << "Loop #: " << loopCount  << endl;
        cout << "New Edges Added: " << numTotalEdgesAdded << endl;
        newEdgeAdded = false;

        for(auto grammarIt : grammarData) {
            vector<graphEdge> newGraphData = graphData;
            vector<graphEdge> firstSymbolEdges;
            vector<graphEdge> secondSymbolEdges;

            // STEP 1: Traverse through graph and mark if edge satisfies grammar rule
            for (int i = 0; i < NUM_THREADS; i++) {
                // Initialize thread argument
                traverseThreadArgs[i].threadId = i;
                traverseThreadArgs[i].graphData = &graphData;
                traverseThreadArgs[i].firstSymbolEdge = &firstSymbolEdges;
                traverseThreadArgs[i].secondSymbolEdge = &secondSymbolEdges;
                traverseThreadArgs[i].currGrammarRule = grammarIt;

                // Call thread function
                threadResult = pthread_create(&threads[i], &threadAttr, traverseGraph, &traverseThreadArgs[i]);
                if (threadResult != 0) {
                    cout << "Error in creating thread, " << threads[i] << endl;
                    exit(-1);
                }
            }
            // Thread function has been completed
            pthread_attr_destroy(&threadAttr);

            // Join threads from traversing
            for (int i = 0; i < NUM_THREADS; i++) {
                threadResult = pthread_join(threads[i], &status);
                if (threadResult != 0) {
                    cout << "Error in joining thread, " << threads[i] << endl;
                    exit(-1);
                }
            }

            // STEP 2: Go through first and second symbol edge arrays, then create a new edge 
            for (int i = 0; i < NUM_THREADS; i++) {
                // Initialize thread argument
                createThreadArgs[i].threadId = i;
                createThreadArgs[i].numEdgesAdded = 0;
                createThreadArgs[i].graphData = &graphData;
                createThreadArgs[i].newGraphData = &newGraphData;
                createThreadArgs[i].firstSymbolEdge = &firstSymbolEdges;
                createThreadArgs[i].secondSymbolEdge = &secondSymbolEdges;
                createThreadArgs[i].currGrammarRule = grammarIt;

                // Call thread function
                threadResult = pthread_create(&threads[i], &threadAttr, createNewEdge, &createThreadArgs[i]);
                if (threadResult != 0) {
                    cout << "Error in creating thread, " << threads[i] << endl;
                    exit(-1);
                }
            }
            // Thread function has been completed
            pthread_attr_destroy(&threadAttr);

            // Join threads from creating
            for (int i = 0; i < NUM_THREADS; i++) {
                threadResult = pthread_join(threads[i], &status);
                if (threadResult != 0) {
                    cout << "Error in joining thread, " << threads[i] << endl;
                    exit(-1);
                }
            }

            for(int i = 0; i < NUM_THREADS; i++) {
                numTotalEdgesAdded += createThreadArgs[i].numEdgesAdded;
            }

            graphData = newGraphData;
        }
        loopCount++;
    }

    // Capture end time
    auto endTime = chrono::system_clock::now();
    chrono::duration<double> durationTime = endTime - startTime;

    // Temporary output (outputs graphData)
    ofstream outputFile;
    outputFile.open("../output/output");
    string outputLine;

    outputFile << "Number of Edge Added: " << numTotalEdgesAdded << "\n";
    outputFile << "Elapsed time: " << durationTime.count() << "s";
    outputFile << "\n";
    
    if(outputFile.is_open()) {
        outputFile << "Graph:\n";
        for(graphEdge it : graphData) {
            outputFile << it.sourceVertex << " " << it.destVertex << " " << it.edgeLabel << "\n";
        }
    }

    outputFile.close();
    
    return 0;
}
