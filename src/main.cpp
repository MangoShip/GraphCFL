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

using namespace std;

#define NUM_THREADS 4

// Global variable to indicate if a new variable has been added
bool newEdgeAdded = true;

// Argument to be passed to thread function
struct threadArg {
    int threadId;
    map<string, vector<pair<string, string>>> graphData;
    map<string, vector<pair<string, string>>> grammarData;
};

// Helper function for checking if edge exists in graphData
bool checkEdgeExists(map<string, vector<pair<string, string>>> graphData, string sourceVertex, string destVertex) {
    for(auto it = graphData.begin(); it != graphData.end(); it++) {
        for(int i = 0; i < (it->second).size(); i++) {
            if((it->second)[i].first == sourceVertex && (it->second)[i].second == destVertex) {
                return true;
            }
        }
    }
    return false;
}

// Thread function for traversing grammar rules
void* traverseGrammar(void *arg) {
    struct threadArg *threadArg;
    threadArg = (struct threadArg *) arg;

    // Retrieve variables from threadArg
    int threadId = threadArg->threadId;
    map<string, vector<pair<string, string>>> graphData = threadArg->graphData;
    map<string, vector<pair<string, string>>> grammarData = threadArg->grammarData;

    // Go through grammarData and apply each grammar to graphData
    /*while (newEdgeAdded) {
        newEdgeAdded = false;
        for(auto grammarIt = grammarData.begin(); grammarIt != grammarData.end(); grammarIt++) {
        string rightFirstSymbol = grammarIt->first;

            for(int i = 0; i < (grammarIt->second).size(); i++) {
                pair<string, string> grammarInfo =  (grammarIt->second)[i];
                string rightSecondSymbol = grammarInfo.first;
                string leftHandSymbol = grammarInfo.second;

                // Current grammar cannot be applied because one of the edge labels doesn't exist in graph
                if(graphData.find(rightFirstSymbol) == graphData.end() || graphData.find(rightSecondSymbol) == graphData.end()) {
                    continue;
                }

                vector<pair<string, string>> firstGraphInfo = graphData[rightFirstSymbol];
                vector<pair<string, string>> secondGraphInfo = graphData[rightSecondSymbol];

                for(int j = 0; j < firstGraphInfo.size(); j++) {
                    pair<string, string> firstVertexInfo = firstGraphInfo[j];
                    string firstSourceVertex = firstVertexInfo.first;
                    string firstDestVertex = firstVertexInfo.second;

                    for(int m = 0; m < secondGraphInfo.size(); m++) {
                        pair<string, string> secondVertexInfo = secondGraphInfo[m];
                        string secondSourceVertex = secondVertexInfo.first;
                        string secondDestVertex = secondVertexInfo.second;

                        if(firstDestVertex == secondSourceVertex) { // Current grammar rule can be applied.
                            // Check if an edge from firstSourceVertex to secondDestVertex already exists
                            if(!checkEdgeExists(graphData, firstSourceVertex, secondDestVertex)) {
                                pair<string, string> newVertexInfo = {firstSourceVertex, secondDestVertex};
                                graphData[leftHandSymbol].push_back(newVertexInfo);
                                newEdgeAdded = true;
                            }
                        }
                    }
                }
            }
        }
    }*/


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
    map<string, vector<pair<string, string>>> graphData;
    
    // Read through graph file then insert to graphData
    string inputLine;
    string inputWord;
    while(getline(graphFile, inputLine)) { // Get a line from file
        stringstream lineStream(inputLine);
        vector<string> lineWords;
        
        while(getline(lineStream, inputWord, ' ')) { // Transfer each word of line to vector
            // lineWords[0] = Source Vertex
            // lineWords[1] = Destination Vertex
            // lineWords[2] = Edge label
            lineWords.push_back(inputWord);
        }

        pair<string, string> vertexInfo = {lineWords[0], lineWords[1]};
        string edgeLabel = lineWords[2];

        // Remove '\n' from string
        if(edgeLabel.length() > 1) {
            edgeLabel = edgeLabel.substr(0, 1);
        } 
        graphData[edgeLabel].push_back(vertexInfo);
    }

    // Grammar Data that stores all grammar rules
    map<string, vector<pair<string, string>>> grammarData;

    // Read through grammar file then insert to grammarData
    while(getline(grammarFile, inputLine)) { // Get a line from file
        stringstream lineStream(inputLine);
        vector<string> lineWords;
        
        while(getline(lineStream, inputWord, ' ')) { // Transfer each word of line to vector
            // lineWords[0] = Left hand symbol
            // lineWords[1] = Right hand first symbol
            // lineWords[2] = Right hand second symbol
            lineWords.push_back(inputWord);
        }

        string rightFirstSymbol = lineWords[1];
        string rightSecondSymbol = lineWords[2];

        // Remove '\n' from string
        if(rightSecondSymbol.length() > 1) {
            rightSecondSymbol = rightSecondSymbol.substr(0, 1);
        } 

        pair<string, string> grammarInfo = {rightSecondSymbol, lineWords[0]};

        grammarData[rightFirstSymbol].push_back(grammarInfo);
    }

    // Create threads and attribute varaibles
    pthread_t threads[NUM_THREADS];
    pthread_attr_t threadAttr;

    int threadResult;
    void *status; // For debugging
    struct threadArg threadArgs[NUM_THREADS];

    // Initialize and set thread joinable
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);
    
    // Measure time for performance result
    auto startTime = chrono::system_clock::now(); 

    /* NOTES
    1. Easy to paralleize
    2. Avoid redundancy (Adding check mechanism)
    3. Locality (Accessing elements that are close to each other is faster than being separated away)
    */

    for (int i = 0; i < NUM_THREADS; i++) {
        // Initialize thread argument
        threadArgs[i].threadId = i;
        threadArgs[i].graphData = graphData;
        threadArgs[i].grammarData = grammarData;

        // Call thread function
        threadResult = pthread_create(&threads[i], &threadAttr, traverseGrammar, (void *)&threadArgs[i]);
        if (threadResult != 0) {
            cout << "Error in creating thread, " << threads[i] << endl;
            exit(-1);
        }
    }

    pthread_attr_destroy(&threadAttr);

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        threadResult = pthread_join(threads[i], &status);
        if (threadResult != 0) {
            cout << "Error in joining thread, " << threads[i] << endl;
            exit(-1);
        }
    }

    // Capture end time
    auto endTime = chrono::system_clock::now();
    chrono::duration<double> durationTime = endTime - startTime;

    // Temporary output (outputs graphData)
    ofstream outputFile;
    outputFile.open("../output/output");
    string outputLine;

    if(outputFile.is_open()) {
        outputFile << "Graph:\n";
        for(auto it = graphData.begin(); it != graphData.end(); ++it) {
            outputFile << it->first << ":\n";
            for(int i = 0; i < it->second.size(); i++) {
                pair<string, string> tempPair = (it->second)[i];
                outputFile << "  " << tempPair.first << "  " << tempPair.second << "\n";
            }
        }
    }

    outputFile << "\n";
    outputFile << "Elapsed time: " << durationTime.count() << "s";
    outputFile.close();
    
    return 0;
}
