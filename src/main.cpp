#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

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

    // TODO - Read through graph file and create a graph with edges
    
    // Temporary output (Just outputs input graph and grammar)
    ofstream outputFile;
    outputFile.open("../output/output");
    string outputLine;

    if(outputFile.is_open()) {
        outputFile << "Input Graph:\n";
        while( getline(graphFile, outputLine) ) {
            outputFile << outputLine;
        }
        outputFile << "\n\n";
        outputFile << "Input Grammar:\n";
        while( getline(grammarFile, outputLine) ) {
            outputFile << outputLine;
        } 
    }

    outputFile.close();
    return 0;
}