#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <chrono>

using namespace std;

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
    map<string, map<string, string>> graphData;
    
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

        map<string, string> destData = {{lineWords[1], lineWords[2]}};
        graphData.insert({lineWords[0], destData});
    }

    // Measure time for performance result
    auto startTime = chrono::system_clock::now(); 

    /* TO-DO: Go through Graph Data and apply Grammar rule (CFL-Reachability)
    *
    *
    */
    
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
            for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
                outputFile << "  " << it2->first << "  " << it2->second << "\n";
            }
        }
    }

    outputFile << "\n";
    outputFile << "Elapsed time: " << durationTime.count() << "s";
    outputFile.close();
    
    return 0;
}