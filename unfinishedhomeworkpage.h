#ifndef UNFINISHEDHOMEWORKPAGE_H
#define UNFINISHEDHOMEWORKPAGE_H

#include <QWidget>

namespace Ui {
class UnfinishedHomeworkPage;
}

class UnfinishedHomeworkPage : public QWidget
{
    Q_OBJECT

public:
    //explicit UnfinishedHomeworkPage(QWidget *parent = nullptr);
    UnfinishedHomeworkPage(int pageNo, QString title, QString content, QDateTime ddl);
    ~UnfinishedHomeworkPage();

    //获取页编号
    int getPageNo();

signals:
    void withdraw();//撤回至作业列表

private:
    Ui::UnfinishedHomeworkPage *ui;

    //页编号
    int pageNo;
};

#endif // UNFINISHEDHOMEWORKPAGE_H
