#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <unordered_map>
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
struct GraphEdge {
    string edgeLabel;
    int sourceVertexLabel;
    int destVertexLabel;
};

// Variable that stores information of graph vertex
struct GraphVertex {
    vector<GraphEdge> graphEdges;
    int vertexLabel;
};

// Argument to be passed to traverse thread function
/*struct traverseThreadArg {
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
};*/

// Helper function for checking if edge exists in graphData
bool checkEdgeExists(GraphVertex currGraphVertex, GraphEdge newEdge) {
    for (GraphEdge graphEdge : currGraphVertex.graphEdges) {
        if (graphEdge.edgeLabel == newEdge.edgeLabel &&
            graphEdge.sourceVertexLabel == newEdge.sourceVertexLabel &&
            graphEdge.destVertexLabel == newEdge.destVertexLabel) {
                return true;
            }
    }
    return false;
}

// Thread function for traversing graph
/*void* traverseGraph(void *arg) {
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

    // Variables for assigning chunk of firstSymbolEdge for each thread
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
}*/

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
    unordered_map<int, GraphVertex> graphData;

    // Variable for holding original graphData 
    int origNumVertex = 0;
    int origNumEdge = 0;
    
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

        int vertexLabel = stoi(lineWords[0]);
        auto graphIt = graphData.find(vertexLabel);
        
        // Check if vertex already exists in graphData
        if (graphIt == graphData.end()) { // not found
            GraphVertex newGraphVertex;
            newGraphVertex.vertexLabel = vertexLabel;
            graphData[vertexLabel] = newGraphVertex;
            graphIt = graphData.find(vertexLabel);
            origNumVertex++;
        }

        GraphEdge newGraphEdge;
        newGraphEdge.sourceVertexLabel = vertexLabel;
        newGraphEdge.destVertexLabel = stoi(lineWords[1]);
        
        string edgeLabel = lineWords[2];
        // Remove '\n' from string
        if(edgeLabel.length() > 1) {
            edgeLabel = edgeLabel.substr(0, 1);
        } 
        newGraphEdge.edgeLabel = edgeLabel;

        graphIt->second.graphEdges.push_back(newGraphEdge);
        origNumEdge++;
    }

    // Grammar Data that stores all grammar rules
    //vector<string> grammarData;
    unordered_map<string, unordered_map<string, string>> grammarData;

    // Read through grammar file then insert to grammarData
    while(getline(grammarFile, inputLine)) { // Get a line from file
        if (inputLine.length() == 0) {
            continue;
        }
        stringstream lineStream(inputLine);
        vector<string> lineWords;
        
        while(getline(lineStream, inputWord, '\t')) { // Transfer each word of line to vector
            // lineWords[0] = Left hand symbol
            // lineWords[1] = Right hand first symbol
            // lineWords[2] = Right hand second symbol
            if (inputWord.length() > 1) {
                inputWord = inputWord.substr(0, 1);
            }
            lineWords.push_back(inputWord);
        }

        auto grammarIt = grammarData.find(lineWords[1]);

        // Check if fist time adding grammar rule with this right hand first symbol
        if(grammarIt == grammarData.end()) { // First time
            unordered_map<string, string> grammarInfo;
            grammarInfo[lineWords[2]] = lineWords[0];
            grammarData[lineWords[1]] = grammarInfo;
        }
        else {
            (grammarIt->second)[lineWords[2]] = lineWords[0];
        }
    }

    // Create threads and attribute varaibles
    //pthread_t threads[NUM_THREADS];
    //pthread_attr_t threadAttr;

    //int threadResult;
    //void *status; // For debugging
    //struct traverseThreadArg traverseThreadArgs[NUM_THREADS];
    //struct createThreadArg createThreadArgs[NUM_THREADS];

    // Initialize and set thread joinable
    //pthread_attr_init(&threadAttr);
    //pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);

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

        // STEP 1: Traverse through graph
        for(auto graphIt : graphData) {
            
            // STEP 2: Go through vertex's edges
            for(auto graphEdge : graphIt.second.graphEdges) {
                
                // STEP 3: Check if edgeLabel exists in grammarData
                // If yes, this vertex could satisfy one of grammar rules.
                // If not, move on to next graphEdge.
                auto checkGrammarData = grammarData.find(graphEdge.edgeLabel);
                if(checkGrammarData != grammarData.end()) {
                    unordered_map<string, string> currGrammarRule = checkGrammarData->second;

                    // STEP 4: Check if graphEdges' destVertexLabel exists in graphData
                    // If yes, the dest vertex contains edges that could satisfy the current grammar rule.
                    // If not, the dest vertex doesn't contain any edges, so move on to next graphEdge.
                    auto checkDestVertex = graphData.find(graphEdge.destVertexLabel);
                    if(checkDestVertex != graphData.end()) {
                        
                        // STEP 5: Go through dest vertex's edges
                        for(auto destVertexIt : checkDestVertex->second.graphEdges) {
                            
                            // STEP 6: Check if the edge's label is found in currGrammarRule
                            // If yes, the grammar rule is satisfied!
                            auto checkGrammarRule = currGrammarRule.find(destVertexIt.edgeLabel);
                            if(checkGrammarRule != currGrammarRule.end()) {
                                GraphEdge newGraphEdge;
                                newGraphEdge.edgeLabel = checkGrammarRule->second;
                                newGraphEdge.sourceVertexLabel = graphEdge.sourceVertexLabel;
                                newGraphEdge.destVertexLabel = destVertexIt.destVertexLabel;

                                auto graphDataVertex = graphData.find(graphIt.first);
                                
                                // STEP 7: Check if edge already exists
                                // If not, add to graphData
                                if(!checkEdgeExists(graphDataVertex->second, newGraphEdge)) {
                                    graphDataVertex->second.graphEdges.push_back(newGraphEdge);

                                    newEdgeAdded = true;
                                    numTotalEdgesAdded++;
                                }                           
                            }

                        }
                    }

                }
            }

        }



        /*for(auto grammarIt : grammarData) {
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
        }*/
        loopCount++;
    }

    // Capture end time
    auto endTime = chrono::system_clock::now();
    chrono::duration<double> durationTime = endTime - startTime;

    // Temporary output (outputs graphData)
    ofstream outputFile;
    outputFile.open("../output/output");
    string outputLine;

    outputFile << "Original Number of Vertices: " << origNumVertex << "\n";
    outputFile << "Original Number of Edges: " << origNumEdge << "\n";
    outputFile << "Number of Edges Added: " << numTotalEdgesAdded << "\n";
    outputFile << "Elapsed time: " << durationTime.count() << "s\n";
    
    if(outputFile.is_open()) {
        outputFile << "Graph:\n";
        for(auto graphIt : graphData) {
            outputFile << graphIt.first << ": \n";
            for(auto edgeIt : graphIt.second.graphEdges) {
                outputFile << "  " << edgeIt.destVertexLabel << "  " << edgeIt.edgeLabel << "\n";
            }
        }
        outputFile << "\nGrammar:\n";
        for(auto grammarIt : grammarData) {
            outputFile << grammarIt.first << ": \n";
            for(auto grammarInfoIt : grammarIt.second) {
                outputFile << "  " << grammarInfoIt.first << "  " << grammarInfoIt.second << "\n";
            }
        }
    }

    outputFile.close();
    
    return 0;
}
