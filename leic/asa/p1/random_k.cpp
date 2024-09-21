/*************************************************************
 * IST - ASA 21/22
 * Pedro T. Monteiro
 *************************************************************/

#include<string>
#include<iostream>
#include<climits>

using namespace std;

//-------------------------------------------------------------------
void printUsage() {
	cout << "Sequence generator" << endl;
	cout << ". sequences of k numbers, with prob p of increasing from i to i+1" << endl;
	cout << "Usage: random_k p a k k2" << endl;
	cout << "\tp: either (problem) 1 or 2" << endl;
	cout << "\ti: initial value" << endl;
	cout << "\ta: probability (0 < a <= 1) of increasing from i-th to the (i+1)-th element" << endl;
	cout << "\tk: sequence of k numbers (int > 0)" << endl;
	cout << "\tk2: sequence of k2 numbers (int > 0 for problem 2)" << endl;
	exit(0);
}

//-------------------------------------------------------------------
void genSeq(int n, int init, int a) {
	string s;
	int next = init;
	srand(rand());
	for (int i=1; i<=n; i++) {
		if (rand() < a) next += (rand() % 5) + 1;
		else            next -= (rand() %  3) + 1;
		if (next <= 0) { // guard
			cout << "ERROR: number <= 0" << endl;
			exit(1);
		}
		s.append(to_string(next) + " ");
	}
	s.pop_back();
	cout << s << endl;
}

//-------------------------------------------------------------------
int main(int argc, char *argv[]) {
	int p, k, k2, ia, i;
	float a;

	if (argc < 5) printUsage();
	sscanf(argv[1], "%d", &p);
	if (p != 1 && p != 2) {
		cout << "ERROR: p must be either 1 or 2" << endl;
		printUsage();
	}
	sscanf(argv[2], "%d", &i);
	if (i < 0) {
		cout << "ERROR: i must be > 0" << endl;
		printUsage();
	}
	sscanf(argv[3], "%f", &a);
	if (a > 1 || a <= 0) { 
		cout << "ERROR: a must be 0 < a <= 1" << endl;
		printUsage();
	}
	ia = (int)(a*INT_MAX);
	sscanf(argv[4], "%d", &k);
	if (k < 1) { 
		cout << "ERROR: k must be an int > 0" << endl;
		printUsage();
	}
	if (p == 2) {
		if (argc == 6) {
			sscanf(argv[5], "%d", &k2);
			if (k2 < 1) {
				cout << "ERROR: k2 must be an int > 0" << endl;
				printUsage();
			}
		} else printUsage();
	}

	srand(time(NULL));
	cout << to_string(p) << endl;
	if (p == 1) genSeq(k,i, ia);
	else {
		genSeq(k,i, ia);
		genSeq(k2,i, ia);
	}
	return 0;
}
