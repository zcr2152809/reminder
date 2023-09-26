#ifndef FINISHEDHOMEWORKPAGE_H
#define FINISHEDHOMEWORKPAGE_H

#include <QWidget>

namespace Ui {
class finishedhomeworkPage;
}

class finishedhomeworkPage : public QWidget
{
    Q_OBJECT

public:
    //explicit finishedhomeworkPage(QWidget *parent = nullptr);
    finishedhomeworkPage(int pageNo, QString title, QString content, QDateTime ddl, QDateTime finishTime);
    ~finishedhomeworkPage();

    //获取页编号
    int getPageNo();

signals:
    void withdraw();//撤回至作业列表信号

private:
    Ui::finishedhomeworkPage *ui;

    //页编号
    int pageNo;
};

#endif // FINISHEDHOMEWORKPAGE_H
