# Dynamic Bidirected Dyck Reachability

This readme is divided into following sections:
- Brief description
- Phase 1 : Kick the tires 
    - Installing the artifact
    - Building the tool
    - Simple example 1
    - Simple example 2
- Phase 2 : Artifact Evaluation
    - Part 1
        - Option 1
            - Option 1 Subpart A
            - Option 1 Subpart B
        - Option 2
            - Option 2 Subpart A
            - Option 2 Subpart B
    - Part 2
        - Subpart A
        - Subpart B
    - Details about evaluation script run.py
- Evaluating Reusability
    - Running Custom Benchmarks
    - Directory Description
    - Class DynamicDyckReachability

## Brief description
Implementation of paper *On-The-Fly Static Analysis via Dynamic Bidirected Dyck Reachability*

Artifact repository is also maintained on [GithubRepo].

The DynamicBidirectedDyckReachability Tool accepts an initial graph and a sequence of update edge operations as input. It then calculates Bidirected Dyck reachability among nodes after executing the specified update operations on the initial graph.

Tool can be executed as follows:
`./build/main.out [--verbose] [--show_result] <initialgraph_dot> <seq_filename>`

Description of above command:
1. --verbose flag : Generates verbose output
2. --show_result : Displays the componenets after perfoming all opertaions in seq_filename over initialgraph_dot. Each line represents a single component. 
    *When this flag is not passed, the tool outputs TotalElapsedTime.*
3. initialgraph_dot argument : Provide the initial graph dot file containing edges in the format:
    `fromNode->toNode[label="paranthesisType--paranthesis"]`
    where parathesisType is "op" for opening paranthesis and "cp" for closing paranthesis.
    Either one of the closing or opening paranthesis edge can be provided. Please refer ./input/fig1.dot, which shows the initial graph in Fig.1 of the paper.
4. seq_filename argument: Provide the insert and delete edge operations in the format:
    `opeationType fromNode toNode paranthesisType--paranthesis`
    where opeationType is 'A' for insertion and 'D' for deletion. Please refer ./input/fig1.seq, which demonstrates the edge update operations shown in Fig.1 of the paper.

Edges in the above files can be commented by preceeding the line with '#' character.

**Note : Commands mentioned in the Readme.md file are exectued on a machine with 16GB RAM and 12th Gen Intel Core i7 processor. Runtimes may differ depending on machine configuration.**

## Phase 1 : Kick the tires 

### Installing the artifact

1. Download artifact from [Zenodo]
2. Extract DynamicDyckReachability_POPL24.zip to obtain Virtual machine image. Following are VM Image details:
    - Name: DynamicDyckReachability_Artifact_POPL24
    - Host OS: Ubuntu 64 bit
    - RAM: 8 GB
    - No of Processor: 4
    - Secondary Storage: 30GB
    - Username : user
    - Password : user
3. Open VirtualBox (the provided ova file can be executed on any supported Virtual Machine software, we provide instructuions pertaining to VirtualBox software) 
4. Navigate to File -> Import Appliance -> Select provided ova file
5. Choose the desired setting of RAM (we recommend 8GB or more) and No of CPUs
6. Click Import
7. Once import is complete, select the VM and click "Start"
8. Login using above username and password

### Building the tool

1. Navigate to the artifact's directory using command `cd ~/Desktop/Artifact/`
2. Execute shell script `./compile.sh`, this command should generate the executable ./build/main.out 

### Simple example 1:

We run our tool on the example illustrated in Fig.1 of the *On-The-Fly Static Analysis via Dynamic Bidirected Dyck Reachability* paper. 

The file `./input/fig1.dot` is the initial graph, and `./input/fig1.seq` is the graph update sequence. Execute the following command:

```
./build/main.out --show_result ./input/fig1.dot ./input/fig1.seq
```

Expected output:
```
h,
d,
f,
g,
c,e,
```
The output above shows the reachable nodes in the graph, where each line is a new component.

### Simple example 2 ( < 5 mins ):

Change to directory : `cd ~/Desktop/Artifact/Benchmarks/`

We run a simple comparision with Offline and Datalog algorithms over a subset of DataDependence benchmarks (details of this command is explained in further sections)
```
python3 run.py 3 all incr 90 DataDep btree,check,sample,mushroom dir_simple_ex_2
```

Above command should generate a csv of running time in the directory : `./results/dir_simple_ex_2/all_incr_90_DataDep_btreechecksamplemushroom.csv`.

To generate plot from csv, execute: 
```
python3 plot.py dir_simple_ex_2
```

This should generate plot in `./results/dir_simple_ex_2/all_incr_90_DataDep_btreechecksamplemushroom.pdf`

The plot should look something like `./precompiled_results/SimpleExample2/all_incr_90_DataDep_btreechecksamplemushroom.pdf`

>Note: Executing the same command again will overwrite the produced results.


## Phase 2 : Artifact Evaluation
The artifact evaluation is divided into two parts. Part 1 focuses on replicating the data that supports our claim that our algorithm/tool outperforms baseline algorithms (Offline and DataLog) on Data Dependence benchmarks and Alias Analysis benchmarks. Part 2 produces all the relevant data, as shown in Figure 10 of the submitted paper.

>Note: All the commands in this section should be executed from directory : `~/Desktop/Artifact/Benchmarks/`


>Note: All the commands in this section execute the tool for 90-10 split setting (Fig.10 in the paper). The same commands can be executed for Fig. 11 (80_20_split) and Fig.12 (100_0_split) by passing 80 and 100 respectively as the *split* argument to run.py file.
Refer the subsection **"Details about evaluation script run.py"** for detailed description of the run.py script. 

>Note: Precompiled results for all benchmarks are available at `./Benchmarks/precompiled_results/`


### Part 1 
Replicating the experiments that appear in the submitted paper requires a significant amount of time `(4 days)`. We conducted three runs of all benchmarks for all algorithms (Dynamic, Offline, and Datalog) and reported the average time. Our algorithm (DynamicAlgo) finishes execution quickly, while the baseline algorithms (Offline and Datalog) take a significant amount of time. Under these circumstances, we provide the reviewer with the following two options to evaluate the tool.

#### Option 1 ( < 4 hours )
We average the runtime calculation over a **single run**. 
This option has two parts.

#### Option 1 Subpart A ( < 1 hours )
We compute runtime for **all Data Dependence benchmarks except xml** on all settings (incr,decr,mixed) over 90-10 split (Fig.10 in the paper), comparing Offline,Dynamic and DDLog algorithms.

```
python3 run.py 1 all all 90 DataDep btree,check,compiler,compress,crypto,derby,helloworld,mpegaudio,mushroom,parser,sample,scimark,startup,sunflow except_xml_datadep
```

Above command should generate following csv files:
- `./results/except_xml_datadep/all_incr_90_DataDep_btreecheckcompilercompresscryptoderbyhelloworldmpegaudiomushroomparsersamplescimarkstartupsunflow.csv`
- `./results/except_xml_datadep/all_mixed_90_DataDep_btreecheckcompilercompresscryptoderbyhelloworldmpegaudiomushroomparsersamplescimarkstartupsunflow.csv`
- `./results/except_xml_datadep/all_decr_90_DataDep_btreecheckcompilercompresscryptoderbyhelloworldmpegaudiomushroomparsersamplescimarkstartupsunflow.csv`

To generate plot for above csv, execute: 
```
python3 plot.py except_xml_datadep
```

#### Option 1 Subpart B ( < 3 hours )

We compute runtime for **3 AliasAnalysis benchmarks** on **decremental settings** over 90-10 split (Fig.10 in the paper), comparing Offline,Dynamic and DDLog algorithms.
```
python3 run.py 1 all decr 90 AliasAnalysis hsqldb,luindex,xalan three_alias_bench
```
Above command should generate following csv files:
- `./results/three_alias_bench/all_decr_90_AliasAnalysis_hsqldbluindexxalan.csv`

To generate plot from csv, execute: 
```
python3 plot.py three_alias_bench
```

#### Option 2 (< 13 hours)
We average the runtime calculation over a **single run**, but over more benchmarks and setting.
This option has two parts.

#### Option 2 Subpart A (< 3 hours)

We compute runtime for **all Data Dependence benchmarks** on all settings (incr,decr,mixed) over 90-10 split (Fig.10 in the paper), comparing Offline,Dynamic and DDLog algorithms.

```
python3 run.py 1 all all 90 DataDep all all_datadep
```
Above command should generate following csv files:
- `./results/all_datadep/all_incr_90_DataDep_all.csv`
- `./results/all_datadep/all_mixed_90_DataDep_all.csv`
- `./results/all_datadep/all_decr_90_DataDep_all.csv`

To generate plot for above csv, execute: 
```
python3 plot.py all_datadep
```

#### Option 2 Subpart B (< 10 hours)

We compute runtime for **any 4 AliasAnalysis benchmarks** on **all settings (incr,decr,mixed)** over 90-10 split (Fig.10 in the paper), comparing Offline,Dynamic and DDLog algorithms.

```
python3 run.py 1 all all 90 AliasAnalysis antlr,eclipse,jython,pmd four_alias_bench
```

Above command should generate following csv files:
- `./results/four_alias_bench/all_incr_90_AliasAnalysis_antlreclipsejythonpmd.csv`
- `./results/four_alias_bench/all_mixed_90_AliasAnalysis_antlreclipsejythonpmd.csv`
- `./results/four_alias_bench/all_decr_90_AliasAnalysis_antlreclipsejythonpmd.csv`

To generate plot for above csv, execute: 
```
python3 plot.py four_alias_bench
```    


### Part 2
Reproduce all the relevant data as shown in Fig.10 of the submitted paper averaged over three runs.

#### subpart A (< 10 hours)

We compute runtime for **all Data Dependence benchmarks** on ***all settings (incr,decr,mixed)*** over ***90-10 split*** (Fig.10 in the paper), comparing Offline,Dynamic and DDLog algorithms, averaged over 3 runs.

```
python3 run.py 3 all all 90 DataDep all all_datadep_3
```

Above command should generate following csv files:
- `./results/all_datadep_3/all_incr_90_DataDep_all.csv`
- `./results/all_datadep_3/all_mixed_90_DataDep_all.csv`
- `./results/all_datadep_3/all_decr_90_DataDep_all.csv`

To generate plot for above csv, execute: 
```
python3 plot.py all_datadep_3
```
    
#### subpart B (< 80 hours)
We compute runtime for **all AliasAnalysis benchmarks** on ***all settings (incr,decr,mixed)*** over ***90-10 split*** (Fig.10 in the paper), comparing Offline,Dynamic and DDLog algorithms, averaged over 3 runs.

```
python3 run.py 3 all all 90 AliasAnalysis all all_alias_bench_3
```

Above command should generate following csv files:
- `./results/all_alias_bench_3/all_incr_90_DataDep_all.csv`
- `./results/all_alias_bench_3/all_mixed_90_DataDep_all.csv`
- `./results/all_alias_bench_3/all_decr_90_DataDep_all.csv`


To generate plot for above csv, execute: 
```
python3 plot.py all_alias_bench_3
```

### Details about evaluation script run.py

`python3 run.py <noOfRuns> <algorithms> <sequence_setting> <split> <analysisType> <benchmark_names> <directory_name>`

-   noOfRuns : Specify the no of runs to average over the runtime. Pass 1 for quicker execution.

-   algorithms : Specify the algorithms to be compared. Provide comma separated string containing any combination of (Offline,Dynamic,DDLog) or "all". 
Single csv is generated comparing runtimes of provided algorithms.
Example, "Offline,Dynamic" "Dynamic,DDLog" "all".

-   sequence_setting : Specify the sequence setting. Provide comma separated string containing any combination of (incr,decr,mixed) or "all". 
Separate csv are generated for each sequence setting.
Example, "incr,decr" "incr,mixed" "all".

-   split : Specify the split percentage of sequence. Provide comma separated string containing any combination of (80,90,100) or "all". 
Separate csv are generated for each split setting.
Value 80 corresponds to Fig 11 in supplementary material where sequence files are generated with 80-20 split of original graph.
Value 90 corresponds to Fig 10 in the paper where sequence files are generated with 90-10 split of original graph.
Value 100 corresponds to Fig 12 in supplementary material where sequence files are generated with 100-0 split of original graph.
Example, "80,90" "90" "80,100" "all"

-   analysisType : Specify the analysis type. Provide ONE of (AliasAnalysis,DataDep).
"AliasAnalysis" corresponds to the benchmarks : antlr,bloat,chart,eclipse,fop,hsqldb,jython,luindex,lusearch,pmd,xalan
"DataDep" corresponds to the benchmarks : btree,check,compiler,compress,crypto,derby,helloworld,mpegaudio,mushroom,parser,sample,scimark,startup,sunflow,xml
Example, "alisaAnalysis" "DataDep"

-   benchmark_names : This argument is used if only a subset of benchmarks are to be executed. Provide a comma separated string of specific benchmarks within the provided analysisType. 
Single csv is generated comparing provided benchmarks.
If analysisType is "AliasAnalysis", you can provide comma separated string containing any combination of (antlr,bloat,chart,eclipse,fop,hsqldb,jython,luindex,lusearch,pmd,xalan) or "all"
If analysisType is "DataDep", you can provide comma separated string containing any combination of (btree,check,compiler,compress,crypto,derby,helloworld,mpegaudio,mushroom,parser,sample,scimark,startup,sunflow,xml) or "all"
Example, "btree,check,derby" "hsqldb,jython,luindex" "all"

-   director_name : csv files are be written to ./results/directory_name/

Examples:
1. To generate DataDependenceAnalysis incremental plot in Fig 10 of the paper, execute:

    `python3 run.py 1 all incr 90 DataDep all dir1`
    
    Above command will generate all_incr_90_DataDep_all.csv in ./results/dir1/

2. To generate DataDependenceAnalysis decremental and mixed plot in Fig 10 of the paper comparing only Offline and Dynamic, execute:

    `python3 run.py 1 Offline,Dynamic decr,mixed 90 DataDep all dir3`

    Above command will generate OfflineDynamic_decr_90_DataDep_all.csv, OfflineDynamic_mixed_90_DataDep_all.csv in ./results/dir3/

3. To generate AliasAnalysis mixed in Fig 10 of the paper, only for bloat and eclipse benchmark, execute:

    `python3 run.py 1 all mixed 90 AliasAnalysis bloat,eclipse dir4`

    Above command will generate all_mixed_90_AliasAnalysis_bloateclipse.csv in ./results/dir4

4. To generate incremental and decremental plots from Fig 11 and Fig 12 for DataDep benchmarks btree,check and crypto comparing Dynamic and DDLog algorithms, execute:

    `python3 run.py 1 Dynamic,DDLog incr,decr 80,100 DataDep btree,check,crypto dir6`

    Above command will generate DynamicDDLog_incr_80_DataDep_btreecheckcrypto.csv, DynamicDDLog_decr_80_DataDep_btreecheckcrypto.csv, DynamicDDLog_incr_90_DataDep_btreecheckcrypto.csv, DynamicDDLog_decr_90_DataDep_btreecheckcrypto.csv in ./results/dir6/

## Evaulating Reusability 

### Running custom benchmarks

Users can obtain the running time of Offline, Dynamic and DDLog over custom benchmarks.

This is done using the following script : `python3 run.py <noOfRuns> <algorithms> <initgraph> <seqfile> <outfilecsv>` 
- noOfRuns : Specify the no of runs to average over the runtime. Pass 1 for quicker execution.
- algorithms : Specify the algorithms to be compared. Provide comma separated string containing any combination of (Offline,Dynamic,DDLog) or "all".
- initgraph : dot file containing the initial graph. 
- seqfile : sequence file containing the edge insert and/or delete operations. 

The scripts outputs the running time of given algorithms over given initial graph and sequence file.

Create DDLog input from the initial graph and sequence file, and execute the script : 
```
python3 genDDLogInput.py ./CustomBenchmarks/bm1.dot ./CustomBenchmarks/bm1.seq > ./CustomBenchmarks/bm1.dat
python3 runCustomBenchmarks.py 3 all ./CustomBenchmarks/bm1.dot ./CustomBenchmarks/bm1.seq ./CustomBenchmarks/bm1.dat
```


Expected output will be similar to :

```
Executing DynamicAlgo...
./../build/main.out ./CustomBenchmarks/bm1.dot ./CustomBenchmarks/bm1.seq
DynamicAlgo duration : 77.0 microsecs
./../build/main.out ./CustomBenchmarks/bm1.dot ./CustomBenchmarks/bm1.seq
DynamicAlgo duration : 69.0 microsecs
./../build/main.out ./CustomBenchmarks/bm1.dot ./CustomBenchmarks/bm1.seq
DynamicAlgo duration : 70.0 microsecs
Avg DynamicAlgo duration : 72.0 Sequence Length : 37
Amortized DynamicAlgo duration : 1.945945945945946 microsecs
Executing OfflineAlgo...
./../OfflineAlgo/Alias_Analysis/build/main.out ./CustomBenchmarks/bm1.dot ./CustomBenchmarks/bm1.seq 0
OfflineAlgo duration : 635.0 microsecs
./../OfflineAlgo/Alias_Analysis/build/main.out ./CustomBenchmarks/bm1.dot ./CustomBenchmarks/bm1.seq 0
OfflineAlgo duration : 632.0 microsecs
./../OfflineAlgo/Alias_Analysis/build/main.out ./CustomBenchmarks/bm1.dot ./CustomBenchmarks/bm1.seq 0
OfflineAlgo duration : 629.0 microsecs
Avg offlineTime duration : 632.0 Sequence Length : 37
Amortized OfflineAlgo duration : 17.08108108108108 microsecs
Executing DDLog...
./../DDLogTool/BiDyckReach_cli < ./CustomBenchmarks/bm1.dat
DDLog duration : 9977.014000000001 microsecs
./../DDLogTool/BiDyckReach_cli < ./CustomBenchmarks/bm1.dat
DDLog duration : 11077.85 microsecs
./../DDLogTool/BiDyckReach_cli < ./CustomBenchmarks/bm1.dat
DDLog duration : 9507.429 microsecs
Avg DDLog duration : 10187.431 Sequence Length : 37
Amortized DDLog duration : 275.33597297297297 microsecs
```


### Directory description

- ./alias_analysis : Implementation of [Chatterjee et al. 2018] described as **Algorithm 1: OfflineAlgo (Page 9)** in the paper. 
- ./dynamic-connectivity-hdt-master : Implementation of  [Holm et al. 2001] (maintained at [Tseng 2020]), provides a datastructure for maintaining dynamic reachability on undirected graphs, referred to as **PrimCompDS** in the paper.
- ./DDLogTool : Dyck reachability rules are encoded as Datalog constraints and executed by [Differential Datalog].
- ./OfflineAlgo : Implementation of [Chatterjee et al. 2018], with changes made to accept an initial graph and a sequence of update edge operations, and invoke the OfflineAlgo from scratch to handle each update.
- ./src and ./include :
    - ./include/DynamicDyckReachability.h and ./src/DynamicDyckReachability.cpp : Implements the **Class DynamicDyckReachability**, which contain the algorithms developed in the paper.
    - ./include/Helper.h and ./src/Helper.cpp: Implements a Helper Class, with datastructures and methods which aid in running the alogrithm.
    - ./src/main.cpp : Execution of the tool starts from main.cpp
- ./Benchmarks : contains the benchmarks and scripts to execute the same
    - ./Benchmarks/CompletedGraphs : contains the complete graphs of the benchmarks
    - ./Benchmarks/CustomBenchmarks : contains example for custom benchmark
    - ./Benchmarks/precompiled_results : contains the precompiled results of the benchmarks
- ./build : contains the executable main.out generated after running compile.sh
- ./input : contains sample inputs

### Class DynamicDyckReachability
The Dynamic Dyck Reachability Algorithm is designed and implemented as a Class to aid in easy reusability and maintainance. 
We describe the code in comments in the following files:
- ./include/DynamicDyckReachability.h
- ./src/DynamicDyckReachability.cpp

Above code files provide the following methods:
- DynamicDyckReachability::insertDirectedEdge(fromNode, toNode, edgeType) : This method implements **Algorithm 2** in the paper.
- DynamicDyckReachability::deleteDirectedEdge(fromNode, toNode, paranthesisType) : This method implements **Algorithm 3** and **Algorithm 4** in the paper.

The provided can be easily incorporated in different codebase and above methods provide an easy interface for implementing the Dynamic Dyck Reachability algorithm.

[//]: # (These are reference links)
   [Chatterjee et al. 2018]: <https://dl.acm.org/doi/10.1145/3158118>
   [Holm et al. 2001]: <https://doi.org/10.1145/502090.502095>
   [Tseng 2020]: <https://github.com/tomtseng/dynamic-connectivity-hdt>
   [GithubRepo]: <https://github.com/lalaniket8/DynamicDyckReachability_artifact>
   [Differential Datalog]: <https://github.com/vmware/differential-datalog>
   [Zenodo]: <https://zenodo.org/record/8405641>
