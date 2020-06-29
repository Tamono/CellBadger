#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QString>
#include <QTableWidgetItem>
#include <QStringList>
#include <QObject>
#include <QTableView>

#include "BioModels/FeatureCollection.h"
#include "Utils/Models/GeneTableModel.h"
#include "Utils/Models/ProxyModel.h"

namespace Ui {
class TabWidget;
}

class TabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TabWidget(QWidget *parent = nullptr, QString title = "");
    ~TabWidget();

    void populateTableTypeCorrelations(QVector<QVector<QPair<QString, double>>> correlations, int numberOfItems);

    void populateTableGeneExpressions(QVector<FeatureCollection> geneExpressions, QStringList completeGeneIDs);

signals:
    void minRawCountSet(double minRawCount);
    void maxRawCountSet(double maxRawCount);
    void minFoldChangeSet(double minFoldChange);
    void maxFoldChangeSet(double maxFoldChange);
    void rawCountInAtLeastSet(int numberOfClusters);
    void rawCountInAtLeastToggled(bool state);
    void foldChangeInAtLeastSet(int numberOfClusters);
    void foldChangeInAtLeastToggled(bool state);
    void highestMetRawCountAndFoldChangeValuesChanged(double highestMetRawCount, double highestMetFoldChange);

public slots:

    void on_minRawCountSet(double minRawCount);
    void on_maxRawCountSet(double maxRawCount);
    void on_minFoldChangeSet(double minFoldChange);
    void on_maxFoldChangeSet(double maxFoldChange);
    void on_rawCountInAtLeastSet(int numberOfClusters);
    void on_rawCountInAtLeastToggled(bool state);
    void on_foldChangeinAtLeastSet(int numberOfClusters);
    void on_foldChangeInAtLeastToggled(bool state);

private slots:
//    void on_lineEditGeneID_textChanged(const QString &arg1);

//    void on_tableWidgetGeneExpressions_cellDoubleClicked(int row, int column);

//    void on_pushButtonScatterPlot_clicked();

//    void on_pushButtonBarChart_clicked();

//    void on_pushButtonCorrelationOptionsRun_clicked();

private:
    Ui::TabWidget *ui;
    QString title;

    QTableView * tableView;
    GeneTableModel * geneTableModel;
    ProxyModel * proxyModel;

//    QVector<QPair<QString, QPair<QVector<double>, double>>> retrieveExpressionDataForSelectedGenes();

    void showAlertForInvalidGeneID(QString geneID);

    template<typename F>
    void openExportWidgetWithPlot(F plottingFunction);

    const char lineEditDelimiter = ',';

    double minRawCount;
    double maxRawCount;
    double minFoldChange;
    double maxFoldChange;
    int rawCountInAtLeast;
    bool includeRawCountInAtLeast;
    int foldChangeInAtLeast;
    bool includeFoldChangeInAtLeast;
};

#endif // TABWIDGET_H
