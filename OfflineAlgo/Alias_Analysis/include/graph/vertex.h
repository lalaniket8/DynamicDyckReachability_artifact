#ifndef VERTEX_H
#define VERTEX_H

#include <unordered_map>//map - > umap
#include <map>
#include <string>
#include <list>
#include "field.h"
#include <cassert>
#include <iostream>

using namespace std;

class Vertex{
public:
	int tag;  //for var/obj/sym
	std::map<field,list<int>> edges;//map - > umap

/*public:*/
	int id;
	string name;
	int graph_num;
	
	Vertex(int id,string s){
		this->id = id;
		this->name = s;
		graph_num=id;
	}

	Vertex(){
		id = -1;
	}

	map<field,list<int>>::iterator edgesbegin(){//map - > umap
		return edges.begin();
	}

	map<field,list<int>>::iterator edgesend(){//map - > umap
		return edges.end();
	}

	list<int>::iterator edgesbegin(field &f){
		return edges[f].begin();
	}

	list<int>::iterator edgesend(field &f){
		return edges[f].end();
	}

	int edgesNumber(const field &f){
		return edges[f].size();
	}

	void append(const field &f,list<int> &l){
		edges[f].splice(edges[f].end(),l);  
		// assert(l.size()==0);
	}

	list<int>::iterator remove(field &f, list<int>::iterator pos){
		return edges[f].erase(pos);
	}

	void clear(field &f){
		edges.erase(f);
	}

	void clear(){
		edges.clear(); //calls both key and value destructors
	}

	void addedge(field &f,int vid){
		edges[f].push_back(vid);
	}

	void set(field &f,int vtx_id){
		edges[f] = list<int>(1,vtx_id);
	}

	list<int>& getedgeList(field &f){
		return edges[f];
	}

	void printvtxid(){
		cout<<"vertex id : "<<id<<endl;
		cout<<"different fields are "<<edges.size()<<endl;
		// cout<<"size of eps is "<<edges[field(0,"eps")].size()<<endl;
	}

};

#endif