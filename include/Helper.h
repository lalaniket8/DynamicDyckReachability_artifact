#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <queue>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <bits/stdc++.h>
#include <time.h>
#include <sys/time.h>
#include <dynamic_graph/dynamic_connectivity.hpp>
#include "graph/graph.h"
#include "graph/Ngraph.h"
#include <boost/functional/hash.hpp>
#include <boost/algorithm/string.hpp>

typedef int64_t NUM;
typedef struct timeval timestruct;

static hash<string> hasher;

static std::unordered_map<std::string, NUM> NODES = {};
static std::unordered_map<std::string, NUM> EDGETYPE = {};

class Helper{

    public:
        struct struct_edgeop
        {
            char oper;
            NUM fromNode;
            NUM toNode;
            NUM edgeType;
            bool computeTime; //when true, computes the time taken when processing this operation
        };
        typedef struct struct_edgeop EdgeOperation;

        static std::queue<EdgeOperation> parseGraphInputFromFile(std::string input_initgraph_filename, string input_seq_filename, NUM* NodeCount, NUM* EdgeTypeCount, NUM* EdgeCount_initgraph, NUM* SeqCount);

        static std::string nodeIDtoName(NUM nodeID);
        static std::string edgeTypeIDtoName(NUM edgeTypeID);
        static std::string edgeTypeIDtoName(string edgeTypeID_str);
        static void displayNodes_Edges();
        static double getDuration(timestruct *tv1, timestruct *tv2);

};

#endif