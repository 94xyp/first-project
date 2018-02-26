#include "connectdialog.h"
#include "ui_connectdialog.h"
#include "viewwidget.h"
#include <QMessageBox>

connectDialog::connectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::connectDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowMinMaxButtonsHint;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    lst_Roomid.clear();
    ui->tabWidget->setTabsClosable(true);
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(on_closetab(int)));
}

connectDialog::~connectDialog()
{
    delete ui;
}

void connectDialog::on_pushButton_clicked()
{
    if(!ui->lineEdit->text().isEmpty() && !lst_Roomid.contains(ui->lineEdit->text())){
        viewwidget* view = new viewwidget(ui->lineEdit->text(),this);
        ui->tabWidget->addTab(view,ui->lineEdit->text());
        lst_Roomid.append(ui->lineEdit->text());
    }else{
        QMessageBox::warning(this,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("房间号为空或已存在"));
    }
}

void connectDialog::on_closetab(int index){
    qDebug()<<index;
    qDebug()<<"remove"<<ui->tabWidget->tabText(index);
    lst_Roomid.removeOne(ui->tabWidget->tabText(index));
    ui->tabWidget->widget(index)->deleteLater();
    ui->tabWidget->removeTab(index);
}


