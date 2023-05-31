#pragma once
#include<QString>
#include<QFileInfo>
#include<QList>
#include<QStandardItem>

//图片列表的信息
class ImageInfo
{
public:
    //图片的绝对路径
    QString imgPath;
    //图片的名称
    QString localName;
    //相机光心坐标
    float camCenter[3];
    //相机旋转矩阵
    float camRotation[9];
    //文件是否存在
    bool fileExistFlag=true;

    ImageInfo()
    {
        imgPath="";
        localName="";
        for(int i=0;i<3;++i) camCenter[i]=0;
        for(int i=0;i<9;++i) camRotation[i]=0;
    }

    //设置image的文件路径
    void setImgPath(QString imgPath)
    {
        //记录绝对路径
        this->imgPath=imgPath;
        //从绝对路径里面解析出来相对路径
        QFileInfo parsedFile(imgPath);
        //记录文件的局部名称
        localName=parsedFile.fileName();
        //如果文件不存在就把它记录成false,但暂时并不会销毁它
        if(!parsedFile.exists())
        {
            fileExistFlag=false;
        }
    }

    //把一个向量弄成string
    template<typename T>
    static QString arrayToString(T* data,unsigned length)
    {
        QString baseStr="";
        for(unsigned idData=0;idData<length;++idData)
        {
            //添加临时的数据
            baseStr+=(QString::number(data[idData]) + " ");
        }
        //返回数字叠加的结果
        return baseStr;
    }



    //从一个imageInfo里面形成项目列表的必要信息
    static QList<QStandardItem*> getRowInfoFromImage(ImageInfo& imageInfo)
    {
        //初始化新的列表
        QList<QStandardItem*> tempList;
        //记录图片的局部名称
        tempList<<new QStandardItem(imageInfo.localName);
        //记录相机光心
        tempList<<new QStandardItem(arrayToString(imageInfo.camCenter,3));
        //记录相机旋转
        tempList<<new QStandardItem(arrayToString(imageInfo.camRotation,9));
        return tempList;
    }
};
