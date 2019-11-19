#ifndef SNOWMAINWND_H
#define SNOWMAINWND_H

#include "configcontainer.h"
#include "workerthread.h"
#include <QDialog>

namespace Ui {
class SnowMainWnd;
}

class SnowMainWnd : public QDialog
{
    Q_OBJECT

public:
    explicit SnowMainWnd(QWidget *parent = nullptr);
    ~SnowMainWnd();

protected:
    void loadGlobalConfig();
    bool loadDatasetListFromDatabase();

private slots:
    void on_pushButton_Db_Next_clicked();
    void on_toolButton_Source_Gtf_clicked();
    void on_toolButton_Source_MsAlign_clicked();
    void on_toolButton_Source_Csv_clicked();
    void on_pushButton_Source_Start_clicked();
    void onWorkerThreadFinished();

    void on_toolButton_Source_addNewDataset_clicked();

private:
    Ui::SnowMainWnd *ui;
    ConfigContainer globalConfig;
    WorkerThread *workerThread;
};

#endif // SNOWMAINWND_H
