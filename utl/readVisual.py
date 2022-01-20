import os
import numpy as np
import pandas as pd
import glob
import seaborn as sns
import math
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter


def ReadandMerge():
    path = os.getcwd()
    all_files = glob.glob(path + "/*.csv")
    appended_data = []
    datadf = pd.DataFrame()
    for filename in all_files:
        df = pd.read_csv(filename)
        appended_data.append(df)
    dfResults = pd.concat(appended_data)
    return dfResults


def ParseDevice(dfResults):
    dataTargetHpcA = dfResults[(dfResults['target'] == "hpcclassa")];
    dataTargetLocal = dfResults[(dfResults['target'] == "local")];
    dataTargetNuc = dfResults[(dfResults['target'] == "nuc")];
    return dataTargetHpcA, dataTargetLocal, dataTargetNuc


def ParseSchema(dfResults):
    dataSchema01 = dfResults[(dfResults['schema'] == "MpiViSchema01")];
    dataSchema02 = dfResults[(dfResults['schema'] == "MpiViSchema02")];
    dataSchema03 = dfResults[(dfResults['schema'] == "MpiViSchema03")];
    return dataSchema01, dataSchema02, dataSchema03


def BoxPlot(dfResults, x, y, hue):
    boxplotruntime = sns.boxplot(x=dfResults[x], y=dfResults[y], hue=dfResults[hue], data=dfResults)
    boxplotruntime.set_title('Runtime  ')
    boxplotruntime.set_ylabel(str(y))
    boxplotruntime.set_xlabel(str(x))
    plt.figure()
    return


def Lineplot(dfResults, x, y):
    dataSchema01, dataSchema02, dataSchema03 = ParseSchema(dfResults)
    plt.plot(dataSchema01[x], dataSchema01[y], label='Schema3')
    plt.plot(dataSchema02[x], dataSchema02[y], label='Schema2')
    plt.plot(dataSchema03[x], dataSchema03[y], label='Schema1')
    plt.legend()
    # new_list = range(math.floor(min(dataSchema02['com_intervall'])), math.ceil(max(dataSchema02['com_intervall']))+1)
    #  plt.xticks(new_list)
    plt.ylabel(str(y))
    plt.xlabel(str(x))
    plt.figure()
    return


def Scatterplot(dfResults, x, y, hue):
    Scatter = sns.scatterplot(data=dfResults, x=dfResults[x], y=dfResults[y], hue=dfResults[hue])
    Scatter.set_ylabel(str(y))
    Scatter.set_xlabel(str(x))
    plt.figure()
    # Scatter.set_legend()
    return


def Jointplot(dfResults, x, y):
    Joint = sns.JointGrid(data=dfResults, x=dfResults[x], y=dfResults[y])
    # Joint.set_ylabel(str(y))
    # Joint.set_xlabel(str(x))
    plt.figure()

    return


def Barplot(dfResults, x, y):
    dataSchema01, dataSchema02, dataSchema03 = ParseSchema(dfResults)
    plt.bar(dataSchema03[x] - 0.25, dataSchema03[y], label='Schema3', width=0.25)
    plt.bar(dataSchema02[x], dataSchema02[y], label='Schema2', width=0.25)
    plt.bar(dataSchema01[x] + 0.25, dataSchema01[y], label='Schema1', width=0.25)
    plt.legend()

    plt.ylabel(str(y))
    plt.xlabel(str(x))
    plt.figure()
    return


if __name__ == "__main__":
    dfResults = ReadandMerge()
    print(dfResults)
    print(dfResults.info())
    dataTargetHpcA, dataTargetLocal, dataTargetNuc = ParseDevice(dfResults)
    print(dataTargetHpcA)
    print()

    BoxPlot(dataTargetHpcA, 'world_size', 'runtime_vi_ms', 'schema')
    BoxPlot(dataTargetHpcA, 'com_intervall', 'runtime_vi_ms', 'schema')
    Barplot(dataTargetHpcA, 'world_size', 'rss_max_kb')
    Lineplot(dataTargetHpcA, 'world_size', 'steps_total')
    Scatterplot(dataTargetHpcA, 'world_size', 'rss_max_kb', 'schema')
    Scatterplot(dataTargetHpcA, 'world_size', 'eps_global', 'schema')
    Jointplot(dataTargetHpcA, 'runtime_vi_ms', 'eps_global')
    plt.show()
