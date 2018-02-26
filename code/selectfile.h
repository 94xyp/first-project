#ifndef SELECTFILE_H
#define SELECTFILE_H

#include <QDialog>

namespace Ui {
class SelectFile;
}

class SelectFile : public QDialog
{
    Q_OBJECT

public:
    explicit SelectFile(QString html,QWidget *parent = 0);
    ~SelectFile();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::SelectFile *ui;
    QString m_html;
};

#endif // SELECTFILE_H
