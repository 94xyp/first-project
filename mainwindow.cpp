#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "viewwidget.h"
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lst_Roomid.clear();
    ui->tabWidget->setTabsClosable(true);
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(on_closetab(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_pushButton_clicked()
{
    if(!ui->lineEdit->text().isEmpty() && !lst_Roomid.contains(ui->lineEdit->text())){
        viewwidget* view = new viewwidget(ui->lineEdit->text(),this);
        ui->tabWidget->addTab(view,ui->lineEdit->text());
        lst_Roomid.append(ui->lineEdit->text());
    }else{
        QMessageBox::warning(this,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("房间号为空或者已存在"));
    }
}

void MainWindow::on_closetab(int index){
    qDebug()<<index;
    qDebug()<<"remove"<<ui->tabWidget->tabText(index);
    lst_Roomid.removeOne(ui->tabWidget->tabText(index));
    ui->tabWidget->widget(index)->deleteLater();
    ui->tabWidget->removeTab(index);
}

