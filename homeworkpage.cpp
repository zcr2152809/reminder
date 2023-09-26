#include "homeworkpage.h"
#include "ui_homeworkpage.h"
#include <QDateTime>

HomeworkPage::HomeworkPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeworkPage)
{
    ui->setupUi(this);

    //设置placeholder
    ui->titleEdit->setPlaceholderText("请输入作业名称");
    ui->contentEdit->setPlaceholderText("请输入作业内容");

    //设置初始截止日期和剩余天数
    ui->ddlEdit->setDateTime(QDateTime::currentDateTime());

    //设置保存按钮
    connect(ui->saveBtn, &QPushButton::clicked, [=](){
        emit this->save();//触发保存信号
    });
    connect(this, &HomeworkPage::save, [=](){
        ui->ddlEdit->setEnabled(false);//禁用编辑截止时间
        ui->confirmFinish->setEnabled(true);
        ui->confirmUnfinish->setEnabled(true);//启用确认完成按钮和确认未完成按钮
    });

    //根据ddl计算剩余时间
    connect(ui->ddlEdit, &QDateEdit::dateTimeChanged, [=](){
        this->changeTimeLeft();
    });

    //设置确认完成按钮
    connect(ui->confirmFinish, &QPushButton::clicked, [=](){
        this->state = 1;//设置状态为已完成
        ui->titleEdit->setEnabled(false);
        ui->contentEdit->setEnabled(false);
        ui->saveBtn->setEnabled(false);
        ui->confirmFinish->setEnabled(false);
        ui->confirmUnfinish->setEnabled(false);//禁用该页所有编辑
        emit this->confirmFinish();//触发确认完成信号
    });

    //设置确认未完成按钮
    connect(ui->confirmUnfinish, &QPushButton::clicked, [=](){
        this->state = 2;//设置状态为未完成
        ui->titleEdit->setEnabled(false);
        ui->contentEdit->setEnabled(false);
        ui->saveBtn->setEnabled(false);
        ui->confirmFinish->setEnabled(false);
        ui->confirmUnfinish->setEnabled(false);//禁用该页所有编辑
        emit this->confirmUnfinish();//触发确认未完成信号
    });
}

int HomeworkPage::getPageNo() {
    return this->pageNo;
}

void HomeworkPage::setPageNo(int pageNo) {
    this->pageNo = pageNo;
}

QString HomeworkPage::getTitle() {
    return ui->titleEdit->text();
}

void HomeworkPage::setTitle(QString title) {
    ui->titleEdit->setText(title);
}

QString HomeworkPage::getContent() {
    return  ui->contentEdit->toPlainText();
}

void HomeworkPage::setContent(QString content) {
    ui->contentEdit->setPlainText(content);
}

QDateTime HomeworkPage::getDDL() {
    return ui->ddlEdit->dateTime();
}

void HomeworkPage::setDDL(QDateTime ddl) {
    ui->ddlEdit->setDateTime(ddl);
}

int HomeworkPage::getState() {
    return this->state;
}

void HomeworkPage::setState(int state) {
    this->state = state;
}

void HomeworkPage::fireEdit() {
    ui->ddlEdit->setEnabled(true);
}

void HomeworkPage::stopEdit() {
    ui->ddlEdit->setEnabled(false);
}

void HomeworkPage::fireConfirm() {
    ui->confirmFinish->setEnabled(true);
    ui->confirmUnfinish->setEnabled(true);
}

void HomeworkPage::stopConfirm() {
    ui->confirmFinish->setEnabled(false);
    ui->confirmUnfinish->setEnabled(false);
}

void HomeworkPage::changeTimeLeft() {
    int currentTime = QDateTime::currentDateTime().toSecsSinceEpoch();//当前时间
    int ddlTime = ui->ddlEdit->dateTime().toSecsSinceEpoch();//截止日期
    int days = (ddlTime - currentTime) / 60 / 60 / 24;//计算还有多少天
    int hours = (ddlTime - currentTime) / 60 / 60 - days * 24;//计算还有多少小时
    int minute = (ddlTime - currentTime) / 60 - days * 24 * 60 - hours * 60;//计算还有多少分钟
    ui->dayShow->display(days);
    ui->hourShow->display(hours);
    ui->minuteShow->display(minute);//显示
}

void HomeworkPage::emitSave() {
    emit save();
}

HomeworkPage::~HomeworkPage()
{
    delete ui;
}
