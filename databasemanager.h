#ifndef DATABASEOPERATIONS_H
#define DATABASEOPERATIONS_H
#include <QObject>
#include <QSqlDatabase>
#include "configcontainer.h"

class DatabaseManager : public QObject
{
public:
    DatabaseManager(QObject *parent = Q_NULLPTR);
    bool connectingToDatabase(DBCONFIG dbConfig);
    static void closeDatabaseConnection();
    static QSqlDatabase getDatabaseConnection();

    bool initDatabaseTables();
    void insertGtfRecord(QString fileOneLine, QString datasetId);
    void insertMsAlignRecord(QString fileOneLine, QString datasetId);
    void insertCsvRecord(QString fileOneLine, QString datasetId);
    QStringList selectDatasetsRecord();
    void insertNewDatasetRecord(QString newDatasetName);

private:
    QObject *parentObject;
//    QSqlDatabase databaseConnection;
};

#endif // DATABASEOPERATIONS_H
