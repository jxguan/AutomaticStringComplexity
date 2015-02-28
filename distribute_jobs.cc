#include "myth-nodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <thread>
#include <algorithm>
#include <sstream>
using namespace std;

vector<string> kMythNodes = loadMythNodes();


void spawnWorker(size_t length, const string& command) {
  cout << "Spawning worker for " << length << " with ssh command: " << endl 
       << "\t\"" << command << "\"" << endl;
  int status = system(command.c_str());
  if (status != -1) status = WEXITSTATUS(status);
  cout << "Remote ssh command for " << length << " executed and returned a status " 
       << status << "." << endl;
}

/**
 * Method: buildCommand
 * --------------------
 * Constructs the command needed to invoke a worker on a remote machine using
 * the system (see "man system") function.  See spawnWorker below for even more information.
 */ 
static const string kWorkerExecutable = "str_complexity";
static const string kUserName = "jxguan";
static const string kExecutablePath = "2015winter/cs199";
string buildCommand(const string& workerHost, size_t length, bool resume) {
  ostringstream oss;
  oss << "ssh -o ConnectTimeout=5 " << kUserName << "@" << workerHost
      << " '" << kExecutablePath << "/" + kWorkerExecutable
      << " " << length;
  if (resume) oss << " resume";
  oss << "'";
  return oss.str();
}


void spawnWorkers(vector<size_t>& lengths, bool resume) {
  vector<string> workerNodes(kMythNodes);
  random_shuffle(workerNodes.begin(), workerNodes.end());
  size_t numWorkersToSpawn = workerNodes.size() > lengths.size() ? lengths.size() : workerNodes.size();
  vector<thread> workerThreads;
  for (size_t i = 0; i < numWorkersToSpawn; i++) {
    string command = buildCommand("myth", lengths[i], resume);
    workerThreads.push_back(thread([=]{
      spawnWorker(lengths[i], command);
    }));
  }
  for (size_t i = 0; i < workerThreads.size(); i++) {
    workerThreads[i].join();
  }
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		cout << "Too Few Arguments." << endl;
		return 1;
	}
	bool resume = (strcmp(argv[1], "resume") == 0);
	vector<size_t> lengths;
	for (int i = 2; i < argc; i++) {
		size_t length;
		if (sscanf(argv[i], "%lu", &length) != 1) {
			cout << "Illegal input format." << endl;
			return 1;
		}
		lengths.push_back(length);
	}
	spawnWorkers(lengths, resume);
	return 0;
}
