#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

class myThread : public QThread
{
    Q_OBJECT
public:
    myThread();
    void run();
    int init_serial(int fd);

signals:        //自定义信号，不需要
    void send_str(QString str);

private:
    int fd;
    char buf[100];
    char msg[100];
};

#endif // MYTHREAD_H
