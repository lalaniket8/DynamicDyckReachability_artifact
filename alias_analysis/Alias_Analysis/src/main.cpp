#include "graph/graph.h"
#include "graph/Ngraph.h"
#include <iostream>
#include <time.h>
#include <sys/time.h>

void show(graph graphOffline);

int main(int argc, const char * argv[]){
	if(argc!=2){
		cerr<<"the argument should be path to file containing spg graph"<<endl;
		return 1;
	}
	// Bidirected Reach Algorithm
	graph g;
	clock_t time;
	struct timeval tv1,tv2;
	g.construct(argv[1]);
	g.initWorklist();
	
	gettimeofday(&tv1,NULL);
	time = clock();
	g.bidirectedReach();
	time = clock()-time;
	gettimeofday(&tv2,NULL);	

	g.printReach();
	show(g);

	// time required for bidirectedReach
	cout<<"\nBidirected Reach Algorithm"<<endl;
	cout<<"\tTime recorded in seconds : "<<(double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +(double) (tv2.tv_sec - tv1.tv_sec)<<"\n\n";

	// previous dyck Reachability Algorithm
	cout<<"previous existing Algorithm"<<endl;
	Ngraph ng;   // 
	clock_t ntime;
	struct timeval ntv1,ntv2;
	ng.construct(argv[1]);
	ng.initWorklist();

	gettimeofday(&ntv1,NULL);
	ntime = clock();
	ng.dyck_reach();
	ntime = clock()-ntime;
	gettimeofday(&ntv2,NULL);
	

	// time required for dyckReach
	cout<<"\tTime recorded in seconds : "<<(double) (ntv2.tv_usec - ntv1.tv_usec) / 1000000 +(double) (ntv2.tv_sec - ntv1.tv_sec)<<endl;
}


void show(graph graphOffline){
	
    cout << "vertices:" << graphOffline.vertices.size() << endl;
    cout << "vtxatpos:" << graphOffline.dsu.vtxatpos.size() << endl;
    cout << "parent_pos:" << graphOffline.dsu.parent_pos.size() << endl;
    cout << "posofvtx:" << graphOffline.dsu.posofvtx.size() << endl;
    cout << "dsu.N:" << graphOffline.dsu.getN() << endl;

    for(VertexAA* v : graphOffline.vertices){
		cout << "Vertex name:" << v->name << " id:" << v->id << endl;
		for(auto it = v->edgesbegin(); it != v->edgesend(); it++){
			cout << "\tfield:" << it->first.field_name << "[";
			for(list<int>::iterator it1 = it->second.begin(); it1!= it->second.end(); it1++){
				int64_t vertexid = (*it1);
				cout << vertexid << ",";
			}
			cout << "]" << endl;
		}		
	}
    cout << endl;

    for(VertexAA* v : graphOffline.vertices){
        string vertexName = v->name;
		cout << "Vertex name:" << vertexName << endl;
		for(auto it = v->edgesbegin(); it != v->edgesend(); it++){
            string fieldName = it->first.field_name;
            if(it->second.size() > 0){
                for(list<int>::iterator it1 = it->second.begin(); it1!= it->second.end(); it1++){
                    int64_t vertexid = (*it1);
                    string toVertexName = graphOffline.vertices[vertexid]->name;
                    cout << vertexName << "->" << toVertexName << "(" << fieldName << ")" << endl;
                }
            }
		}		
	}
    cout << endl;

	cout << "dsu contents:" << endl;
	for(int i=0; i<graphOffline.vertices.size(); i++){
		
        VertexAA* vertexAAPointer = graphOffline.vertices[i];
		cout << "Root of " << vertexAAPointer->name << " is ";
        int root = graphOffline.dsu.root(vertexAAPointer->id);
        VertexAA* rootVertexAAPointer = graphOffline.vertices[root];

        cout << "(" << rootVertexAAPointer->name << ")" << endl;
	}
	cout << endl;
	cout << "worklist.size():" << graphOffline.worklist.size() << "):" << endl;

	queue<pair<int,field>> wl = graphOffline.worklist;
	while (!wl.empty()){
		// cout << ":" << endl;
		pair<int,field> el = wl.front();
		cout << "{" << el.first << "," << el.second.field_name <<"},";
		wl.pop();
	}
}