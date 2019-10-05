#include "configcontainer.h"

ConfigContainer::ConfigContainer()
{

}

ConfigContainer::ConfigContainer(DBCONFIG dbConfig, FILEPATHCONFIG filePathConfig)
{
    this->setDbConfig(dbConfig);
    this->setFilePath(filePathConfig);
}

ConfigContainer::ConfigContainer(
        QString dbServerAddrress, QString dbDatabaseName, QString dbUsername,
        QString dbPassword, QString pathGtfFile, QString pathMsAlignFile,
        QString pathCsvFile
)
{
    DBCONFIG dbConfig;
    dbConfig.setDbServerAddrress(dbServerAddrress);
    dbConfig.setDbDatabaseName(dbDatabaseName);
    dbConfig.setDbUsername(dbUsername);
    dbConfig.setDbPassword(dbPassword);

    FILEPATHCONFIG filePathConfig;
    filePathConfig.setPathGtfFile(pathGtfFile);
    filePathConfig.setPathMsAlignFile(pathMsAlignFile);
    filePathConfig.setPathCsvFile(pathCsvFile);

    this->setDbConfig(dbConfig);
    this->setFilePath(filePathConfig);
}

DBCONFIG ConfigContainer::getDbConfig() const
{
    return dbConfig;
}

void ConfigContainer::setDbConfig(const DBCONFIG &value)
{
    dbConfig = value;
}

FILEPATHCONFIG ConfigContainer::getFilePath() const
{
    return filePath;
}

void ConfigContainer::setFilePath(const FILEPATHCONFIG &value)
{
    filePath = value;
}


QString _dbConfig::getDbServerAddrress() const
{
    return dbServerAddrress;
}

void _dbConfig::setDbServerAddrress(const QString &value)
{
    dbServerAddrress = value;
}

QString _dbConfig::getDbDatabaseName() const
{
    return dbDatabaseName;
}

void _dbConfig::setDbDatabaseName(const QString &value)
{
    dbDatabaseName = value;
}

QString _dbConfig::getDbUsername() const
{
    return dbUsername;
}

void _dbConfig::setDbUsername(const QString &value)
{
    dbUsername = value;
}

QString _dbConfig::getDbPassword() const
{
    return dbPassword;
}

void _dbConfig::setDbPassword(const QString &value)
{
    dbPassword = value;
}


QString _FilePathConfig::getPathGtfFile() const
{
    return pathGtfFile;
}

void _FilePathConfig::setPathGtfFile(const QString &value)
{
    pathGtfFile = value;
}

QString _FilePathConfig::getPathMsAlignFile() const
{
    return pathMsAlignFile;
}

void _FilePathConfig::setPathMsAlignFile(const QString &value)
{
    pathMsAlignFile = value;
}

QString _FilePathConfig::getPathCsvFile() const
{
    return pathCsvFile;
}

void _FilePathConfig::setPathCsvFile(const QString &value)
{
    pathCsvFile = value;
}
