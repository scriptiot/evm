#include "mainwindow.h"
#include <QTimer>
#include <QPainter>
#include <QThread>
#include <QMouseEvent>
#include "lvgl_main.h"
#include "evm_main.h"
#include "lvgl.h"

class GuiThread: public QThread
{
public:
    GuiThread(char * file){
        m_file = file;
    }
private:
    void run()
    {
        evm_main(m_file);
    }
    char * m_file;
};

MainWindow::MainWindow(char * file, QWidget *parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_NoBackground);
    setFixedHeight(LV_VER_RES_MAX);
    setFixedWidth(LV_HOR_RES_MAX);
    setWindowTitle("EVM QML");

    GuiThread* thread = new GuiThread(file);
    thread->start();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(30);//30ms
}

MainWindow::~MainWindow()
{

}

void MainWindow::paintEvent(QPaintEvent* p)
{
    uchar * fb = (uchar*)evm_get_fb();
    if(fb){
        QPainter painter(this);
        QImage img(fb, LV_HOR_RES_MAX, LV_VER_RES_MAX, QImage::Format_RGB16);
        painter.drawImage(0, 0, img);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    evm_touchpad(e->x(), e->y(), 1);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    evm_touchpad(e->x(), e->y(), 0);
}
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    evm_touchpad(e->x(), e->y(), 1);
}
