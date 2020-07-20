#include <QTableWidgetItem>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QCategoryAxis>
#include <QLineSeries>
#include <set>

#include "TabWidget.h"
#include "ui_TabWidget.h"
#include "BioModels/FeatureCollection.h"
#include "ExportDialog.h"
#include "Utils/Plots.h"
#include "Utils/Models/GeneTableModel.h"
#include "Utils/Helper.h"
#include "Utils/Math.h"


TabWidget::TabWidget(QWidget *parent, QString title, QStringList clusterNames) :
    QWidget(parent), ui(new Ui::TabWidget), title(title), clusterNames(clusterNames)
{
    ui->setupUi(this);

    // Disable the "in at least n clusters" spinboxes -> Changed by the radio button in front
    this->ui->spinBoxFilterOptionsRawCountCutOffInAtLeast->setDisabled(true);
    this->ui->spinBoxFilterOptionsFoldChangeCutOffInAtLeast->setDisabled(true);
}

TabWidget::~TabWidget()
{
    delete ui;
}

/**
 * @brief TabWidget::populateTableTypeCorrelations - Populates the table showing the top n correlations for each cluster.
 * @param correlations - List of clusters with corresponding type corrlations - sorted.
 * @param numberOfItems - Number of items that should be shown in the table.
 */
void TabWidget::populateTableTypeCorrelations(QVector<QVector<QPair<QString, double>>> correlations, QVector<double> qualityScores, int numberOfItems) {

    if(this->ui->tableWidgetTypeCorrelations->columnCount() > 0) {
        this->cleanCorrelationTable();
    }

    int numberOfClusters = correlations.length();

    this->ui->tableWidgetTypeCorrelations->setColumnCount(numberOfClusters);
    this->ui->tableWidgetTypeCorrelations->setRowCount(numberOfItems);

    // Create header with cluster numbers
    QStringList clusterNameHeaderItems;
    for (int i = 0; i < numberOfClusters; i++)
        clusterNameHeaderItems.append(this->clusterNames.at(i) + " - qs: " + QString::number(qualityScores.at(i), 'g', 3));

    // Add it to the table
    this->ui->tableWidgetTypeCorrelations->setHorizontalHeaderLabels(clusterNameHeaderItems);

    // Go through the top n of every cluster and populate the table with it
    for (int i = 0; i < correlations.length(); i++) {
        for (int j = 0; j < numberOfClusters; j++) {
            QPair<QString, double> type = correlations[i][j];
            QString cell = QString::number(type.second) + ": " + type.first;

            // A TableWidgetItem is needed for every cell
            QTableWidgetItem * tableWidgetItem = new QTableWidgetItem(0);
            tableWidgetItem->setData(Qt::DisplayRole, cell);

            this->ui->tableWidgetTypeCorrelations->setItem(j, i, tableWidgetItem);
        }
    }

    this->ui->tableWidgetTypeCorrelations->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


/**
 * @brief TabWidget::populateTableGeneExpressions - Populates the gene expression table with the gene expression counts
 * * @param geneExpressions - list of clusters with corresponding gene expression counts - unsorted.
 */
void TabWidget::populateTableGeneExpressions(QVector<FeatureCollection> geneExpressions, QStringList completeGeneIDs) {

    // ############################################ GENE TABLE MODEL ############################################
    this->geneTableModel = new GeneTableModel(geneExpressions, completeGeneIDs, this->clusterNames);

    // Save the highest met values for the raw count and the fold change from the expression values
    // This is neccessary to control the max set cut-off values
    double highestMetRawCount, highestMetFoldChange;

    for (FeatureCollection cluster : geneExpressions) {
        // Check for the highest raw count
        if (cluster.getHighestRawCount() > highestMetRawCount)
            highestMetRawCount = cluster.getHighestRawCount();

        // Check for the highest fold change
        if (cluster.getHighestFoldChange() > highestMetFoldChange)
            highestMetFoldChange = cluster.getHighestFoldChange();
    }

    // Set the maximum that stands for the number of clusters
    this->ui->spinBoxFilterOptionsRawCountCutOffInAtLeast->setMaximum(geneExpressions.length());
    this->ui->spinBoxFilterOptionsFoldChangeCutOffInAtLeast->setMaximum(geneExpressions.length());

    // ############################################ PROXY MODEL ############################################
    this->proxyModel = new ProxyModel(completeGeneIDs.length(), (geneExpressions.length() * 2) + 1, highestMetRawCount, highestMetFoldChange);
    this->proxyModel->setSourceModel(geneTableModel);

    // Use the highest met values for raw count and fold change to change tweek the gui values
    // Needs to be done after the proxymodel has been initialized
    this->setMaxValuesForGUIElements(highestMetRawCount, highestMetFoldChange);

    // ############################################ TABLE VIEW ############################################
    this->tableView = new QTableView;
    this->tableView->setModel(this->proxyModel);

    this->tableView->verticalHeader()->setMinimumWidth(25);
    this->tableView->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    this->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->tableView->setSortingEnabled(true);
    this->tableView->sortByColumn(0, Qt::AscendingOrder);
    // The Contigous selection mode prevents the selection of single cells out of line
    this->tableView->setSelectionMode(QAbstractItemView::ContiguousSelection);

    this->ui->horizontalLayoutGeneExpressionTable->insertWidget(0, this->tableView);
}


/**
 * @brief TabWidget::on_lineEditGeneID_textEdited - When the line edit text has been edited the corresponding table is filtered according to the line edit content
 * @param lineEditContent - The string that is currently written in the line edit - Used to filter the table
 */
void TabWidget::on_lineEditGeneID_textChanged(const QString & lineEditContent) {
    // Read search string from line edit
    QString searchString = lineEditContent.toLower();

    // If empty report an empty list to the TableView
    if (searchString == "")
        this->proxyModel->setSearchedGeneIDs(QStringList());
    // Else report the separated gene IDs
    else
        this->proxyModel->setSearchedGeneIDs(searchString.split(','));
}

/**
 * @brief TabWidget::on_tableWidgetGeneExpressions_cellDoubleClicked - Adds the gene ID (header item) for the clicked item to the list of selected IDs - handles duplicates and autocomplete
 * @param row - Index of row that was clicked - used to get the corresponding header item (gene ID)
 * @param column - Unused
 */
//void TabWidget::on_tableWidgetGeneExpressions_cellDoubleClicked(int row, int column) {

//    Q_UNUSED(column);

//    // If the first gene has been added, enable the plot functionality
//    if (this->ui->lineEditGeneID->text() == "") {
//        this->ui->pushButtonScatterPlot->setEnabled(true);
//        this->ui->pushButtonBarChart->setEnabled(true);
//    }

//    QString currentLineEditText = this->ui->lineEditGeneID->text(),
//            headerItemForSelectedRow = this->ui->tableWidgetGeneExpressions->verticalHeaderItem(row)->text().toLower(),
//            newLineEditText;

//    QStringList currentGeneIDs = currentLineEditText.split(this->lineEditDelimiter);

//    for (int i = 0; i < currentGeneIDs.length(); i++) {
//        QString geneID = currentGeneIDs[i].toLower();

//        // If the user clicks on an item that is already selected, return to prevent doubled items
//        if (geneID == headerItemForSelectedRow) {
//            qDebug() << "Duplicate item";
//            return;
//        }

//        // If the user clicks on an item that he / she was beginning to type beforehand,
//        // exchange the typed ID with the clicked ID to prevent doubled entries -> Autocomplete
//        if (headerItemForSelectedRow.contains(geneID)) {
//            qDebug() << "Found started item:" << geneID;
//            currentGeneIDs.removeAt(i);
//            currentGeneIDs.append(headerItemForSelectedRow);
//            newLineEditText = currentGeneIDs.join(",").append(",");
//            this->ui->lineEditGeneID->setText(newLineEditText);
//            return;
//        }
//    }

//    // If neither duplicates were found nor autocomplete could be done just add the item
//    newLineEditText = currentLineEditText;

//    // Remove last space to avoid confusion with table - line edit comparisons
//    if (currentLineEditText.endsWith(" ")) {
//        newLineEditText.chop(1);
//    }

//    newLineEditText += headerItemForSelectedRow + this->lineEditDelimiter;

//    this->ui->lineEditGeneID->setText(newLineEditText);
//}


/**
 * @brief TabWidget::showAlertForInvalidGeneID - Show an alert with the given gene ID
 * @param geneID - ID that is shown as invalid in the alert
 */
//void TabWidget::showAlertForInvalidGeneID(QString geneID) {
//    QMessageBox invalidGeneIDAlert;
//    invalidGeneIDAlert.setText("Invalid gene ID: " + geneID);
//    invalidGeneIDAlert.setWindowFlags(Qt::FramelessWindowHint);
//    invalidGeneIDAlert.exec();
//    return;
//}


/**
 * @brief TabWidget::retrieveExpressionDataForSelectedGenes - Go through the TableView and gather all data that has been selected
 * @return - IDs and gene expression data for selected genes
 */
std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> TabWidget::retrieveExpressionDataForSelectedGenes() {

    QVector<std::tuple<QString, QVector<double>, double>> dataForSelectedGenes;

    QModelIndexList selectedIndices = this->tableView->selectionModel()->selectedIndexes();
    QStringList clusterNames;

    QMap<QString, QVector<double>> expressionDataForSelectedGenes;
    for (QModelIndex modelIndex : selectedIndices) {

        // Skip the first column with gene IDs and the last column with the mean counts
        if (modelIndex.column() == 0 || modelIndex.column() == this->tableView->model()->columnCount() - 1)
            continue;

        // Grab the cluster name from the horizontal header items and append it to the list of cluster names
        QString clusterName = this->tableView->model()->headerData(modelIndex.column(), Qt::Horizontal).toString();
        clusterNames.append(clusterName);

        // Grab the gene ID corresponding to the currently selected cell's row
        QModelIndex geneIDCellIndex = this->tableView->model()->index(modelIndex.row(), 0);
        QString geneID = this->tableView->model()->data(geneIDCellIndex).toString();

        // Grab the underlying value of the current selected cell
        double currentCellData = this->tableView->model()->data(modelIndex).toDouble();

        // And add it to a map to prevent multiple mentions of the same gene
        expressionDataForSelectedGenes[geneID].append(currentCellData);
    }

    for (QString geneID : expressionDataForSelectedGenes.keys()) {
        std::tuple<QString, QVector<double>, double> expressionDataForSelectedGene(geneID, {}, 0);

        QVector<double> * geneExpressiondata = & expressionDataForSelectedGenes[geneID];

        // Add the expression values to the list
        std::get<1>(expressionDataForSelectedGene) = expressionDataForSelectedGenes[geneID];
        std::get<2>(expressionDataForSelectedGene) = Math::mean(* geneExpressiondata);

        dataForSelectedGenes.append(expressionDataForSelectedGene);
    }

    return std::make_tuple(dataForSelectedGenes, clusterNames);
}


/**
 * @brief TabWidget::retrieveAllSeenData - Gathers all data that is currently seen in the table view
 * @return - All data currently seen in the table view
 */
QVector<FeatureCollection> TabWidget::retrieveAllSeenData() {
    QVector<FeatureCollection> allSeenClusters;

    QAbstractItemModel * itemModel = this->tableView->model();

    // Grab all cluster names from the horizontal table header and create a FeatureCollection for each
    for (int i = 1; i < itemModel->columnCount() - 1; i++) {
        QString clusterName = itemModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        allSeenClusters.append(FeatureCollection(clusterName));
    }

    // Go through every row and every column and grab the expression values for the genes and sort them to the correct cluster
    for (int row = 0; row < itemModel->rowCount(); row++) {
        QModelIndex geneNameCellIndex = itemModel->index(row, 0);
        QString geneName = itemModel->data(geneNameCellIndex).toString();

        for (int column = 1; column < itemModel->columnCount() - 1; column++) {
            QModelIndex cellIndex = itemModel->index(row, column);
            double geneRawCount = itemModel->data(cellIndex).toDouble();

            allSeenClusters[column - 1].addFeature(geneName, "nAn", geneRawCount, 0, 0);
        }
    }

    return allSeenClusters;
}


template<typename F>
/**
 * @brief TabWidget::openExportWidgetWithPlot - Ceates a plot with the given plotting function and opens it in an ExportDialog
 * @param plottingFunction - Function that creates a QChartView * that is used to create a plot which is then transfered onto an ExportDialog
 */
void TabWidget::openExportWidgetWithPlot(F plottingFunction) {

    std::tuple<QVector<std::tuple<QString, QVector<double>, double>>, QStringList> expressionDataForSelectedGenes = this->retrieveExpressionDataForSelectedGenes();

    // This case appears if at least one of the gene IDs is not found in the table and therefore is invalid
    if (std::get<0>(expressionDataForSelectedGenes).isEmpty())
        return;

    QChartView * chartView = plottingFunction(expressionDataForSelectedGenes, this->title);

    ExportDialog * exportDialog = new ExportDialog(this);
    exportDialog->addPlot(chartView);
    exportDialog->show();
}


/**
 * @brief TabWidget::cleanCorrelationTable - Deletes all instances of QTableWidget items from the correlation TabWidget
 */
void TabWidget::cleanCorrelationTable() {
    for (int i = 0; i < this->ui->tableWidgetTypeCorrelations->rowCount(); i++) {
        for (int j = 0; j < this->ui->tableWidgetTypeCorrelations->columnCount(); j++) {
            delete this->ui->tableWidgetTypeCorrelations->item(i, j);
        }
    }
}


/**
 * @brief TabWidget::setHighestRawCountAndFoldChangeValuesInGUI - Set the given max values for all gui elements
 * @param highestMetRawCount - Max possible raw count cut-off
 * @param highestMetFoldChange - Max possible fold change cut-off
 */
void TabWidget::setMaxValuesForGUIElements(const double highestMetRawCount, const double highestMetFoldChange) {
    this->ui->spinBoxFilterOptionsRawCountCutOffMin->setMaximum(highestMetRawCount);
    this->ui->spinBoxFilterOptionsRawCountCutOffMax->setMaximum(highestMetRawCount);
    this->ui->horizontalSliderFilterOptionsRawCountCutOffMin->setMaximum(highestMetRawCount);
    this->ui->horizontalSliderFilterOptionsRawCountCutOffMax->setMaximum(highestMetRawCount);
    this->ui->spinBoxFilterOptionsFoldChangeCutOffMin->setMaximum(highestMetFoldChange);
    this->ui->spinBoxFilterOptionsFoldChangeCutOffMax->setMaximum(highestMetFoldChange);
    this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMin->setMaximum(highestMetFoldChange);
    this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMax->setMaximum(highestMetFoldChange);
    this->ui->horizontalSliderFilterOptionsRawCountCutOffMax->setValue(highestMetRawCount);
    this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMax->setValue(highestMetFoldChange);
    emit this->ui->horizontalSliderFilterOptionsRawCountCutOffMax->sliderMoved(highestMetRawCount);
    emit this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMax->sliderMoved(highestMetFoldChange);
}


// ############################################### SLOTS ###############################################


/**
 * @brief TabWidget::on_pushButtonPlot_clicked
 */
void TabWidget::on_pushButtonScatterPlot_clicked() {
    this->openExportWidgetWithPlot(Plots::createScatterPlot);
}


/**
 * @brief TabWidget::on_pushButtonBarChart_clicked
 */
void TabWidget::on_pushButtonBarChart_clicked() {
    this->openExportWidgetWithPlot(Plots::createBarChart);
}


// MIN RAW COUNT
void TabWidget::on_spinBoxFilterOptionsRawCountCutOffMin_valueChanged(int value) {
    this->minRawCount = value;
    this->ui->horizontalSliderFilterOptionsRawCountCutOffMin->setValue(value);
    this->proxyModel->setMinRawCount(value);
}

void TabWidget::on_horizontalSliderFilterOptionsRawCountCutOffMin_sliderMoved(int position) {
    this->minRawCount = position;
    this->ui->spinBoxFilterOptionsRawCountCutOffMin->setValue(position);
}


// MAX RAW COUNT
void TabWidget::on_spinBoxFilterOptionsRawCountCutOffMax_valueChanged(int value) {
    this->maxRawCount = value;
    this->ui->horizontalSliderFilterOptionsRawCountCutOffMax->setValue(value);
    this->proxyModel->setMaxRawCount(value);
}

void TabWidget::on_horizontalSliderFilterOptionsRawCountCutOffMax_sliderMoved(int position) {
    this->maxRawCount = position;
    this->ui->spinBoxFilterOptionsRawCountCutOffMax->setValue(position);
}


// MIN FOLD CHANGE
void TabWidget::on_spinBoxFilterOptionsFoldChangeCutOffMin_valueChanged(int value) {
    this->minFoldChange = value;
    this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMin->setValue(value);
    this->proxyModel->setMinFoldChange(value);
}

void TabWidget::on_horizontalSliderFilterOptionsFoldChangeCutOffMin_sliderMoved(int position) {
    this->minFoldChange = position;
    this->ui->spinBoxFilterOptionsFoldChangeCutOffMin->setValue(position);
}


// MAX FOLD CHANGE
void TabWidget::on_spinBoxFilterOptionsFoldChangeCutOffMax_valueChanged(int value) {
    this->maxFoldChange = value;
    this->ui->horizontalSliderFilterOptionsFoldChangeCutOffMax->setValue(value);
    this->proxyModel->setMaxFoldChange(value);
}

void TabWidget::on_horizontalSliderFilterOptionsFoldChangeCutOffMax_sliderMoved(int position) {
    this->maxFoldChange = position;
    this->ui->spinBoxFilterOptionsFoldChangeCutOffMax->setValue(position);
}


// RAW COUNT IN AT LEAST
void TabWidget::on_checkBoxFilterOptionsRawCountCutOffInAtLeast_toggled(bool checked) {
    this->ui->spinBoxFilterOptionsRawCountCutOffInAtLeast->setEnabled(checked);
    this->proxyModel->setIncludeRawCountInAtLeast(checked);
}

void TabWidget::on_spinBoxFilterOptionsRawCountCutOffInAtLeast_valueChanged(int number) {
    this->proxyModel->setRawCountInAtLeast(number);
}


// FOLD CHANGE IN AT LEAST
void TabWidget::on_checkBoxFilterOptionsFoldChangeCutOfftInAtLeast_toggled(bool checked) {
    this->ui->spinBoxFilterOptionsFoldChangeCutOffInAtLeast->setEnabled(checked);
    this->proxyModel->setIncludeFoldChangeInAtLeast(checked);
}

void TabWidget::on_spinBoxFilterOptionsFoldChangeCutOffInAtLeast_valueChanged(int number) {
    this->proxyModel->setFoldChangeInAtLeast(number);
}
