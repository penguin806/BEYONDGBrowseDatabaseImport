#include "snowmainwnd.h"
#include "ui_snowmainwnd.h"
#include <QFileDialog>

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

ConfigContainer SnowMainWnd::loadGlobalConfig()
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
    return ConfigContainer(dbConfig, filePathConfig);
}

void SnowMainWnd::on_pushButton_Db_Next_clicked()
{
    this->ui->tabWidget->setCurrentIndex(1);
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
    ConfigContainer globalConfig =
            this->loadGlobalConfig();
    this->workerThread = new WorkerThread(
                globalConfig,
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
