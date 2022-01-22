#!/usr/bin/python3
import os
import pandas as pd
import glob
import seaborn as sns
import matplotlib.pyplot as plt

def ReadandMerge():
    path = os.getcwd()
    all_files = glob.glob(path + "/data/master/*.csv")
    appended_data = []
    for filename in all_files:
        df = pd.read_csv(filename)
        appended_data.append(df)
    dfResults = pd.concat(appended_data)
    # handle typo in column of old measurement files
    if 'com_intervall' in dfResults.columns:
        dfResults.rename(columns = {'com_intervall':'com_interval'}, inplace = True)

    return dfResults


def ParseDevice(dfResults, target):
    dataFrame = dfResults[(dfResults['target'] == target)];
    return dataFrame


def ParseSchema(dfResults):
    dataSchema01 = dfResults[(dfResults['schema'] == "MpiViSchema01")];
    dataSchema02 = dfResults[(dfResults['schema'] == "MpiViSchema02")];
    dataSchema03 = dfResults[(dfResults['schema'] == "MpiViSchema03")];
    return dataSchema01, dataSchema02, dataSchema03


def BoxPlot(dataTarget, x, y, hue):
    dfResults = dataTarget.getDataFrame()

    plt.figure()
    boxplot = sns.boxplot(x=dfResults[x], y=dfResults[y], hue=dfResults[hue], data=dfResults)
    boxplot.set_title('Runtime')
    boxplot.set_ylabel(str(y))
    boxplot.set_xlabel(str(x))
    plt.savefig(dataTarget.getOutDir() + dataTarget.getName() + "/" + "boxplot_" + x + ".svg")
    plt.close()
    return


def Lineplot(dataTarget, x, y):
    dfResults = dataTarget.getDataFrame()
    dataSchema01, dataSchema02, dataSchema03 = ParseSchema(dfResults)

    plt.figure()
    plt.plot(dataSchema01[x], dataSchema01[y], label='Schema3')
    plt.plot(dataSchema02[x], dataSchema02[y], label='Schema2')
    plt.plot(dataSchema03[x], dataSchema03[y], label='Schema1')
    plt.legend()
    # new_list = range(math.floor(min(dataSchema02['com_intervall'])), math.ceil(max(dataSchema02['com_intervall']))+1)
    #  plt.xticks(new_list)
    plt.ylabel(str(y))
    plt.xlabel(str(x))
    #plt.show()
    plt.savefig(dataTarget.getOutDir() + dataTarget.getName() + "/" + "lineplot_" + x + ".svg")
    plt.close()
    return


def Scatterplot(dataTarget, x, y, hue):
    dfResults = dataTarget.getDataFrame()

    scatterplot = sns.scatterplot(data=dfResults, x=dfResults[x], y=dfResults[y], hue=dfResults[hue])
    scatterplot.set_ylabel(str(y))
    scatterplot.set_xlabel(str(x))
    # scatterplot.set_legend()
    plt.figure()
    plt.savefig(dataTarget.getOutDir() + dataTarget.getName() + "/" + "scatterplot_" + x + ".svg")
    plt.close()
    return


def Jointplot(dataTarget, x, y):
    dfResults = dataTarget.getDataFrame()

    jointplot = sns.JointGrid(data=dfResults, x=dfResults[x], y=dfResults[y])
    plt.figure()
    plt.savefig(dataTarget.getOutDir() + dataTarget.getName() + "/" + "jointplot_" + x + ".svg")
    plt.close()
    # Joint.set_ylabel(str(y))
    # Joint.set_xlabel(str(x))
    return


def Barplot(dataTarget, x, y):
    dfResults = dataTarget.getDataFrame()
    dataSchema01, dataSchema02, dataSchema03 = ParseSchema(dfResults)

    plt.figure()
    plt.bar(dataSchema03[x] - 0.25, dataSchema03[y], label='Schema3', width=0.25)
    plt.bar(dataSchema02[x], dataSchema02[y], label='Schema2', width=0.25)
    plt.bar(dataSchema01[x] + 0.25, dataSchema01[y], label='Schema1', width=0.25)
    plt.legend()

    plt.ylabel(str(y))
    plt.xlabel(str(x))
    plt.savefig(dataTarget.getOutDir() + dataTarget.getName() + "/" + "barplot_" + x + ".svg")
    plt.close()
    return


def GeneratePlots(keyTarget):
    dataTarget = DataTarget(keyTarget, ParseDevice(dfResults, keyTarget))

    if not dataTarget.getDataFrame().empty:
        path = os.path.join(dataTarget.getOutDir() + dataTarget.getName())
        try:
            os.makedirs(path)
        except OSError as error:
            print("OSError: " + str(error))
            pass

        BoxPlot(dataTarget, 'world_size', 'runtime_vi_ms', 'schema')
        BoxPlot(dataTarget, 'com_interval', 'runtime_vi_ms', 'schema')
        Barplot(dataTarget, 'world_size', 'rss_max_rank0_kb')
        Lineplot(dataTarget, 'world_size', 'steps_total')
        Scatterplot(dataTarget, 'world_size', 'rss_max_rank0_kb', 'schema')
        Scatterplot(dataTarget, 'world_size', 'eps_global', 'schema')
        Jointplot(dataTarget, 'runtime_vi_ms', 'eps_global')
    return


class DataTarget():
    def getName(self):
        return self.name

    def getDataFrame(self):
        return self.data_frame

    def getOutDir(self):
        return self.out_dir

    def __init__(self, name, data_frame):
        self.name = name
        self.data_frame = data_frame
        self.out_dir = "./gen/img/"


if __name__ == "__main__":
    dfResults = ReadandMerge()

    keyTarget = ["hpcclassa", "hpcclassb", "hpcclassmixed", "nuc", "rpi", "local"]
    for key in keyTarget:
        GeneratePlots(key)