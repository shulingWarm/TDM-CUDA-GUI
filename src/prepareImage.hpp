#pragma once
#include<QWidget>
#include<QGridLayout>
#include"imageList.hpp"
#include<QPixmap>
#include"infoChangeBlock.hpp"

//水平方向的列表，用于观察已经添加进来的图片
class ImageListContainer : public QWidget
{
public:
    QGridLayout* mainLayout;

    //左边的图片列表
    ImageListWidget* leftImgList;
    //右边显示图片的label,后面可能要单独做一个控件来实现这件事
    InfoChangeBlock* rightImgLabel;

    ImageListContainer(QWidget* parent=nullptr) : QWidget(parent)
    {
        //核心的网格，左边是放图片列表的，右边是放展示图片的区域的，自己只是个容器
        mainLayout=new QGridLayout(this);
        //设置行方向的比例
        mainLayout->setColumnStretch(0,8);
        mainLayout->setColumnStretch(1,2);
        //不同的元素之间不需要间隔
        mainLayout->setSpacing(0);
        //新建左边的图片列表，里面涉及相机位姿之类的东西
        leftImgList=new ImageListWidget(this);
        //把左边的图片列表放在网格的左边
        mainLayout->addWidget(leftImgList,0,0);
        //链接图片列表的点击事件
        connect(leftImgList,&ImageListWidget::clicked,this,&ImageListContainer::imageListClick);
        //后面要把显示图片的控件放在右边
        rightImgLabel=new InfoChangeBlock(this);
        mainLayout->addWidget(rightImgLabel,0,1);
    }

public slots:
    //获取图片列表的点击事件
    void imageListClick(const QModelIndex& index)
    {
        //获取点击到的图片的路径
        QString imgPath=leftImgList->addedImgList[index.row()].imgPath;
        //更新图片的显示
        rightImgLabel->updateImage(imgPath);
        //载入新的外参信息
        rightImgLabel->changeBoard->loadExtrinsic(
            &(leftImgList->addedImgList[index.row()]),leftImgList->itemModel,
                index.row());
    }

};
