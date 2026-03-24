#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    logindlg = new LoginDialog(this);
    setCentralWidget(logindlg);
    //logindlg->show();
    //创建和注册消息连接
    connect(logindlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    registdlg = new RegisterDialog(this);

    logindlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    registdlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    registdlg->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchReg() {
    setCentralWidget(registdlg);
    logindlg->hide();
    registdlg->show();
}
