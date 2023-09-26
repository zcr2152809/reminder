#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTimeEdit>
#include <QDebug>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVector>
#include "finishedhomeworkpage.h"
#include "unfinishedhomeworkpage.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /**移除无用的第一页**********************************/
    ui->homeworkToolBox->removeItem(0);
    ui->finishedhomeworkToolBox->removeItem(0);
    ui->unfinishedhomeworkToolBox->removeItem(0);


    /**设置按钮的状态********************************/
    this->setBtnState();


    /**加载文件信息*************************************/
    //尝试连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("reminder.db");
    if (!db.open()) {
        qWarning("连接数据库失败");
        exit(-1);
    }
    //如果没有作业信息表，则创建
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS homework (id INTEGER PRIMARY KEY, title TEXT, content TEXT, deadline DATETIME, finishTime DATETIME, state INT)");


    /**启动剩余时间************************************/
    this->leftTimer = new QTimer;
    this->leftTimer->start(3000);


    /**初始化界面**************************************/
    //初始化未截止作业界面
    this->initHomeworkList();

    //初始化已完成作业界面
    this->initFinishList();

    //初始化未完成作业页面
    this->initUnfinishList();


    /**以下是作业栏样式的安排和功能的实现******************/
    //实现添加作业
    connect(ui->newHomework, &QPushButton::clicked, [=](){
        HomeworkPage *homeworkPage = new HomeworkPage;
        homeworkPage->changeTimeLeft();//初始化剩余时间
        homeworkPage->stopConfirm();//开始时禁用确认按钮
        ui->homeworkToolBox->addItem(homeworkPage, "新作业");
        ui->homeworkToolBox->setCurrentIndex(ui->homeworkToolBox->count() - 1);//打开当前页
        //添加功能
        this->singlePageFunction(homeworkPage);
    });
    //实现删除作业
    connect(ui->deleteHomework, &QPushButton::clicked, [=](){
        int currentIndex = ui->homeworkToolBox->currentIndex();
        QWidget *currentPage = ui->homeworkToolBox->currentWidget();
        HomeworkPage *tmpPage = (HomeworkPage *)(currentPage);
        if (tmpPage->getPageNo() != -1) {//如果在数据库内有这条数据，则在数据库内删除
            QSqlQuery query;
            query.prepare("DELETE FROM homework WHERE id = :id");
            query.bindValue(":id", tmpPage->getPageNo());//准备删除语句
            if (query.exec()) {
                qDebug() << "Delete successful!";
            } else {
                qDebug() << "Delete failed:" << query.lastError().text();
            }//执行删除
        }
        if (currentIndex > 0) {
            ui->homeworkToolBox->setCurrentIndex(currentIndex - 1);
        }//确定删除后显示哪一页
        ui->homeworkToolBox->removeItem(currentIndex);//在界面上删除
    });
    //实现编辑作业
    connect(ui->editHomework, &QPushButton::clicked, [=](){
        QWidget *currentPage = ui->homeworkToolBox->currentWidget();
        HomeworkPage *homeworkPage = (HomeworkPage *)(currentPage);
        if (homeworkPage->getState() == 0) {//如果在作业队列里才能启用编辑
            QDateTimeEdit *ddlEdit = currentPage->findChild<QDateTimeEdit *>();
            ddlEdit->setEnabled(true);//启用编辑截止时间
            QPushButton *confirmFinish = currentPage->findChild<QPushButton *>("confirmFinish");
            confirmFinish->setEnabled(false);
            QPushButton *confirmUnfinish = currentPage->findChild<QPushButton *>("confirmUnfinish");
            confirmUnfinish->setEnabled(false);//禁用确认完成和确认未完成按钮
        }
    });


    /**实现菜单功能*************************************/
    //实现退出功能
    connect(ui->actionexit, &QAction::triggered, [=](){
        this->close();
    });

    //实现全部保存并刷新功能
    connect(ui->actionsaveAndFlush, &QAction::triggered, [=](){
        //全部保存
        int pageCount = ui->homeworkToolBox->count();
        for (int i = 0; i < pageCount; ++i) {
            QWidget *page = ui->homeworkToolBox->widget(i);
            HomeworkPage *homeworkPage = (HomeworkPage *)(page);
            homeworkPage->emitSave();
        }
        //停止时钟
        this->leftTimer->stop();
        this->leftTimer->deleteLater();
        //取消一些连接
        disconnect(ui->homeworkToolBox, &QToolBox::currentChanged, 0, 0);
        //刷新
        this->leftTimer = new QTimer;
        this->leftTimer->start(3000);
        this->initHomeworkList();
        this->initFinishList();
        this->initUnfinishList();
        //重新建立连接
        this->setBtnState();
    });
}

void MainWindow::initHomeworkList() {
    int pageCount = ui->homeworkToolBox->count();
    for (int i = 0; i < pageCount; ++i) {
        ui->homeworkToolBox->widget(0)->deleteLater();
        ui->homeworkToolBox->removeItem(0);
    }//移除原有页
    QSqlQuery query;
    query.exec("SELECT * FROM homework");
    QVector<qlonglong> leftTimes;//维护一个存储剩余时间的数组，便于排序
    while (query.next()) {
        int state = query.value(5).toInt();//获取作业状态
        if (state == 0) {//如果作业状态为在作业列表中，则添加页
            HomeworkPage *homeworkPage = new HomeworkPage;
            homeworkPage->changeTimeLeft();//初始化剩余时间
            homeworkPage->stopEdit();
            homeworkPage->setPageNo(query.value(0).toInt());
            homeworkPage->setTitle(query.value(1).toString());
            homeworkPage->setContent(query.value(2).toString());
            homeworkPage->setDDL(query.value(3).toDateTime());//初始化页的各种属性
            //判断该页面应当插入到哪个位置
            bool flag = 0;
            qlonglong leftTime = query.value(3).toDateTime().toSecsSinceEpoch() - QDateTime::currentDateTime().toSecsSinceEpoch();
            for (int i = 0; i < leftTimes.count(); ++i) {
                if (leftTimes[i] > leftTime) {
                    leftTimes.insert(i, leftTime);
                    ui->homeworkToolBox->insertItem(i, homeworkPage, query.value(1).toString());//将页添加入未截止的作业栏
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) {
                leftTimes.push_back(leftTime);
                ui->homeworkToolBox->addItem(homeworkPage, query.value(1).toString());//将页添加入未截止的作业栏
            }
            //添加功能
            this->singlePageFunction(homeworkPage);
        }
    }
    ui->homeworkToolBox->setCurrentIndex(0);//设置打开的为最上面的一页
}

void MainWindow::initFinishList() {
    int pageCount = ui->finishedhomeworkToolBox->count();
    for (int i = 0; i < pageCount; ++i) {
        ui->finishedhomeworkToolBox->widget(0)->deleteLater();
        ui->finishedhomeworkToolBox->removeItem(0);
    }//移除原有页
    QSqlQuery query;
    query.exec("SELECT * FROM homework");
    QVector<qlonglong> finishTimes;//维护一个存储完成时间的数组，便于排序
    while (query.next()) {
        int state = query.value(5).toInt();//获取作业状态
        if (state == 1) {//如果作业状态为已完成，则添加作业
            finishedhomeworkPage *finishedhomeworkpage = new finishedhomeworkPage(query.value(0).toInt(), query.value(1).toString(), query.value(2).toString(), query.value(3).toDateTime(), query.value(4).toDateTime());
            //判断该页面应当插入到哪个位置
            bool flag = 0;
            qlonglong finishTime = query.value(4).toDateTime().toSecsSinceEpoch();
            for (int i = 0; i < finishTimes.count(); ++i) {
                if (finishTimes[i] < finishTime) {
                    finishTimes.insert(i, finishTime);
                    ui->finishedhomeworkToolBox->insertItem(i, finishedhomeworkpage, query.value(1).toString());//将页添加入完成作业栏
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) {
                finishTimes.push_back(finishTime);
                ui->finishedhomeworkToolBox->addItem(finishedhomeworkpage, query.value(1).toString());//将页添加入完成作业栏
            }
            //撤回至作业列表
            connect(finishedhomeworkpage, &finishedhomeworkPage::withdraw, [=](){
                QSqlQuery query;
                query.prepare("UPDATE homework SET state = :state WHERE id = :id");
                query.bindValue(":id", finishedhomeworkpage->getPageNo());
                query.bindValue(":state", 0);//准备更新语句
                if (query.exec()) {
                    qDebug() << "Update successful!";
                } else {
                    qDebug() << "Update failed:" << query.lastError().text();
                }//在数据库中更新状态
                this->leftTimer->stop();
                delete this->leftTimer;
                this->leftTimer = new QTimer;
                this->leftTimer->start(3000);
                //取消一些连接
                disconnect(ui->homeworkToolBox, &QToolBox::currentChanged, 0, 0);
                this->initFinishList();
                this->initHomeworkList();//刷新页面
                //重新建立连接
                this->setBtnState();
            });
            //刷新时断开所有连接
            connect(ui->actionsaveAndFlush, &QAction::triggered, [=](){
                disconnect(finishedhomeworkpage);
            });
        }
    }
}

void MainWindow::initUnfinishList() {
    int pageCount = ui->unfinishedhomeworkToolBox->count();
    for (int i = 0; i < pageCount; ++i) {
        ui->finishedhomeworkToolBox->widget(0)->deleteLater();
        ui->unfinishedhomeworkToolBox->removeItem(0);
    }//移除原有页
    QSqlQuery query;
    query.exec("SELECT * FROM homework");
    QVector<qlonglong> unfinishTimes;//维护一个存储完成时间的数组，便于排序
    while (query.next()) {
        int state = query.value(5).toInt();//获取作业状态
        if (state == 2) {//如果作业状态为未完成，则添加作业
            UnfinishedHomeworkPage *unfinishedHomeworkPage = new UnfinishedHomeworkPage(query.value(0).toInt(), query.value(1).toString(), query.value(2).toString(), query.value(3).toDateTime());
            //判断该页面应当插入到哪个位置
            bool flag = 0;
            qlonglong unfinishTime = query.value(4).toDateTime().toSecsSinceEpoch();
            for (int i = 0; i < unfinishTimes.count(); ++i) {
                if (unfinishTimes[i] < unfinishTime) {
                    unfinishTimes.insert(i, unfinishTime);
                    ui->unfinishedhomeworkToolBox->insertItem(i, unfinishedHomeworkPage, query.value(1).toString());//将页添加入完成作业栏
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) {
                unfinishTimes.push_back(unfinishTime);
                ui->unfinishedhomeworkToolBox->addItem(unfinishedHomeworkPage, query.value(1).toString());//将页添加入完成作业栏
            }
            //撤回至作业列表
            connect(unfinishedHomeworkPage, &UnfinishedHomeworkPage::withdraw, [=](){
                QSqlQuery query;
                query.prepare("UPDATE homework SET state = :state WHERE id = :id");
                query.bindValue(":id", unfinishedHomeworkPage->getPageNo());
                query.bindValue(":state", 0);//准备更新语句
                if (query.exec()) {
                    qDebug() << "Update successful!";
                } else {
                    qDebug() << "Update failed:" << query.lastError().text();
                }//在数据库中更新状态
                this->leftTimer->stop();
                delete this->leftTimer;
                this->leftTimer = new QTimer;
                this->leftTimer->start(3000);
                //取消一些连接
                disconnect(ui->homeworkToolBox, &QToolBox::currentChanged, 0, 0);
                this->initUnfinishList();
                this->initHomeworkList();//刷新页面
                //重新建立连接
                this->setBtnState();
            });
            //刷新时断开所有连接
            connect(ui->actionsaveAndFlush, &QAction::triggered, [=](){
                disconnect(unfinishedHomeworkPage);
            });
        }
    }
}

void MainWindow::singlePageFunction(HomeworkPage *homeworkPage) {
    //实现保存功能
    connect(homeworkPage, &HomeworkPage::save, [=](){
        int pageIndex = ui->homeworkToolBox->indexOf(homeworkPage);
        ui->homeworkToolBox->setItemText(pageIndex, homeworkPage->getTitle());
        if (homeworkPage->getPageNo() == -1) {//如果是刚创建的页，则插入
            QSqlQuery query;
            query.prepare("INSERT INTO homework (title, content, deadline, state) VALUES (:title, :content, :deadline, :state)");
            query.bindValue(":title", homeworkPage->getTitle());
            query.bindValue(":content", homeworkPage->getContent());
            query.bindValue(":deadline", homeworkPage->getDDL());
            query.bindValue(":state", homeworkPage->getState());//准备插入语句
            if (query.exec()) {
                int lastInsertedId = query.lastInsertId().toInt();
                homeworkPage->setPageNo(lastInsertedId);
                qDebug() << "Last Inserted ID:" << lastInsertedId;
            } else {
                qDebug() << "Insert failed:" << query.lastError().text();
            }//执行插入，并赋予新建页唯一编号
        }
        else {//否则，则更新
            QSqlQuery query;
            query.prepare("UPDATE homework SET title = :title, content = :content, deadline = :deadline, state = :state WHERE id = :id");
            query.bindValue(":id", homeworkPage->getPageNo());
            query.bindValue(":title", homeworkPage->getTitle());
            query.bindValue(":content", homeworkPage->getContent());
            query.bindValue(":deadline", homeworkPage->getDDL());
            query.bindValue(":state", homeworkPage->getState());//准备更新语句
            if (query.exec()) {
                qDebug() << "Update successful!";
            } else {
                qDebug() << "Update failed:" << query.lastError().text();
            }//执行更新
        }
    });
    //实现时间的更新
    connect(leftTimer, &QTimer::timeout, [=](){
        homeworkPage->changeTimeLeft();//更新剩余时间
    });
    //实现确认已完成功能
    connect(homeworkPage, &HomeworkPage::confirmFinish, [=](){
        QSqlQuery query;
        query.prepare("UPDATE homework SET title = :title, content = :content, deadline = :deadline, finishTime = :finishTime, state = :state WHERE id = :id");
        query.bindValue(":id", homeworkPage->getPageNo());
        query.bindValue(":title", homeworkPage->getTitle());
        query.bindValue(":content", homeworkPage->getContent());
        query.bindValue(":deadline", homeworkPage->getDDL());
        query.bindValue(":finishTime", QDateTime::currentDateTime());
        query.bindValue(":state", homeworkPage->getState());//准备更新语句
        if (query.exec()) {
            qDebug() << "Update successful!";
        } else {
            qDebug() << "Update failed:" << query.lastError().text();
        }//在数据库中更新状态
        ui->editHomework->setEnabled(false);
        ui->deleteHomework->setEnabled(false);//禁用按钮
    });
    //实现确认未完成功能
    connect(homeworkPage, &HomeworkPage::confirmUnfinish, [=](){
        QSqlQuery query;
        query.prepare("UPDATE homework SET title = :title, content = :content, deadline = :deadline, finishTime = :finishTime, state = :state WHERE id = :id");
        query.bindValue(":id", homeworkPage->getPageNo());
        query.bindValue(":title", homeworkPage->getTitle());
        query.bindValue(":content", homeworkPage->getContent());
        query.bindValue(":deadline", homeworkPage->getDDL());
        query.bindValue(":finishTime", QDateTime::currentDateTime());
        query.bindValue(":state", homeworkPage->getState());//准备更新语句
        if (query.exec()) {
            qDebug() << "Update successful!";
        } else {
            qDebug() << "Update failed:" << query.lastError().text();
        }//在数据库中更新状态
        ui->editHomework->setEnabled(false);
        ui->deleteHomework->setEnabled(false);//禁用按钮
    });
    //在刷新时取消一切连接
    connect(ui->actionsaveAndFlush, &QAction::triggered, [=](){
        disconnect(homeworkPage);
    });
}

void MainWindow::setBtnState() {
    //初始状态
    int currentIndex = ui->homeworkToolBox->currentIndex();
    if (currentIndex >= 0) {
        ui->editHomework->setEnabled(true);
        ui->deleteHomework->setEnabled(true);
    }
    else {
        ui->editHomework->setEnabled(false);
        ui->deleteHomework->setEnabled(false);
    }
    //状态切换
    connect(ui->homeworkToolBox, &QToolBox::currentChanged, [=](){
        QWidget *currentPage = ui->homeworkToolBox->currentWidget();
        HomeworkPage *homeworkPage = (HomeworkPage *)(currentPage);
        if (homeworkPage->getState() == 0) {
            ui->editHomework->setEnabled(true);
            ui->deleteHomework->setEnabled(true);
        }
        else {
            ui->editHomework->setEnabled(false);
            ui->deleteHomework->setEnabled(false);
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

