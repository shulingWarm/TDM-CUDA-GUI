#pragma once
#include<QWidget>
#include<QHBoxLayout>
#include<QFrame>
#include<QString>
#include<QLabel>

//图片列表里面的图片项目
class ImageItem : public QWidget
{
    //标准的一个图片label
    class ItemLabel : public QLabel
    {
    public:
        ItemLabel(unsigned idRow,unsigned idCol,QString content="",QWidget* parent=nullptr) : QLabel(content,parent)
        {
            //设置左边框
            QString baseStyle=idCol==0 ? "border-left: 2px solid black;" :
                                         "border-left: none;";
            //上边框
            if(idRow==0)
            {
                baseStyle+="border-top: 2px solid black;";
            }
            else
            {
                baseStyle+="border-top: none;";
            }
            //右边框
            baseStyle+="border-right: 2px solid black;";
            //下边框
            baseStyle+="border-bottom: 2px solid black;";
            //把文本设置成带边框的
            this->setStyleSheet(baseStyle);
            this->setFixedHeight(50);
            this->setAlignment(Qt::AlignCenter);

        }
    };

public:
    ImageItem(unsigned idRow,//当前的图片信息所在的行
              QWidget* parent=nullptr) : QWidget(parent)
    {
        //设置分布方向为水平分布
        auto itemLayout=new QHBoxLayout(this);
        //控件之间没有间隔
        itemLayout->setSpacing(0);
        //设置它的方向为从左到右
        itemLayout->setDirection(QHBoxLayout::LeftToRight);
        //添加图片名称
        auto testLabel=new ItemLabel(idRow,0,"test content",this);
        itemLayout->addWidget(testLabel,0,Qt::AlignTop);
        //pose的信息
        auto poseInfo=new ItemLabel(idRow,1,"位姿信息",this);
        itemLayout->addWidget(poseInfo,0,Qt::AlignTop);
    }
};
