echo "compiling the files"
PROJECT_DIR="/home/aniket/MSThesis/DynamicDyckReachability_v1"
INCLUDE_FLAG="-I" 

INCL1="/dynamic-connectivity-hdt-master/src/dynamic_graph/include"
INCL2="/dynamic-connectivity-hdt-master/src/dynamic_graph/src"
INCL3="/dynamic-connectivity-hdt-master/src/utilities/include"
INCL4="/dynamic-connectivity-hdt-master/src"

CPP1="./dynamic_graph/src/main.cpp"
CPP2="./dynamic_graph/src/dynamic_connectivity.cpp"
CPP3="./dynamic_graph/src/dynamic_forest.cpp"
CPP4="./dynamic_graph/src/graph.cpp"
CPP5="./dynamic_graph/src/sequence.cpp"
CPP6="./utilities/src/hash.cpp"
CPP7="./utilities/src/assert.cpp"

INCL="${INCLUDE_FLAG}${PROJECT_DIR}${INCL1} ${INCLUDE_FLAG}${PROJECT_DIR}${INCL2} ${INCLUDE_FLAG}${PROJECT_DIR}${INCL3} ${INCLUDE_FLAG}${PROJECT_DIR}${INCL4}"
CPP="${CPP1} ${CPP2} ${CPP3} ${CPP4} ${CPP5} ${CPP6} ${CPP7}"

mkdir -p build;
g++ $INCL $CPP -o ./build/main.out

# g++ -I/home/aniket/MSThesis/DynamicDyckReachability_v1/dynamic-connectivity-hdt-master/src/dynamic_graph/include -I/home/aniket/MSThesis/DynamicDyckReachability_v1/dynamic-connectivity-hdt-master/src/dynamic_graph/src -I/home/aniket/MSThesis/DynamicDyckReachability_v1/dynamic-connectivity-hdt-master/src/utilities/include ./dynamic_graph/src/main.cpp ./dynamic_graph/src/dynamic_connectivity.cpp ./dynamic_graph/src/dynamic_forest.cpp ./dynamic_graph/src/graph.cpp ./dynamic_graph/src/sequence.cpp ./utilities/src/hash.cpp ./utilities/src/assert.cpp -o ./build/main.out

#./utilities/src/assert.cpp ./utilities/src/hash.cpp ./dynamic_graph/src/main.cpp ./dynamic_graph/src/dynamic_forest.cpp ./dynamic_graph/src/graph.cpp ./dynamic_graph/src/sequence.cpp -o ./build/main.out

exit $? 
