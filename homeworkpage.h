#ifndef HOMEWORKPAGE_H
#define HOMEWORKPAGE_H

#include <QWidget>

namespace Ui {
class HomeworkPage;
}

class HomeworkPage : public QWidget
{
    Q_OBJECT

public:
    explicit HomeworkPage(QWidget *parent = nullptr);
    ~HomeworkPage();

    //获取页编号
    int getPageNo();
    //设置页编号
    void setPageNo(int pageNo);
    //获取标题内容
    QString getTitle();
    //设置标题内容
    void setTitle(QString title);
    //获取描述内容
    QString getContent();
    //设置描述内容
    void setContent(QString content);
    //获取截止时间
    QDateTime getDDL();
    //设置截止时间
    void setDDL(QDateTime ddl);
    //获取作业状态
    int getState();
    //设置作业状态
    void setState(int state);
    //启用编辑状态
    void fireEdit();
    //禁用编辑状态
    void stopEdit();
    //启用确认按钮
    void fireConfirm();
    //禁用确认按钮
    void stopConfirm();
    //计算并设置剩余时间
    void changeTimeLeft();
    //触发保存信号
    void emitSave();

signals:
    void save();//保存信号
    void confirmFinish();//确认完成信号
    void confirmUnfinish();//确认未完成信号

private:
    Ui::HomeworkPage *ui;

    //该页的编号
    int pageNo = -1;
    //作业状态
    int state = 0;
};

#endif // HOMEWORKPAGE_H
