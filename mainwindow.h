#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "homeworkpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //初始化作业列表界面
    void initHomeworkList();
    //初始化已完成作业列表界面
    void initFinishList();
    //初始化未完成作业列表界面
    void initUnfinishList();
    //实现作业列表中一张单页的功能
    void singlePageFunction(HomeworkPage *homeworkPage);
    //设置按钮状态
    void setBtnState();

    //更新剩余时间的计时器
    QTimer *leftTimer;

private:
    Ui::MainWindow *ui;

    //未截止作业信息
    QStringList homeworkInform;
    //已截止作业信息
    QStringList outHomeworkInform;
};
#endif // MAINWINDOW_H
