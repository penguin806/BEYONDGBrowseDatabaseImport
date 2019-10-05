#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "configcontainer.h"
#include "databasemanager.h"
#include <QObject>
#include <QThread>
#include <QTextEdit>

class WorkerThread : public QThread
{
public:
    WorkerThread(ConfigContainer config, QTextEdit *progressPanel, QObject *parent = Q_NULLPTR);
    ~WorkerThread();
    void printProgress(QString message);

protected:
    virtual void run();
    void processGtfFile();
    void processMsAlignFile();
    void processCsvFile();

private:
    ConfigContainer globalConfig;
    DatabaseManager *dbManager;
    QTextEdit *progressPanel;

};

#endif // WORKERTHREAD_H
