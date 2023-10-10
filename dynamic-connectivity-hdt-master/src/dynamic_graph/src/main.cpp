#include <dynamic_graph/dynamic_connectivity.hpp>
#include <iostream>
#include <sstream>

using namespace std;

std::string int64_t_tostring(const int64_t& number) {
    std::ostringstream ss;
    ss << number;
    return ss.str();
}

std::string elToString(sequence::Element el){
    std::string str = "(" + int64_t_tostring(el.id_.first) + "," + int64_t_tostring(el.id_.second) + ")"; 
    return str;
}

int main(){

    DynamicConnectivity graph(10);

    graph.AddEdge(UndirectedEdge(1, 2));   
    graph.AddEdge(UndirectedEdge(2, 3));
    graph.AddEdge(UndirectedEdge(1, 3));
    graph.AddEdge(UndirectedEdge(4, 5));
    graph.AddEdge(UndirectedEdge(7, 8));
    graph.AddEdge(UndirectedEdge(6, 8));
    graph.AddEdge(UndirectedEdge(2, 4));
    graph.AddEdge(UndirectedEdge(9, 8));
    graph.AddEdge(UndirectedEdge(0, 9));

    // set<int64_t> roots1 = graph.getRoots();
    // cout << "Roots1:";
    // for(set<int64_t>::iterator it = roots1.begin(); it != roots1.end(); it++){
    //     cout << *it << ",";
    // }
    // cout << endl;
    // graph.showComponents();cout << endl;
    // graph.print();

    // graph.DeleteEdge(UndirectedEdge(2,3));
    
    // set<int64_t> roots2 = graph.getRoots();
    // cout << "Roots2:";
    // for(set<int64_t>::iterator it = roots2.begin(); it != roots2.end(); it++){
    //     cout << *it << ",";
    // }
    // cout << endl;
    // graph.showComponents();cout << endl;
    // graph.print();

    // graph.DeleteEdge(UndirectedEdge(1,3));
    // set<int64_t> roots3 = graph.getRoots();
    // cout << "Roots3:";
    // for(set<int64_t>::iterator it = roots3.begin(); it != roots3.end(); it++){
    //     cout << *it << ",";
    // }
    // cout << endl;
    graph.showComponents();cout << endl;
    // graph.print();

    for(int i=0; i<10; i++){
        std::pair<int64_t, int64_t> r = graph.getRoot(i);
        cout << "getRoot(" << i << "): (" << r.first << "," << r.second << ")" << endl;
    }
    cout << "*************" << endl;
    graph.set_rootnode_null();

    for(int i=0; i<10; i++){
        // std::pair<int64_t, int64_t> r = graph.getRoot_optimized(i);
        cout << "getRoot(" << i << "): (" << graph.getRoot_optimized(i) << ")" << endl;
    }


    // DynamicConnectivity graph(10);

    // // Graph is two triangles:
    // //   0          5
    // //   |\        /|
    // //   | \      / |
    // //   2--1    4--3
    // graph.AddEdge(UndirectedEdge(0, 1));
    // graph.AddEdge(UndirectedEdge(1, 2));
    // graph.AddEdge(UndirectedEdge(2, 0));

    // graph.AddEdge(UndirectedEdge(3, 4));
    // graph.AddEdge(UndirectedEdge(4, 5));
    // graph.AddEdge(UndirectedEdge(5, 3));

    // graph.AddEdge(UndirectedEdge(6, 7));

    // graph.AddEdge(UndirectedEdge(8, 9));

    // // cout << graph.HasEdge(UndirectedEdge(2,1)) << endl;
    // // cout << graph.HasEdge(UndirectedEdge(2,4)) << endl;

    // // for(int i=0; i<10; i++){
    // //     cout << i << ":getNode{" << graph.getNode(i).first << "," << graph.getNode(i).second << "}\t"
    // //     << "getRoot:{" << graph.getRoot(i).first << "," << graph.getRoot(i).second << "}\t";
    // //     cout << "pred:{" << graph.getPred(i).first << "," << graph.getPred(i).second << "}\t";
    // //     cout << "sequence:{";
    // //     vector<std::pair<int64_t, int64_t>> roots = graph.getSequenceOfNode(i);
    // //     for(pair<int64_t, int64_t> p : roots){
    // //         cout << "{" << p.first << "," << p.second << "},";
    // //     }        
    // //     cout << "}" << endl;
    // //     cout << "\tcomps:{";
    // //     vector<int64_t> comps = graph.getComponent(i);
    // //     for(int64_t p : comps){
    // //         cout << p << ",";
    // //     }
    // //     cout << "}\tparent:";
    // //     std::pair<int64_t, int64_t> parent = graph.getParent(i);
    // //     cout << "{" << parent.first << "," << parent.second << "}" << endl;
    // // }

    // for(int i=0; i<10; i++){
    //     cout << i << ":getNode{" << graph.getNode(i).first << "," << graph.getNode(i).second << "}\t";
    //     cout << "root:" << graph.getRoot(i).first << "," << graph.getRoot(i).second << "\t";
    //     cout << "predRoot:" << graph.getPredRoot(i) << "\t";
    //     cout << "\tisRoot:" << graph.isRoot(i);
    //     cout << "\tcomps:{";
    //     set<int64_t> comps = graph.getComponent(i);
    //     for(int64_t p : comps){
    //         cout << p << ",";
    //     }
    //     cout << "}" << endl;
    // }

    // auto c = graph.getComponents();
    // for(auto iter = c.begin(); iter != c.end(); iter++){
    //     cout << "Root:" << iter->first << "[";
    //     for(int64_t n : iter->second){
    //         cout << n << ",";
    //     }
    //     cout << "]" << endl;
    // }

    // set<pair<int,int>> intset = {};
    // intset.insert({1,2});
    // intset.insert({3,4});
    // intset.insert({5,6});
    
    // for(set<pair<int,int>>::iterator iter = intset.begin(); iter != intset.end(); iter++){
    //     cout << iter->first << "," << iter->second << endl;
    // }

    // intset.erase(intset.begin());
    // intset.insert({5,6});
    // cout << endl;
    // for(set<pair<int,int>>::iterator iter = intset.begin(); iter != intset.end(); iter++){
    //     cout << iter->first << "," << iter->second << endl;
    // }


    return 0;
}