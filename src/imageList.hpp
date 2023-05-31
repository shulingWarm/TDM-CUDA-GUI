#pragma once
#include<QWidget>
#include<QVBoxLayout>
#include"imageItem.hpp"
#include<QTreeView>
#include<QStandardItemModel>
#include<QFileInfo>
#include<QDebug>
#include"imageInfo.hpp"
#include<QMessageBox>

//pose列表的操作 每个都包括了图片的路径和对应的相机光心
class ImageListWidget : public QTreeView
{
public:

    //列表的根项目
    QStandardItem* rootItem;
    //已经添加过的图片路径
    QList<ImageInfo> addedImgList;
    //treeview的模型
    QStandardItemModel* itemModel;

    //一个图片项目里涉及到的信息
    QList<QStandardItem*> getRowInfo(QString imgPath,QString camCenter,QString camRotation)
    {
        return QList<QStandardItem*>()<<new QStandardItem(imgPath)<<
                                        new QStandardItem(camCenter)<<
                                        new QStandardItem(camRotation);
    }

    //构造的时候初始化自己的内容
    ImageListWidget(QWidget* parent=nullptr) : QTreeView(parent)
    {
        //设置根项目是不可编辑的
        this->setEditTriggers(QAbstractItemView::NoEditTriggers);
        //设置字体大小
        this->setStyleSheet("font-size: 22px;");
        //qt的项目模型，目前不知道是做什么用的
        itemModel=new QStandardItemModel;
        this->setModel(itemModel);
        //水平信息的列表
        auto infoHeader=(QStringList()<<"图片名称"<<"相机光心"<<"旋转矩阵");
        //设置每一列的信息标题
        itemModel->setHorizontalHeaderLabels(infoHeader);
        //设置列表的根项
        rootItem=itemModel->invisibleRootItem();
        //设置每一列的宽度
        this->setColumnWidth(0,200);
        this->setColumnWidth(1,400);
    }

    //向列表里面添加一个新的图片
    void addNewImage(QString newImgPath)
    {
        //新建一个图片信息
        ImageInfo tempInfo;
        //设置图片所在的路径
        tempInfo.setImgPath(newImgPath);
        //记录刚刚添加过的图片
        addedImgList.append(tempInfo);
        //在根节点里面添加新的项目
        rootItem->appendRow(ImageInfo::getRowInfoFromImage(tempInfo));
    }

public slots:
    //添加图片列表的响应函数，会接收一个自定义的信号
    void receiveImageList(QList<ImageInfo> infoList)
    {
        //读取失败的警告信息
        QString failedImgList="";
        //遍历所有要添加的列表
        for(unsigned idImage =0;idImage<infoList.size();++idImage)
        {
            //当前位置的图片
            auto& currImage=infoList[idImage];
            //查看图片是否正常加载过
            if(currImage.fileExistFlag==false)
            {
                //记录下来它是一个失败的图片
                failedImgList+=(currImage.localName + "\n");
            }
            else
            {
                //记录新的图片数据
                addedImgList.append(currImage);
                rootItem->appendRow(ImageInfo::getRowInfoFromImage(currImage));
            }
        }
        //如果有读取失败的图片，就记录一下异常
        if(failedImgList.size()>0)
        {
            QMessageBox::information(nullptr,"以下图片无法读取",failedImgList);
        }
    }
};
