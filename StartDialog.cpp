#include "StartDialog.h"
#include "ui_StartDialog.h"

#include <QStringList>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QObject>
#include <QAction>


/**
 * @brief StartDialog::StartDialog
 * @param parent
 */
StartDialog::StartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartDialog)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->buttonRun->setDisabled(true);
}


/**
 * @brief StartDialog::~StartDialog
 */
StartDialog::~StartDialog() {
    delete this->ui;
    delete this;
}


/**
 * @brief StartDialog::createPushButton
 * @return
 */
QPushButton * StartDialog::createPushButton() {
    QPushButton * button = new QPushButton();
    button->setText("+");
    return button;
}


/**
 * @brief StartDialog::getFileName
 * @param filePath
 * @return
 */
QString StartDialog::getFileName(QString filePath) {
    return filePath.split(QDir::separator()).last();
}

/**
 * @brief StartDialog::openFileDialog - Opens a file dialog specific to files with given type to files with given type
 * @param validMimeTypeExtensions - StringList that contains the valid file types that the dialog shows
 * @return - List of user-selected files
 */
QStringList StartDialog::openFileDialog(QStringList validMimeTypeExtensions) {
    QFileDialog fileDialog(this);
    fileDialog.setDirectory(QDir::home());
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setMimeTypeFilters(validMimeTypeExtensions);

    QStringList fileNames;
    if (fileDialog.exec())
        fileNames = fileDialog.selectedFiles();

    return fileNames;
}

/**
 * @brief StartDialog::addDatasetToLayout
 * @param name
 */
void StartDialog::addDatasetToLayout(QString filePath) {
    QString fileName = getFileName(filePath);
    QListWidgetItem * item = new QListWidgetItem(fileName);
    ui->listDatasets->addItem(item);
}


/**
 * @brief StartDialog::enableRunButtonIfReady - If every requirement is set, enable the run button
 */
void StartDialog::enableRunButtonIfReady() {
    bool isUploadedDataset = uploadedDatasets.length() > 0;
    bool isUseDefaultSelected = ui->checkBoxUseDefault->isChecked();

    if (isUploadedDataset && isUseDefaultSelected) {
        ui->buttonRun->setEnabled(true);
    }
}

// ++++++++++++++++++++++++++++++++ SLOTS ++++++++++++++++++++++++++++++++
// STACKED WIDGET PAGE ONE
/**
 * @brief StartDialog::on_buttonMenuBarExit_clicked
 */
__attribute__((noreturn)) void StartDialog::on_buttonMenuBarExit_clicked() {
    exit(0);
}


/**
 * @brief StartDialog::on_buttonExit_clicked
 */
__attribute__((noreturn)) void StartDialog::on_buttonExit_clicked() {
    exit(0);
}


/**
 * @brief StartDialog::on_buttonLoadProject_clicked
 */
void StartDialog::on_buttonLoadProject_clicked() {
    QStringList csvMimeTypes = { "text/plain" };
    QStringList fileNames = this->openFileDialog(csvMimeTypes);

    if (fileNames.empty())
        return;

    qDebug() << "Sent project file name.";
    emit projectFileUploaded(fileNames);
}


/**
 * @brief StartDialog::on_buttonNewProject_clicked
 */
void StartDialog::on_buttonNewProject_clicked() {
    qDebug() << "New Project.";
    ui->stackedWidget->setCurrentIndex(1);
}


// STACKED WIDGET PAGE TWO
/**
 * @brief StartDialog::on_buttonMenuBarBack_clicked
 */
void StartDialog::on_buttonMenuBarBack_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}


/**
 * @brief StartDialog::on_checkBoxUseDefault_stateChanged
 * @param state
 */
void StartDialog::on_checkBoxUseDefault_stateChanged(int state) {
    if (state == Qt::CheckState::Checked) {
        ui->buttonLoadCustom->setDisabled(true);
        enableRunButtonIfReady();
    } else {
        ui->buttonLoadCustom->setDisabled(false);
        ui->buttonRun->setDisabled(true);
    }
}


/**
 * @brief StartDialog::on_buttonMenuBarExit_2_clicked
 */
__attribute__((noreturn)) void StartDialog::on_buttonMenuBarExit_2_clicked() {
    exit(0);
}


/**
 * @brief StartDialog::on_buttonLoadCustom_clicked
 */
void StartDialog::on_buttonLoadCustom_clicked() {
    QStringList csvMimeTypes = { "text/csv" };
    QStringList fileNames = this->openFileDialog(csvMimeTypes);

    if (fileNames.empty())
        return;

    qDebug() << "Uploaded" << fileNames[0];
}


/**
 * @brief StartDialog::on_buttonAddDataset_clicked
 */
void StartDialog::on_buttonAddDataset_clicked() {
    QStringList csvMimeTypes = { "text/csv" };
    QStringList fileNames = openFileDialog(csvMimeTypes);

    if (fileNames.empty())
        return;

    for (int i = 0; i < fileNames.length(); i++) {
        QString fileName = getFileName(fileNames[i]);

        // If file has already been uploaded, skip it
        if (uploadedDatasets.contains(fileName)) {
            continue;
        }

        uploadedDatasets.append(fileName);
        addDatasetToLayout(fileName);
        qDebug() << "Uploaded" << fileName;
    }
    enableRunButtonIfReady();
}


/**
 * @brief StartDialog::on_buttonRemoveDataset_clicked
 */
void StartDialog::on_buttonRemoveDataset_clicked() {
    qDebug() << uploadedDatasets;
    for (QListWidgetItem * item : ui->listDatasets->selectedItems()) {
        uploadedDatasets.removeOne(item->text());
        delete ui->listDatasets->takeItem(ui->listDatasets->row(item));
    }
    qDebug() << uploadedDatasets;
}


/**
 * @brief StartDialog::on_buttonRun_clicked
 */
void StartDialog::on_buttonRun_clicked() {
    emit runNewProject(uploadedDatasets);
    this->close();
    //REMEMBER: How to delete this the right way?
//    this->deleteLater(); ?!
//    this->~StartDialog();
}

// ++++++++++++++++++++++++++++++++ SLOTS ++++++++++++++++++++++++++++++++
