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

    // Dataset Catalog
    // sqlQuery.exec("DROP TABLE IF EXISTS `dataset_catalog`;");
    sqlQuery.exec(
                "CREATE TABLE IF NOT EXISTS `dataset_catalog` (\
                `id` int(11) NOT NULL AUTO_INCREMENT,\
                `dataset_name` varchar(64) NOT NULL,\
                PRIMARY KEY (`id`),\
                UNIQUE KEY `dataset_name` (`dataset_name`)\
              ) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;"
    );

    // Gtf
    // sqlQuery.exec("DROP TABLE IF EXISTS `protein_annotation`");
    sqlQuery.exec(
                "CREATE TABLE IF NOT EXISTS `protein_annotation` (\
                  `dataset_id` int(11) NOT NULL,\
                  `name` varchar(10) NOT NULL,\
                  `start` int(11) NOT NULL,\
                  `end` int(11) NOT NULL,\
                  `strand` char(1) NOT NULL,\
                  `ensembl_id` varchar(45) NOT NULL,\
                  `uniprot_id` varchar(45) DEFAULT NULL,\
                  PRIMARY KEY (`dataset_id`,`name`,`start`,`end`,`ensembl_id`)\
                ) ENGINE=MyISAM DEFAULT CHARSET=utf8;"
    );

    // Online protein&proteoform annotation
    // sqlQuery.exec("DROP TABLE IF EXISTS `protein_comments`");
    sqlQuery.exec(
               "CREATE TABLE IF NOT EXISTS `protein_comments` (\
                `dataset_id` int(11) NOT NULL,\
                `id` int(11) NOT NULL AUTO_INCREMENT,\
                `name` varchar(10) NOT NULL,\
                `position` int(11) NOT NULL,\
                `time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,\
                `contents` text NOT NULL,\
                PRIMARY KEY (`id`),\
                UNIQUE KEY `dataset_id` (`dataset_id`,`name`,`position`,`time`)\
              ) ENGINE=InnoDB AUTO_INCREMENT=0 DEFAULT CHARSET=utf8;"
    );

    // MsAlign
    // sqlQuery.exec("DROP TABLE IF EXISTS `protein_scan`");
    sqlQuery.exec(
               "CREATE TABLE IF NOT EXISTS `protein_scan` (\
                `dataset_id` int(11) NOT NULL,\
                `scan_id` int(4) DEFAULT NULL,\
                `ions` varchar(6497) DEFAULT NULL\
                PRIMARY KEY (`dataset_id`,`scan_id`)\
              ) ENGINE=MyISAM DEFAULT CHARSET=utf8;"
    );

    // Csv
    // sqlQuery.exec("DROP TABLE IF EXISTS `protein_sequence`");
    sqlQuery.exec(
               "CREATE TABLE IF NOT EXISTS `protein_sequence` (\
                `dataset_id` int(11) NOT NULL,\
                `Data file name` varchar(256) DEFAULT NULL,\
                `Scan(s)` int(4) DEFAULT NULL,\
                `Proteoform ID` int(4) DEFAULT NULL,\
                `Protein accession` varchar(32) DEFAULT NULL,\
                `Protein description` varchar(256) DEFAULT NULL,\
                `Proteoform` varchar(512) DEFAULT NULL\
                PRIMARY KEY (`dataset_id`,`Scan(s)`)\
              ) ENGINE=MyISAM DEFAULT CHARSET=utf8;"
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

void DatabaseManager::insertMsAlignRecord(QString fileOneLine)
{
    QStringList oneLineColumns = fileOneLine.split(',', QString::SkipEmptyParts);
    if(oneLineColumns.size() != 2)
    {
        throw QString("Fields not equal to 2, file format invalid");
    }

    QSqlQuery sqlQuery(this->getDatabaseConnection());
    sqlQuery.prepare(
                "INSERT INTO `protein_scan` "
                "(`scan_id`, `ions`) "
                "VALUES (:scan_id, :ions)"
            );
    sqlQuery.bindValue(":scan_id", oneLineColumns.at(0).toInt());
    sqlQuery.bindValue(":ions", oneLineColumns.at(1));

    bool bResult = sqlQuery.exec();
    if(false == bResult || sqlQuery.numRowsAffected() != 1)
    {
        throw QString("statement execute failed");
    }
}

void DatabaseManager::insertCsvRecord(QString fileOneLine)
{
    QStringList oneLineColumns = fileOneLine.split(',', QString::SkipEmptyParts);
    if(oneLineColumns.size() != 26)
    {
        throw QString("Fields not equal to 26, file format invalid");
    }

    QSqlQuery sqlQuery(this->getDatabaseConnection());
    sqlQuery.prepare(
                "INSERT INTO `protein_sequence` "
                "(`Data file name`, `Scan(s)`, `Proteoform ID`, `Protein accession`, `Protein description`, `Proteoform`) "
                "VALUES (:Data_file_name, :Scans, :Proteoform_ID, :Protein_accession, :Protein_description, :Proteoform)"
            );
    sqlQuery.bindValue(":Data_file_name", oneLineColumns.at(0));
    sqlQuery.bindValue(":Scans", oneLineColumns.at(4).toInt());
    sqlQuery.bindValue(":Proteoform_ID", oneLineColumns.at(10).toInt());
    sqlQuery.bindValue(":Protein_accession", oneLineColumns.at(12)
                       .split('|',QString::SkipEmptyParts).last());
    sqlQuery.bindValue(":Protein_description", oneLineColumns.at(13));
    sqlQuery.bindValue(":Proteoform", oneLineColumns.at(16));

    bool bResult = sqlQuery.exec();
    if(false == bResult || sqlQuery.numRowsAffected() != 1)
    {
        throw QString("statement execute failed (") +
                sqlQuery.lastError().text() + ")";
    }
}

QStringList DatabaseManager::selectDatasetsRecord()
{
    QSqlQuery sqlQuery(this->getDatabaseConnection());
    QStringList datasetsNameList;
    bool bQueryResult = sqlQuery.exec("SELECT `id`, `dataset_name` FROM `dataset_catalog`");
    while(sqlQuery.next())
    {
        QString oneRowRecord =
                sqlQuery.value("id").toString() +
                QString("# ") +
                sqlQuery.value("dataset_name").toString();
        datasetsNameList.append(oneRowRecord);
    }

    return datasetsNameList;
}

void DatabaseManager::insertNewDatasetRecord(QString newDatasetName)
{
    QSqlQuery sqlQuery(this->getDatabaseConnection());
    sqlQuery.prepare(
                "INSERT INTO `dataset_catalog`(`dataset_name`) "
                "VALUES (:dataset_name)"
            );
    sqlQuery.bindValue(":dataset_name", newDatasetName);

    bool bResult = sqlQuery.exec();
    if(false == bResult || sqlQuery.numRowsAffected() != 1)
    {
        throw QString("Statement execute failed (") +
                sqlQuery.lastError().text() + ")";
    }
}
