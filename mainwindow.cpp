#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QtCore>
#include <time.h>
#include <QtDebug>
#include <QtSql>
#include <QProcess>
#include <QMessageBox>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <QtCharts>
using namespace QtCharts;
int flag1=0;
int flag2=0;
int flag1_out=0;
int flag2_out=0;
int time_lot1=0;
int time_lot2=0;
int fd=0;
int fd_led=0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    process = new QProcess;
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(1);
    ui->label_29->setFocus();
    ui->lineEdit_3->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    //时间处理
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpDate()));
    timer->start(1000);
    
    //开启串口线程
    //qDebug() << "!@#!$!@$" <<endl;
    thread = new myThread;
    thread->start();
    connect(thread, SIGNAL(send_str(QString)),this, SLOT(receive_info(QString)));


    //新建蜂鸣器线程
    thread_b = new wThread;
    fd = open("/dev/beep",O_RDWR | O_NOCTTY);
    if(fd < 0)
    {
        qDebug() <<"open beep failed";
    }

    //开启LED
    fd_led = open("/dev/Led",O_RDWR | O_NOCTTY);
    if(fd_led < 0)
    {
        qDebug() <<"open led failed";
    }

    //数据库创建
    QSqlDatabase db;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("./MyDataBase.db");
    }
    if (!db.open())
    {
        qDebug()<<db.lastError();
        qFatal("Failed to connect.");
    }
    QSqlQuery query;
    bool success = query.exec("create table users(username varchar, password varchar)");
    if(success)
    {
        qDebug() << QObject::tr("user success！\n");
    }
    else
    {
        qDebug() << QObject::tr("user fail！\n");
    }
    QSqlQuery qry;
    success = qry.exec("create table no1(temp int, humidity int, air int, lighting int)");
    if(success)
    {
        qDebug() << QObject::tr("1 success！\n");
    }
    else
    {
        qDebug() << QObject::tr("1 fail！\n");
    }

    /*query.prepare("insert into users(username, password) values(?, ?)");
    query.addBindValue("lichaoyu");
    query.addBindValue("12341234");*/
    connect(ui->radioButton,SIGNAL(clicked()),this,SLOT(setLed()));
    connect(ui->radioButton_2,SIGNAL(clicked()),this,SLOT(setLed()));
    connect(ui->radioButton_3,SIGNAL(clicked()),this,SLOT(setLed()));
}

MainWindow::~MainWindow()
{
    delete ui;

    delete process;
}

//系统时间更新
void MainWindow::timerUpDate()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    ui->lblCurDate->setText(str);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);
    ui->lblCurDate->setPalette(pa);
}

//登录功能
void MainWindow::on_pushButton_clicked()
{
    //QSqlDatabase db = QSqlDatabase::database("sqlite1");
    QString username = ui->lineEdit_2->text();
    QString password = ui->lineEdit_3->text();
    QSqlQuery query;
    if (!query.exec(QString("select username, password from users where username = '%1'").arg(username)))
    {
        qDebug() << query.lastError();
    }
    else
    {
        //qDebug() << username;
        //qDebug() << password;
        QString temp_name, temp_pass;
        while(query.next())
        {
            temp_name = query.value(0).toString();
            temp_pass = query.value(1).toString();
        }
        if (temp_name==NULL)
        {
            ui->label_52->setText("用户名或密码错误，请重新输入");
            ui->lineEdit_2->setText("");
            ui->lineEdit_3->setText("");
        }
        else
        {
            if (temp_pass == password)
            {
                ui->label_52->setText("");
                ui->stackedWidget->setCurrentIndex(0);
            }
            else
            {
                ui->label_52->setText("密码错误，请重新输入");
                //ui->lineEdit_2->setText("");
                ui->lineEdit_3->setText("");
            }
        }
    }
}

//注册功能
void MainWindow::on_pushButton_4_clicked()
{
    //QSqlDatabase db = QSqlDatabase::database("sqlite1");
    QString username = ui->lineEdit_2->text();
    QString password = ui->lineEdit_3->text();
    if (username=="")
        ui->label_52->setText("用户名不能为空");
    else
    if (password=="")
        ui->label_52->setText("密码不能为空");
    else
    {
        QSqlQuery query;
        if (!query.exec(QString("select username, password from users where username = '%1'").arg(username)))
        {
            qDebug() << query.lastError();
        }
        else
        {
            QString temp;
            while(query.next())
            {
                temp = query.value(0).toString();
            }
            if (temp==NULL)
            {
                QSqlQuery qry;
                if(!qry.exec(QString("insert into users(username, password) values('%1', '%2')").arg(username).arg(password)))
                    qDebug() << qry.lastError();
                else
                    qDebug( "Inserted!" );
                ui->label_52->setText("注册成功");
            }
            else
                ui->label_52->setText("用户名已经存在");
        }
    }
    /*QSqlQuery qry;
    qry.prepare( "SELECT * FROM users" );
    if( !qry.exec() )
      qDebug() << qry.lastError();
    else
    {
        while(qry.next())
                {
            QString name = qry.value(0).toString();
            QString pass = qry.value(1).toString();
                    qDebug()<<QString("username:%1    password:%2").arg(name).arg(pass);
        }
    }*/
}

//进入监控界面1
void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    process->start("./camera");
}

//退出监控界面
void MainWindow::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    process->close();
}

//退出监控界面
void MainWindow::on_pushButton_10_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    process->close();
}

//进入监控界面2
void MainWindow::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    process->start("./camera");
}

//从串口得到的数据
void MainWindow::receive_info(QString str)
{
    QStringList list,list_one;
    //qDebug() << str <<endl;
    list = str.split("\n");
    QString number = list[0];
    int j=1;
    while (j<=5)
    {
        list_one.append(list[j++]);
    }
    //qDebug() << list_one <<endl;
    if (list_one.count() == 5)
        show_info(list_one);
    list_one.clear();
}

//显示传感器数据
void MainWindow::show_info(QStringList list)
{
    int T=QString(list[0]).toInt();
    int H=QString(list[1]).toInt();
    int A=int(QString(list[2]).toFloat());
    QString M=QString(list[3]);
    float C=QString(list[4]).toFloat();
    int L=int(140-71/C);
    //qDebug() << T << endl;
    //qDebug() << QString(T) << endl;
    flag1=0;
    flag2=0;
    if (T>25 || H<10 || H>80 || T<10)
        flag1=1;
    if (T>25 || H<25 || T<15)
        flag2=1;
    if (flag1==1 && flag2==1)
    {
        if (flag1_out==1)
        {
            if (time_lot1>10)
            {
                flag1_out=0;
                ui->label_warning1->setText("危险");
             //   thread_b->pause.unlock();
                if (!thread_b->isRunning())
                    thread_b->start();
            }
            else
                time_lot1++;
        }
        else
        {
            ui->label_warning1->setText("危险");
           // thread_b->pause.unlock();
            if (!thread_b->isRunning())
                thread_b->start();
        }
        if (flag2_out==1)
        {
            if (time_lot2>10)
            {
                flag2_out=0;
                ui->label_warning2->setText("危险");
             //   thread_b->pause.unlock();
                if (!thread_b->isRunning())
                    thread_b->start();
            }
            else
                time_lot2++;
        }
        else
        {
            ui->label_warning2->setText("危险");
//            thread_b->pause.unlock();
            if (!thread_b->isRunning())
                thread_b->start();
        }
    }
    else
    if (flag1==1)
    {
        if (flag1_out==1)
        {
            if (time_lot1>10)
            {
                flag1_out=0;
                ui->label_warning1->setText("危险");
             //   thread_b->pause.unlock();
                if (!thread_b->isRunning())
                    thread_b->start();
            }
            else
                time_lot1++;
        }
        else
        {
            ui->label_warning1->setText("危险");
          //  thread_b->pause.unlock();
            if (!thread_b->isRunning())
                thread_b->start();
        }
    }
    else
    if (flag2==1)
    {
        if (flag2_out==1)
        {
            if (time_lot2>10)
            {
                flag2_out=0;
                ui->label_warning2->setText("危险");
            //    thread_b->pause.unlock();
                if (!thread_b->isRunning())
                    thread_b->start();
            }
            else
                time_lot2++;
        }
        else
        {
            ui->label_warning2->setText("危险");
           // thread_b->pause.unlock();
            if (!thread_b->isRunning())
                thread_b->start();
        }
    }
    else
    {
        time_lot1=11;
        time_lot2=11;
        flag1_out=0;
        flag2_out=0;
    }
    ui->label_12->setText(tr("%1").arg(T));
    ui->label_13->setText(tr("%1").arg(H));
    ui->label_14->setText(tr("%1").arg(L));
    ui->label_15->setText(tr("%1").arg(A*13));
    ui->label_16->setText(M);
    ui->label_17->setText(tr("%1").arg(T));
    ui->label_18->setText(tr("%1").arg(H));
    ui->label_19->setText(tr("%1").arg(L));
    ui->label_20->setText(tr("%1").arg(A*13));
    ui->label_21->setText(M);
    ui->label_74->setText(tr("%1").arg(T));
    ui->label_75->setText(tr("%1").arg(H));
    ui->label_76->setText(tr("%1").arg(A*13));
    ui->label_77->setText(tr("%1").arg(L));
    ui->label_92->setText(tr("%1").arg(T));
    ui->label_93->setText(tr("%1").arg(H));
    ui->label_94->setText(tr("%1").arg(A*13));
    ui->label_95->setText(tr("%1").arg(L));
    QSqlQuery query;
    if(!query.exec(QString("insert into no1(temp, humidity, air, lighting) values('%1', '%2', '%3', '%4')").arg(T).arg(H).arg(A*13).arg(L)))
        qDebug() << query.lastError();
    else
        qDebug( "Inserted!" );
}

void MainWindow::on_pushButton_6_clicked()
{
    QMessageBox::information(NULL, "Hint", "拍照成功", QMessageBox::Yes);
}

void MainWindow::on_pushButton_9_clicked()
{
    QMessageBox::information(NULL, "Hint", "拍照成功", QMessageBox::Yes);
}

//绘制历史数据折线图
void MainWindow::on_pushButton_11_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    QSqlQuery qry;
    qry.prepare( "SELECT * FROM no1" );
    if( !qry.exec() )
    qDebug() << qry.lastError();
    else
    {
        QChartView *chartView = new QChartView(this);
        QChart *chart = new QChart();
        chart->setTitle("历史数据曲线");   //图标的名字
        ui->widget->setChart(chart);
        //setCentralWidget(chartView);
        QLineSeries *series0 = new QLineSeries();
        QLineSeries *series1 = new QLineSeries();
        QLineSeries *series2 = new QLineSeries();
        QLineSeries *series3 = new QLineSeries();
        series0->setName("温度曲线");   //自动添加折线名字
        series1->setName("湿度曲线");
        series2->setName("光照强度曲线");
        series3->setName("气体浓度曲线");
        chart->addSeries(series0);
        chart->addSeries(series1);
        chart->addSeries(series2);
        chart->addSeries(series3);
        int i=0;
        while(qry.next())
        {
            i++;
            //temp, humidity, air, lighting, people
            int temp= qry.value(0).toInt();
            int humidity= qry.value(1).toInt();
            int air= qry.value(2).toInt();
            int lighting= qry.value(3).toInt();
            //序列添加数值
            series0->append(i, temp);
            series1->append(i, humidity);
            series2->append(i, lighting);
            series3->append(i, air);
        }
        //创建坐标
        QValueAxis *axisX = new QValueAxis;
        axisX->setRange(0, 30);//设置坐标轴范围
        axisX->setTitleText("count(times)");//标题
        axisX->setLabelFormat("%d"); //标签格式：每个单位保留几位小数
        axisX->setTickCount(15); //主分隔个数：0到30分成30个单位
        axisX->setMinorTickCount(4); //每个单位之间绘制了多少虚网线
        // axisX->setGridLineVisible(false);

        QValueAxis *axisY = new QValueAxis; //Y 轴
        axisY->setRange(20, 100);
        axisY->setTitleText("value");
        axisY->setLabelFormat("%d"); //标签格式
        axisY->setTickCount(5);
        axisY->setMinorTickCount(4);
        //    axisX->setGridLineVisible(false);

        //为序列设置坐标轴
        chart->setAxisX(axisX, series0);
        chart->setAxisY(axisY, series0);

        chart->setAxisX(axisX, series1);
        chart->setAxisY(axisY, series1);

        chart->setAxisX(axisX, series2);
        chart->setAxisY(axisY, series2);

        chart->setAxisX(axisX, series3);
        chart->setAxisY(axisY, series3);
    }
}

void MainWindow::on_pushButton_12_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->label_warning1->setText("");
    flag1_out=1;
    time_lot1=0;
    if (flag2==0 || flag2_out==1)
    {
        thread_b->terminate();
        thread_b->wait();
        ioctl(fd,1,1);
    }
}


void MainWindow::on_pushButton_8_clicked()
{
    ui->label_warning2->setText("");
    flag2_out=1;
    time_lot2=0;
    if (flag1==0 || flag1_out==1)
    {
        thread_b->terminate();
        thread_b->wait();
        ioctl(fd,1,1);
    }
}

void MainWindow::setLed()
{
    if (ui->radioButton->isChecked())
    {
        ioctl(fd_led,LED1,0);
        ioctl(fd_led,LED2,0);
        ioctl(fd_led,LED3,0);
        ioctl(fd_led,LED4,0);
    }
    else if (ui->radioButton_2->isChecked())
    {
        ioctl(fd_led,LED1,0);
        ioctl(fd_led,LED2,0);
        ioctl(fd_led,LED3,1);
        ioctl(fd_led,LED4,1);
    }
    else if (ui->radioButton_3->isChecked())
    {
        ioctl(fd_led,LED1,1);
        ioctl(fd_led,LED2,1);
        ioctl(fd_led,LED3,1);
        ioctl(fd_led,LED4,1);
    }
    else
    {
        ioctl(fd_led,LED1,1);
        ioctl(fd_led,LED2,1);
        ioctl(fd_led,LED3,1);
        ioctl(fd_led,LED4,1);
    }
}
