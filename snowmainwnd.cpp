#include "snowmainwnd.h"
#include "ui_snowmainwnd.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

SnowMainWnd::SnowMainWnd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnowMainWnd)
{
    ui->setupUi(this);
    this->ui->tabWidget->setCurrentIndex(0);
}

SnowMainWnd::~SnowMainWnd()
{
    delete ui;
}

void SnowMainWnd::loadGlobalConfig()
{
    DBCONFIG dbConfig;
    dbConfig.setDbServerAddrress(this->ui->lineEdit_DbUrl->text());
    dbConfig.setDbDatabaseName(this->ui->lineEdit_DbName->text());
    dbConfig.setDbUsername(this->ui->lineEdit_DbUser->text());
    dbConfig.setDbPassword(this->ui->lineEdit_DbPassword->text());
    FILEPATHCONFIG filePathConfig;
    filePathConfig.setPathGtfFile(this->ui->lineEdit_Source_Gtf->text());
    filePathConfig.setPathMsAlignFile(this->ui->lineEdit_Source_MsAlign->text());
    filePathConfig.setPathCsvFile(this->ui->lineEdit_Source_Csv->text());

    QString datasetId;
    if(
        this->ui->listWidget_Source_Dataset->count() > 0 &&
        this->ui->listWidget_Source_Dataset->currentItem()
    )
    {
        QStringList datasetIdAndNameFields =
                this->ui->listWidget_Source_Dataset->currentItem()
                ->text().split("# ", QString::SkipEmptyParts);
        if(datasetIdAndNameFields.length() == 2)
        {
            datasetId = datasetIdAndNameFields.at(0);
        }
    }

    this->globalConfig =
            ConfigContainer(dbConfig, filePathConfig, datasetId);
}

bool SnowMainWnd::loadDatasetListFromDatabase()
{
    this->ui->listWidget_Source_Dataset->clear();
    DatabaseManager dbManager(this);
    bool bResult;
    try {
        bResult = dbManager.connectingToDatabase(this->globalConfig.getDbConfig());
    } catch (QString message) {
        QMessageBox::critical(this, "Error", message);
        return false;
    }

    if(true == bResult)
    {
        QStringList datasetsList =
                dbManager.selectDatasetsRecord();
        this->ui->listWidget_Source_Dataset->addItems(datasetsList);
    }
    return true;
}

void SnowMainWnd::on_pushButton_Db_Next_clicked()
{
    this->loadGlobalConfig();
    bool bResult = this->loadDatasetListFromDatabase();
    if(bResult)
    {
        this->ui->tabWidget->setCurrentIndex(1);
    }
}

void SnowMainWnd::on_toolButton_Source_Gtf_clicked()
{
    QString gtfFileName =
            QFileDialog::getOpenFileName(this, "Select Gtf File");
    this->ui->lineEdit_Source_Gtf->setText(gtfFileName);
}

void SnowMainWnd::on_toolButton_Source_MsAlign_clicked()
{
    QString msAlignFileName =
            QFileDialog::getOpenFileName(this, "Select MsAlign File");
    this->ui->lineEdit_Source_MsAlign->setText(msAlignFileName);
}

void SnowMainWnd::on_toolButton_Source_Csv_clicked()
{
    QString csvFileName =
            QFileDialog::getOpenFileName(this, "Select Csv File");
    this->ui->lineEdit_Source_Csv->setText(csvFileName);
}

void SnowMainWnd::on_pushButton_Source_Start_clicked()
{
    this->loadGlobalConfig();
    QString confirmText =
            QString("Please confirm the source configuration:\n") +
            QString("DatasetId: ") + this->globalConfig.getDatasetId() + '\n' +
            QString("GtfPath: ") + this->globalConfig.getFilePath().getPathGtfFile() + '\n' +
            QString("MsAlignPath: ") + this->globalConfig.getFilePath().getPathMsAlignFile() + '\n' +
            QString("CsvPath: ") + this->globalConfig.getFilePath().getPathCsvFile();
    QMessageBox::StandardButtons selectedButton = QMessageBox::question(
                this, "Info", confirmText,
                QMessageBox::Ok, QMessageBox::Cancel
            );
    if(selectedButton != QMessageBox::Ok)
    {
        return;
    }

    this->workerThread = new WorkerThread(
                this->globalConfig,
                this->ui->textEdit_ProgressPanel,
                this
            );
    QObject::connect(
                this->workerThread, SIGNAL(finished()),
                this, SLOT(onWorkerThreadFinished())
            );

    this->ui->pushButton_Db_Next->setDisabled(true);
    this->ui->pushButton_Source_Start->setDisabled(true);
    this->ui->tabWidget->setCurrentIndex(2);
    this->workerThread->start();
}

void SnowMainWnd::onWorkerThreadFinished()
{
    this->ui->pushButton_Db_Next->setDisabled(false);
    this->ui->pushButton_Source_Start->setDisabled(false);
    this->workerThread->deleteLater();
    this->workerThread = Q_NULLPTR;
}

void SnowMainWnd::on_toolButton_Source_addNewDataset_clicked()
{
    bool bNewDatasetNameOk;
    QString newDatasetName =
        QInputDialog::getText(
                    this,
                    "Dataset Add",
                    "Enter the new dataset name:",
                    QLineEdit::Normal,
                    QString(),
                    &bNewDatasetNameOk
                );
    if(!bNewDatasetNameOk || newDatasetName.isEmpty())
    {
        return;
    }

    DatabaseManager dbManager(this);
    if(!dbManager.getDatabaseConnection().isOpen())
    {
        bool bResult;
        try {
            bResult = dbManager.connectingToDatabase(this->globalConfig.getDbConfig());
        } catch (QString message) {
            QMessageBox::critical(this, "Error", message);
            return;
        }
    }

    dbManager.initDatabaseTables();
    try
    {
        dbManager.insertNewDatasetRecord(newDatasetName);
    } catch (QString errorMsg)
    {
        QMessageBox::critical(this, "Error", errorMsg);
        return;
    }

    this->loadDatasetListFromDatabase();
}
