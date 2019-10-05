#include "databasemanager.h"
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{

}

bool DatabaseManager::connectingToDatabase(DBCONFIG dbConfig)
{
    bool isMysqlAvailable = QSqlDatabase::isDriverAvailable("QMYSQL");
    if(!isMysqlAvailable)
    {
        throw QString("[Error] QMYSQL Driver Not Available!");
        return false;
    }
    QSqlDatabase databaseConnection =
            QSqlDatabase::addDatabase("QMYSQL", "beyondgbrowse");
    databaseConnection.setHostName(dbConfig.getDbServerAddrress());
    databaseConnection.setDatabaseName(dbConfig.getDbDatabaseName());
    databaseConnection.setUserName(dbConfig.getDbUsername());
    databaseConnection.setPassword(dbConfig.getDbPassword());

    bool isOpenDatabaseSuccess = databaseConnection.open();
    if(isOpenDatabaseSuccess)
    {
//        this->databaseConnection = databaseConnection;
    }
    else
    {
        throw QString("[Error] Connect to database fail!");
    }

    return isOpenDatabaseSuccess;
}

void DatabaseManager::closeDatabaseConnection()
{
    QSqlDatabase::database("beyondgbrowse").close();
}

QSqlDatabase DatabaseManager::getDatabaseConnection()
{
    return QSqlDatabase::database("beyondgbrowse");
}

bool DatabaseManager::initDatabaseTables()
{
    this->getDatabaseConnection().transaction();
    QSqlQuery sqlQuery(this->getDatabaseConnection());

    // Gtf
    sqlQuery.exec("DROP TABLE IF EXISTS `protein_annotation`");
    sqlQuery.exec(
                "CREATE TABLE IF NOT EXISTS `protein_annotation` (\
                  `name` varchar(10) NOT NULL,\
                  `start` int(11) NOT NULL,\
                  `end` int(11) NOT NULL,\
                  `strand` char(1) NOT NULL,\
                  `ensembl_id` varchar(45) NOT NULL,\
                  `uniprot_id` varchar(45) DEFAULT NULL,\
                  PRIMARY KEY (`name`,`start`,`end`,`ensembl_id`)\
                ) ENGINE=InnoDB DEFAULT CHARSET=utf8;"
    );

    // Online protein&proteoform annotation
    sqlQuery.exec("DROP TABLE IF EXISTS `protein_comments`");
    sqlQuery.exec(
                "CREATE TABLE IF NOT EXISTS `protein_comments` (\
                `id` int(11) NOT NULL AUTO_INCREMENT,\
                `name` varchar(10) NOT NULL,\
                `position` int(11) NOT NULL,\
                `time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,\
                `contents` text NOT NULL,\
                PRIMARY KEY (`id`),\
                UNIQUE KEY `name` (`name`,`position`,`time`)\
              ) ENGINE=MyISAM AUTO_INCREMENT=53 DEFAULT CHARSET=utf8;"
    );

    // MsAlign
    sqlQuery.exec("DROP TABLE IF EXISTS `protein_scan`");
    sqlQuery.exec(
                "CREATE TABLE IF NOT EXISTS `protein_scan` (\
                `scan_id` int(4) DEFAULT NULL,\
                `ions` varchar(6497) DEFAULT NULL\
              ) ENGINE=MyISAM DEFAULT CHARSET=utf8;"
    );

    // Csv
    sqlQuery.exec("DROP TABLE IF EXISTS `protein_sequence`");
    sqlQuery.exec(
                "CREATE TABLE IF NOT EXISTS `protein_sequence` (\
                `Data file name` varchar(21) DEFAULT NULL,\
                `Scan(s)` int(4) DEFAULT NULL,\
                `Proteoform ID` int(3) DEFAULT NULL,\
                `Protein accession` varchar(11) DEFAULT NULL,\
                `Protein description` varchar(72) DEFAULT NULL,\
                `Proteoform` varchar(193) DEFAULT NULL\
              ) ENGINE=InnoDB DEFAULT CHARSET=utf8;"
    );

    if(sqlQuery.lastError().isValid())
    {
        qDebug() << sqlQuery.lastError().text();
        this->getDatabaseConnection().rollback();
        return false;
    }
    else
    {
        this->getDatabaseConnection().commit();
        return true;
    }
}

void DatabaseManager::insertGtfRecord(QString fileOneLine)
{
    QStringList oneLineColumns = fileOneLine.split(',', QString::SkipEmptyParts);
    if(oneLineColumns.size() < 5)
    {
        throw QString("Fields less than 5, file format invalid");
    }

    QSqlQuery sqlQuery(this->getDatabaseConnection());
    sqlQuery.prepare(
                "INSERT INTO `protein_annotation` "
                "(`name`, `start`, `end`, `strand`, `ensembl_id`, `uniprot_id`) "
                "VALUES (:name, :start, :end, :strand, :ensembl_id, :uniprot_id)"
            );
    sqlQuery.bindValue(":name", oneLineColumns.at(0));
    sqlQuery.bindValue(":start", oneLineColumns.at(1).toInt());
    sqlQuery.bindValue(":end", oneLineColumns.at(2).toInt());
    sqlQuery.bindValue(":strand", oneLineColumns.at(3));
    sqlQuery.bindValue(":ensembl_id", oneLineColumns.at(4));
    sqlQuery.bindValue(
                ":uniprot_id",
                oneLineColumns.size() == 6 ? oneLineColumns.at(5) : Q_NULLPTR
            );

    bool bResult = sqlQuery.exec();
    if(false == bResult || sqlQuery.numRowsAffected() != 1)
    {
        throw QString("statement execute failed");
    }
}
