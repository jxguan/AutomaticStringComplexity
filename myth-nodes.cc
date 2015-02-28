/**
 * File: myth-nodes.cc
 * -------------------
 * Presents the (very straightforward) implementation of
 * loadMythNodes.  In an ideal world this would actually
 * build a list of myth hostnames that were reachable and ssh'able,
 * but right now it just loads the list of hostnames from a file. #weak
 */

#include "myth-nodes.h"
#include "string-utils.h"
#include <iostream>
#include <fstream>
using namespace std;

vector<string> loadMythNodes() {
  const string kMythNodesInputFile = "nodes.txt";
  ifstream instream(kMythNodesInputFile);
  if (!instream) {
    cerr << "Failed to load list of available myth nodes from \"" 
         << kMythNodesInputFile << "\"." << endl;
    cerr << "Aborting..." << endl;
    exit(1);
  }

  vector<string> nodes;
  while (true) {
    string node;
    getline(instream, node);
    if (instream.fail()) return nodes;
    nodes.push_back(trim(node));
  }
}


