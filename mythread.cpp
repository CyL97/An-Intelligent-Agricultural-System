#include "mythread.h"
#include <QDebug>

myThread::myThread()
{
    //1、打开串口
    fd = open("/dev/ttySAC2",O_RDWR | O_NOCTTY);
    if(fd < 0)
    {
        qDebug() <<"open serial failed";

    }

    //buf[0] = '\0';
    //2、初始化串口
    init_serial(fd);
}




//线程start之后自动执行该函数
void myThread::run()
{
    //static int num = 0;
    while(1)
    {
        read(fd,buf,sizeof(buf));
        if (strlen(buf) %27 == 0)
        {
            //strcpy(msg,buf);
            qDebug() << "buf:" << QString::fromUtf8(buf) <<endl;
            emit send_str(QString::fromUtf8(buf));
        }
        /*strcpy(msg,buf);

        T = strtok(msg,"\n");
        H = strtok(NULL,"\n");
        printf("T = %s,H = %s\n",T,H);*/

        sleep(2);

    }
}

int myThread::init_serial(int fd)
{
    struct termios old_cfg,new_cfg;
    //1）保存原先的串口参数：
    tcgetattr(fd, &old_cfg);
    //2）激活选项：
    new_cfg.c_cflag  |=  CLOCAL | CREAD;
    //3）设置波特率：
    cfsetispeed(&new_cfg, B115200);
    cfsetospeed(&new_cfg, B115200);
    //4）设置数据位，8位
    new_cfg.c_cflag &= ~CSIZE; /* 用数据位掩码清空数据位设置 */
    new_cfg.c_cflag |= CS8;
    //5）设置奇偶校验位，设置为无校验位
    new_cfg.c_cflag &= ~PARENB;

    //6）设置停止位，设置为1位停止位
    new_cfg.c_cflag &= ~CSTOPB;

    //7）设置最少字符和等待时间
    new_cfg.c_cc[VTIME] = 0;
    new_cfg.c_cc[VMIN]   = 0;
    //8）清空串口缓冲
    tcflush(fd, TCIFLUSH);
    //9）激活配置
    if ((tcsetattr(fd, TCSANOW, &new_cfg)) != 0)
    {
        perror("tcsetattr");
        return -1;
    }
}
