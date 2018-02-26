#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class connectDialog;
}

class connectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit connectDialog(QWidget *parent = 0);
    ~connectDialog();

private slots:
    void on_pushButton_clicked();
    void on_closetab(int);

private:
    Ui::connectDialog *ui;
    QStringList lst_Roomid;
};

#endif // CONNECTDIALOG_H
