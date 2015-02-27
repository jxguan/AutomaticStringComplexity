#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "handler.cc"

using namespace std;

struct automaton {
	int numStates;
	int finalState;
	int* transFunctions;
} automaton;

static bool interrupted = false;
static const int LENGTH_STRING = 10;
static const int ALPHABET_SIZE = 2;
static const string OUTPUT_FORMAT = "output/size";
struct node{
	node* child[ALPHABET_SIZE];
	int value;
};

ofstream output;
node* allStrings;
int numStringsRemaining;

static void handler (int sig) {
	interrupted = true;
}

static string itos(int i) // convert int to string
{
    stringstream s;
    s << i;
    return s.str();
}

static node* buildTree(int depth, bool fromFile, ifstream& file) {
	node* root = new node;
	if (depth == LENGTH_STRING) {
		if (fromFile)
			file >> root->value;
		else
			root->value = 0;
		return root;
	}
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		root->child[i] = buildTree(depth + 1, fromFile, file);
	}
	root->value = -1;
	return root;
}

static void outputTree(node* root, int depth, ofstream& file) {
	if (depth == LENGTH_STRING) {
		file << root->value << endl;
		return;
	}
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		outputTree(root->child[i], depth + 1, file);
	}
}

static void loadFromFile () {
	ifstream file;
	file.open("loadFile");
	file >> numStringsRemaining;
	file >> automaton.numStates;
	file >> automaton.finalState;
	automaton.transFunctions = new int[ALPHABET_SIZE * automaton.numStates];
	for (int i = 0; i < ALPHABET_SIZE * automaton.numStates; i++) {
		file >> automaton.transFunctions[i];
	}
	allStrings = buildTree(0, true, file);
	file.close();
	string outputPath = OUTPUT_FORMAT + itos(automaton.numStates) + ".out";
	output.open(outputPath.c_str(), ofstream::out | ofstream::app);
	cout << "Loaded from last place where we stopped" << endl;
	cout << "Current Automaton Size: " << automaton.numStates << endl;
	cout << "Strings Left: " << numStringsRemaining << endl;
}

static void writeToFile() {
	ofstream file;
	file.open("loadFile");
	file << numStringsRemaining << endl;
	file << automaton.numStates << endl;
	file << automaton.finalState << endl;
	for (int i = 0; i < automaton.numStates; i++) {
		int index = ALPHABET_SIZE * i;
		file << automaton.transFunctions[index];
		for (int j = 1; j < ALPHABET_SIZE; j++) {
			file << " " << automaton.transFunctions[index + j];
		}
		file << endl;
	}
	outputTree(allStrings, 0, file);
	file.close();
}

static void killTree(node* root, int depth) {
	if (depth == LENGTH_STRING) {
		delete root;
		return;
	}
	for (int i = 0; i < ALPHABET_SIZE; i++)
		killTree(root->child[i], depth + 1);
	delete root;
}

static void initialize () {
	numStringsRemaining = pow(ALPHABET_SIZE, LENGTH_STRING);
	automaton.numStates = 2;
	automaton.finalState = 0;
	automaton.transFunctions = new int[ALPHABET_SIZE * automaton.numStates];
	for (int i = 0; i < ALPHABET_SIZE * automaton.numStates; ++i) {
		automaton.transFunctions[i] = 0;		
	}
	string outputPath = OUTPUT_FORMAT + itos(automaton.numStates) + ".out";
	output.open(outputPath.c_str(), ofstream::out | ofstream::app);
	ifstream dummy;
	allStrings = buildTree(0, false, dummy);
	cout << "Starting from the Beginning: " << endl;
	cout << "Alphabet Size: " << ALPHABET_SIZE << endl;
	cout << "String Length: " << LENGTH_STRING << endl;
	cout << "Number of Total Strings: " << numStringsRemaining << endl;
}

static void growAutomaton() {
	automaton.numStates++;
	automaton.finalState = 0;
	delete[] automaton.transFunctions;
	automaton.transFunctions = new int[ALPHABET_SIZE * automaton.numStates];
	for (int i = 0; i < ALPHABET_SIZE * automaton.numStates; ++i) {
		automaton.transFunctions[i] = 0;		
	}
	output.close();
	string outputPath = OUTPUT_FORMAT + itos(automaton.numStates) + ".out";
	output.open(outputPath.c_str(), ofstream::out | ofstream::app);
	cout << "Automaton Size Growed To: " << automaton.numStates << endl;
}

static void iterateToNextAutomaton () {
	int index = automaton.numStates * ALPHABET_SIZE - 1;
	automaton.transFunctions[index]++;
	while (index >= 1 && automaton.transFunctions[index] >= automaton.numStates) {
		automaton.transFunctions[index] = 0;
		index--;
		automaton.transFunctions[index]++;
	}
	if (automaton.transFunctions[0] >= automaton.numStates) {
		if (automaton.finalState == 0) {
			automaton.transFunctions[0] = 0;
			automaton.finalState = 1;
		}
		else
			growAutomaton();
	}
}

static void leftTimes(vector<int>& left, vector<vector<int>>& mat) {
	vector<int> temp;
	for (int j = 0; j < automaton.numStates; j++) {
		int sum = 0;
		for (int k = 0; k < automaton.numStates; k++) {
			sum += left[k] * mat[k][j];
		}
		temp.push_back(sum);
	}
	left = temp;
}

static void rightTimes(vector<int>& right, vector<vector<int>>& mat) {
	vector<int> temp;
	for (int j = 0; j < automaton.numStates; j++) {
		int sum = 0;
		for (int k = 0; k < automaton.numStates; k++) {
			sum += right[k] * mat[j][k];
		}
		temp.push_back(sum);
	}
	right = temp;
}

static bool checkString (string& str) {
	node* curr = allStrings;
	for (int i = 0; i < LENGTH_STRING; i++) {
		curr = curr->child[str[i] - '0'];
	}
	if (curr->value == 0) {
		curr->value = 1;
		return true;
	}
	return false;
}

static string stateSequence(vector<int>& left, vector<int>& right, const int numTransitions, 
							vector<vector<int>>& mat) {
	if (numTransitions < 0) {
		return "";
	}
	if (numTransitions == 0) {
		string currentState;
		for (int i = 0; i < automaton.numStates; i++) {
			if (left[i] > 0 && right[i] > 0) {
				currentState = itos(i);
			}
		}
		return currentState;
	}

	int mid = numTransitions / 2;
	vector<int> currentLeft = left;
	for (int i = 0; i < mid; i++) {
		leftTimes(currentLeft, mat);
	}
	int remaining = numTransitions - mid;
	vector<int> currentRight = right;
	for (int i = 0; i < remaining; i++) {
		rightTimes(currentRight, mat);
	}
	string currentState;
	for (int i = 0; i < automaton.numStates; i++) {
		if (currentLeft[i] > 0 && currentRight[i] > 0) {
			currentState = itos(i);
		}
	}

	leftTimes(currentLeft, mat);
	rightTimes(currentRight, mat);
	return stateSequence(left, currentRight, mid - 1, mat) + currentState + 
			stateSequence(currentLeft, right, remaining - 1, mat);
}

static bool recogUniqueString(string& states) {
	vector<vector<int>> mat;
	for (int i = 0; i < automaton.numStates; i++) {
		vector<int> row;
		for (int j = 0; j < automaton.numStates; j++) {
			row.push_back(0);
		}
		mat.push_back(row);
	}
	for(int i = 0; i < automaton.numStates; i++) {
		int index = ALPHABET_SIZE * i;
		for (int j = 0; j < ALPHABET_SIZE; j++) {
			mat[i][automaton.transFunctions[index + j]]++;
		}
	}
	vector<int> currentStates;
	currentStates.push_back(1);
	for (int i = 1; i < automaton.numStates; i++) {
		currentStates.push_back(0);
	}
	for (int i = 0; i < LENGTH_STRING; i++) {
		leftTimes(currentStates, mat);
	}
	if (currentStates[automaton.finalState] != 1)
		return false;
	vector<int> left(automaton.numStates);
	vector<int> right(automaton.numStates);
	left[0] = 1;
	right[automaton.finalState] = 1;
	states = stateSequence(left, right, LENGTH_STRING, mat);
	return true;
}

static int getSymbol(int currentState, int nextState) {
	int index = currentState * ALPHABET_SIZE;
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (automaton.transFunctions[index + i] == nextState)
			return i;
	}
	return -1;
}

static string generateInput(string& states) {
	string ans;
	int currentState = states[0] - '0';
	for (int i = 1; i < LENGTH_STRING + 1; i++) {
		ans += itos(getSymbol(currentState, states[i] - '0'));
		currentState = states[i] - '0';
	}
	return ans;
}

static void outputAutomaton(string& str) {
	output << "Final State: " << automaton.finalState << endl;
	for (int i = 0; i < automaton.numStates; i++) {
		int index = ALPHABET_SIZE * i;
		output << i << ": " << automaton.transFunctions[index];
		for (int j = 1; j < ALPHABET_SIZE; j++) {
			output << " " << automaton.transFunctions[index + j];
		}
		output << endl;
	}
	output << "Unique String: " << str << endl << endl;
}

int main(int argc, char *argv[]) {
	installSignalHandler(SIGINT, handler);
	if (argc > 1 && strcmp(argv[1], "resume") == 0) {
		loadFromFile();
	}
	else if (argc == 1) {
		initialize();
	}
	else {
		cout << "Error: please call with an optional \"resume\" parameter or no parameter" << endl;
		return 1;
	}
	

	while(!interrupted) {
		string states;
		if (recogUniqueString(states)) {
			string input = generateInput(states);
			if (checkString(input)) {
				outputAutomaton(input);
				numStringsRemaining--;
				cout << "Number of Strings Remaining: " << numStringsRemaining << endl;
				if (numStringsRemaining == 0) {
					cout << "All strings Found! Done!" << endl;
					output.close();
					killTree(allStrings, 0);
					return 0;
				}

			}
		}
		iterateToNextAutomaton();
	}
	writeToFile();
	output.close();
	killTree(allStrings, 0);
	cout << endl << "Interrupted, current status written to loadFile." << endl;
	return 0;
}
