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
    void insertGtfRecord(QString fileOneLine);
    void insertMsAlignRecord(QString fileOneLine);
    void insertCsvRecord(QString fileOneLine);

private:
    QObject *parentObject;
//    QSqlDatabase databaseConnection;
};

#endif // DATABASEOPERATIONS_H
