#include "finishedhomeworkpage.h"
#include "ui_finishedhomeworkpage.h"
#include <QDateTime>

finishedhomeworkPage::finishedhomeworkPage(int pageNo, QString title, QString content, QDateTime ddl, QDateTime finishTime) :
    ui(new Ui::finishedhomeworkPage)
{
    ui->setupUi(this);

    this->pageNo = pageNo;
    ui->title->setText(title);
    ui->content->setText(content);
    ui->ddl->setText(ddl.toString());
    ui->finishTime->setText(finishTime.toString());//初始化各项属性值

    //实现撤回至作业列表功能
    connect(ui->withdrawBtn, &QPushButton::clicked, [=](){
        emit this->withdraw();
    });
}

int finishedhomeworkPage::getPageNo() {
    return this->pageNo;
}

finishedhomeworkPage::~finishedhomeworkPage()
{
    delete ui;
}
