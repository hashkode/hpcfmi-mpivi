#!/usr/bin/python3
import glob

import math
import matplotlib.pyplot as plt
import os

import numpy as np
import pandas as pd
import pwd
import seaborn as sns
from distutils.dir_util import copy_tree

schema_list = ['MpiViSchema01', 'MpiViSchema02', 'MpiViSchema03']

class DataTarget():
    def __init__(self, name, data_frame, ds_dirstring):
        self.name = name
        self.data_frame = data_frame
        self.out_dir = "./gen/img/"
        self.ds_dirstring = ds_dirstring

    def getName(self):
        return self.name

    def getDataFrame(self):
        return self.data_frame

    def getOutDir(self):
        return self.out_dir

    def getDSDirString(self):
        return self.ds_dirstring


class DataSetConfiguration():
    keyDataSet = ["/data/data_debug/", "/data/data_small/", "/data/data_normal/"]
    dirStringDataSet = ["debug", "small", "normal"]

    # default, small, normal; each array holds the settings per plot type/category
    # 0: runtime (log!), 1: steps, 2: RAM
    ylimList = [[[], [], [], []],
                [[1, 100], [100, 8500], [10, 2000], [0, .25]],
                [[100, 10000], [100, 8500], [1000, 100000], [0, .5]]]

    def getNumberDatasets(self):
        return len(self.keyDataSet)

    def getDataKey(self, idx):
        return self.keyDataSet[idx]

    def getDirString(self, idx):
        return self.dirStringDataSet[idx]

    def getYLim(self, idxD, idxPlt):
        return self.ylimList[idxD][idxPlt]


class TargetConfiguration():
    targetKeys = ["hpcclassa", "hpcclassb", "hpcclassmixed", "nuc", "rpi", "local"]

    def getNumberTargets(self):
        return len(self.targetKeys)

    def getTargetKey(self, idx):
        return self.targetKeys[idx]


def CollectMeasurementFiles():
    user_name = pwd.getpwuid(os.getuid())[0]
    script_dir = os.getcwd()
    print("un: " + user_name + ", sr: " + script_dir)
    var_dir = "/var/tmp/"
    results_dirs = ["/results/measurement-campaign/"]
    for dir in results_dirs:
        copy_tree(var_dir + user_name + dir, script_dir + "/data/")


def ReadandMerge(dsConfiguration, idx):
    dataset = dsConfiguration.getDataKey(idx)
    path = os.getcwd()
    all_files = glob.glob(path + "/data/*.csv")
    appended_data = []
    for filename in all_files:
        df = pd.read_csv(filename)
        appended_data.append(df)
    dfResults = pd.concat(appended_data)

    dfResults = dfResults[(dfResults['data_set'] == dataset)]
    # add maximum configured iterations to remove not fully converged measurements
    filter_broken_runs = [5000, 10000]
    dfResults = dfResults[dfResults['steps_total'].isin(filter_broken_runs) == False]

    # calculate new columns with alternative units
    dfResults['runtime_vi_s'] = dfResults['runtime_vi_ms'] / 1000
    dfResults['rss_max_rank0_mb'] = dfResults['rss_max_rank0_kb'] / 1024
    dfResults['rss_sum_all_mb'] = dfResults['rss_sum_all_kb'] / 1024

    return dfResults


def ParseDevice(dfResults, target):
    dataFrame = dfResults[(dfResults['target'] == target)];
    return dataFrame


def ParseSchema(dfResults):
    dataSchema01 = dfResults[(dfResults['schema'] == schema_list[0])];
    dataSchema02 = dfResults[(dfResults['schema'] == schema_list[1])];
    dataSchema03 = dfResults[(dfResults['schema'] == schema_list[2])];
    return dataSchema01, dataSchema02, dataSchema03


def BoxPlot(dataTarget, x, y, hue, yLim=None, do_log=False, filter_com_intervall=True):
    dfResults = dataTarget.getDataFrame()

    # consider only standard set of com_interval values for better comparability
    if filter_com_intervall:
        filter_com_interval = [1, 2, 4, 8, 16, 32, 64]
        dfResults = dfResults[dfResults['com_interval'].isin(filter_com_interval) == True]

    if ((not dfResults[x].empty) and (not dfResults[y].empty) and (not dfResults[hue].empty)):
        ncount = len(dfResults.index)
        plt.figure()
        hue_order = schema_list
        boxplot = sns.boxplot(x=dfResults[x], y=dfResults[y], hue=dfResults[hue], data=dfResults, hue_order=hue_order,
                              showfliers=False)
        boxplot.set_title(dataTarget.getName() + ": " + y + "/" + x + " (" + str(ncount) + " runs)")
        boxplot.set_ylabel(str(y))
        boxplot.set_xlabel(str(x))
        if yLim != None:
            plt.ylim(yLim[0], yLim[1])

        if do_log:
            plt.yscale('log')

        plt.grid(axis='y')
        plt.grid(axis='x')
    if filter_com_intervall:
        SaveFigureToFilesTarget(dataTarget, "boxplot_" + x + "_" + y)
    else:
        SaveFigureToFilesTarget(dataTarget, "boxplot_" + x + "_" + y + "_raw")
    plt.close()
    return


def LinePlot(dataTarget, x, y):
    dfResults = dataTarget.getDataFrame()
    dfResults = dfResults.sort_values(x, ascending=False).reset_index(drop=True)
    dataSchema01, dataSchema02, dataSchema03 = ParseSchema(dfResults)

    plt.figure()
    plt.plot(dataSchema01[x], dataSchema01[y], label='Schema1')
    plt.plot(dataSchema02[x], dataSchema02[y], label='Schema2')
    plt.plot(dataSchema03[x], dataSchema03[y], label='Schema3')
    plt.legend()
    # new_list = range(math.floor(min(dataSchema02['com_intervall'])), math.ceil(max(dataSchema02['com_intervall']))+1)
    #  plt.xticks(new_list)
    plt.ylabel(str(y))
    plt.xlabel(str(x))
    plt.title("")
    SaveFigureToFilesTarget(dataTarget, "lineplot_" + x + "_" + y)
    plt.close()
    return


def ScatterPlot(dataTarget, x, y, hue):
    dfResults = dataTarget.getDataFrame()
    ncount = len(dfResults.index)

    plt.figure()
    hue_order = schema_list
    scatterplot = sns.scatterplot(data=dfResults, x=dfResults[x], y=dfResults[y], hue=dfResults[hue], hue_order=hue_order)
    scatterplot.set_ylabel(str(y))
    scatterplot.set_xlabel(str(x))
    scatterplot.set_title(dataTarget.getName() + ": " + y + "/" + x + " (" + str(ncount) + " runs)")
    plt.grid(axis='y')
    plt.grid(axis='x')
    # scatterplot.set_legend()
    SaveFigureToFilesTarget(dataTarget, "scatterplot_" + x + "_" + y)
    plt.close()
    return


def JointPlot(dataTarget, x, y):
    dfResults = dataTarget.getDataFrame()

    plt.figure()
    jointplot = sns.JointGrid(data=dfResults, x=dfResults[x], y=dfResults[y])
    SaveFigureToFilesTarget(dataTarget, "jointplot_" + x + "_" + y)
    plt.close()
    # Joint.set_ylabel(str(y))
    # Joint.set_xlabel(str(x))
    return


def BarPlot(dataTarget, x, y):
    dfResults = dataTarget.getDataFrame()
    dataSchema01, dataSchema02, dataSchema03 = ParseSchema(dfResults)

    plt.figure()
    plt.bar(dataSchema03[x] - 0.25, dataSchema03[y], label='Schema3', width=0.25)
    plt.bar(dataSchema02[x], dataSchema02[y], label='Schema2', width=0.25)
    plt.bar(dataSchema01[x] + 0.25, dataSchema01[y], label='Schema1', width=0.25)
    plt.legend()

    plt.ylabel(str(y))
    plt.xlabel(str(x))

    SaveFigureToFilesTarget(dataTarget, "barplot_" + x + "_" + y)
    plt.close()
    return

def DataDistributionPlot(dfResults, dsConfiguration, idxD):
    ncount = len(dfResults.index)

    plt.figure()
    axSmall = dfResults.groupby(['schema', 'target'])['schema'].count().unstack(0).plot.bar(
        title="Measurement count/target for <" + dsConfiguration.getDirString(idxD) + "> dataset" + " (" + str(ncount) + " runs)")
    axSmall.set_ylabel("Measurement count []")
    axSmall.set_xlabel("Target")

    y_min = min(dfResults.groupby(['schema', 'target'])['schema'].count())
    y_max = math.ceil(max(dfResults.groupby(['schema', 'target'])['schema'].count()))
    yint = np.linspace(min(0, y_min), y_max ,num = 10, endpoint = True).astype(int)
    plt.yticks(yint)

    plt.grid(axis='y')
    plt.grid(axis='x')
    graphicName = "number_measurement_target"
    SaveFigureToFilesData(dsConfiguration, graphicName, idxD)

    plt.figure()
    axSmallRuntime = dfResults.groupby(['schema', 'target'])['runtime_vi_s'].sum().unstack(0).plot.bar(
        title="Measurement duration/target for <" + dsConfiguration.getDirString(idxD) + "> dataset " + " (" + str(ncount) + " runs)")
    axSmallRuntime.set_ylabel("Measurement duration [s]")
    axSmallRuntime.set_xlabel("Target")
    plt.grid(axis='y')
    plt.grid(axis='x')
    graphicName = "runtime_measurement_target"

    SaveFigureToFilesData(dsConfiguration, graphicName, idxD)

    plt.figure()
    axSmallRuntime = dfResults.groupby(['schema', 'world_size'])['rss_max_rank0_mb'].mean().unstack(0).plot.line(
        title="Mean Max-RSS at rank0/world_size for <" + dsConfiguration.getDirString(idxD) + "> dataset"  + " (" + str(ncount) + " runs)")
    axSmallRuntime.set_ylabel("Max-RSS [MB]")
    axSmallRuntime.set_xlabel("world_size")
    plt.grid(axis='y')
    plt.grid(axis='x')
    graphicName = "max_rss_rank0_world_size"

    SaveFigureToFilesData(dsConfiguration, graphicName, idxD)

    plt.figure()
    axSmallRuntime = dfResults.groupby(['schema', 'world_size'])['rss_sum_all_mb'].mean().unstack(0).plot.line(
        title="Mean Max-RSS sum all ranks/world_size for <" + dsConfiguration.getDirString(idxD) + "> dataset"  + " (" + str(ncount) + " runs)")
    axSmallRuntime.set_ylabel("Max-RSS [MB]")
    axSmallRuntime.set_xlabel("world_size")
    plt.grid(axis='y')
    plt.grid(axis='x')
    graphicName = "rss_sum_all_world_size"

    SaveFigureToFilesData(dsConfiguration, graphicName, idxD)

    return


def SaveFigureToFilesTarget(dataTarget, graphicName):
    fileName = (dataTarget.getOutDir() + dataTarget.getName() + "/" + dataTarget.getDSDirString() + "/" + graphicName)
    plt.savefig(fileName + ".svg")
    plt.savefig(fileName + ".pdf")
    return


def SaveFigureToFilesData(dsConfiguration, graphicName, idxD):
    fileName = ("./gen/img/ds/" + dsConfiguration.getDirString(idxD) + "/" + graphicName)
    plt.savefig(fileName + ".svg")
    plt.savefig(fileName + ".pdf")
    return


def BuildDirectoryNameTarget(dataTarget):
    directoryName = (dataTarget.getOutDir() + dataTarget.getName() + "/" + dataTarget.getDSDirString() + "/")
    return directoryName


def BuildDirectoryNameData(dsConfiguration, idxD):
    directoryName = ("./gen/img/ds/" + dsConfiguration.getDirString(idxD) + "/")
    return directoryName


def GeneratePlots(dfResults, dsConfiguration, idxD, targetConfiguration, idxT):
    keyTarget = targetConfiguration.getTargetKey(idxT)
    ds_dirstring = dsConfiguration.getDirString(idxD)

    dataTarget = DataTarget(keyTarget, ParseDevice(dfResults, keyTarget), ds_dirstring)
    print("Processing dataset <" + ds_dirstring + "> for target <" + keyTarget + ">")

    yLimRT = dsConfiguration.getYLim(idxD, 0)
    yLimSteps = dsConfiguration.getYLim(idxD, 1)
    yLimRam = dsConfiguration.getYLim(idxD, 2)
    yLimJDiff = dsConfiguration.getYLim(idxD, 3)

    if not dataTarget.getDataFrame().empty:
        path = os.path.join(BuildDirectoryNameTarget(dataTarget))
        try:
            os.makedirs(path)
        except OSError as error:
            pass

        BoxPlot(dataTarget, 'world_size', 'runtime_vi_s', 'schema', yLimRT, True)
        BoxPlot(dataTarget, 'world_size', 'steps_total', 'schema', yLimSteps, True)
        BoxPlot(dataTarget, 'world_size', 'rss_max_rank0_mb', 'schema', yLimRam, True)
        BoxPlot(dataTarget, 'world_size', 'rss_sum_all_mb', 'schema', yLimRam, True)
        BoxPlot(dataTarget, 'world_size', 'jdiff_maxnorm', 'schema', yLimJDiff)
        BoxPlot(dataTarget, 'world_size', 'jdiff_l2norm', 'schema', yLimJDiff)
        BoxPlot(dataTarget, 'world_size', 'jdiff_mse', 'schema', yLimJDiff)

        BoxPlot(dataTarget, 'com_interval', 'runtime_vi_s', 'schema', yLimRT, True)
        BoxPlot(dataTarget, 'com_interval', 'steps_total', 'schema', yLimSteps, True)
        BoxPlot(dataTarget, 'com_interval', 'jdiff_maxnorm', 'schema', yLimJDiff)
        BoxPlot(dataTarget, 'com_interval', 'jdiff_l2norm', 'schema', yLimJDiff)
        BoxPlot(dataTarget, 'com_interval', 'jdiff_mse', 'schema', yLimJDiff)

        # create com_interval sweep plot for target nuc for dataset small
        if keyTarget == 'nuc' and ds_dirstring == 'small':
            ScatterPlot(dataTarget, 'com_interval', 'runtime_vi_s', 'schema')
            ScatterPlot(dataTarget, 'com_interval', 'steps_total', 'schema')

    return


def GenerateDataPlots(dfResults, dsConfiguration, idxD):
    if not dfResults.empty:
        path = os.path.join(BuildDirectoryNameData(dsConfiguration, idxD))
        try:
            os.makedirs(path)
        except OSError as error:
            pass
        DataDistributionPlot(dfResults, dsConfiguration, idxD)

    return

def GenerateMiscPlots(dsConfiguration, targetConfiguration):
    return

if __name__ == "__main__":
    dsConfiguration = DataSetConfiguration()
    targetConfiguration = TargetConfiguration()
    for idxD in range(0, dsConfiguration.getNumberDatasets()):
        dfResults = ReadandMerge(dsConfiguration, idxD)
        GenerateDataPlots(dfResults, dsConfiguration, idxD)
        for idxT in range(0, targetConfiguration.getNumberTargets()):
            GeneratePlots(dfResults, dsConfiguration, idxD, targetConfiguration, idxT)

    GenerateMiscPlots(dsConfiguration, targetConfiguration)
