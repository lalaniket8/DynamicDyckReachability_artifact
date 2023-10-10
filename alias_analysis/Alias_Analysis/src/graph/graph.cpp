#include "graph/graph.h"
#include "utils/utils.h"
#include <iostream>
#include <cassert>
#include <algorithm>


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
			addedge(getVertex(stoi(tokens[1])),getVertex(stoi(tokens[2])),getfield(stoi(tokens[3])));
			// if(getfield(tokens[3])==EPS){
			// 	addedge(getVertex(tokens[2]),getVertex(tokens[1]),EPS);	
			// }
			continue;
		}
		if(tokens[0] == "v"){
			// assert(tokens.size()==2);
			getVertex(stoi(tokens[1]));
			continue;
		}
		if(tokens[0] == "f"){
			// assert(tokens.size()==2);
			getfield(stoi(tokens[1]));
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
	cout<<"Number of vertices : "<<vertices.size()<<endl;
	cout<<"Number of edges : "<<numedges<<endl;
	cout<<"Number of field types : "<<fields.size()<<endl;
	for(int i=0;i<vertices.size();i++){//change this
		VertexAA* vtx = vertices[i];
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
		VertexAA* vtx =  vertices[p.first];
		// cout<<vtx->name<<endl;
		if(p.first == dsu.root(p.first) ){
			// cout<<vertices[p.first]->name<<endl;
			std::set<int> roots;
			int rootS = -1;
			int rootA = getRoot(vtx,p.second,roots,rootS);
			// cout << "rootA:" << rootA << endl;
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
			VertexAA* rootvtx = vertices[rootA];
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
		if(!HUSH) cout << "hardmerge(" << vertices[rootA]->name << "," << vertices[*it]->name << ")" << endl;
		dsu.hardmerge(rootA,*it);
		it++;
	}
}

// finds all the vertex's root pointed by vtx and field
// returns the merge vtx id of root of the merged disjoint set
int graph::getRoot(VertexAA* vtx,field& f,std::set<int>& roots,int& rootS){
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
void graph::appendedges(std::set<int>& roots,VertexAA*rootvtx, int uid,field mf){
	for(auto i=roots.begin();i!=roots.end();i++){ 
		VertexAA* rvtx = vertices[*i];
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
void graph::addnewPair(VertexAA* rootvtx){
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


VertexAA* graph::getVertex(const int64_t &s,const int64_t &s2){
	auto it = str2vtx.find(s);
	if(it==str2vtx.end()){
		VertexAA* vtx( new VertexAA(this->N,s,s2));
		vertices.push_back(vtx);
		vtx->addedge(EPS,vtx->id);
		this->N++;
		str2vtx[s]=vtx;
		return vtx;
	}
	return it->second; 
}


VertexAA* graph::getVertex(const int64_t &s){//O(1) average
	auto it = str2vtx.find(s);// O(1) average
	if(it==str2vtx.end()){
		VertexAA* vtx( new VertexAA(this->N,s));
		vertices.push_back(vtx);
		vtx->addedge(EPS,vtx->id);
		this->N++;
		str2vtx[s]=vtx;
		return vtx;
	}
	return it->second; 
}

field& graph::getfield(const int64_t &s){
	auto it = str2field.find(s);
	if(it==str2field.end()){
		field f(fields.size(),s);
		fields.push_back(f);
		str2field[s]=fields[fields.size()-1];
		return fields[fields.size()-1];
	}
	return it->second;
}

void graph::addedge(VertexAA* u,VertexAA* v,field &f){
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
	unordered_map<int,set<int>> scc;
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
		VertexAA*vtx = vertices[i];
		cout<<"*****  "<<vtx->name<<"  *****\n";
		vtx->printvtxid();
		printEdges(vtx);
		cout<<endl;
	}
}



void graph::printEdges(VertexAA* vtx){
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

unordered_map<int,set<int64_t>> graph::getComponents(){
	unordered_map<int,set<int64_t>> scc;
	int vertices_size = vertices.size();
	for(int i=0;i<vertices_size;i++){
		scc[dsu.root(vertices[i]->id)].insert(vertices[i]->name);
	}
	return scc;
}

VertexAA* graph::getVertexAAPointerfromVertexAAID(int id){return vertices[id];}

//removes all outgoing edges from brokenVertexAAIDs except EPS edges
void graph::removeOutgoingEdges_exceptEPS(unordered_set<int>& brokenVertexAAIDs){
	// cout << "removeOutgoingEdges_exceptEPS" << endl;
	for(const int& brokenVertexAAID : brokenVertexAAIDs){
		for(auto& edgesIter : vertices[brokenVertexAAID]->edges){
			if(edgesIter.first.field_id != EPS.field_id){
				edgesIter.second.clear();}
		}
	}
}

//removes all outgoing edges from nonbroken incoming vertices to broken vertices
void graph::removeOutgoingEdgesFromNonBrokenSourceNodesToBrokenNodes(unordered_set<int>& nonbroken_incomingNodes_vertexAAID, unordered_set<int>& brokenVertexAAIDs){
	//iterate over destination nodes in edges
	//if dest node is broken, remove it
	// cout << "removeOutgoingEdgesToBrokenNodes" << endl;
	for(const int& nonbroken_incomingNode_VertexAAID : nonbroken_incomingNodes_vertexAAID){
		for(auto& edgesIter : vertices[nonbroken_incomingNode_VertexAAID]->edges){
			if(edgesIter.first.field_id != EPS.field_id){
				auto toNodesIter = edgesIter.second.begin();
				
				if(edgesIter.second.size() > 0){
					if(brokenVertexAAIDs.find(*toNodesIter) != brokenVertexAAIDs.end()){//*toNodesIter is broken
						edgesIter.second.clear();
					}
				}
				// while(toNodesIter != edgesIter.second.end()){
				// 	if(brokenVertexAAIDs.find(*toNodesIter) != brokenVertexAAIDs.end()){//*toNodesIter is broken
				// 		// cout << "...erasing " << nonbroken_incomingNode_VertexAAID << "," << edgesIter.first.field_id << endl;
				// 		toNodesIter = (edgesIter.second).erase(toNodesIter);
				// 	}else{
				// 		toNodesIter++;
				// 	}
				// }
			}
		}
	}
}

void graph::removeAllEdgesExceptEPS_associatedWithVertexAAIDs(unordered_set<int>& brokenVertexAAIDs){

	for(VertexAA* iter : vertices){
	// for(vector<VertexAA*>::iterator iter = vertices.begin(); iter != vertices.end(); iter++){
		
		int current_VertexAAID = iter->id;

		// if current node is broken, remove all edges except EPS
		if(brokenVertexAAIDs.find(current_VertexAAID) != brokenVertexAAIDs.end()){
			//iterate over destination nodes in edges
			//if dest node is broken, remove it
			for(auto& edgesIter : vertices[current_VertexAAID]->edges){
				if(edgesIter.first.field_id != EPS.field_id){edgesIter.second.clear();}
			}

		}else{
			//if current vertex is not broken, remove all outgoing edges to broken vertices
			//iterate over destination nodes in edges
			//if dest node is broken, remove it
			for(auto& edgesIter : vertices[current_VertexAAID]->edges){
				auto toNodesIter = edgesIter.second.begin();
				// while(toNodesIter != edgesIter.second.end()){
				// 	if(brokenVertexAAIDs.find(*toNodesIter) != brokenVertexAAIDs.end()){
				// 		toNodesIter = (edgesIter.second).erase(toNodesIter);
				// 	}else{
				// 		toNodesIter++;
				// 	}
				// }
				if(edgesIter.second.size() > 0){
					if(brokenVertexAAIDs.find(*toNodesIter) != brokenVertexAAIDs.end()){//*toNodesIter is broken
						edgesIter.second.clear();
					}
				}
			}

		}

	}
	
}

void graph::clean(){ 
	for(VertexAA* i : vertices){delete i;} // These pointers cause mem leak if not deleted
}