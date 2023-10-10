#include <DynamicDyckReachability.h>

DynamicDyckReachability::DynamicDyckReachability(NUM maxNoOfNodes, bool HUSH_value){
    HUSH = HUSH_value; // This flag controls verbosity, when set to True the print statements are hushed.

    this->maxNoOfNodes = maxNoOfNodes;
    graphHDT = new DynamicConnectivity(maxNoOfNodes); //Implementation of Holm et al. 2001 (maintained at Tseng 2020), provides a datastructure for maintaining dynamic reachability on undirected graphs, referred to as PrimCompDS in the paper

    if(!HUSH) cout << "maxNoOfNodes:" << maxNoOfNodes << endl;

    graphOffline = graph(); //Chatterjee et al. 2018's Offline algo implementation, contains code for Fixpoint() function
    graphOffline.dsu.init(maxNoOfNodes);
    
    visitedComponents.clear();
    
}

void DynamicDyckReachability::clean(){// to avoid mem leaks
    delete graphHDT;
    graphOffline.clean();
}

void DynamicDyckReachability::insertDirectedEdge(NUM fromNodeID, NUM toNodeID, NUM edgeTypeID){
    if(!HUSH) cout << "***insert start***" << endl;

    //if edge is already present, exit
    if(directedEdgeSet.find({fromNodeID,toNodeID,edgeTypeID}) != directedEdgeSet.end()){ 
        // if(!HUSH) cout << "directed edge already present:" << Helper::nodeIDtoName(fromNodeID) << "," << Helper::nodeIDtoName(fromNodeID) << Helper::edgeTypeIDtoName(edgeTypeID)<< endl;
        return;
    }

    //add edge to directedEdgeSet
    directedEdgeSet.insert({fromNodeID,toNodeID,edgeTypeID});
    
    //add edgeTypeID to nodeToEdgeTypeMap for fromNode
    auto nodeToEdgeTypeIter = nodeToEdgeTypeMap.find(fromNodeID);
    if(nodeToEdgeTypeIter != nodeToEdgeTypeMap.end()){
        nodeToEdgeTypeIter->second.insert(edgeTypeID);
    }else{
        nodeToEdgeTypeMap[fromNodeID] = {edgeTypeID};
    }

    NUM y = -1; //dest node of fromNodeID on edge edgeTypeID // head of outEdges

    bool is_outedges_empty = false;

    auto outEdgesTupleFindIter = outEdges.find({fromNodeID,edgeTypeID});
    if(outEdgesTupleFindIter != outEdges.end()){
        if((outEdgesTupleFindIter->second).size() > 0){
            y = (outEdgesTupleFindIter->second).front();
        }else{is_outedges_empty = true;}
    }else{
        outEdges[{fromNodeID,edgeTypeID}] = {};
        is_outedges_empty = true;
    }

    if(is_outedges_empty){
        //update inPrimary
        auto inPrimaryTupleFindIter = inPrimary.find({toNodeID,edgeTypeID});
        if(inPrimaryTupleFindIter != inPrimary.end()){
            inPrimaryTupleFindIter->second.push_front(fromNodeID);
        }else{
            inPrimary[{toNodeID,edgeTypeID}] = {fromNodeID};

            //updating inPrimaryEdgeTypeMap
            auto inPrimaryEdgeTypeMapIter = inPrimaryEdgeTypeMap.find(toNodeID);
            if(inPrimaryEdgeTypeMapIter == inPrimaryEdgeTypeMap.end()){
                inPrimaryEdgeTypeMap[toNodeID] = {edgeTypeID};
            }else{
                inPrimaryEdgeTypeMapIter->second.insert(edgeTypeID);
            }
        }
    }

    (outEdges.find({fromNodeID,edgeTypeID})->second).push_front(toNodeID);
    
    tuple<NUM,NUM,NUM> entry = {fromNodeID,toNodeID,edgeTypeID};
    auto iter = outEdges[{fromNodeID,edgeTypeID}].begin();
    pointerToOutEdges.insert({entry, iter}); 

    if(y != -1){
        NUM tuple_el1, tuple_el2;

        // maintaining invariant : undirected edge (a,b) in undirected graph datastructure has the id values such that a < b
        if (y < toNodeID){
            tuple_el1 = y; 
            tuple_el2 = toNodeID;
        }else{
            tuple_el2 = y; 
            tuple_el1 = toNodeID;
        }
        insertUndirectedEdge(tuple_el1,tuple_el2);
    }

    //OfflineAlgo insert edge(root of fromNodeID in dsu tree, root of toNodeID in dsu tree, edgeTypeID)
    //OfflineAlgo maintains nodes in VertexAA.name
    //graphOffline.dsu uses VertexAA.id
    NUM dsuRoot_fromNode_VertexAAId = graphOffline.dsu.root(graphOffline.getVertex(fromNodeID)->id);
    NUM dsuRoot_toNode_VertexAAId = graphOffline.dsu.root(graphOffline.getVertex(toNodeID)->id);
    field& fieldRef = graphOffline.getfield(edgeTypeID);

    if(!HUSH) cout << "Adding directed edge in offline graph " << Helper::nodeIDtoName(graphOffline.vertices[dsuRoot_fromNode_VertexAAId]->name) << "->" << Helper::nodeIDtoName(graphOffline.vertices[dsuRoot_toNode_VertexAAId]->name) << "(" << Helper::edgeTypeIDtoName(edgeTypeID) << ")" << endl; 
    graphOffline.addedge(graphOffline.vertices[dsuRoot_fromNode_VertexAAId],graphOffline.vertices[dsuRoot_toNode_VertexAAId],fieldRef);
    graphOffline.worklist.push(pair<int,field>(graphOffline.vertices[dsuRoot_fromNode_VertexAAId]->id,fieldRef));
    
    //compute fixpoint
    if(!HUSH) cout << "bidirectedReach" << endl;
    graphOffline.bidirectedReach();

    if(!HUSH) cout << "***insert end***" << endl;

    return;
}

NUM DynamicDyckReachability::deleteDirectedEdge(NUM fromNodeID, NUM toNodeID, NUM edgeTypeID){
    if(!HUSH) cout << "***Delete start***" << endl;
    struct timeval timeBegin, timeEnd;

    //STEP:initilize datastructures
    //if edge is not present, exit
    auto edge_iter = directedEdgeSet.find({fromNodeID,toNodeID,edgeTypeID});
    if(edge_iter == directedEdgeSet.end()){ 
        if(!HUSH) cout << "directed edge not present:" << Helper::nodeIDtoName(fromNodeID) << "," << Helper::nodeIDtoName(toNodeID) << Helper::edgeTypeIDtoName(edgeTypeID)<< endl;
        return -1;
    }
    
    //reinitizialize visitedComponents data strucutre
    visitedComponents.clear(); 

    //delete edge from directedEdgeSet
    directedEdgeSet.erase(edge_iter);

    auto pointer_toNodeID_ListEntry = pointerToOutEdges[{fromNodeID,toNodeID,edgeTypeID}];

    //STEP:updating PrimCompDS, while maintaining sparsification
    bool isToNodeIDfirstInList = false;
    bool isToNodeIDlastInList = false;
    list<NUM>::iterator pointerTo_previousListEntry;
    list<NUM>::iterator pointerTo_nextListEntry;
    if(outEdges[{fromNodeID,edgeTypeID}].size() == 0){
        cout << "outEdges[{" << fromNodeID << "," << edgeTypeID << "}] is empty" << endl;
        exit(1);
    }else if(outEdges[{fromNodeID,edgeTypeID}].size() == 1){
        isToNodeIDfirstInList = true;
        isToNodeIDlastInList = true;
    }else{
        NUM firstElementInList = outEdges[{fromNodeID,edgeTypeID}].front(); 
        NUM lastElementInList = outEdges[{fromNodeID,edgeTypeID}].back(); 
        if(*pointer_toNodeID_ListEntry == firstElementInList){//toNode is first in list
            isToNodeIDfirstInList = true;
            isToNodeIDlastInList = false;
            //initialize pointerTo_nextListEntry
            pointerTo_nextListEntry = pointer_toNodeID_ListEntry;
            pointerTo_nextListEntry++;
        }else if(*pointer_toNodeID_ListEntry == lastElementInList){//toNode is last in list
            isToNodeIDfirstInList = false;
            isToNodeIDlastInList = true;
            // initialize pointerTo_previousListEntry
            pointerTo_previousListEntry = pointer_toNodeID_ListEntry;
            pointerTo_previousListEntry--;
        }else{// toNode is neither at first nor at last position in the list
            isToNodeIDfirstInList = false;
            isToNodeIDlastInList = false;
            pointerTo_previousListEntry = pointer_toNodeID_ListEntry;
            pointerTo_previousListEntry--;
            pointerTo_nextListEntry = pointer_toNodeID_ListEntry;
            pointerTo_nextListEntry++;
        }
    }

    // check if undirected edge (previousListEntry,toNodeID) is present
    bool undirectedEdge_prevNodeID_toNodeID_isPresent = false;
    if(!isToNodeIDfirstInList){
        if(*pointerTo_previousListEntry < toNodeID){
            undirectedEdge_prevNodeID_toNodeID_isPresent = graphHDT->HasEdge(UndirectedEdge(*pointerTo_previousListEntry,toNodeID));
        }else{
            undirectedEdge_prevNodeID_toNodeID_isPresent = graphHDT->HasEdge(UndirectedEdge(toNodeID,*pointerTo_previousListEntry));
        }
    }
    
    // check if undirected edge (nextListEntry, toNodeID) is present
    bool undirectedEdge_nextNodeID_toNodeID_isPresent = false;
    if(!isToNodeIDlastInList){
        if(*pointerTo_nextListEntry < toNodeID){
            undirectedEdge_nextNodeID_toNodeID_isPresent = graphHDT->HasEdge(UndirectedEdge(*pointerTo_nextListEntry,toNodeID));
        }else{
            undirectedEdge_nextNodeID_toNodeID_isPresent = graphHDT->HasEdge(UndirectedEdge(toNodeID,*pointerTo_nextListEntry));
        }
    }
    
    // delete undirected edge (previousListEntry,toNodeID) if present
    if(undirectedEdge_prevNodeID_toNodeID_isPresent){
        NUM tuple_el1, tuple_el2;
        if(*pointerTo_previousListEntry < toNodeID){
            tuple_el1 = *pointerTo_previousListEntry;
            tuple_el2 = toNodeID;
        }else{
            tuple_el1 = toNodeID;
            tuple_el2 = *pointerTo_previousListEntry;
        }
        deleteUndirectedEdge(tuple_el1,tuple_el2);
    }

    // delete undirected edge (nextListEntry,toNodeID) if present
    if(undirectedEdge_nextNodeID_toNodeID_isPresent){
        NUM tuple_el1, tuple_el2;
        if(*pointerTo_nextListEntry < toNodeID){
            tuple_el1 = *pointerTo_nextListEntry;
            tuple_el2 = toNodeID;
        }else{
            tuple_el1 = toNodeID;
            tuple_el2 = *pointerTo_nextListEntry;
        }
        deleteUndirectedEdge(tuple_el1,tuple_el2);
    }

    // add undirected edge (previousListEntry,nextListEntry) if edges to both were previously present
    if(undirectedEdge_prevNodeID_toNodeID_isPresent && undirectedEdge_nextNodeID_toNodeID_isPresent){
        NUM tuple_el1, tuple_el2;
        if(*pointerTo_previousListEntry < *pointerTo_nextListEntry){
            tuple_el1 = *pointerTo_previousListEntry;
            tuple_el2 = *pointerTo_nextListEntry;
        }else{
            tuple_el1 = *pointerTo_nextListEntry;
            tuple_el2 = *pointerTo_previousListEntry;
        }
        insertUndirectedEdge(tuple_el1,tuple_el2);
    }
    
    //STEP:forward search on the components, while breaking them simultaneously 
    int toNodeRootVertexAAID = graphOffline.dsu.root(graphOffline.getVertex(toNodeID)->id);
    components.push(toNodeRootVertexAAID); //contain id of VertexAA 
    
    visitedComponents.insert(graphOffline.getVertexAAPointerfromVertexAAID(toNodeRootVertexAAID)->name);
    
    NUM brokenNodeCount = 0;

    unordered_set<int> brokenNodeVertexAAIDs = {};
    unordered_set<NUM> brokenNodeIDs = {};

    while(!components.empty()){
        
        NUM curr_comp_id = components.front();//VertexAA id
        VertexAA* curr_comp = graphOffline.getVertexAAPointerfromVertexAAID(curr_comp_id);
        components.pop();

        // get the nodes of dsu with root curr_comp_id
        // unordered_set<int> dsu_nodes = graphOffline.dsu.getDSUContent(curr_comp_id);
        // graphOffline.dsu.root(curr_comp_id);
        int curr_comp_id_pos = graphOffline.dsu.posofvtx[curr_comp_id];
        list<int> dsu_nodes_posofvtx = graphOffline.dsu.dsuContent[curr_comp_id_pos];

        // SUM_COMP_SIZE += dsu_nodes_posofvtx.size();
        // NO_OF_COMP++;
        // if(dsu_nodes_posofvtx.size() > MAX_COMP_SIZE){MAX_COMP_SIZE = dsu_nodes_posofvtx.size();}

        // in the original graph count number of outgoing edges per field for the nodes in this component
        unordered_map<NUM,NUM, boost::hash<NUM>> curr_comp_outgoingEdgesCount_by_field = {};
        
        // for(int _i = 0; _i < dsu_nodes_posofvtx->size(); _i++){
        for(const int dsu_node_posofvtx : dsu_nodes_posofvtx){//VertexAA id
        // for each node in the current component we iterate over each outgoing edge only once, marking components for breakage 
        // no component is marked for breakage more than once
        // (marking for breakage means inserting the root of that component in datastructure queue "components")

            int dsu_node = graphOffline.dsu.vtxatpos[dsu_node_posofvtx];
            VertexAA* dsu_node_vertexAA = graphOffline.getVertexAAPointerfromVertexAAID(dsu_node);

            NUM nodeID = dsu_node_vertexAA->name;

            //add EPS edge to self, if not already present
            if(dsu_node_vertexAA->edges.find(graphOffline.EPS)->second.size() == 0)
                dsu_node_vertexAA->addedge(graphOffline.EPS, dsu_node);

            // graphOffline.dsu.dsuContent[curr_comp_id_pos].remove(dsu_node_posofvtx);
            graphOffline.dsu.dsuContent[dsu_node_posofvtx] = {dsu_node_posofvtx};
            graphOffline.dsu.parent_pos[dsu_node_posofvtx] = -1;


            // Now, we will not mark vertices for breakage that only have a single outgoing edge to them (from any node in the current component to any 
            // node in the candidate component on a particular edge type) in the original graph
            // To carry out this optimization we need the no of destination node (in the original graph) per field over outgoing edges 
            // from the nodes in the current component 

            // get all outgoing edge types of nodeID
            auto nodeToEdgeTypeIter = nodeToEdgeTypeMap.find(nodeID);
            if(nodeToEdgeTypeIter != nodeToEdgeTypeMap.end()){
                
                //iterate over edgeTypes
                for(const NUM& edgeType_iter : nodeToEdgeTypeIter->second){
                // for(auto edgeType_iter = nodeToEdgeTypeIter->second.begin(); edgeType_iter != nodeToEdgeTypeIter->second.end(); edgeType_iter++){
                    
                    // get no of dest nodes from nodeID on type *edgeType_iter
                    NUM noOfDestNodes = 0;
                    auto outEdgesIter = outEdges.find({nodeID,edgeType_iter});
                    if(outEdgesIter != outEdges.end()){
                        noOfDestNodes = outEdgesIter->second.size();
                    }

                    auto curr_comp_outgoingEdgesCount_by_field_iter = curr_comp_outgoingEdgesCount_by_field.find(edgeType_iter);
                    if(curr_comp_outgoingEdgesCount_by_field_iter != curr_comp_outgoingEdgesCount_by_field.end()){
                        NUM curr_comp_outgoingEdgesCount_by_field_value = curr_comp_outgoingEdgesCount_by_field_iter->second;
                        curr_comp_outgoingEdgesCount_by_field_iter->second = curr_comp_outgoingEdgesCount_by_field_value + noOfDestNodes;
                    }else{
                        curr_comp_outgoingEdgesCount_by_field[edgeType_iter] = noOfDestNodes;
                    }

                }
            }

            brokenNodeCount++;
            
            brokenNodeVertexAAIDs.insert(dsu_node); 
            brokenNodeIDs.insert(nodeID); 

        }
        
        for(const auto& iter : curr_comp->edges){//offline graph edges

            // donot break next component if curr_comp is not responsible for its merger
            if(iter.first.field_id != 0){

                // if(!HUSH) cout << "*" << iter.first.field_name << "*" << endl;
                NUM fieldID = iter.first.field_name;

                // if the no of outgoing edges in original graph on same type is more than one, then we break component
                // iterate over all nodes in curr_comp
                // count the no of outgoing edges of type iter
                // if count is greater than one, break component

                auto curr_comp_outgoingEdgesCount_by_field_iter = curr_comp_outgoingEdgesCount_by_field.find(fieldID);
                if(curr_comp_outgoingEdgesCount_by_field_iter != curr_comp_outgoingEdgesCount_by_field.end()){
                    if(curr_comp_outgoingEdgesCount_by_field_iter->second < 2){
                        continue;//skip this component
                    }
                }

            }

            for(const auto& iter2 : iter.second){
                //value at iter2 is VertexAA id

                int destCompRoot_VertexAA_id = iter2;

                VertexAA* destCompRoot_VertexAA = graphOffline.getVertexAAPointerfromVertexAAID(destCompRoot_VertexAA_id);
                NUM destCompRoot = destCompRoot_VertexAA->name;


                if(visitedComponents.find(destCompRoot) == visitedComponents.end()){// we avoid marking the already visited components again
                    visitedComponents.insert(destCompRoot);
                    
                    // graphOffline.dsu.root(destCompRoot_VertexAA_id);

                    if(graphOffline.dsu.dsuContent[graphOffline.dsu.posofvtx[destCompRoot_VertexAA_id]].size() > 1){
                        components.push(destCompRoot_VertexAA_id);//push VertexAA id in components queue for breakage
                    }
                }

            }
        }
    }

    //STEP:removing edges(outgoing and incoming) associated with broken nodes

    //remove outgoing edges from broken nodes
    graphOffline.removeOutgoingEdges_exceptEPS(brokenNodeVertexAAIDs);

    //collecting source nodes associated with brokenNodes from inPrimary
    unordered_set<int> nonbroken_sourceNodes_vertexAAID = {};
    for(const NUM& brokenNodeID : brokenNodeIDs){
        if(!HUSH) cout << "checking for " << brokenNodeID << endl;
        auto inPrimaryEdgeTypeMapIter = inPrimaryEdgeTypeMap.find(brokenNodeID);
        if(inPrimaryEdgeTypeMapIter != inPrimaryEdgeTypeMap.end()){
            for(const NUM& inPrimary_edgeTypeID : inPrimaryEdgeTypeMapIter->second){

                field& fieldRef = graphOffline.getfield(inPrimary_edgeTypeID);

                auto inPrimaryEntry = inPrimary.find({brokenNodeID,inPrimary_edgeTypeID});
                if(inPrimaryEntry != inPrimary.end()){
                    for(const NUM& inPrimary_fromNodeID : inPrimaryEntry->second){
                        if(brokenNodeIDs.find(inPrimary_fromNodeID) == brokenNodeIDs.end()){
                            nonbroken_sourceNodes_vertexAAID.insert(graphOffline.dsu.root(graphOffline.getVertex(inPrimary_fromNodeID)->id));
                        }    
                    }
                }
            }
        }
    }

    //removes all outgoing edges from nonbroken source vertices to broken vertices
    graphOffline.removeOutgoingEdgesFromNonBrokenSourceNodesToBrokenNodes(nonbroken_sourceNodes_vertexAAID, brokenNodeVertexAAIDs);

    //STEP:Update datastructures
    //updating inPrimary
    if(!HUSH) cout << "updating inPrimary" << endl;
    auto outEdgesIter = outEdges.find({fromNodeID,edgeTypeID});
    if(outEdgesIter->second.back() == toNodeID){
        auto inPrimaryIter = inPrimary.find({toNodeID,edgeTypeID});
        auto fromNodeID_inPrimaryIter = find(inPrimaryIter->second.begin(), inPrimaryIter->second.end(), fromNodeID);
        if(fromNodeID_inPrimaryIter == inPrimaryIter->second.end()){
            cout << "ERROR: inPrimary[" << toNodeID << "][" << edgeTypeID << "] list does not contain fromNodeID:" << fromNodeID << endl;
            exit(1);
        }else{
            inPrimaryIter->second.erase(fromNodeID_inPrimaryIter); //remove fromNodeID from list //fromNode list can become empty here
            if(inPrimaryIter->second.size() == 0){
                inPrimary.erase(inPrimaryIter);

                //updating inPrimaryEdgeTypeMap
                //remove edgeTypeID from inPrimaryEdgeTypeMap[toNodeID]

                auto inPrimaryEdgeTypeMapIter = inPrimaryEdgeTypeMap.find(toNodeID);
                if(inPrimaryEdgeTypeMapIter != inPrimaryEdgeTypeMap.end()){
                    auto fromNodeListIter = inPrimaryEdgeTypeMapIter->second.find(edgeTypeID);
                    if(fromNodeListIter != inPrimaryEdgeTypeMapIter->second.end()){
                        inPrimaryEdgeTypeMapIter->second.erase(fromNodeListIter);
                    }
                }

            }
        }
        if(outEdgesIter->second.size() >= 2){

            NUM penultimateNodeID = *prev(outEdgesIter->second.end(), 2);

            auto inPrimaryIter2 = inPrimary.find({penultimateNodeID,edgeTypeID});

            if(inPrimaryIter2 == inPrimary.end()){
                inPrimary[{penultimateNodeID,edgeTypeID}] = {fromNodeID};

                //add edgeTypeID for penultimateNodeID in inPrimaryEdgeTypeMap
                auto inPrimaryEdgeTypeMapIter = inPrimaryEdgeTypeMap.find(penultimateNodeID);
                if(inPrimaryEdgeTypeMapIter != inPrimaryEdgeTypeMap.end()){
                    inPrimaryEdgeTypeMapIter->second.insert(edgeTypeID);
                }else{
                    inPrimaryEdgeTypeMap[penultimateNodeID] = {edgeTypeID};
                }

            }else{
                inPrimaryIter2->second.push_front(fromNodeID);
            }
        }
    }

    // remove entry from outEdges
    outEdges[{fromNodeID,edgeTypeID}].erase(pointerToOutEdges[{fromNodeID,toNodeID,edgeTypeID}]); 

    // remove entry from pointerToOutEdges
    pointerToOutEdges.erase(pointerToOutEdges.find({fromNodeID,toNodeID,edgeTypeID}));

    // remove entry from nodeToEdgeTypeMap
    if(outEdges[{fromNodeID,edgeTypeID}].size() == 0){
        nodeToEdgeTypeMap[fromNodeID].erase(nodeToEdgeTypeMap[fromNodeID].find(edgeTypeID));
    }

    //STEP:reconstruct PDSCCs
    if(!HUSH) cout << "reconstruct PDSCCs" << endl;
    for(const NUM& brokenNodeID : brokenNodeIDs){

        NUM root_of_brokenNodeID = graphHDT->getRoot(brokenNodeID).first;
        // if(!HUSH) cout << "root of brokenNodeID " << Helper::nodeIDtoName(brokenNodeID) << " is " << Helper::nodeIDtoName(root_of_brokenNodeID) << endl;
        if(root_of_brokenNodeID != brokenNodeID){
            VertexAA* brokenNode_vertexAA = graphOffline.getVertex(brokenNodeID);
            brokenNode_vertexAA->removeEPSEdge(graphOffline.EPS.field_id);

            int root_of_brokenNode_vertexAAID = graphOffline.getVertex(root_of_brokenNodeID)->id;
            int root_of_brokenNode_vertexAA_pos = graphOffline.dsu.posofvtx[root_of_brokenNode_vertexAAID];
            int brokenNode_vertexAA_pos = graphOffline.dsu.posofvtx[brokenNode_vertexAA->id];

            graphOffline.dsu.parent_pos[brokenNode_vertexAA_pos] = root_of_brokenNode_vertexAA_pos; 
            graphOffline.dsu.dsuContent[root_of_brokenNode_vertexAA_pos].push_back(brokenNode_vertexAA_pos); //dsuContent contain vertexAAID
            graphOffline.dsu.dsuContent[brokenNode_vertexAA_pos].clear();
            
        }

    }
    //gettimeofday(&timeEnd,NULL);
    //cout << Helper::getDuration(&timeBegin, &timeEnd) << ",";

    //gettimeofday(&timeBegin,NULL);
    //STEP:adding edges among PDSCCs in graphOffline
    if(!HUSH) cout << "adding edges among PDSCCs" << endl;
    for(const NUM &node : brokenNodeIDs){
        VertexAA* nodeVertexAA = graphOffline.getVertex(node);
        NUM nodeVertexAA_id = nodeVertexAA->id;
        NUM root_node = graphHDT->getRoot(node).first; //getRoot_optimized()
        // if(!HUSH) cout << "root_node:" << Helper::nodeIDtoName(root_node) << endl;
        VertexAA* root_nodeVertexAA = graphOffline.getVertex(root_node);
        // NUM root_nodeVertexAA_id = root_nodeVertexAA->id;

        //adding outgoing edges from PDSCCs
        unordered_set<NUM> &edgeType_USet = nodeToEdgeTypeMap[node];
        for(const NUM& node_edgeType : edgeType_USet){
        // for(auto edgeType_USetIter = edgeType_USet.begin(); edgeType_USetIter != edgeType_USet.end(); edgeType_USetIter++){
            
            // NUM node_edgeType = *edgeType_USetIter;

            auto outEdgesIter = outEdges.find({node,node_edgeType});
            if(outEdgesIter != outEdges.end()){
                if(outEdgesIter->second.size() > 0){
                    graphOffline.addedge(root_nodeVertexAA, graphOffline.getVertexAAPointerfromVertexAAID(graphOffline.dsu.root(graphOffline.getVertex(outEdgesIter->second.front())->id)), graphOffline.getfield(node_edgeType));
                    // graphOffline.worklist.push(pair<int,field>(root_nodeVertexAA_id,fieldRef));
                }
            }
        }

        // adding incoming edges from PDSCCs
        auto inPrimaryEdgeTypeMapIter = inPrimaryEdgeTypeMap.find(node);
        if(inPrimaryEdgeTypeMapIter != inPrimaryEdgeTypeMap.end()){
            for(const NUM& inPrimary_edgeTypeID : inPrimaryEdgeTypeMapIter->second){
            // for(auto edgeTypeListIter = inPrimaryEdgeTypeMapIter->second.begin(); edgeTypeListIter != inPrimaryEdgeTypeMapIter->second.end(); edgeTypeListIter++){
                // NUM inPrimary_edgeTypeID = *edgeTypeListIter;

                field& fieldRef = graphOffline.getfield(inPrimary_edgeTypeID);

                auto inPrimaryEntry = inPrimary.find({node,inPrimary_edgeTypeID});
                if(inPrimaryEntry != inPrimary.end()){
                    for(const NUM& inPrimary_fromNodeID : inPrimaryEntry->second){
                    // for(auto inPrimaryListIter = inPrimaryEntry->second.begin(); inPrimaryListIter != inPrimaryEntry->second.end(); inPrimaryListIter++){
                        
                        // NUM inPrimary_fromNodeID = *inPrimaryListIter;
                        VertexAA* inPrimary_fromNodeID_vertexAA = graphOffline.getVertex(inPrimary_fromNodeID);
                        NUM inPrimary_fromNodeID_vertexAA_id = inPrimary_fromNodeID_vertexAA->id;

                        NUM root_inPrimary_fromNodeID_vertexAA_id = graphOffline.dsu.root(inPrimary_fromNodeID_vertexAA_id);//graphHDT->getRoot(dest_node).first; //getRoot_optimized()
                        VertexAA* root_inPrimary_fromNodeID_vertexAA = graphOffline.getVertexAAPointerfromVertexAAID(root_inPrimary_fromNodeID_vertexAA_id);

                        graphOffline.addedge(root_inPrimary_fromNodeID_vertexAA, root_nodeVertexAA, fieldRef);
                        graphOffline.worklist.push(pair<int,field>(root_inPrimary_fromNodeID_vertexAA_id,fieldRef));
                        
                    }
                }
            }
        }
    }

    //STEP:run offline algo for fixpoint computation

    if(!HUSH) cout << "bidirectedReach()" << endl;
    graphOffline.bidirectedReach();

    if(!HUSH) cout << "***Delete end***" << endl;
    
    return brokenNodeCount;
}

void DynamicDyckReachability::queryComponents(){

    if(!HUSH) cout << "SOLUTION:" << endl;
    unordered_map<int, set<int64_t>> comp = graphOffline.getComponents();
    for(const auto& iter : comp){
        set<int64_t> comp_str = iter.second;
        for(int64_t nodeID : comp_str){
            cout << Helper::nodeIDtoName(nodeID) << ",";
        }
        cout << endl;
    }
}

bool DynamicDyckReachability::isReachable(NUM node1, NUM node2){
    return graphOffline.dsu.root(graphOffline.getVertex(node1)->id) == graphOffline.dsu.root(graphOffline.getVertex(node2)->id);
}

void DynamicDyckReachability::insertUndirectedEdge(NUM node1, NUM node2){    
    //get edge count for undirected edge
    auto edgeCounterIter = edgeCounter.find({node1,node2}); 
    if(edgeCounterIter == edgeCounter.end()){//entry not present in edgeCounter
        // if(!HUSH) cout << "(" << Helper::nodeIDtoName(node1) << "," << Helper::nodeIDtoName(node2) << ") entry not present in EdgeCounter. Adding entry with count 1." << endl;
        edgeCounter[{node1,node2}] = 1;
    }else{
        // if(!HUSH) cout << "(" << Helper::nodeIDtoName(node1) << "," << Helper::nodeIDtoName(node2) << ") present in EdgeCounter. Incrementing count." << endl;
        edgeCounterIter->second = edgeCounterIter->second + 1;
    }

    if(!graphHDT->HasEdge(UndirectedEdge(node1,node2))){
        // if(!HUSH) cout << "Undirected Edge ("+Helper::nodeIDtoName(node1)+","+Helper::nodeIDtoName(node2)+") inserted into HDTGraph." << endl;
        graphHDT->AddEdge(UndirectedEdge(node1, node2));
    }
}

void DynamicDyckReachability::deleteUndirectedEdge(NUM node1, NUM node2){
    auto edgeCounterIter = edgeCounter.find({node1, node2});
    if(edgeCounterIter == edgeCounter.end()){
        cout << "Deleting UndirectedEdge:(" << Helper::nodeIDtoName(node1) << "," << Helper::nodeIDtoName(node2) << "), which is not present in EdgeCounter, it does not contain entry." << endl;
        exit(1);
    }else{
        NUM count = edgeCounterIter->second;
        if(!HUSH) cout << "attempting to delete UndirectedEdge:(" << Helper::nodeIDtoName(node1) << "," << Helper::nodeIDtoName(node2) << "), count:" << count << endl;
        if (count == 0){
            cout << "Deleting UndirectedEdge:(" << Helper::nodeIDtoName(node1) << "," << Helper::nodeIDtoName(node2) << "), which is not present in EdgeCounter, count is 0." << endl;
            exit(1);
        }else{
            if(graphHDT->HasEdge(UndirectedEdge(node1, node2))){
                // if(!HUSH) cout << "UndirectedEdge is present in HDTGraph" << endl;

                if(count == 1){//delete entry in EdgeCounter
                    edgeCounter.erase(edgeCounterIter); // O(1) time
                    graphHDT->DeleteEdge(UndirectedEdge(node1, node2));
                    // if(!HUSH) cout << "UndirectedEdge removed from HDTGraph" << endl;

                }else{//decrement EdgeCounter count
                    edgeCounterIter->second = count - 1;
                }

            }
        }
    }
}

void DynamicDyckReachability::displayEdgeCounter(){
    cout << "EdgeCounter:" << endl;
    for(auto ecIter = edgeCounter.begin(); ecIter != edgeCounter.end(); ecIter++){
        cout << "{" << Helper::nodeIDtoName(ecIter->first.first) << "," << Helper::nodeIDtoName(ecIter->first.second) << "}:" << ecIter->second << endl;
    }
}

void DynamicDyckReachability::displayNodeToEdgeMap(unordered_map<NUM,unordered_set<NUM>> nodeToEdgeTypeMap){
    for(auto it = nodeToEdgeTypeMap.begin(); it != nodeToEdgeTypeMap.end(); it++){
        cout << Helper::nodeIDtoName(it->first) << ":{";
        for(auto et_it = it->second.begin(); et_it != it->second.end(); et_it++){
            cout << Helper::edgeTypeIDtoName(*et_it) ;
        }
        cout << "}" << endl;
    }
}

// get no of prim component greater than 1
NUM DynamicDyckReachability::getNoOfPrimaryComp(){
    set<pair<NUM, set<NUM>>> componentsHDT = graphHDT->getComponents();
    NUM primCompCount = 0;
    for(set<pair<NUM, set<NUM>>>::iterator comp_iter = componentsHDT.begin(); comp_iter != componentsHDT.end(); comp_iter++){
        if( comp_iter->second.size() > 1){
            primCompCount++;
        }
    }
    return primCompCount;
}

// get avg size of primary comps
double DynamicDyckReachability::getAvgSizeOfPrimaryComp(){
    set<pair<NUM, set<NUM>>> componentsHDT = graphHDT->getComponents();
    NUM primCompCount = 0;
    NUM totalPrimSize = 0;
    for(set<pair<NUM, set<NUM>>>::iterator comp_iter = componentsHDT.begin(); comp_iter != componentsHDT.end(); comp_iter++){
        primCompCount++;
        totalPrimSize += comp_iter->second.size();
    }
    return ((double)totalPrimSize/primCompCount);
}

// get avg size of primary comps having size greater than one
double DynamicDyckReachability::getAvgSizeOfPrimaryComp_sizeGreaterThan1(){
    set<pair<NUM, set<NUM>>> componentsHDT = graphHDT->getComponents();
    NUM primCompCount = 0;
    NUM totalPrimSize = 0;
    for(set<pair<NUM, set<NUM>>>::iterator comp_iter = componentsHDT.begin(); comp_iter != componentsHDT.end(); comp_iter++){
        if( comp_iter->second.size() > 1){
            primCompCount++;
            totalPrimSize += comp_iter->second.size();
        }
    }
    return ((double)totalPrimSize/primCompCount);
}

NUM DynamicDyckReachability::getTotalNoOfComp(){
    return graphOffline.getComponents().size();
}

void DynamicDyckReachability::graphOffline_show(){
    if(!HUSH) cout << "graphOffline.show():" << endl;

    cout << "vertices:" << graphOffline.vertices.size() << endl;
    cout << "vtxatpos:" << graphOffline.dsu.vtxatpos.size() << endl;
    cout << "parent_pos:" << graphOffline.dsu.parent_pos.size() << endl;
    cout << "posofvtx:" << graphOffline.dsu.posofvtx.size() << endl;
    cout << "dsu.N:" << graphOffline.dsu.getN() << endl;
    cout << "vertexAAID:";
    for(VertexAA* v : graphOffline.vertices){
        cout << v->name << "[" << Helper::nodeIDtoName(v->name) << "]" << ",";
    }
    cout << endl;

    cout << "Edges:" << endl;
    for(VertexAA* v : graphOffline.vertices){
		cout << "Vertex name:" << v->name << "(" << Helper::nodeIDtoName(v->name) << ") id:" << v->id << endl;
		for(auto it = v->edgesbegin(); it != v->edgesend(); it++){
			cout << "\tfield:" << Helper::edgeTypeIDtoName(it->first.field_name) << "[";
			for(list<int>::iterator it1 = it->second.begin(); it1!= it->second.end(); it1++){
				int64_t vertexid = (*it1);
				cout << vertexid << "(" << Helper::nodeIDtoName(graphOffline.vertices[vertexid]->name) << "),";
			}
			cout << "]" << endl;
		}		
	}
    cout << endl;

    cout << "Edges:" << endl;
    for(VertexAA* v : graphOffline.vertices){
        string vertexName = Helper::nodeIDtoName(v->name);
		cout << "Vertex name:" << vertexName << endl;
		for(auto it = v->edgesbegin(); it != v->edgesend(); it++){
            string fieldName = Helper::edgeTypeIDtoName(it->first.field_name);
            if(it->second.size() > 0){
                for(list<int>::iterator it1 = it->second.begin(); it1!= it->second.end(); it1++){
                    int64_t vertexid = (*it1);
                    string toVertexName = Helper::nodeIDtoName(graphOffline.vertices[vertexid]->name);
                    cout << vertexName << "->" << toVertexName << "(" << fieldName << ")" << endl;
                }
            }
		}		
	}
    cout << endl;
	cout << "dsu contents:" << endl;

	for(int i=0; i<graphOffline.vertices.size(); i++){
		
        VertexAA* vertexAAPointer = graphOffline.vertices[i];
        cout << "Root of " << Helper::nodeIDtoName(vertexAAPointer->name) << " is ";
        int root = graphOffline.dsu.root(vertexAAPointer->id);
        VertexAA* rootVertexAAPointer = graphOffline.vertices[root];

        cout << "(" << Helper::nodeIDtoName(rootVertexAAPointer->name) << ")" << endl;
	        
	}
	cout << endl;

    cout << "component contents(dsuContent):" << endl;

    for(int i=0; i<graphOffline.vertices.size(); i++){
        cout << Helper::nodeIDtoName(graphOffline.vertices[i]->name) << ":";
        for(int node : graphOffline.dsu.dsuContent[graphOffline.dsu.posofvtx[i]]){
            cout << Helper::nodeIDtoName(graphOffline.vertices[node]->name) << ",";
        }
        cout << endl;
    }
    cout << endl;

	cout << "worklist(" << graphOffline.worklist.size() << "):" << endl;
	queue<pair<int,field>> wl = graphOffline.worklist;
	while (!wl.empty()){
		pair<int,field> el = wl.front();
		cout << "{" << el.first << "," << el.second.field_name <<"},";
		wl.pop();
	}
}

void DynamicDyckReachability::graphHDT_show(){
  std::cout << "graphHDT_show():" << std::endl;
  std::set<std::pair<int64_t, std::set<int64_t>>> componentsHDT = graphHDT->getComponents();
  for(std::set<std::pair<int64_t, std::set<int64_t>>>::iterator comp_iter = componentsHDT.begin(); comp_iter != componentsHDT.end(); comp_iter++){
      std::set<int64_t> component = comp_iter->second;
      std::cout << Helper::nodeIDtoName(comp_iter->first) << ":";
      for(auto it = component.begin(); it!=component.end(); it++){
          std::cout << Helper::nodeIDtoName(*it) << ",";
      }
      std::cout << std::endl;
  }

}

void DynamicDyckReachability::displayPointerToOutedges(unordered_map<tuple<NUM,NUM,NUM>,list<NUM>::iterator, boost::hash<tuple<NUM,NUM,NUM>>> pointerToOutEdges){
    cout << "PointerToOutedges:" << endl;
    for(auto it = pointerToOutEdges.begin(); it != pointerToOutEdges.end(); it++){
        tuple<NUM,NUM,NUM> tup = it->first;
        cout << "{" << get<0>(tup) << "," << get<1>(tup) << "," << get<2>(tup) << "}:" << *(it->second) << endl;
    }
}

void DynamicDyckReachability::displayDirectedEdgeSet(set<tuple<NUM,NUM,NUM>> directedEdgeSet){
    cout << "DirectedEdgeSet:" << endl;
    for(auto desIter = directedEdgeSet.begin(); desIter != directedEdgeSet.end(); desIter++){
        tuple<NUM,NUM,NUM> tup = *desIter;
        NUM fromNodeTup = get<0>(tup);
        NUM toNodeTup = get<1>(tup);
        NUM edgeTypeTup = get<2>(tup);
        cout << Helper::nodeIDtoName(fromNodeTup) << "->" << Helper::nodeIDtoName(toNodeTup) << "(" << Helper::edgeTypeIDtoName(edgeTypeTup) << ")" << endl;
    }
}

void DynamicDyckReachability::displayOutEdges(unordered_map<pair<NUM,NUM>,list<NUM>, boost::hash<pair<NUM,NUM>>> outEdges){
    cout << "Outedges:" << endl;
    for(unordered_map<pair<NUM,NUM>,list<NUM>, boost::hash<pair<NUM,NUM>>>::iterator outEdgesIter = outEdges.begin(); outEdgesIter != outEdges.end(); outEdgesIter++){
        cout << "(" << Helper::nodeIDtoName(outEdgesIter->first.first) << "," << Helper::edgeTypeIDtoName(outEdgesIter->first.second) << "):{";
        for(list<NUM>::iterator lstIter = outEdgesIter->second.begin(); lstIter != outEdgesIter->second.end(); lstIter++){
            cout << Helper::nodeIDtoName(*lstIter) << ",";
        }
        cout << "}"<<endl;
    }
}

