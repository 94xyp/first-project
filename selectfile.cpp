#include "selectfile.h"
#include "ui_selectfile.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

SelectFile::SelectFile(QString html,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectFile)
{
    ui->setupUi(this);
    ui->lineEdit->setText("danmu.html");
    ui->lineEdit_2->setText(QDir::currentPath());
    m_html = html;
}

SelectFile::~SelectFile()
{
    delete ui;
}

void SelectFile::on_pushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,QString::fromLocal8Bit("导出位置"),QDir::currentPath(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->lineEdit_2->setText(dir);
}

void SelectFile::on_pushButton_2_clicked()
{
    if(ui->lineEdit_2->text().isEmpty()){
        this->reject();
    }else{
        QFile file(ui->lineEdit_2->text()+"\\"+ui->lineEdit->text());
        if(file.open(QIODevice::WriteOnly)){
            QTextStream ts(&file);
            ts << m_html;
            file.close();
            this->accept();
        }else{
            this->reject();
        }
    }
}
