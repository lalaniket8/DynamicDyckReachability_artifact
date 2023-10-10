#ifndef GRAPH_H
#define GRAPH_H

#include "graph/vertex.h"
#include "graph/field.h" 
#include "DSU/DSU.h"
#include <vector> 
#include <queue>
#include <set>
#include <string>
#include <fstream>
// #include <unordered_map>
#include <map>
#include <memory>

using namespace std;

class graph {

	// steps
	// g.construct(string);
	// g.initWorklist();
	// g.bidirectedReach();


	std::vector<Vertex*> vertices;
	std::vector<field> fields;
	field EPS = field(0,"eps");
	
	queue<pair<int,field>> worklist;  //vertex id and field
	map<string,Vertex*> str2vtx; //map - > umap
	map<string,field> str2field; //map - > umap

	void addnewPair(Vertex* rootvtx);
	void appendedges(std::set<int>& roots,Vertex* rootvtx,int uid,field mf);
	void removeRepeatedges(list<int> &l);
	void dsuUnion(int root,std::set<int>& roots);
	

public:
	DSU dsu;
	int N = 0;
	double numedges;
	graph(){
		numedges=0;
		fields.push_back(EPS);
		str2field["eps"] = EPS;
	}
	void initWorklist();
	void bidirectedReach();
	void construct(string infile_name);
	void addedge(Vertex* u,Vertex* v, field &f);
	void printReach();
	void printDetailReach();
	void printGraph();
	void printEdges(Vertex* vtx);
	bool query(int uid,int vid);
	int getRoot(Vertex* vtx,field& f,std::set<int>& roots,int &rootS);
	field& getfield(const string &s);
	// gets the vertex with name s, if not present creates it
	Vertex* getVertex(const string &s);
	void processInput(set<string>* nodes_set, set<string>* edges_set);
	map<int,set<string>> getComponents();
	void clean();
};

#endif