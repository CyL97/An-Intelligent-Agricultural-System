#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QComboBox>
#include <QPushButton>
#include <QtCharts>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QChartView>
#include <QLineSeries>
#include <QPieSeries>
#include "keyBoard.h"
#include "mythread.h"
#include "wthread.h"
#define TEST_MAGIC 'x'
#define LED1 _IO(TEST_MAGIC, 0)
#define LED2 _IO(TEST_MAGIC, 1)
#define LED3 _IO(TEST_MAGIC, 2)
#define LED4 _IO(TEST_MAGIC, 3)
using namespace QtCharts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

public slots:
    void receive_info(QString str);

private slots:
    void timerUpDate();

    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_3_clicked();

    void setLed();

    void on_pushButton_6_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

private:
    Ui::MainWindow *ui;
    //void timerEvent(QTimerEvent *);
    QProcess *process;
    myThread *thread;
    wThread *thread_b;
    void show_info(QStringList list);

};

#endif // MAINWINDOW_H
