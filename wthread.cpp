#include "wthread.h"
#include <QDebug>

wThread::wThread()
{
    fd = open("/dev/beep",O_RDWR | O_NOCTTY);
    if(fd < 0)
    {
        qDebug() <<"open beep failed";
    }
}


void wThread::run()
{
    while(1)
    {
        //pause.lock();
        ioctl(fd,0,1);
        usleep(500000);
        ioctl(fd,1,1);
        usleep(500000);
        //pause.unlock();
    }
}
