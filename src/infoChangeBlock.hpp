#pragma once
#include<QWidget>
#include<QVBoxLayout>
#include<QLabel>
#include<QString>
#include<QPixmap>
#include"extrinsicChangeBoard.hpp"
#include<QFrame>
#include<QDebug>

class InfoChangeBlock : public QWidget
{
public:
    //用于显示图片的label
    QLabel* imageLabel;
    //外参的修改框
    ExtrinsicChangeBoard* changeBoard;
    //目前载入的大图
    QPixmap totalImage;

    InfoChangeBlock(QWidget* parent=nullptr) : QWidget(parent)
    {
        //设置它整体上是一个垂直的分布，下面是输入框，再往下是输入的矩阵
        auto boxLayout=new QVBoxLayout(this);
        //设置layout的方向
        boxLayout->setDirection(QVBoxLayout::TopToBottom);
        //准备显示图片的label
        imageLabel=new QLabel(this);
        //设置它的对齐方式为中心对齐
        imageLabel->setAlignment(Qt::AlignCenter);
        //设置内容的自动缩放
        imageLabel->setScaledContents(false);
        //把图片加到layout里面 要想让图片的大小跟随窗口的大小变化，这一句是关键
        //但这一句目前只能变大不能变小
        boxLayout->addWidget(imageLabel,Qt::AlignCenter);
        //当前相机光心的修改框
        changeBoard = new ExtrinsicChangeBoard(this);
        //把修改框添加到layout里面
        boxLayout->addWidget(changeBoard,Qt::AlignCenter);
        //初始的时候把控件设为不可见
        changeBoard->setVisible(false);
    }

    //更新图片的显示
    void updateImage(QString imgPath)
    {
        totalImage.load(imgPath);
        //对图片做适当的缩放
        auto tempImage=totalImage.scaled(imageLabel->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        //加载点击的图片显示在右面
        imageLabel->setPixmap(tempImage);
        //把输入框设置为可见
        changeBoard->setVisible(true);
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        //更新label的大小
        QWidget::resizeEvent(event);
        //判断图片是否载入过
        if(totalImage.isNull()==false)
        {
            //重新修改大小
            auto tempImage=totalImage.scaled(imageLabel->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
            //加载点击的图片显示在右面
            imageLabel->setPixmap(tempImage);
        }
    }
};
