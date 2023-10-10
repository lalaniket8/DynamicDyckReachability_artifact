#ifndef GRAPH_H
#define GRAPH_H

#include "graph/vertex.h"
#include "graph/field.h" 
#include "DSU/DSU.h"
#include <vector> 
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <fstream>
#include <memory>
#include <unordered_set>

using namespace std;

class graph {

	unordered_map<int64_t,VertexAA*> str2vtx;
	unordered_map<int64_t,field> str2field;

	void addnewPair(VertexAA*rootvtx);
	void appendedges(std::set<int>& roots,VertexAA*rootvtx,int uid,field mf);
	void removeRepeatedges(list<int> &l);
	void dsuUnion(int root,std::set<int>& roots);
	
public:
	field EPS = field(0,-1); // field EPS = field(0,"eps");
	int N = 0;
	bool HUSH = true;
	DSU dsu;
	std::vector<VertexAA*> vertices;
	std::vector<field> fields;
	queue<pair<int,field>> worklist;  //VertexAA id and field
	double numedges;
	graph(){
		numedges=0;
		fields.push_back(EPS);
		str2field[-1] = EPS; // str2field["eps"] = EPS;
	}
	void initWorklist();
	void bidirectedReach();
	void construct(string infile_name);
	void addedge(VertexAA*u,VertexAA*v, field &f);
	void printReach();
	void printDetailReach();
	void printGraph();
	void printEdges(VertexAA*vtx);
	bool query(int uid,int vid);
	int getRoot(VertexAA* vtx,field& f,std::set<int>& roots,int &rootS);
	field& getfield(const int64_t &s);
	// gets the vertex with name s, if not present creates it
	VertexAA* getVertex(const int64_t &s);
	VertexAA* getVertex(const int64_t &s, const int64_t &s2);
	// void show(unordered_map<std::string, int64_t> NODES);
	unordered_map<int,set<int64_t>> getComponents();
	VertexAA* root(VertexAA* vtx);
	VertexAA* getVertexAAPointerfromVertexAAID(int id);
	void removeEPSedgeFromVertex(int vertexAAID);
	void removeAllEdgesExceptEPS_associatedWithVertexAAIDs(unordered_set<int>& vertexAAIDs);
	void removeOutgoingEdges_exceptEPS(unordered_set<int>& vertexAAIDs);
	void removeOutgoingEdgesFromNonBrokenSourceNodesToBrokenNodes(unordered_set<int>& nonbroken_incomingNodes_vertexAAID, unordered_set<int>& vertexAAIDs);
	void clean();
	
};

#endif