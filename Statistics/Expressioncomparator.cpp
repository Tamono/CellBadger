#include "Expressioncomparator.h"

#include <QDebug>
#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>

ExpressionComparator::ExpressionComparator()
{

}

QVector<QVector<QPair<QPair<QString, QString>, double>>> ExpressionComparator::findCellTypeCorrelations(QVector<QPair<QPair<QString, QString>, QStringList>> cellTypesWithAssociatedMarkers, QVector<QStringList> clusterFeatureExpressions) {
    QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellMappingLikelihoods;

    qDebug() << "Go: Find cell type correlations";
    for (QStringList cluster : clusterFeatureExpressions) {
        QVector<QPair<QPair<QString, QString>, double>> cellMappingLikelihoods;

        for (QPair<QPair<QString, QString>, QStringList> cellType : cellTypesWithAssociatedMarkers) {
            int numberOfFeatures = cellType.second.length();
            int numberOfExpressedFeatures = 0;

            for (QString marker : cellType.second) {
                bool isMarkerExpressed = cluster.contains(marker);

                if (isMarkerExpressed) {
                    numberOfExpressedFeatures++;
                }
            }

            double mappingLikelihood = double(numberOfExpressedFeatures) / double(numberOfFeatures);
            cellMappingLikelihoods.append(qMakePair(cellType.first, mappingLikelihood));
        }
        clustersWithCellMappingLikelihoods.append(cellMappingLikelihoods);
    }

    qDebug() << "Finished";
    return clustersWithCellMappingLikelihoods;

//    for (QPair<QString, QStringList> cellTypeWithMarkers : cellTypesWithAssociatedMarkers) {

//        QVector<QPair<QString, double>> mappingLikelihoods;
//        int numberOfFeatures = cellTypeWithMarkers.second.length();
//        int numberOfExpressedFeaturesInCluster = 0;

//        qDebug() << "Checking correlation with:" << cellTypeWithMarkers.first;
//        for (int i = 0; i < cellTypeWithMarkers.second.length(); i++) {
//            QString currentMarker = cellTypeWithMarkers.second[i];
//            bool isMarkerExpressed = clusterFeatureExpressions[i].contains(currentMarker);

//            if (isMarkerExpressed) {
//                numberOfExpressedFeaturesInCluster++;
//            }
//        }

//        double mappingLikelihood = double(numberOfExpressedFeaturesInCluster) / double(numberOfFeatures);

//        QString currentCellType = cellTypeWithMarkers.first;
////        qDebug() << "Likelihood for" << currentCellType << ":" << mappingLikelihood;
//        mappingLikelihoods.append(qMakePair(currentCellType, mappingLikelihood));
//    }
}

//QVector<QVector<QPair<QPair<QString, QString>, double>>> ExpressionComparator::findCellTypeCorrelationsCellWise(QHash <QString, QVector<QPair<QString, QString>>> cellMarkersWithAssociatedTissues, QVector<QStringList> clusterFeatureExpressions) {

//    qDebug() << "Go: Find cell type correlations cellwise";
//    for (QStringList cluster : clusterFeatureExpressions) {
//        QVector<QVector<QPair<QString, QString>>> mappedTissuesPerCell;

//        for (QString expressedCellFeature : cluster) {
//            QVector<QPair<QString, QString>> associatedTissueTypes = cellMarkersWithAssociatedTissues.value(expressedCellFeature);

//        }
//    }

////    QVector<QVector<QPair<QPair<QString, QString>, double>>> clustersWithCellMappingLikelihoods;

////    qDebug() << "Go: Find cell type correlations cellwise";
////    for (QStringList cluster : clusterFeatureExpressions) {
////        QVector<QPair<QString, QString>> cellTypeMappingsPerCell;

////        for (QString cell : cluster) {
////            QPair<QString, QString> cellTypeMappingForCell;

////            for (QPair<QPair<QString, QString>, QStringList> cellType : cellTypesWithAssociatedMarkers) {

////                for (QString marker : cellType.second) {
////                    bool isMarkerExpressed = cell
////                }
////            }
////        }
////    }


////    for (QStringList cluster : clusterFeatureExpressions) {
////        QVector<QPair<QPair<QString, QString>, double>> cellMappingLikelihoods;

////        for (QPair<QPair<QString, QString>, QStringList> cellType : cellTypesWithAssociatedMarkers) {
////            int numberOfFeatures = cellType.second.length();
////            int numberOfExpressedFeatures = 0;

////            for (QString marker : cellType.second) {
////                bool isMarkerExpressed = cluster.contains(marker);

////                if (isMarkerExpressed) {
////                    numberOfExpressedFeatures++;
////                }
////            }

////            double mappingLikelihood = double(numberOfExpressedFeatures) / double(numberOfFeatures);
////            cellMappingLikelihoods.append(qMakePair(cellType.first, mappingLikelihood));
////        }
////        clustersWithCellMappingLikelihoods.append(cellMappingLikelihoods);
////    }

//    qDebug() << "Finished";
//    return clustersWithCellMappingLikelihoods;

//}
