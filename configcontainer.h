#ifndef CONFIGCONTAINER_H
#define CONFIGCONTAINER_H

#include <QObject>

struct _dbConfig
{
public:

    QString getDbServerAddrress() const;
    void setDbServerAddrress(const QString &value);

    QString getDbDatabaseName() const;
    void setDbDatabaseName(const QString &value);

    QString getDbUsername() const;
    void setDbUsername(const QString &value);

    QString getDbPassword() const;
    void setDbPassword(const QString &value);

private:
    QString dbServerAddrress;
    QString dbDatabaseName;
    QString dbUsername;
    QString dbPassword;
};
typedef struct _dbConfig DBCONFIG;

struct _FilePathConfig
{
public:
    QString getPathGtfFile() const;
    void setPathGtfFile(const QString &value);

    QString getPathMsAlignFile() const;
    void setPathMsAlignFile(const QString &value);

    QString getPathCsvFile() const;
    void setPathCsvFile(const QString &value);

private:
    QString pathGtfFile;
    QString pathMsAlignFile;
    QString pathCsvFile;
};
typedef struct _FilePathConfig FILEPATHCONFIG;



class ConfigContainer
{
public:
    ConfigContainer();
    ConfigContainer(DBCONFIG dbConfig, FILEPATHCONFIG filePathConfig, QString datasetId);
    ConfigContainer(
            QString dbServerAddrress,
            QString dbDatabaseName,
            QString dbUsername,
            QString dbPassword,
            QString pathGtfFile,
            QString pathMsAlignFile,
            QString pathCsvFile,
            QString datasetId
    );

    DBCONFIG getDbConfig() const;
    void setDbConfig(const DBCONFIG &value);

    FILEPATHCONFIG getFilePath() const;
    void setFilePath(const FILEPATHCONFIG &value);

    QString getDatasetId() const;
    void setDatasetId(const QString &value);

private:
    DBCONFIG dbConfig;
    FILEPATHCONFIG filePath;
    QString datasetId;
};

#endif // CONFIGCONTAINER_H
