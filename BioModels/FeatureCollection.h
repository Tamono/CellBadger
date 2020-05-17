#ifndef CLUSTER_H
#define CLUSTER_H

#include <QStringList>
#include <QVector>

#include "BioModels/Feature.h"

/**
 * @brief The FeatureCollection class serves as a container class for features. It is one of 10x-cluster or tissue-type
 */
class FeatureCollection
{
private:
    QVector<Feature> features;

public:
    QString ID;

    FeatureCollection();
    FeatureCollection(const QString collectionID);
//    FeatureCollection(FeatureCollection & featureCollection);

    void addFeature(const Feature feature);
    void addFeature(const QString featureID);
    void addFeature(const QString featureID, const QString featureEnsemblID, const double featureSensitivity, const double featureSpecifity);
    void addFeature(const QString featureID, const QString featureEnsemblID, const double featureMeanCount, const double featureLog2FoldChange, const double featureFoldChange);

    bool isFeatureExpressed(QString markerID);
    bool isFeatureExpressed(Feature feature);

    Feature getFeature(int index);
    QString getFeatureID(int index);
    double getFeatureExpressionCount(int index);
    int getNumberOfFeatures();
    QVector<Feature> getFeatures();
    QVector<double> getMostExpressedFeaturesCounts(int number);
    double getFeatureFoldChange(int index);
    double getFoldChangeSum();

};

#endif // CLUSTER_H
