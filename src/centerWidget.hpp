#pragma once
#include<QWidget>
#include<QGridLayout>
#include<QPushButton>
#include"imageList.hpp"
#include"prepareImage.hpp"
#include<QDebug>
#include<QFileDialog>
#include"topButtonBar.hpp"
#include"intrinsicBoard.hpp"
#include<QPixmap>

//中心的控件，本来是只使用一个普通的wideget就可以了，现在又要添加新的东西了
class CenterWidget : public QWidget
{
    Q_OBJECT
public:
    //图片的包含列表
    ImageListContainer* listContainer;
    //内参输入框
    IntrinsicBoard* intrinsic;

    //顶部的按钮栏
    TopButtonBar* buttonBar;

    CenterWidget(QWidget* parent=nullptr);

    //发送生成DOM结束的信息
    void sendGenerateEnd();

    //发送一个消息，产生一个提示框
    void sendMessageBoxMessage(QString title,QString content);

    //记录正射影像的结果 这个函数是给非主线程来调用的
    //因为QT的界面操作只能由主线程来处理，所以需要给主线程和非主线程之间做一个桥梁
    void recordDomResult(QPixmap pixelMap);

signals:
    //生成正射影像结束的消息
    void generateFinish();

    //产生消息提示框的信号 因为qt的所有UI相关的操作都需要是单线程进行的
    void messageBoxMessage(QString title,QString content);

    //回传正射影像运算结果的信号
    void resultSignal(QPixmap pixelMap);

public slots:
    //选择图片文件的按钮响应函数
    void getImageClick();

    //处理开始生成正射影像的信号
    void beginGenerate();

    //产生消息框的响应
    void messageBoxEvent(QString title,QString content);

    //处理结果的信号，需要把正射影像结果显示在窗口中，并且还要把生成正射影像的按钮重新恢复成可以使用
    void domResultEvent(QPixmap pixelMap);
};
