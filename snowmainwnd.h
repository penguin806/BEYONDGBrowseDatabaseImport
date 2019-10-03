#ifndef SNOWMAINWND_H
#define SNOWMAINWND_H

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

private slots:
    void on_pushButton_Db_Next_clicked();

    void on_toolButton_Source_Gtf_clicked();

    void on_toolButton_Source_MsAlign_clicked();

    void on_toolButton_Source_Csv_clicked();

private:
    Ui::SnowMainWnd *ui;
};

#endif // SNOWMAINWND_H
