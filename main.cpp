﻿#include <QApplication>
#include <QDir>
#include <QObject>

#include <QPair>

#include "Mainwindow.h"
#include "StartDialog.h"
#include "TabWidget.h"
#include "Utils/FileOperators/ConfigFileOperator.h"
#include "Utils/FileOperators/ConfigFileOperator.h"
#include "System/Coordinator.h"
#include "System/InformationCenter.h"

#define run 0

#if !run
#include "Utils/FileOperators/CSVReader.h"
#include "Statistics/Expressioncomparator.h"
#include "Utils/Sorter.h"
#include "Statistics/Correlator.h"
#endif
int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

#if !run
//    QString markerFilePath = "/home/numelen/Documents/Bachelorarbeit/Daten/PanglaoDB_markers.tsv";
//    QString datasetFilepath = "/home/numelen/Documents/Bachelorarbeit/Daten/Pbmc_expression.csv";

    // Windows
    QString markerFilePath = "C:\\Users\\Kademuni\\Documents\\Bachelorarbeit\\Daten\\PanglaoDB_markers_07_Feb_2020.tsv";
    QString datasetFilepath  = "C:\\Users\\Kademuni\\Documents\\Bachelorarbeit\\Daten\\Pbmc_expression.csv";

    QVector<FeatureCollection> clustersWithMarkers = CSVReader::read10xGenomicsClustersFromFile(datasetFilepath, 8, 0);

    QVector<FeatureCollection> cellTypesWithMarkers = CSVReader::readCellTypesFromPanglaoDBFile(markerFilePath);

    QVector<QVector<QPair<QString, double>>> cellTypeFoldChangeCorrelationsFor10xClusters = ExpressionComparator::findClusterCellFoldChangeCorrelations(clustersWithMarkers, cellTypesWithMarkers);

    for (int i = 0; i < cellTypeFoldChangeCorrelationsFor10xClusters.length(); i++) {
        double qualityScore = qAbs(cellTypeFoldChangeCorrelationsFor10xClusters.at(i).at(0).second -
                         cellTypeFoldChangeCorrelationsFor10xClusters.at(i).at(1).second);

        qDebug() << "\nCluster:" << i + 1 << "- qs:" << qualityScore;

        for (int j = 0; j < 5; j++) {
            qDebug() << cellTypeFoldChangeCorrelationsFor10xClusters.at(i).at(j).first << "--" <<
                        cellTypeFoldChangeCorrelationsFor10xClusters.at(i).at(j).second;
        }

    }

#endif

#if run
    // Declaration of the used widgets
    MainWindow mainWindow;
    StartDialog startDialog;
    TabWidget tabWidget;

    // Start of the main software - The start dialog is the first that is shown to the user
    startDialog.show();
    // ++++++++++++++++++++++++++++++++++++++++  CHECK FOR CONFIG FILE ++++++++++++++++++++++++++++++++++++++++
    QString configFilePath = QDir::homePath().append("/.badger.conf");
    ConfigFile configFile;

    bool isConfigFileExits = ConfigFileOperator::isConfigFileExists(configFilePath);

    if (isConfigFileExits) {
        configFile = ConfigFileOperator::readConfigFile(configFilePath);
        bool isDefaultMarkerFileExists = ConfigFileOperator::isFileExists(configFile.cellMarkersFilePath);

        if (!isDefaultMarkerFileExists) {
            startDialog.disableUseDefaultButton();
        }

    } else {
        ConfigFileOperator::createConfigFile(configFilePath);
        configFile = ConfigFileOperator::initializeConfigFile();
    }

    // ++++++++++++++++++++++++++++++++++++++++  CREATE PROGRAM BASICS  ++++++++++++++++++++++++++++++++++++++++

    // InformationCenter is used to store all relevant software data
    InformationCenter informationCenter(configFile);
    // Coordinator is used to control the main data flow
    Coordinator coordinator(informationCenter);


    // +++++++++++++++++++++++++++++++++++++  BUILD SIGNAL AND SLOT LOGIC  +++++++++++++++++++++++++++++++++++++
    // StartDialog -> Coordinator
    QObject::connect(&startDialog, &StartDialog::runNewProject, &coordinator, &Coordinator::on_newProjectStarted);

    // Coordinator -> Main Window
    QObject::connect(&coordinator, &Coordinator::finishedFileParsing, &mainWindow, &MainWindow::on_clusterFileParsed);

    // Coordinator -> Main Window
    QObject::connect(&coordinator, &Coordinator::finishedCorrelating, &mainWindow, &MainWindow::on_correlatingFinished);

    // At this point, the complete control over the system workflow is handed over to the Coordinator

#endif
    return application.exec();
}
