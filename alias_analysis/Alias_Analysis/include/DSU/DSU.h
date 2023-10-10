#ifndef DSU_H
#define DSU_H

#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <list>
using namespace std;

class DSU {
	int N;
public:
	vector<int> parent_pos;   // position of parent of vertex at position p
	vector<int> vtxatpos;
	vector<int> posofvtx;
	vector<list<int>> dsuContent;//contains the constituent nodes in each dsu. list is empty when corresponding node is not root. //contains vertexAAID 
	bool HUSH = true;

	int getN();

	void init(int n);
	// gives the root of the Disjoint set containing vertex id v 
	int root(int v);
	//merge the two Disjoined set and returns the root of the merged Disjoined set
	// int merge(int a,int b);

	void hardmerge(int r,int a);
	// gives the rank of the root r
	int rank(int r);

	int rootpos(int pos);

	void exchange(int uid,int vid);
	int getArraySize();
	void display();
	void displayVector(vector<int> v);
	int getVerticesCount();

	list<int>& getDSUContent(int vertexAA_pos);
};

#endif