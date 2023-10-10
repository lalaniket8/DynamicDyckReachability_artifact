import os,re,subprocess,sys,datetime

runs = 1 #average runtime calculation over these many runs

ddlog_executable = "./../DDLogTool/BiDyckReach_cli"
dynamicAlgo_executable = './../build/main.out'
offlineAlgo_executable = './../OfflineAlgo/Alias_Analysis/build/main.out'

usagestring = """
Usage: python3 run.py <noOfRuns> <algorithms> <initgraph> <seqfile> <outfilecsv>

<noOfRuns> : Specify the no of runs to average over the runtime. Pass 1 for quicker execution.

<algorithms> : Specify the algorithms to be compared. Provide comma separated string containing any combination of (Offline,Dynamic,DDLog) or "all". 

<initgraph> : dot file containing the initial graph. 

<seqfile> : sequence file containing the edge insert and/or delete operations. 

The computed runtimes will be displayed on the terminal.
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

if __name__ == "__main__":
    if(len(sys.argv) != 6):
        print(usagestring)
        exit(1)

    runs = 1 if int(sys.argv[1]) <= 0 else int(sys.argv[1])
    arg_algos = sys.argv[2].split(",")
    arg_initgraph = sys.argv[3]
    arg_seqfile = sys.argv[4]
    arg_datfile = sys.argv[5]
    algo_values = ['all','Offline','Dynamic','DDLog']
    if(len(arg_algos) == 0):
        print(f'No algo specified. Provide any combination of the following: Offline,Dynamic,DDLog')
        exit(1)
    for algo in arg_algos:
        if algo not in algo_values:
            print(f'Incorrect algo:{algo}. Provide any combination of the following: Offline,Dynamic,DDLog')
            exit(1)
        else:
            if(algo == 'Offline' or algo == 'all'):
                runOffline = True
            if(algo == 'Dynamic' or algo == 'all'):
                runDynamic = True
            if(algo == 'DDLog' or algo == 'all'):
                runDDLog = True
    
    executeAlgo(arg_initgraph, arg_seqfile, arg_datfile, runOffline, runDynamic, runDDLog)

