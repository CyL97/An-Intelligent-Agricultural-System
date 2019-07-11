#ifndef WTHREAD_H
#define WTHREAD_H

#include <QThread>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <QMutex>

class wThread : public QThread
{
    Q_OBJECT
public:
    wThread();
    void run();
   // QMutex pause;

signals:        //自定义信号，不需要

private:
    int fd;
};

#endif // WTHREAD_H
