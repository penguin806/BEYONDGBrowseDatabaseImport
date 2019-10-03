#include "snowmainwnd.h"
#include "ui_snowmainwnd.h"
#include <QFileDialog>

SnowMainWnd::SnowMainWnd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SnowMainWnd)
{
    ui->setupUi(this);
}

SnowMainWnd::~SnowMainWnd()
{
    delete ui;
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

}
