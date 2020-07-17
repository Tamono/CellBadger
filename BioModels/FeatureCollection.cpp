#include "FeatureCollection.h"

#include <QStringList>
#include <QVector>
#include <math.h>
#include <QDebug>

#include "BioModels/Feature.h"

FeatureCollection::FeatureCollection(){}

/**
 * @brief Cluster::Cluster - Only container class - only default constructor
 */
FeatureCollection::FeatureCollection(QString collectionID)
    : ID (collectionID)
{}


/**
 * @brief FeatureCollection::addFeature
 * @param feature
 */
void FeatureCollection::addFeature(const Feature feature) {
    this->expressionCountSum += feature.count;
    this->features.append(feature);
}


// REMEMBER: Do I need this one?
/**
 * @brief FeatureCollection::addFeature - The FeatureCollection that is appended to here represents a 10x Cluster and the new Feature represents an expressed gene in that cluster
 * @param featureID - Gene ID
 * @param featureMeanCount - Mean count of the expression of the selected gene / UMI count for the selected gene
 */
void FeatureCollection::addFeature(const QString featureID) {
    Feature feature(featureID);
    this->features.append(feature);
}


/**
 * @brief FeatureCollection::addFeature - The FeatureCollection that is appended to here represents a cell type and the new Feature represents a gene / cell marker
 * @param ID - Gene ID
 * @param sensitivity - Gene expression sensitivity for the cell type it is appended to
 * @param specifity - Gene expression specifity for the cell type it is appended to
 */
void FeatureCollection::addFeature(const QString featureID, const QString featureEnsemblID, const double featureSensitivity, const double featureSpecifity) {
    double foldChange = featureSensitivity / featureSpecifity;
    double log2FoldChange = log2(foldChange);

    Feature feature(featureID, featureEnsemblID, log2FoldChange, foldChange);
    this->features.append(feature);
}


/**
 * @brief FeatureCollection::addFeature - The FeatureCollection that is appended to here represents a 10x Cluster and the new Feature represents an expressed gene in that cluster
 * @param featureID - Gene ID
 * @param featureMeanCount - Mean count of the expression of the selected gene / UMI count for the selected gene
 * @param featureLog2FoldChange - Log 2 of ratio of expression of the selected gene in comparison to the expression of this gene in every other cluster
 * @param featureFoldChange - Actual ratio of expression of the selected gene in comparison to the expression of this gene in every other cluster
 */
void FeatureCollection::addFeature(const QString featureID, const QString featureEnsemblID, const double featureMeanCount, const double featureLog2FoldChange, const double featureFoldChange) {
    Feature feature(featureID, featureEnsemblID, featureMeanCount, featureLog2FoldChange, featureFoldChange);
    this->features.append(feature);
}


/**
 * @brief FeatureCollection::filterFeatures - Keep only $number most expressed features
 * @param number - Number of features to keep
 */
QVector<double> FeatureCollection::getMostExpressedFeaturesCounts(int number) const {
    QVector<double> sortedFeaturesExpressions;
    sortedFeaturesExpressions.reserve(this->getNumberOfFeatures());
    for (int i = 0; i < this->getNumberOfFeatures(); i++) {
        sortedFeaturesExpressions.append(this->getFeatureExpressionCount(i));
    }
    std::sort(sortedFeaturesExpressions.begin(), sortedFeaturesExpressions.end());
    sortedFeaturesExpressions.resize(number);
    sortedFeaturesExpressions.squeeze();
    return sortedFeaturesExpressions;
}


bool FeatureCollection::isFeatureExpressed(QString markerID) {
    //REMEMBER: MAYBE -> WOULDTHAT WORK?
//    Feature feature(markerID, COUNT);
//    return features.contains(feature);
    for (Feature feature : this->features) {
        bool isSameFeature = feature.ID == markerID;
        if (isSameFeature)
            return true;
    }
    return false;
}


bool FeatureCollection::isFeatureExpressed(Feature feature) {
    return this->isFeatureExpressed(feature.ID);
}


/**
 * @brief Cluster::getFeature
 * @param index
 * @return
 */
Feature FeatureCollection::getFeature(int index) const {
    return features[index];
}


/**
* @brief FeatureCollection::getFeature
 * @param index
 * @return
 */
Feature FeatureCollection::getFeature(QString featureName) const {
    for (Feature feature : this->features) {
        if (feature.ID.compare(featureName) == 0)
            return feature;
    }

    return Feature();
}


/**
 * @brief Cluster::getFeatureID
 * @param index
 * @return
 */
QString FeatureCollection::getFeatureID(int index) const {
    return features[index].ID;
}


/**
 * @brief Cluster::getFeatureExpressionCount
 * @param index
 * @return
 */
double FeatureCollection::getFeatureExpressionCount(int index) const {
    return features[index].count;
}


/**
 * @brief FeatureCollection::getNumberOfFeatures
 * @return Number of expressed features
 */
int FeatureCollection::getNumberOfFeatures() const {
    return features.length();
}


/**
 * @brief FeatureCollection::getFeatures
 * @return List of features associated with FeatureCollection
 */
QVector<Feature> FeatureCollection::getFeatures() const {
    QVector<Feature> copyCollection = this->features;
    return copyCollection;
}


/**
 * @brief FeatureCollection::getFeatureFoldChange
 * @param index
 * @return
 */
double FeatureCollection::getFeatureFoldChange(int index) const {
   return this->features.at(index).foldChange;
}


/**
 * @brief FeatureCollection::getFeatureRawCount - Fetch the feature i's raw count
 * @param index - Index of the gene inside of the feature collection
 * @return - Raw expression count of the selected gene
 */
double FeatureCollection::getFeatureRawCount(int index) const {
   return this->features.at(index).count;
}


/**
 * @brief FeatureCollection::getFoldChangeSum - Calculates the normalized fold change sum for the current genes
 * @return - Sum of all genes currently listed in this FeatureCollection normalized with the number of features
 */
double FeatureCollection::getFoldChangeSum() const {
    double foldChangeSum = 0;
    for (Feature feature : this->features) {
        foldChangeSum += feature.foldChange;
    }
    return foldChangeSum;
}


/**
 * @brief FeatureCollection::getExpressionCountSum - Calculates the normalized raw count sum for the current genes
 * @return - Sum of all gene expression counts currently listed in this FeatureCollection normalized with the number of features
 */
double FeatureCollection::getExpressionCountSum() const {
    return this->expressionCountSum / this->getNumberOfFeatures();
}
