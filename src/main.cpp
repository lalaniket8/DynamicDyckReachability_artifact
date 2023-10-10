#include <Helper.h>
#include <DynamicDyckReachability.h>

int main(int argc, char *argv[]){
    string usageString = "USAGE: ./main.out --verbose --show_result <initialgraph_dot> <seq_filename>\n--verbose flag : Gives verbose output\n--show_result : Displays the componenets after perfoming all opertaions in seq_filename over initialgraph_dot. Each line represents a single component.";
    string input_initgraph_filename = "";
    string input_seq_filename = "";
    bool HUSH = true;
    bool SHOW_SOLUTION = false;
    if(argc < 3 || argc > 5){
        cout << usageString << endl;
        exit(1);
    }
    else{
        vector<string> arguments = {};
        for(int i=1;i<argc;i++){
            string arg = argv[i];
            if(arg.compare("--verbose") == 0){HUSH = false;}
            else if(arg.compare("--show_result") == 0){SHOW_SOLUTION = true;}
            else{arguments.push_back(arg);}
        }
        if(arguments.size() == 2){
            input_initgraph_filename = arguments[0];
            input_seq_filename = arguments[1];
        }else{
            cout << usageString << endl;
            exit(1);
        }
        
    }

    if(!HUSH) cout << "initialgraph_dot:" << input_initgraph_filename << endl;
    if(!HUSH) cout << "seq_filename:" << input_seq_filename << endl;

    NUM NodeCount = -1;
    NUM SeqCount = -1;
    NUM EdgeCount_initgraph = -1;
    NUM EdgeTypeCount = -1;

    queue<Helper::EdgeOperation> edgeOperationSequence = Helper::parseGraphInputFromFile(input_initgraph_filename,input_seq_filename, &NodeCount, &EdgeTypeCount, &EdgeCount_initgraph, &SeqCount);

    if(NodeCount == -1 || EdgeTypeCount == -1 || EdgeCount_initgraph == -1 || SeqCount == -1){cerr << "ERROR NodeCount incorrect. Kindly check the input format/parameters/files";exit(1);}
    
    DynamicDyckReachability dynamicDyckReachability(NodeCount, HUSH);

    if(!HUSH) Helper::displayNodes_Edges();

	struct timeval timeBegin, timeEnd;
    double elapsed = 0.0;
    NUM insertEdgeOperationCount = 0;
    NUM deleteEdgeOperationCount = 0;
    NUM totalNodeBroken = 0;

    while(!edgeOperationSequence.empty()){
        Helper::EdgeOperation edgeOperation = edgeOperationSequence.front();
        
        if(!HUSH) cout << endl << "********************************************************************" << endl;
        if(!HUSH) cout << "Processing(" << (insertEdgeOperationCount+deleteEdgeOperationCount) << "):" << edgeOperation.oper << " " << Helper::nodeIDtoName(edgeOperation.fromNode) << " " << Helper::nodeIDtoName(edgeOperation.toNode) << " " << Helper::edgeTypeIDtoName(edgeOperation.edgeType) << " " << endl;

        if(edgeOperation.oper == 'A'){ //add edge
            if(edgeOperation.computeTime){
                insertEdgeOperationCount++;

                gettimeofday(&timeBegin,NULL);
                dynamicDyckReachability.insertDirectedEdge(edgeOperation.fromNode, edgeOperation.toNode, edgeOperation.edgeType);
                gettimeofday(&timeEnd,NULL);

                double duration = Helper::getDuration(&timeBegin, &timeEnd);
                elapsed += duration;
            }else{
                dynamicDyckReachability.insertDirectedEdge(edgeOperation.fromNode, edgeOperation.toNode, edgeOperation.edgeType);
            }
        }else if(edgeOperation.oper == 'D'){ //delete edge
            if(edgeOperation.computeTime){
                deleteEdgeOperationCount++;
                gettimeofday(&timeBegin,NULL);
                NUM nodeBrokenCount = dynamicDyckReachability.deleteDirectedEdge(edgeOperation.fromNode, edgeOperation.toNode, edgeOperation.edgeType);
                gettimeofday(&timeEnd,NULL);

                if(nodeBrokenCount >= 0){totalNodeBroken += nodeBrokenCount;}
                double duration = Helper::getDuration(&timeBegin, &timeEnd);
                elapsed += duration;
            }else{
                dynamicDyckReachability.deleteDirectedEdge(edgeOperation.fromNode, edgeOperation.toNode, edgeOperation.edgeType);
            }
            
        }else{
            cerr << "Invalid operator string:" << edgeOperation.oper;
            exit(0);
        }
        edgeOperationSequence.pop();
    }
    if(!SHOW_SOLUTION) cout << "TotalElapsedTime:" << elapsed << endl;
    // if(!HUSH) cout << "NodeCount:" << NodeCount << endl;
    // if(!HUSH) cout << "EdgeTypeCount:" << EdgeTypeCount << endl;
    // if(!HUSH) cout << "EdgeCount_initgraph:" << EdgeCount_initgraph << endl;
    // if(!HUSH) cout << "insertEdgeOperationCount:" << insertEdgeOperationCount << endl;
    // if(!HUSH) cout << "deleteEdgeOperationCount:" << deleteEdgeOperationCount << endl;
    // if(!HUSH) cout << "TotalElapsedTime:" << elapsed << endl;
    // if(!HUSH) cout << "Primary componenets:" << dynamicDyckReachability.getNoOfPrimaryComp() << endl; 
    // if(!HUSH) cout << "Total components:" << dynamicDyckReachability.getTotalNoOfComp() << endl;
    // if(!HUSH) cout << "totalNodeBroken:" << totalNodeBroken << endl;
    // if(!HUSH) cout << "MAX_COMP_SIZE:" << dynamicDyckReachability.MAX_COMP_SIZE << endl;
    // if(!HUSH) cout << "SUM_COMP_SIZE:" << dynamicDyckReachability.SUM_COMP_SIZE << endl;
    // if(!HUSH) cout << "NO_OF_COMP:" << dynamicDyckReachability.NO_OF_COMP << endl;
    // if(!HUSH) cout << "avgWorklistSize_for_delete:" << ((double)dynamicDyckReachability.totalWorklistSize_for_delete/deleteEdgeOperationCount) << endl;
    // if(!HUSH) cout << "AvgPrimaryCompSize:" << dynamicDyckReachability.getAvgSizeOfPrimaryComp() << endl; 
    // if(!HUSH) cout << "AvgPrimaryCompSize_havingSizeGreaterThan1:" << dynamicDyckReachability.getAvgSizeOfPrimaryComp_sizeGreaterThan1() << endl; 
    
    if(SHOW_SOLUTION) dynamicDyckReachability.queryComponents();

    dynamicDyckReachability.clean();

    return 0;
}
