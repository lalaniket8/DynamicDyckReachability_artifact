import pandas as pd,sys,subprocess,os

def readData(outfilecsv):
    dataframe = pd.read_csv(outfilecsv)
    legendtext = ""
    offlineCoords = ""
    dynamicCoords = ""
    ddlogCoords = ""
    algos = []
    bmnames = dataframe['Benchmark'].str.cat(sep=',')
    xtickvalues = ','.join([str(i) for i in range(1, len(dataframe) + 1)])
    if 'offlineAlgo(microsecs)' in dataframe.columns:
        algos.append("Offline")
        offlineCoords = "\\addplot [fill=\offlinecolor] coordinates {\n"
        for el in [f'({index + 1},{value})' for index, value in enumerate(dataframe['offlineAlgo(microsecs)'])]:
            offlineCoords = offlineCoords + el + "\n"
        offlineCoords = offlineCoords + "};"
    if 'ddlog(microsecs)' in dataframe.columns:
        algos.append("DDLog")
        ddlogCoords = "\\addplot [fill=\ddLogcolor] coordinates {\n"
        for el in [f'({index + 1},{value})' for index, value in enumerate(dataframe['ddlog(microsecs)'])]:
            ddlogCoords = ddlogCoords + el + "\n"
        ddlogCoords = ddlogCoords + "};"
    if 'dynamicAlgo(microsecs)' in dataframe.columns:
        algos.append("Dynamic")
        dynamicCoords = "\\addplot [fill=\dynamiccolor] coordinates {\n"
        for el in [f'({index + 1},{value})' for index, value in enumerate(dataframe['dynamicAlgo(microsecs)'])]:
            dynamicCoords = dynamicCoords + el + "\n"
        dynamicCoords = dynamicCoords + "};"
    legendtext = ','.join(algos)
    return [xtickvalues,bmnames,legendtext,offlineCoords,ddlogCoords,dynamicCoords]

def writeToTemplate(data, destTexFilename):
    xtickvalues = data[0]
    bmnames = data[1]
    legendtext = data[2]
    offlineplot = data[3]
    ddlogplot = data[4]
    dynamicplot = data[5]
    plot_template_content = ""

    try:
        with open('plot_template.tex', 'r') as f:
            plot_template_content = f.read()
    except FileNotFoundError:
        print(f"File plot_template.tex not found.")
        exit(1)
    except Exception as e:
        print(str(e))
        exit(1)
    plot_template_content = plot_template_content.replace("<<xtickvalues>>",xtickvalues)
    plot_template_content = plot_template_content.replace("<<bmnames>>",bmnames)
    plot_template_content = plot_template_content.replace("<<legendtext>>",legendtext)
    plot_template_content = plot_template_content.replace("<<offlineplot>>",offlineplot)
    plot_template_content = plot_template_content.replace("<<ddlogplot>>",ddlogplot)
    plot_template_content = plot_template_content.replace("<<dynamicplot>>",dynamicplot)
    plot_template_content = plot_template_content.replace("<<captiontext>>",destTexFilename.replace('_','\_'))

    try:
        with open(destTexFilename, 'w') as f:
           f.write(plot_template_content)
    except Exception as e:
        print(str(e))
        exit(1)
    
def createPDF(dir, texfilename, pdffilename):
    cmd = f'cd ./results/{dir} && pdflatex {texfilename} && rm *.aux *.log *.out'
    print(cmd)
    subprocess.check_output(cmd,shell=True)
    print(f"Created ./results/{dir}/{pdffilename}")

if __name__ == "__main__":
    if(len(sys.argv) != 2):
        print("USAGE: python3 plot.py <dirname>")
        print("<dirname> : refers to the directory name passed to run.py. Python script run.py creates this directory inside results directory")
        print("plot.py will create plots of all csv files present in ./results/<dirname>")
        print("example: python3 plot.py dir1\nAbove command will generate plot.pdf files for csv files present in ./results/dir1 director")
        exit(1)
    
    dirName = sys.argv[1]
    csvfiles = [file for file in os.listdir(f'./results/{dirName}') if file.endswith('.csv')]
    for csvfile in csvfiles:
        data = readData(f'./results/{dirName}/{csvfile}')
        texfilename = f'{csvfile.split(".")[0]}.tex'
        pdffilename = f'{csvfile.split(".")[0]}.pdf'
        writeToTemplate(data,f'./results/{dirName}/{texfilename}')
        createPDF(dirName,texfilename,pdffilename)