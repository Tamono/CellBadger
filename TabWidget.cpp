#include <QTableWidgetItem>
#include <QDebug>
#include <QString>

#include "TabWidget.h"
#include "ui_TabWidget.h"
#include "BioModels/FeatureCollection.h"

TabWidget::TabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabWidget)
{
    ui->setupUi(this);
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
void TabWidget::populateTableTypeCorrelations(QVector<QVector<QPair<QString, double>>> correlations, int numberOfItems) {
    int numberOfClusters = correlations.length();

    this->ui->tableWidgetTypeCorrelations->setColumnCount(numberOfClusters);
    this->ui->tableWidgetTypeCorrelations->setRowCount(numberOfItems);

    // Create header with cluster numbers
    QStringList clusterNameHeaderItems;
    for (int i = 1; i < numberOfClusters + 1; i++) {
        clusterNameHeaderItems.append("Cluster " + QString::number(i));
    }

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
void TabWidget::populateTableGeneExpressions(QVector<FeatureCollection> geneExpressions) {
    int numberOfClusters = geneExpressions.length();
    int numberOfGeneIDs = geneExpressions[0].getNumberOfFeatures();

    this->ui->tableWidgetGeneExpressions->setColumnCount(numberOfClusters);
    this->ui->tableWidgetGeneExpressions->setRowCount(numberOfGeneIDs);

    // Create header with cluster numbers
    QStringList clusterNameHeaderItems;
    for (int i = 1; i < numberOfClusters + 1; i++) {
        clusterNameHeaderItems.append("Cluster " + QString::number(i));
    }

    // Create header with cluster numbers
    QStringList geneIDHeaderItems;
    for (int i = 0; i < numberOfGeneIDs; i++) {
        geneIDHeaderItems.append(geneExpressions[0].getFeatureID(i));
    }

    // Add it to the table
    this->ui->tableWidgetGeneExpressions->setHorizontalHeaderLabels(clusterNameHeaderItems);
    this->ui->tableWidgetGeneExpressions->setVerticalHeaderLabels(geneIDHeaderItems);

    // Go through  every cluster and populate the table with the gene expression counts
    for (int i = 0; i < geneExpressions.length(); i++) {
        for (int j = 0; j < geneExpressions[i].getNumberOfFeatures(); j++) {
            double geneExpressionCount = geneExpressions[i].getFeatureExpressionCount(j);

            QTableWidgetItem * tableWidgetItem = new QTableWidgetItem(0);
            tableWidgetItem->setData(Qt::DisplayRole, geneExpressionCount);

            this->ui->tableWidgetGeneExpressions->setItem(j, i, tableWidgetItem);
        }
    }

    this->ui->tableWidgetGeneExpressions->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


#include <iostream>

/**
 * @brief TabWidget::on_lineEditGeneID_textEdited - When the line edit text has been edited the corresponding table is filtered according to the line edit content
 * @param lineEditContent - The string that is currently written in the line edit - Used to filter the table
 */
void TabWidget::on_lineEditGeneID_textChanged(const QString & lineEditContent) {
    // Reset the previously hidden rows
    for (int i = 0; i < this->ui->tableWidgetGeneExpressions->rowCount(); i++) {
        this->ui->tableWidgetGeneExpressions->setRowHidden(i, false);
    }

    // Read search string from line edit
    QString searchString = lineEditContent.toLower();

    // In case the user deleted the search string, just unhide the rows and return
    if (searchString == " ") {
        return;
    }

    QStringList searchStrings = searchString.split(", ");

    for (QString string : searchStrings) {
        std::cout << string.toStdString() << std::endl;
    }

    // Filter list of gene IDs for search string and hide rows that don't contain it
    for (int i = 0; i < this->ui->tableWidgetGeneExpressions->rowCount(); i++) {
        bool isContainsAtLeastOneSearchString = false;
        for (QString string : searchStrings) {
            if (this->ui->tableWidgetGeneExpressions->verticalHeaderItem(i)->text().toLower().contains(string)) {
                isContainsAtLeastOneSearchString = true;
            }
        }
        if (!isContainsAtLeastOneSearchString) {
            this->ui->tableWidgetGeneExpressions->setRowHidden(i, true);
        }
    }

}

/**
 * @brief TabWidget::on_tableWidgetGeneExpressions_itemDoubleClicked
 * @param item
 */
void TabWidget::on_tableWidgetGeneExpressions_itemDoubleClicked(QTableWidgetItem * item) {
    QString currentLineEditText = this->ui->lineEditGeneID->text();
    int rowNumberForSelectedItem = this->ui->tableWidgetGeneExpressions->row(item);
    QString headerItemForSelectedRow = this->ui->tableWidgetGeneExpressions->verticalHeaderItem(rowNumberForSelectedItem)->text();

    QString newItem = currentLineEditText;

    if (currentLineEditText.endsWith(" ")) {
        newItem.chop(1);
    }

    newItem = newItem + headerItemForSelectedRow + ", ";

    this->ui->lineEditGeneID->setText(newItem);
}
