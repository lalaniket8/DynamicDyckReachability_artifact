#include "graph/graph.h"
#include "graph/Ngraph.h"
#include <iostream>
#include <time.h>
#include <sys/time.h>

std::set<string> edges_set;
std::set<string> nodes_set;
std::set<string> edgeType_set;

void read_InitGraph(string initGraph, set<string>* nodes_set, set<string>* edges_set,  set<string>* edgeType_set){
	ifstream initGraph_infile(initGraph);
	string op, fromNode, toNode, edge, temp = "";
	while(initGraph_infile >> op >> fromNode >> toNode >> edge){
		
		if(op.length() <= 0 || fromNode.length() <= 0 || toNode.length() <= 0 || edge.length() <= 0){
			cout << "Incorrect input:" << endl;
			cout << "OP:" << op << endl;
			cout << "fromNode:" << fromNode << endl;
			cout << "toNode:" << toNode << endl;
			cout << "edge:" << edge << endl;
			exit(0);
		}

		std::vector<string> edgeInfoVector;
		split(edge,"--",edgeInfoVector);

		if(edgeInfoVector.size() != 2 || edgeInfoVector[0].length() <= 0 || edgeInfoVector[0].length() <= 0 || (edgeInfoVector[0] != "op" && edgeInfoVector[0] != "cp")){
			std::cout << "Incorrect edge info:" << edge << endl;
			exit(0);
		}

		if(edgeInfoVector[0] == "op"){
			temp = fromNode;
			fromNode = toNode;
			toNode = temp;
		}

		if(op == "A"){
			edges_set->insert(fromNode+"||"+toNode+"||"+edgeInfoVector[1]);
			nodes_set->insert(fromNode);
			nodes_set->insert(toNode);
			edgeType_set->insert(edgeInfoVector[1]);
		}else if(op == "D"){
			edges_set->erase(fromNode+"||"+toNode+"||"+edgeInfoVector[1]);
		}else{
			cout << "Incorrect Operator:" << op << endl;
			exit(0);
		}

	}
}

void read_dotfile(string initGraph, set<string>* nodes_set, set<string>* edges_set,  set<string>* edgeType_set){
	ifstream initGraph_infile(initGraph);
	string op, fromNode, toNode, edge, temp = "", line;
	while(initGraph_infile >> line){//op >> fromNode >> toNode >> edge){
		//123->345[label="op--123"]
		int arrowPos = line.find("->");
		int bracketOpenPos = line.find("[");
		int edgeDetStartPos = line.find("\"");
		int edgeDetEndPos = line.find("\"]");
		if(arrowPos == string::npos || bracketOpenPos == string::npos || edgeDetStartPos == string::npos || edgeDetEndPos == string::npos){
			cout << "Incorrect input:" << line << endl;exit(1);
		}
		if(!((arrowPos < bracketOpenPos) && (bracketOpenPos < edgeDetStartPos) && (edgeDetStartPos < edgeDetEndPos))){
			cout << "Incorrect input:" << line << endl;exit(1);
		}
		fromNode = line.substr(0,arrowPos);
		toNode = line.substr(arrowPos+2, bracketOpenPos-arrowPos-2);
		edge = line.substr(edgeDetStartPos+1, edgeDetEndPos-edgeDetStartPos-1);

		std::vector<string> edgeInfoVector;
		split(edge,"--",edgeInfoVector);

		if(edgeInfoVector.size() != 2 || edgeInfoVector[0].length() <= 0 || edgeInfoVector[0].length() <= 0 || (edgeInfoVector[0] != "op" && edgeInfoVector[0] != "cp")){
			std::cout << "Incorrect edge info:" << edge << endl;
			exit(0);
		}

		if(edgeInfoVector[0] == "op"){
			temp = fromNode;
			fromNode = toNode;
			toNode = temp;
		}

		edges_set->insert(fromNode+"||"+toNode+"||"+edgeInfoVector[1]);
		nodes_set->insert(fromNode);
		nodes_set->insert(toNode);
		edgeType_set->insert(edgeInfoVector[1]);
	
	}
}

int main(int argc, const char * argv[]){

	double total_duration = 0;

	if(argc!=4){
		cerr<<"the argument should be path to file containing spg graph"<<endl;
		return 1;
	}
	
	bool SHOW_SOLUTION = false;
	if(argv[3][0] == '1'){SHOW_SOLUTION = true;} //compute the final graph G_n directly, and show solution for it , this will be used for correctness check

	//read initial graph into nodes_set and edges_set
	// read_InitGraph
	read_dotfile(argv[1], &nodes_set, &edges_set, &edgeType_set);
	ifstream seqfile_infile(argv[2]);
	string op, fromNode, toNode, edge, temp = "";
	int seqNo = 0;
	while(seqfile_infile >> op >> fromNode >> toNode >> edge){

		seqNo++;

		if(op.length() <= 0 || fromNode.length() <= 0 || toNode.length() <= 0 || edge.length() <= 0){
			cout << "Incorrect input:" << endl;
			cout << "OP:" << op << endl;
			cout << "fromNode:" << fromNode << endl;
			cout << "toNode:" << toNode << endl;
			cout << "edge:" << edge << endl;
			exit(0);
		}

		std::vector<string> edgeInfoVector;
		split(edge,"--",edgeInfoVector);

		if(edgeInfoVector.size() != 2 || edgeInfoVector[0].length() <= 0 || edgeInfoVector[0].length() <= 0 || (edgeInfoVector[0] != "op" && edgeInfoVector[0] != "cp")){
			std::cout << "Incorrect edge info:" << edge << endl;
			exit(0);
		}

		if(edgeInfoVector[0] == "op"){
			temp = fromNode;
			fromNode = toNode;
			toNode = temp;
		}

		if(op == "A"){
			edges_set.insert(fromNode+"||"+toNode+"||"+edgeInfoVector[1]);
			nodes_set.insert(fromNode);
			nodes_set.insert(toNode);
			edgeType_set.insert(edgeInfoVector[1]);
		}else if(op == "D"){
			edges_set.erase(fromNode+"||"+toNode+"||"+edgeInfoVector[1]);
		}else{
			cout << "Incorrect Operator:" << op << endl;
			exit(0);
		}

		// call Bidirected Reach Algorithm
		if(!SHOW_SOLUTION){
			graph g;
			struct timeval tv1,tv2;

			g.processInput(&nodes_set, &edges_set);

			gettimeofday(&tv1,NULL);

			g.initWorklist();
			g.bidirectedReach();

			gettimeofday(&tv2,NULL);	
			//g.printReach();
			double duration = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +(double) (tv2.tv_sec - tv1.tv_sec);
			total_duration += duration;

			g.clean();
		}
		
	}
	if(!SHOW_SOLUTION){
		// cout << "Nodes:" << nodes_set.size() << endl;
		// cout << "Edges:" << edges_set.size() << endl;
		// cout << "Fields:" << edgeType_set.size() << endl;
		cout << "Total_duration:" << total_duration << endl;
	}else{
		graph g;
		g.processInput(&nodes_set, &edges_set);
		g.initWorklist();
		g.bidirectedReach();
		map<int, set<string>> comp = g.getComponents();
		for(map<int, set<string>>::iterator iter = comp.begin(); iter != comp.end(); iter++){
			set<string> comp_str = iter->second;
			for(string node_str : comp_str){
				cout << node_str << ",";
			}
			cout << endl;
		}
	}

	// previous dyck Reachability Algorithm
	//cout<<"previous existing Algorithm"<<endl;
	//Ngraph ng;   // 
	//clock_t ntime;
	//struct timeval ntv1,ntv2;
	//ng.construct(argv[1]);
	//ng.initWorklist();

	//gettimeofday(&ntv1,NULL);
	//ntime = clock();
	//ng.dyck_reach();
	//ntime = clock()-ntime;
	//gettimeofday(&ntv2,NULL);
	

	// time required for dyckReach
	//cout<<"\tTime recorded in seconds : "<<(double) (ntv2.tv_usec - ntv1.tv_usec) / 1000000 +(double) (ntv2.tv_sec - ntv1.tv_sec)<<endl;
}
