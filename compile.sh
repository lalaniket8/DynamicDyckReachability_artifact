#!/bin/sh
PROJECT_DIR=${PWD}

echo ${PROJECT_DIR}
echo 

ALIASANALYSIS_INCL1="/dynamic-connectivity-hdt-master/src/dynamic_graph/include"
ALIASANALYSIS_INCL2="/dynamic-connectivity-hdt-master/src/dynamic_graph/src"
ALIASANALYSIS_INCL3="/dynamic-connectivity-hdt-master/src/utilities/include"

ALIASANALYSIS_CPP1="/dynamic-connectivity-hdt-master/src/dynamic_graph/src/dynamic_connectivity.cpp "
ALIASANALYSIS_CPP2="/dynamic-connectivity-hdt-master/src/dynamic_graph/src/dynamic_forest.cpp "
ALIASANALYSIS_CPP3="/dynamic-connectivity-hdt-master/src/dynamic_graph/src/graph.cpp "
ALIASANALYSIS_CPP4="/dynamic-connectivity-hdt-master/src/dynamic_graph/src/sequence.cpp "
ALIASANALYSIS_CPP5="/dynamic-connectivity-hdt-master/src/utilities/src/hash.cpp "
ALIASANALYSIS_CPP6="/dynamic-connectivity-hdt-master/src/utilities/src/assert.cpp "
#ALIASANALYSIS_CPP1="/dynamic-connectivity-hdt-master/src/dynamic_graph/src/main.cpp "


HDT_INCL1="/alias_analysis/Alias_Analysis/include"

HDT_CPP1="/alias_analysis/Alias_Analysis/src/graph/Ngraph.cpp "
HDT_CPP2="/alias_analysis/Alias_Analysis/src/utils/fdll.cpp "
HDT_CPP3="/alias_analysis/Alias_Analysis/src/graph/graph.cpp "
HDT_CPP4="/alias_analysis/Alias_Analysis/src/DSU/DSU.cpp "
HDT_CPP5="/alias_analysis/Alias_Analysis/src/utils/utils.cpp "
#HDT_CPP="/alias_analysis/Alias_Analysis/src/main.cpp "

DDR_INCL1="/include"
DDR_CPP2="/src/DynamicDyckReachability.cpp "
DDR_CPP6="/src/Helper.cpp "

DDR_MAIN="/src/main.cpp "
INCLUDE_FLAG="-I"
INCLUDE="${INCLUDE_FLAG}${PROJECT_DIR}${ALIASANALYSIS_INCL1} ${INCLUDE_FLAG}${PROJECT_DIR}${ALIASANALYSIS_INCL2} ${INCLUDE_FLAG}${PROJECT_DIR}${ALIASANALYSIS_INCL3} ${INCLUDE_FLAG}${PROJECT_DIR}${HDT_INCL1} ${INCLUDE_FLAG}${PROJECT_DIR}${DDR_INCL1} "
ALIASANALYSIS_CPP="${PROJECT_DIR}${ALIASANALYSIS_CPP1} ${PROJECT_DIR}${ALIASANALYSIS_CPP2} ${PROJECT_DIR}${ALIASANALYSIS_CPP3} ${PROJECT_DIR}${ALIASANALYSIS_CPP4} ${PROJECT_DIR}${ALIASANALYSIS_CPP5} ${PROJECT_DIR}${ALIASANALYSIS_CPP6} "
HDT_CPP="${PROJECT_DIR}${HDT_CPP1} ${PROJECT_DIR}${HDT_CPP2} ${PROJECT_DIR}${HDT_CPP3} ${PROJECT_DIR}${HDT_CPP4} ${PROJECT_DIR}${HDT_CPP5} "
# DDR_CPP="${PROJECT_DIR}${DDR_CPP1} ${PROJECT_DIR}${DDR_CPP2} ${PROJECT_DIR}${DDR_CPP3} ${PROJECT_DIR}${DDR_CPP4} ${PROJECT_DIR}${DDR_CPP5} ${PROJECT_DIR}${DDR_CPP6} ${PROJECT_DIR}${DDR_MAIN} "
DDR_CPP="${PROJECT_DIR}${DDR_CPP2} ${PROJECT_DIR}${DDR_CPP6} ${PROJECT_DIR}${DDR_MAIN} "

#echo $INCLUDE
#echo $ALIASANALYSIS_CPP
#echo $HDT_CPP
#echo $DDR_CPP

mkdir -p build;

echo "g++ -O3 -g $INCLUDE $ALIASANALYSIS_CPP $HDT_CPP $DDR_CPP -o ./build/main.out"
g++ -O3 -g $INCLUDE $ALIASANALYSIS_CPP $HDT_CPP $DDR_CPP -o ./build/main.out

echo "Compilation completed."

exit $? 
