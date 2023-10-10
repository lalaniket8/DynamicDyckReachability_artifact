#ifndef DDR_H
#define DDR_H

#include <Helper.h>

using namespace std;

class DynamicDyckReachability {//containing the major functionality of our paper
private:
    DynamicConnectivity* graphHDT;
    graph graphOffline;
    double elapsed = 0;
    
    NUM maxNoOfNodes = 0;
    bool HUSH = false;

    unordered_set<tuple<NUM,NUM,NUM>, boost::hash<tuple<NUM,NUM,NUM>>> directedEdgeSet; //tuple format : fromNode,toNode,edgeType
    unordered_map<pair<NUM,NUM>, NUM, boost::hash<pair<NUM,NUM>>> edgeCounter; //<fromNode,toNode> : count (maintains edge count for sparsification)
    unordered_map<NUM,unordered_set<NUM>, boost::hash<NUM>> nodeToEdgeTypeMap = {};
    unordered_map<pair<NUM,NUM>,list<NUM>, boost::hash<pair<NUM,NUM>>> outEdges = {}; //<fromNode,edgeType> : list<toNode>
    unordered_map<tuple<NUM,NUM,NUM>,list<NUM>::iterator, boost::hash<tuple<NUM,NUM,NUM>>> pointerToOutEdges = {}; //maps edges to its entry in outedges
    bool getRootOptimization = false; // graphHDT getRoot optimization flag
    queue<NUM> components;//contains VertexAA ids of root of dsu in offline algo
    unordered_set<NUM, boost::hash<NUM>> visitedComponents;// to make sure we dont mark the same component twice for breakage
    unordered_map<pair<NUM,NUM>,list<NUM>, boost::hash<pair<NUM,NUM>>> inPrimary = {}; //<toNode,edgeType> : list<fromNode>
    unordered_map<NUM,unordered_set<NUM>, boost::hash<NUM>> inPrimaryEdgeTypeMap = {}; //toNode : unordered_set<edgeType>

public:
    int MAX_COMP_SIZE = 0;
    NUM SUM_COMP_SIZE = 0;
    NUM NO_OF_COMP = 0;
    NUM totalWorklistSize_for_delete = 0;

	DynamicDyckReachability(NUM maxNoOfNodes, bool HUSH_value);

    void insertDirectedEdge(NUM fromNodeID, NUM toNodeID, NUM edgeTypeID);
    NUM deleteDirectedEdge(NUM fromNodeID, NUM toNodeID, NUM edgeTypeID);//, bool displayStepwiseTimeDuration);

    void deleteUndirectedEdge(NUM node1, NUM node2);
    void insertUndirectedEdge(NUM node1, NUM node2);
    void queryComponents();
    bool isReachable(NUM node1, NUM node2);
    void displayOutEdges(unordered_map<pair<NUM,NUM>,list<NUM>, boost::hash<pair<NUM,NUM>>> outEdges);
    void displayPointerToOutedges(unordered_map<tuple<NUM,NUM,NUM>,list<NUM>::iterator, boost::hash<tuple<NUM,NUM,NUM>>> pointerToOutEdges);
    void displayDirectedEdgeSet(set<tuple<NUM,NUM,NUM>> directedEdgeSet);
    void displayEdgeCounter();
    void displayNodeToEdgeMap(unordered_map<NUM,unordered_set<NUM>> nodeToEdgeTypeMap);
    void graphOffline_show();
    void graphHDT_show();

    NUM getNoOfPrimaryComp();
    double getAvgSizeOfPrimaryComp();
    double getAvgSizeOfPrimaryComp_sizeGreaterThan1();
    NUM getTotalNoOfComp();
    
    void clean();
};

#endif