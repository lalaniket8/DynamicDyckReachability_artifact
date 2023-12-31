#include "graph/graph.h"
#include "utils/utils.h"
#include <iostream>
#include <cassert>
#include <algorithm>


// constructInitGraph() : processes the inital graph
// processSequenceOperation() : process the operation in the seq file line by line (input parameter is single line)

// These functions are written to compare this offline tool to a dynamic tool
// For give initial graph G_0 in inital graph input file, and series of sequence operations {S_1,S_2,...,S_n} in sequence input file,
// we define graph G_i as the graph G_0 after operations S_1 to S_i,
// The time taken to process the input and sequence file is sum of time taken to process graphs G_0 to G_n
// Assumed format of input: POPL22 tool's format:
// <A/D> <fromNode> <toNode> <op--edge>/<cp--edge>

void graph::processInput(set<string>* nodes_set, set<string>* edges_set){
	for(auto nodes_set_iter = nodes_set->begin(); nodes_set_iter != nodes_set->end(); nodes_set_iter++){
		getVertex(*nodes_set_iter);
	}
	for(set<string>::iterator edges_set_iter = edges_set->begin(); edges_set_iter != edges_set->end(); edges_set_iter++){
		string fromNode,toNode,edgeType = "";
		std::vector<string> contents;
		string edge = *edges_set_iter;
		split(edge,"||",contents);
		fromNode = contents[0];
		toNode = contents[1];
		edgeType = contents[2];
		// cout << "Edge:" << fromNode << "," << toNode << "," << edgeType << "#" << endl;
		addedge(getVertex(fromNode),getVertex(toNode),getfield(edgeType));
	}
	// cout << "vertices.size():" << vertices.size() << endl;
	dsu.init(vertices.size());
}

// takes the file name containing the edge information of the spg as arguement 
// and construct a Ngraph from it
void graph::construct(string infile_name){
	ifstream infile(infile_name);
	string line;
	while(std::getline(infile,line)){
		std::vector<string> tokens;
		split(line,"||",tokens);
		if(tokens.size()==0 || (tokens.size()==1 && tokens[0].size()==0))
			continue;
		if(tokens[0] == "e"){
			// assert(tokens.size()==4);
			addedge(getVertex(tokens[1]),getVertex(tokens[2]),getfield(tokens[3]));
			// if(getfield(tokens[3])==EPS){
			// 	addedge(getVertex(tokens[2]),getVertex(tokens[1]),EPS);	
			// }
			continue;
		}
		if(tokens[0] == "v"){
			// assert(tokens.size()==2);
			getVertex(tokens[1]);
			continue;
		}
		if(tokens[0] == "f"){
			// assert(tokens.size()==2);
			getfield(tokens[1]);
			continue;
		}
		if(tokens[0] == "//"){
			// comment in infile
			continue;
		}
		cerr<<"incorrect syntax in "+infile_name<<endl;
		cerr<<line<<endl;
		cerr<<"tokens were "<<tokens.size()<<endl;
		for(int i=0;i<tokens.size();i++)
			cerr<<tokens[i]<<"   **;**   ";
		cerr<<endl;
	}
	dsu.init(vertices.size());
}


void graph::initWorklist(){
	//cout<<"Number of vertices : "<<vertices.size()<<endl;
	//cout<<"Number of edges : "<<numedges<<endl;
	//cout<<"Number of field types : "<<fields.size()<<endl;
	for(int i=0;i<vertices.size();i++){//change this
		Vertex* vtx = vertices[i];
		if(i!=dsu.root(i)){
			vtx->clear();
			continue;
		}
		// vtx->clear(EPS);
		auto it = vtx->edgesbegin(); //pointer to pair<field,list<int>>
		while(it!=vtx->edgesend()){
			removeRepeatedges(it->second);
			if(it->second.size()>=2)
				worklist.push(pair<int,field>(vtx->id,it->first));
			it++;
		}
	}
}

// algorithm described in this paper 
void graph::bidirectedReach(){
	//precondition graph::initWorklist() has been called
	while(!worklist.empty()){
		pair<int,field> p = worklist.front();
		worklist.pop();
		Vertex* vtx =  vertices[p.first];
		// cout<<vtx->name<<endl;
		if(p.first == dsu.root(p.first) ){
			// cout<<vertices[p.first]->name<<endl;
			std::set<int> roots;
			int rootS = -1;
			int rootA = getRoot(vtx,p.second,roots,rootS);
			if(roots.size()<2){
				// can roots have zero size??
				// assert(roots.size()!=0);
				vtx->set(p.second,*(roots.begin()));
				continue;
			}
			// cout<<"doing union "<<vertices[rootA]->name<<endl;
			dsuUnion(rootA,roots);
			if(rootA==vtx->id){
				// assert(rootS!=-1);
				dsu.exchange(rootA,rootS);
				rootA=rootS;
			}
			Vertex* rootvtx = vertices[rootA];
			appendedges(roots,rootvtx,vtx->id,p.second);
			addnewPair(rootvtx);
			vtx->set(p.second,rootA);
		}
	}
}

void graph::dsuUnion(int rootA,std::set<int>& roots){
	auto it = roots.begin();
	// auto end = roots.end();
	while(it!=roots.end()){
		dsu.hardmerge(rootA,*it);
		it++;
	}
}

// finds all the vertex's root pointed by vtx and field
// returns the merge vtx id of root of the merged disjoint set
int graph::getRoot(Vertex* vtx,field& f,std::set<int>& roots,int& rootS){
	auto it = vtx->edgesbegin(f); //pointer to vertex id (int)
	// auto end = vtx->edgesend(f);
	int rootA = -1; //invariant: rootA = max rank root so far excluding vtx 
	int rankA = 0;
	int rankS = 0;
	while(it!=vtx->edgesend(f)){  //merge
		int rootB = dsu.root(*it);
		int rankB = dsu.rank(rootB);
		if(rankB>rankS){
			if(rankB>rankA){
				rootS=rootA; rankS=rankA; 
				rootA=rootB; rankA=rankB;
			}else if(rootA!=rootB){
				rootS=rootB; rankS=rankB;
			}
		}
		roots.insert(rootB);
		it++;
	}
	return rootA;
}

// append all the edges in roots to rootvtx
void graph::appendedges(std::set<int>& roots,Vertex* rootvtx, int uid,field mf){
	for(auto i=roots.begin();i!=roots.end();i++){ 
		Vertex* rvtx = vertices[*i];
		if(rvtx->id == rootvtx->id)
			continue;
		auto it = rvtx->edgesbegin();
		// auto end = rvtx->edgesend();
		while(it!=rvtx->edgesend()){
			field f = it->first;
			if(rvtx->id==uid and f==mf){
				rootvtx->addedge(f,rootvtx->id);
				it++;
				continue;
			}
			rootvtx->append(it->first,it->second);
			// assert(rvtx->edgesNumber(it->first)==0);
			it++;
		}
	}
}

// add <rootvertex,field> to worklist if possible  
void graph::addnewPair(Vertex* rootvtx){
	//worklist add. scope for optimisation
	auto it = rootvtx->edgesbegin();
	auto end = rootvtx->edgesend();
	while(it!=end){
		if(it->second.size()>=2)
			worklist.push(pair<int,field>(rootvtx->id,it->first));
		it++;
	}
}


void graph::removeRepeatedges(list<int> &l){
	set<int> elem; 
	auto it = l.begin();
	while(it!=l.end()){
		if( elem.find(dsu.root(*it))!=elem.end() ){
			it = l.erase(it); 
		}else{
			elem.insert(*it = dsu.root(*it));
			it++;
		}
	}
}

// query returns true iff variable with uid may point to variable with vid
bool graph::query(int uid,int vid){
	return dsu.root(uid)==dsu.root(vid);
}



Vertex* graph::getVertex(const string &s){
	auto it = str2vtx.find(s);
	if(it==str2vtx.end()){
		Vertex* vtx(new Vertex(this->N,s));
		vertices.push_back(vtx);
		vtx->addedge(EPS,vtx->id);
		this->N++;
		str2vtx[s]=vtx;
		return vtx;
	}
	return it->second; 
}

field& graph::getfield(const string &s){
	auto it = str2field.find(s);
	if(it==str2field.end()){
		field f(fields.size(),s);
		fields.push_back(f);
		str2field[s]=fields[fields.size()-1];
		return fields[fields.size()-1];
	}
	return it->second;
}

void graph::addedge(Vertex* u,Vertex* v,field &f){
	u->addedge(f,v->id);
	// if(f==EPS)
	// 	numedges+=0.5;
	// else 
		numedges++;
}

void graph::printReach(){
	cout<<"\tNumber of Strongly connected components : "<<dsu.getN()<<endl;
}

void graph::printDetailReach(){
	cout<<"Number of Strongly connected components : "<<dsu.getN()<<endl;
	map<int,set<int>> scc;//map - > umap
	for(int i=0;i<this->N;i++){
		scc[dsu.root(i)].insert(i);
	}
	auto it = scc.begin();
	while(it!=scc.end()){
		cout<<"printing elements belonging to scc "<<it->first<<endl;
		for(int elem : it->second){
			cout<<vertices[elem]->name<<"   ";
			// cout<<vertices[elem]->id<<" ";
		}
		cout<<endl;
		it++;
	}
}

void graph::printGraph(){
	cout<<"size of vertices is "<<vertices.size()<<endl;
	cout<<"size of fields in graph is "<<fields.size()<<endl;
	for(int i=0;i<vertices.size();i++){
		Vertex* vtx = vertices[i];
		cout<<"*****  "<<vtx->name<<"  *****\n";
		vtx->printvtxid();
		printEdges(vtx);
		cout<<endl;
	}
}



void graph::printEdges(Vertex* vtx){
	auto fit = vtx->edgesbegin();   
	while(fit!=vtx->edgesend()){   // iterating over field
		field f = fit->first; 
		cout<<"** "<<f.field_name<<endl;
		auto fedgeit = vtx->edgesbegin(f);
		while(fedgeit != vtx->edgesend(f)){   // iterating over edges
			cout<<"\t"<<vertices[*fedgeit]->name<<endl;
			fedgeit++;
		}
		fit++;
	}
}

map<int,set<string>> graph::getComponents(){
	map<int,set<string>> scc;
	int vertices_size = vertices.size();
	int dsu_posofvtx_size = dsu.posofvtx.size();
	for(int i=0;i<vertices_size;i++){
		int vertexID = vertices[i]->id;
		if(vertexID >= dsu.posofvtx.size()){
			cout << "vertexID >= dsu.posofvtx.size(): vertexID=" << vertexID << endl;
			cout << "i:" << i << endl;
			exit(1);
		}
		int rootValue = dsu.root(vertexID);
		string nameValue = vertices[i]->name;
		scc[rootValue].insert(nameValue);
	}
	return scc;
}

void graph::clean(){
	//str2vtx
	for(int i=0; i<vertices.size(); i++){
		delete vertices[i];
	}


}
