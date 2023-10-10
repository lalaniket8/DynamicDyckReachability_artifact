#include <Helper.h>

using namespace std;

void Helper::displayNodes_Edges(){

    for(unordered_map<string,NUM>::iterator it = NODES.begin(); it != NODES.end(); it++){
        cout << "NodeID::"<< it->first <<":"<< it->second << endl;
    }
    for(unordered_map<string,NUM>::iterator it = EDGETYPE.begin(); it != EDGETYPE.end(); it++){
        cout << "EdgeTypeID::"<< it->first <<":"<< it->second << endl;
    }
}


double Helper::getDuration(timestruct *begin, timestruct *end){
    return (double) (end->tv_usec - begin->tv_usec) / 1000000 + (double) (end->tv_sec - begin->tv_sec);
}

queue<Helper::EdgeOperation> Helper::parseGraphInputFromFile(string input_initgraph_filename, string input_seq_filename, NUM* NodeCount, NUM* EdgeTypeCount, NUM* EdgeCount_initgraph, NUM* SeqCount){

    //process initgraph
    ifstream inputstream_initgraph(input_initgraph_filename);
    if(inputstream_initgraph.fail()){
        cout << ":Unable to read file:" << input_initgraph_filename << " (check file name, permissions, sharing violations, etc)" << endl;
        exit(1);
    }
    ifstream inputstream_seq(input_seq_filename);
    if(inputstream_seq.fail()){
        cout << ":Unable to read file:" << input_seq_filename << " (check file name, permissions, sharing violations, etc)" << endl;
        exit(1);
    }

    queue<EdgeOperation> inputQueue;
    char oper;
    string line,operatorString,fromNode, toNode, edgeType, temp, edgeType_dets; 
    NUM NODE_COUNT = 0;
    NUM EDGETYPE_COUNT = 0;
    NUM EDGECOUNT_INITGRAPH = 0;
    NUM SEQCOUNT = 0;

    while (inputstream_initgraph >> line)
    {
        int arrowPos = line.find("->");
		int bracketOpenPos = line.find("[");
		int edgeDetStartPos = line.find("\"");
		int edgeDetEndPos = line.find("\"]");
		if(arrowPos == string::npos || bracketOpenPos == string::npos || edgeDetStartPos == string::npos || edgeDetEndPos == string::npos){
			cout << "Incorrect input:" << line << endl; exit(1);
		}
		if(!((arrowPos < bracketOpenPos) && (bracketOpenPos < edgeDetStartPos) && (edgeDetStartPos < edgeDetEndPos))){
			cout << "Incorrect input:" << line << endl; exit(1);
		}
		fromNode = line.substr(0,arrowPos);
		toNode = line.substr(arrowPos+2, bracketOpenPos-arrowPos-2);
		edgeType = line.substr(edgeDetStartPos+1, edgeDetEndPos-edgeDetStartPos-1);
		
        //skip commented lines (prepended with #)
        if(line.size() == 0 || line.at(0) == '#')continue;

        NUM fromNodeID,toNodeID,edgetypeID;
        EDGECOUNT_INITGRAPH++;

        //validate edge 
        if(edgeType.length() < 5 || (edgeType.substr(0,4).compare("op--") != 0 && edgeType.substr(0,4).compare("cp--") != 0)){
            cout << "incorrect edge:" << edgeType << endl;
            cout << "edge should be of the format 'op--12345' or 'cp--12345' where op denotes opening paranthesis and cp denotes closing paranthesis, and 12345 is the edge type." << endl;
            exit(0);
        }

        //edgeType:"op--123" or "cp--123"
        if(edgeType.substr(0,2).compare("op") == 0){// reverse edge if given paranthesis is opening parenthesis
            temp = fromNode;
            fromNode = toNode;
            toNode = temp;
        }

        edgeType_dets = edgeType.substr(4,edgeType.length()-4);

        //fromNode
        if(NODES.find(fromNode) == NODES.end()){
            NODES[fromNode] = NODE_COUNT;
            fromNodeID = NODE_COUNT;
            NODE_COUNT++;
        }else{
            fromNodeID = NODES.find(fromNode)->second;
        }

        //toNode
        if(NODES.find(toNode) == NODES.end()){
            NODES[toNode] = NODE_COUNT;
            toNodeID = NODE_COUNT;
            NODE_COUNT++;
        }else{
            toNodeID = NODES.find(toNode)->second;
        }

        //edgeType_dets
        if(EDGETYPE.find(edgeType_dets) == EDGETYPE.end()){
            EDGETYPE[edgeType_dets] = EDGETYPE_COUNT;
            edgetypeID = EDGETYPE_COUNT;
            EDGETYPE_COUNT++;
        }else{
            edgetypeID = EDGETYPE.find(edgeType_dets)->second;
        }

        EdgeOperation edgeOp = {'A', fromNodeID, toNodeID, edgetypeID, false}; // dont calculate time for edges present in initgraph

        inputQueue.push(edgeOp);
    }

    while (inputstream_seq >> operatorString >> fromNode >> toNode >> edgeType){

        oper = operatorString.at(0);

        //skip commented lines (prepended with #)
        if(oper == '#')continue;

        NUM fromNodeID,toNodeID,edgetypeID;
        if (oper == 'A' || oper == 'D'){
            SEQCOUNT++;
            
            //validate edge 
            if(edgeType.length() < 5 || (edgeType.substr(0,4).compare("op--") != 0 && edgeType.substr(0,4).compare("cp--") != 0)){
                cout << "incorrect edge:" << edgeType << endl;
                cout << "edge should be of the format 'op--12345' or 'cp--12345' where op denotes opening paranthesis and cp denotes closing paranthesis, and 12345 is the edge type." << endl;
                exit(0);
            }

            //edgeType:"op--123" or "cp--123"
            if(edgeType.substr(0,2).compare("op") == 0){// reverse edge if given paranthesis is opening parenthesis
                temp = fromNode;
                fromNode = toNode;
                toNode = temp;
            }

            edgeType_dets = edgeType.substr(4,edgeType.length()-4);

            //fromNode
            if(NODES.find(fromNode) == NODES.end()){
                NODES[fromNode] = NODE_COUNT;
                fromNodeID = NODE_COUNT;
                NODE_COUNT++;
            }else{
                fromNodeID = NODES.find(fromNode)->second;
            }

            //toNode
            if(NODES.find(toNode) == NODES.end()){
                NODES[toNode] = NODE_COUNT;
                toNodeID = NODE_COUNT;
                NODE_COUNT++;
            }else{
                toNodeID = NODES.find(toNode)->second;
            }

            //edgeType_dets
            if(EDGETYPE.find(edgeType_dets) == EDGETYPE.end()){
                EDGETYPE[edgeType_dets] = EDGETYPE_COUNT;
                edgetypeID = EDGETYPE_COUNT;
                EDGETYPE_COUNT++;
            }else{
                edgetypeID = EDGETYPE.find(edgeType_dets)->second;
            }

            EdgeOperation edgeOp = {oper, fromNodeID, toNodeID, edgetypeID, true}; // calculate time for edges present in initgraph

            inputQueue.push(edgeOp);
        }
    }

    *NodeCount = NODES.size();
    *EdgeTypeCount = EDGETYPE.size(); 
    *EdgeCount_initgraph = EDGECOUNT_INITGRAPH; 
    *SeqCount = SEQCOUNT;

    return inputQueue;
}

string Helper::nodeIDtoName(NUM nodeID){
    for(const auto& it : NODES){
        if(it.second == nodeID){
            return it.first;
        }
    }
    cout << "ERROR nodeIDtoName() cannot find nodeID:" << nodeID << endl;
    return "";
}

string Helper::edgeTypeIDtoName(string edgeTypeID_str){
    if(edgeTypeID_str.compare("EPS") == 0 || edgeTypeID_str.compare("eps") == 0){
        return edgeTypeID_str;
    }else{
        return Helper::edgeTypeIDtoName(stoi(edgeTypeID_str)); 
    }
}

string Helper::edgeTypeIDtoName(NUM edgeTypeID){
    for(const auto& it : EDGETYPE){
        if(it.second == edgeTypeID){
            return it.first;
        }
    }
    cout << "ERROR edgeTypeIDtoName() cannot find edgeTypeID:" << edgeTypeID << endl;
    return "";
}