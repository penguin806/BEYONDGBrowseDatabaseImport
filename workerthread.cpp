#include "workerthread.h"
#include <QFile>
#include <QTextStream>
#include <QSqlError>
#include <QScrollBar>

WorkerThread::WorkerThread(
        ConfigContainer config,
        QTextEdit *progressPanel,
        QObject *parent
) : QThread(parent), globalConfig(config), progressPanel(progressPanel)
{
    this->dbManager = new DatabaseManager(parent);
}

WorkerThread::~WorkerThread()
{
    this->dbManager->deleteLater();
}

void WorkerThread::printProgress(QString message)
{
    this->progressPanel->append(message);
    this->progressPanel->verticalScrollBar()->triggerAction(QScrollBar::SliderToMaximum);
}

void WorkerThread::run()
{
    bool bResult;
    try
    {
        bResult = this->dbManager->connectingToDatabase(
                    this->globalConfig.getDbConfig()
                );
    } catch (QString message)
    {
        this->printProgress(message);
        return;
    }
    if(true == bResult)
    {
        this->printProgress("[Info] Connect to database success!");
    }

    bResult = this->dbManager->initDatabaseTables();
    if(bResult)
    {
        this->printProgress("[Info] Database table initialization success!");
    }
    else
    {
        this->printProgress(
                    "[Error] Database table initialization fail: " +
                    this->dbManager->getDatabaseConnection().lastError().text()
                );
    }

    this->processGtfFile();

    this->dbManager->closeDatabaseConnection();
}

void WorkerThread::processGtfFile()
{
    QFile inputGtfFile(this->globalConfig.getFilePath().getPathGtfFile());
    if( !inputGtfFile.open(QFile::ReadOnly) )
    {
        this->printProgress(
                    "[Error] Unable to open file: " +
                    inputGtfFile.fileName() +
                    " Reason: " + inputGtfFile.errorString()
                );
        return;
    }
    else
    {
        this->printProgress(
                    "[Info] Open input file <" +
                    inputGtfFile.fileName() + "> Success!"
                );
    }

    QTextStream inputFileStream(&inputGtfFile);
    QString stringBuffer;
    qint32 loopCounts = 0;

    // Fetch each line from the inputFile, insert into database
    this->dbManager->getDatabaseConnection().transaction();
    while(inputFileStream.readLineInto(&stringBuffer))
    {
        loopCounts++;
        //Eg: chr1,65419,71585,ENSP00000493376,A0A2U3U0J3_HUMAN

        try
        {
            this->dbManager->insertGtfRecord(stringBuffer);
        } catch (QString errorMsg)
        {
            this->printProgress(
                        "[Error] Insert the " +
                        QString::number(loopCounts) +
                        "th record <" + stringBuffer + "> failed: " + errorMsg
                    );
            if(this->dbManager->getDatabaseConnection().lastError().isValid())
            {
                this->printProgress(
                            "[Error] " + this->dbManager->getDatabaseConnection().lastError().text()
                        );
            }
            this->dbManager->getDatabaseConnection().rollback();
            return;
        }

        if( 0 == loopCounts % 200)
        {
            this->printProgress(
                        "[Info] Processing the " +
                        QString::number(loopCounts) +
                        "th line: " + stringBuffer
                    );
        }
    }
    this->dbManager->getDatabaseConnection().commit();
    this->printProgress("[Info] Parse file <" + inputGtfFile.fileName()
                        + "> and insert into db success!");
}

void WorkerThread::processMsAlignFile()
{

}

void WorkerThread::processCsvFile()
{

}
