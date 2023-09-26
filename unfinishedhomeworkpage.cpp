#include "unfinishedhomeworkpage.h"
#include "ui_unfinishedhomeworkpage.h"
#include <QDateTime>

UnfinishedHomeworkPage::UnfinishedHomeworkPage(int pageNo, QString title, QString content, QDateTime ddl) :
    ui(new Ui::UnfinishedHomeworkPage)
{
    ui->setupUi(this);

    this->pageNo = pageNo;
    ui->title->setText(title);
    ui->content->setText(content);
    ui->ddl->setText(ddl.toString());//初始化各项属性值

    //实现撤回至作业列表功能
    connect(ui->withdrawBtn, &QPushButton::clicked, [=](){
        emit this->withdraw();
    });
}

int UnfinishedHomeworkPage::getPageNo() {
    return this->pageNo;
}

UnfinishedHomeworkPage::~UnfinishedHomeworkPage()
{
    delete ui;
}
