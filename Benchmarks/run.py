import os,re,subprocess,sys,datetime

runs = 1 #average runtime calculation over these many runs

ddlog_executable = "./../DDLogTool/BiDyckReach_cli"
dynamicAlgo_executable = './../build/main.out'
offlineAlgo_executable = './../OfflineAlgo/Alias_Analysis/build/main.out'

AliasAnalysis=[
'antlr',
'bloat',
'chart',
'eclipse',
'fop',
'hsqldb',
'jython',
'luindex',
'lusearch',
'pmd',
'xalan'
]

DataDep = [
'btree',
'check',
'compiler',
'compress',
'crypto',
'derby',
'helloworld',
'mpegaudio',
'mushroom',
'parser',
'sample',
'scimark',
'startup',
'sunflow',
'xml'
]

usagestring = """
Usage: python3 run.py <noOfRuns> <algorithms> <sequence_setting> <split> <analysisType> <benchmark_names> <directory_name>

<noOfRuns> : Specify the no of runs to average over the runtime. Pass 1 for quicker execution.

<algorithms> : Specify the algorithms to be compared. Provide comma separated string containing any combination of (Offline,Dynamic,DDLog) or "all". 

<sequence_setting> : Specify the sequence setting. Provide comma separated string containing any combination of (incr,decr,mixed) or "all". 

<split> : Specify the split percentage of sequence. Provide comma separated string containing any combination of (80,90,100) or "all". 

<analysisType> : Specify the analysis type. Provide ONE of (AliasAnalysis,DataDep).

<benchmark_names> : This argument is used if only a subset of benchmarks are to be executed. Provide a comma separated string of specific benchmarks within the provided analysisType. 
Single csv is generated comparing provided benchmarks.

<director_name> : csv files are be written to ./results/<directory_name>/

Please refer to the Readme.md for detailed explanation.
"""


def getDuration(resultStr):#computes duration in microsecs
    result = resultStr[2:-3].split('\\n')
    startTimestamp = int(result[0][11:])
    endTimestamp = int(result[1][11:])
    duration = (endTimestamp - startTimestamp)*0.001#0.000000001
    return duration

def getSeqLen(seq_filename):
    linecount = 0
    with open(seq_filename, "r") as file:
        for line in file:
            line = line.strip()
            if line != "":
                linecount = linecount + 1
    # print(f'{seq_filename}|{linecount}')
    return linecount

def getGraphParameters(initgraphpath):
    with open(initgraphpath, "r") as file:
        NODES = set()
        EDGETYPES = set()
        edgeCount = 0
        for line in file:
            line = line.strip()
            pattern = r'([A-Za-z0-9_]+)->([A-Za-z0-9_]+)\[label="([A-Za-z0-9_-]+)"\]'
            matches = re.search(pattern, line.strip())
            if not matches:
                print("regex does not match")
                exit(1)
            node_from = matches.group(1)
            node_to = matches.group(2)
            edgeinfo = matches.group(3)
            edgeCount = edgeCount + 1
            edgedets = edgeinfo.split("--")
            edge_type = edgedets[1]
            if edgedets[0] == "op":
                temp = node_from
                node_from = node_to
                node_to = temp

            if node_from not in NODES:
                NODES.add(node_from)
            if node_to not in NODES:
                NODES.add(node_to)
            if edge_type not in EDGETYPES:
                EDGETYPES.add(edge_type)

    return {"NoOfNodes":len(NODES),"NoOfEdges":edgeCount , "NoOfParanthesis":len(EDGETYPES)}

def getFullGraphParameters():
    fullGraphParameters = {}
    for bm in AliasAnalysis:
        params = getGraphParameters(f'./CompleteGraphs/AliasAnalysis/{bm}.dot')
        fullGraphParameters[f'{bm}'] = params
    for bm in DataDep:
        params = getGraphParameters(f'./CompleteGraphs/DataDepAnalysis/{bm}.dot')
        fullGraphParameters[f'{bm}'] = params
    return fullGraphParameters

fullGraphParams = getFullGraphParameters()#computes No of nodes, No of edges and No of paranthesis of full graphs (Table2 and Table3 in paper)

def executeAlgo(initgraph_filename, seq_filename, ddlog_filename, runOffline, runDynamic, runDDLog):#in microsecs

    seqcount = getSeqLen(seq_filename)
    amortizedDynamicTime=0
    amortizedOfflineTime=0
    amortizedDDlogTime=0
    
    if runDynamic:
        DynamicTime = 0
        print("Executing DynamicAlgo...")
        for _ in range(0,runs):
            cmd1 = f'{dynamicAlgo_executable} {initgraph_filename} {seq_filename}'
            print(cmd1)
            DynamicTimetaken = ((float)((str(subprocess.check_output(cmd1,shell=True))[2:-3]).split(":")[1])) * 1000000
            print(f"DynamicAlgo duration : {DynamicTimetaken} microsecs")
            DynamicTime = DynamicTime + DynamicTimetaken
        DynamicTime = DynamicTime/runs
        print(f'Avg DynamicAlgo duration : {DynamicTime} Sequence Length : {seqcount}')
        amortizedDynamicTime = DynamicTime/seqcount
        print(f'Amortized DynamicAlgo duration : {str(amortizedDynamicTime)} microsecs')

    if runOffline:
        offlineTime = 0
        print("Executing OfflineAlgo...")
        for _ in range(0,runs):
            cmd2 = f'{offlineAlgo_executable} {initgraph_filename} {seq_filename} 0'
            print(cmd2)
            offlineTimetaken = ((float)(str(subprocess.check_output(cmd2,shell=True))[2:-3].split(":")[1])) * 1000000
            print(f"OfflineAlgo duration : {offlineTimetaken} microsecs")
            offlineTime = offlineTime + offlineTimetaken
        offlineTime = offlineTime/runs
        print(f'Avg offlineTime duration : {offlineTime} Sequence Length : {seqcount}')
        amortizedOfflineTime = offlineTime/seqcount
        print(f'Amortized OfflineAlgo duration : {str(amortizedOfflineTime)} microsecs')

    if runDDLog:
        ddlogTime = 0
        print("Executing DDLog...")
        for _ in range(0,runs):
            cmd = f'{ddlog_executable} < {ddlog_filename}'
            print(cmd)
            ddlog_runtime = str(getDuration(str(subprocess.check_output(cmd,shell=True))))
            print(f"DDLog duration : {ddlog_runtime} microsecs")
            ddlogTime = ddlogTime + float(ddlog_runtime)
        ddlogTime = ddlogTime/runs
        print(f'Avg DDLog duration : {ddlogTime} Sequence Length : {seqcount}')
        amortizedDDlogTime = ddlogTime/seqcount
        print(f'Amortized DDLog duration : {str(amortizedDDlogTime)} microsecs')

    return [str(amortizedDynamicTime), str(amortizedOfflineTime), str(amortizedDDlogTime), str(seqcount)]        

# split : string (80 or 90 or 100)
# setting : string (incr or decr or mixed)
# algos : array of string (array of string containing algos to be compared)
# AliasAnalysis : string (AliasAnalysis or DataDep)
# bms : array of string (all)
# outcsvfile : string (csv filename)
def computeRuntime(split, setting, algos, analysisType, bms, outfilecsv):

    algo_values = ['all','Offline','Dynamic','DDLog']
    split_values = ['80','90','100']
    setting_values = ['incr','decr','mixed']
    analysisType_values = ['AliasAnalysis','DataDep']
    runDynamic = False
    runOffline = False
    runDDLog = False
    splitName = ""
    benchmarks = []
    header = 'Benchmark,NoOfNodes,NoOfEdges,NoOfParanthesis,seqlen'


    if(analysisType not in analysisType_values):
        print(f'Incorrect analysisType:{analysisType}. Provide any ONE of the following: AliasAnalysis,DataDep')
        exit(1)
    
    if(len(algos) == 0):
        print(f'No algo specified. Provide any combination of the following: Offline,Dynamic,DDLog')
        exit(1)
    for algo in algos:
        if algo not in algo_values:
            print(f'Incorrect algo:{algo}. Provide any combination of the following: Offline,Dynamic,DDLog')
            exit(1)
        else:
            if(algo == 'Offline' or algo == 'all'):
                runOffline = True
                header += ",offlineAlgo(microsecs)"
            if(algo == 'Dynamic' or algo == 'all'):
                runDynamic = True
                header += ",dynamicAlgo(microsecs)"
            if(algo == 'DDLog' or algo == 'all'):
                runDDLog = True
                header += ",ddlog(microsecs)"
    header+='\n'

    if split not in split_values:
        print(f'Incorrect split:{split}. Provide any ONE of the following: 80,90,100')
        exit(1)
    else:
        if(split == '80'):
            splitName = '80_20_split'
        if(split == '90'):
            splitName = '90_10_split'
        if(split == '100'):
            splitName = '100_0_split'

    if(len(bms) == 0):
        print(f'No benchmark specified. Provide any combination of the following {analysisType} benchmarks: {str(["all"] + AliasAnalysis if (analysisType == "AliasAnalysis") else DataDep)}')
        exit(1)
    
    if("all" in bms):
        if(analysisType == "AliasAnalysis"):
            benchmarks = AliasAnalysis
        if(analysisType == "DataDep"):
            benchmarks = DataDep
    else:
        for bm in bms:
            if((bm in AliasAnalysis and analysisType=='AliasAnalysis') or (bm in DataDep and analysisType=='DataDep')):
                    benchmarks.append(bm)
            else:
                print(f'Incorrect benchmark:{bm}. Provide any combination of the following {analysisType} benchmarks: {str(["all"] + AliasAnalysis if (analysisType == "AliasAnalysis") else DataDep)}. \nPlease check if analysisType value passed is correct.\nThe analysisType value should be "AliasAnalysis" for the benchmarks {str(AliasAnalysis)}.\nThe analysisType value should be "DataDep" for the benchmarks {str(DataDep)}.')

    if setting not in setting_values:
        print(f'Incorrect setting:{setting}. Provide any ONE of the following: incr,decr,mixed')
        exit(1)
    else:
        resultfile = open(outfilecsv,"w")
        resultfile.write(header)
        print(f"Writing results to {outfilecsv}")
        for bm in benchmarks:

            if(setting == 'incr'):
                init = f"{splitName}/{analysisType}/{bm}/{bm}_init_incr.dot"
                seq = f"{splitName}/{analysisType}/{bm}/{bm}_incr.seq"
                dat = f"{splitName}/{analysisType}/{bm}/{bm}_incr.dat"
            if(setting == 'decr'):
                init = f"{splitName}/{analysisType}/{bm}/{bm}.dot"
                seq = f"{splitName}/{analysisType}/{bm}/{bm}_decr.seq"
                dat = f"{splitName}/{analysisType}/{bm}/{bm}_decr.dat"
            if(setting == 'mixed'):
                init = f"{splitName}/{analysisType}/{bm}/{bm}_init_mixed.dot"
                seq = f"{splitName}/{analysisType}/{bm}/{bm}_mixed.seq"
                dat = f"{splitName}/{analysisType}/{bm}/{bm}_mixed.dat"

            print(f'Computing benchmark {bm}')
            [dynamicTime, offlineTime, ddlogTime, seqlen] = executeAlgo(init, seq, dat, runOffline, runDynamic, runDDLog)
            print("")

            data = f'{bm},{fullGraphParams[bm]["NoOfNodes"]},{fullGraphParams[bm]["NoOfEdges"]},{fullGraphParams[bm]["NoOfParanthesis"]},{seqlen}'
            if(runOffline):
                data = data + "," + offlineTime
            if(runDynamic):
                data = data + "," + dynamicTime
            if(runDDLog):
                data = data + "," + ddlogTime
            data += '\n'

            resultfile.write(data)
            resultfile.flush()
        
if __name__ == "__main__":
    if(len(sys.argv) != 8):
        print(usagestring)
        exit(1)

    print(f'START: {datetime.datetime.fromtimestamp(datetime.datetime.now().timestamp()).strftime("%Y-%m-%d %H:%M:%S")}')

    runs = 1 if int(sys.argv[1]) <= 0 else int(sys.argv[1])
    arg_algos = sys.argv[2].split(",")
    arg_seqsetting = sys.argv[3].split(",")
    arg_split = sys.argv[4].split(",")
    arg_analysisType = sys.argv[5]
    arg_bmnames = sys.argv[6].split(",")
    arg_dir = sys.argv[7]

    if not os.path.exists(f'./results/{arg_dir}'):
        os.makedirs(f'./results/{arg_dir}')
        print(f"Creating directory ./results/{arg_dir}")

    if('all' in arg_seqsetting):
        arg_seqsetting = ["incr","decr","mixed"]
    
    if('all' in arg_split):
        arg_split = [80,90,100]

    for seqsetting in arg_seqsetting:
        for split in arg_split:
            outfilename = f'./results/{arg_dir}/{sys.argv[2].replace(",","")}_{seqsetting}_{split}_{arg_analysisType}_{sys.argv[6].replace(",","")}.csv'
            computeRuntime(split,seqsetting,arg_algos,arg_analysisType,arg_bmnames,outfilename)

    print(f'END: {datetime.datetime.fromtimestamp(datetime.datetime.now().timestamp()).strftime("%Y-%m-%d %H:%M:%S")}')
