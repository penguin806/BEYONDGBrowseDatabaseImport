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
        if(!this->globalConfig.getDatasetId().isEmpty())
        {
            this->processGtfFile();
            this->processMsAlignFile();
            this->processCsvFile();
        }
        else
        {
            this->printProgress("[Error] DatabaseId is null!");
        }
    }
    else
    {
        this->printProgress(
                    "[Error] Database table initialization fail: " +
                    this->dbManager->getDatabaseConnection().lastError().text()
                );
    }

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
            this->dbManager->insertGtfRecord(stringBuffer, this->globalConfig.getDatasetId());
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
    QFile inputMsAlignFile(this->globalConfig.getFilePath().getPathMsAlignFile());
    if( !inputMsAlignFile.open(QFile::ReadOnly) )
    {
        this->printProgress(
                    "[Error] Unable to open file: " +
                    inputMsAlignFile.fileName() +
                    " Reason: " + inputMsAlignFile.errorString()
                );
        return;
    }
    else
    {
        this->printProgress(
                    "[Info] Open input file <" +
                    inputMsAlignFile.fileName() + "> Success!"
                );
    }

    QTextStream inputFileStream(&inputMsAlignFile);
    QString stringBuffer;
    qint32 loopCounts = 0;

    // Fetch each line from the inputFile, insert into database
    this->dbManager->getDatabaseConnection().transaction();
    while(inputFileStream.readLineInto(&stringBuffer))
    {
        loopCounts++;
        //Eg: 9,520.45185~6838.14;502.09961~52.29;

        try
        {
            this->dbManager->insertMsAlignRecord(stringBuffer, this->globalConfig.getDatasetId());
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
                        "th line: " + stringBuffer.left(10)
                    );
        }
    }
    this->dbManager->getDatabaseConnection().commit();
    this->printProgress("[Info] Parse file <" + inputMsAlignFile.fileName()
                        + "> and insert into db success!");
}

void WorkerThread::processCsvFile()
{
    QFile inputCsvFile(this->globalConfig.getFilePath().getPathCsvFile());
    if( !inputCsvFile.open(QFile::ReadOnly) )
    {
        this->printProgress(
                    "[Error] Unable to open file: " +
                    inputCsvFile.fileName() +
                    " Reason: " + inputCsvFile.errorString()
                );
        return;
    }
    else
    {
        this->printProgress(
                    "[Info] Open input file <" +
                    inputCsvFile.fileName() + "> Success!"
                );
    }

    QTextStream inputFileStream(&inputCsvFile);
    QString stringBuffer;
    qint32 loopCounts = 0;

    // Fetch each line from the inputFile, insert into database
    this->dbManager->getDatabaseConnection().transaction();

    while (inputFileStream.readLineInto(&stringBuffer))
    {
        if(
                stringBuffer.compare(
                    "Data file name,Prsm ID,Spectrum ID,Fragmentation,Scan(s),Retention time,#peaks,Charge,Precursor mass,Adjusted precursor mass,Proteoform ID,Feature intensity,Protein accession,Protein description,First residue,Last residue,Proteoform,#unexpected modifications,MIScore,#variable PTMs,#matched peaks,#matched fragment ions,P-value,E-value,Q-value (spectral FDR),Proteoform FDR",
                    Qt::CaseInsensitive
                ) == 0
                // Two strings are same
        )
        {
            break;
        }
    }

    while(inputFileStream.readLineInto(&stringBuffer))
    {
        loopCounts++;
        //Eg: 2DLC_H3_1_ms2.msalign,0,476,CID,525,4795.66,27,6,2914.70761,2914.70761,140,-,sp|Q9UIG0|BAZ1B_HUMAN,"Tyrosine-protein kinase BAZ1B OS=Homo sapiens OX=9606 GN=BAZ1B PE=1 SV=2",385,411,I.PKKGPPAKKPGKHSDKP(LKAKGRSKGI)[Phospho].L,0,-,1,6,4,8.032177881e-005,8.0325e-005,0.003095975232,0.007518796993

        try
        {
            this->dbManager->insertCsvRecord(stringBuffer, this->globalConfig.getDatasetId());
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
                        "th line: " + stringBuffer.left(10)
                    );
        }
    }
    this->dbManager->getDatabaseConnection().commit();
    this->printProgress("[Info] Parse file <" + inputCsvFile.fileName()
                        + "> and insert into db success!");
}
