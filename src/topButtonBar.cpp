#include"topButtonBar.hpp"
#include<QFileDialog>
#include<QDebug>
#include<QFile>
#include<QTextStream>
#include<QMessageBox>
#include<string>
#include<fstream>

//打开文本失败的情况
const int OPEN_FAILED = -1;
//理论上每一行应有的分段数
const int LINE_SEG_NUM = 13;

//把按钮设置成适当的大小
void TopButtonBar::setButtonFitSize(QPushButton* button)
{
    //这个按钮的理想大小
    auto idealSize=button->sizeHint();
    //让按钮的大小固定为这个大小
    button->setFixedSize(idealSize);
}

TopButtonBar::TopButtonBar(QWidget* parent) : QWidget(parent)
{
    //水平分布的布局
    auto boxLayout=new QHBoxLayout(this);
    //设置方向
    boxLayout->setDirection(QHBoxLayout::LeftToRight);
    //添加从文件载入图片的选项
    loadImgButton=new QPushButton("添加图片",this);
    //设置字体大小
    auto buttonFont=loadImgButton->font();
    buttonFont.setPointSize(22);
    loadImgButton->setFont(buttonFont);
    //把按钮设置成适当的大小
    setButtonFitSize(loadImgButton);
    //从文本载入pose的按钮
    auto buttonByText=new QPushButton("从文本载入",this);
    //设置字体
    buttonByText->setFont(buttonFont);
    //设置它的一个合适的大小
    setButtonFitSize(buttonByText);
    //把按钮添加到网格中
    boxLayout->addWidget(loadImgButton,Qt::AlignCenter);
    boxLayout->addWidget(buttonByText,Qt::AlignCenter);
    //链接从文本载入图片的按钮
    connect(buttonByText,&QPushButton::clicked,this,&TopButtonBar::loadFromTextFile);
    //读取点云的按钮
    auto loadPointButton= new QPushButton("读取点云",this);
    //设置字体
    loadPointButton->setFont(buttonFont);
    boxLayout->addWidget(loadPointButton,Qt::AlignCenter);
    setButtonFitSize(loadPointButton);
    //链接载入点云相关的逻辑
    connect(loadPointButton,&QPushButton::clicked,
            this,&TopButtonBar::loadPointcloud);
    //添加开始生成正射影像的按钮
    beginButton=new QPushButton("生成正射影像",this);
    beginButton->setFont(buttonFont);
    setButtonFitSize(beginButton);
    boxLayout->addWidget(beginButton,Qt::AlignCenter);
    //链接开始生成正射影像的按钮
    connect(beginButton,&QPushButton::clicked,
        this,&TopButtonBar::beginGenerate);
}

//从文本里面读取图片信息的列表
void TopButtonBar::readImgInfoListFromFile(QList<ImageInfo>& infoList,
            const QString& filePath)
{
    //打开文本
    QFile fileHandle(filePath);
    if(!fileHandle.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //打开文本失败的情况
        throw OPEN_FAILED;
    }
    //把文件句柄打包成输入流，感觉这还没std::fstream好用
    QTextStream fileStream(&fileHandle);
    //读取文件流的每一行
    while(!fileStream.atEnd())
    {
        //读取当前行
        auto thisLine=fileStream.readLine();
        //根据空格来分段
        auto lineSeg=thisLine.split(' ');
        //如果分段数不对，就直接结束
        if(lineSeg.size()!=LINE_SEG_NUM)
        {
            break;
        }
        //初始化一个图片信息
        ImageInfo tempInfo;
        //设置图片的路径
        tempInfo.setImgPath(lineSeg[0]);
        //读取光心
        bool toDoubleOkFlag=true;
        for(int i=0;i<3;++i)
        {
            tempInfo.camCenter[i]=lineSeg[i+1].toDouble(&toDoubleOkFlag);
            if(!toDoubleOkFlag) break;
        }
        if(toDoubleOkFlag)
        {
            //继续读取旋转信息
            for(int i=0;i<9;++i)
            {
                tempInfo.camRotation[i]=lineSeg[i+4].toDouble(&toDoubleOkFlag);
                //如果转换失败了就可以结束了
                if(!toDoubleOkFlag) break;
            }
        }
        //如果转换不正常就标记一下图片不存在
        if(toDoubleOkFlag==false)
            tempInfo.fileExistFlag=false;
        //不论是否正常读出来，都把它先放到list里面
        infoList.append(tempInfo);
    }
    //关闭文本输入流
    fileHandle.close();
}

//从文本载入相机pose的响应函数
void TopButtonBar::loadFromTextFile()
{
    //准备读取文件
    auto textFile=QFileDialog::getOpenFileName(this,"从文本载入图片",QString());
    //从文本里面读取图片的信息列表
    try {
        QList<ImageInfo> infoList;
        readImgInfoListFromFile(infoList,textFile);
        //发送出去读取到的文件列表
        emit imageInfoListMessage(infoList);
    } catch (int errorFlag) {
        //出错的情况下要打印读取错误的信息
        if(errorFlag==OPEN_FAILED)
        {
            //弹出消息框
            QMessageBox::information(nullptr,"文件读取错误",
                QString("无法打开: %1").arg(textFile));
        }
        else
        {
            //其它情况下就再向上抛
            throw errorFlag;
        }
    }
}

//载入点云的响应函数
void TopButtonBar::loadPointcloud()
{
    //准备读取点云
    auto pointFile=QFileDialog::getOpenFileName(this,"二进制点云",QString());
    //转换成std::string
    std::string stdFilePath=pointFile.toStdString();
    //读取fstream
    std::fstream fileHandle;
    fileHandle.open(stdFilePath,std::ios::in|std::ios::binary);
    //如果没有打开就报个错
    if(!fileHandle.is_open())
    {
        QMessageBox::information(nullptr,"文件读取错误",QString("无法读取%1").arg(pointFile));
        return;
    }
    //依次读取三个数字
    while(true)
    {
        //临时读取到的点
        double pointBuffer[3];
        if(fileHandle.read((char*)pointBuffer,sizeof(double)*3))
        {
            //读取成功了就把它放进点列表里面
            for(int i=0;i<3;++i) pointcloud.push_back(pointBuffer[i]);
        }
        else
        {
            break;
        }
    }
    //关闭文本流
    fileHandle.close();
}

//开始生成正射影像的按钮
void TopButtonBar::beginGenerate()
{
    //把自己的按钮设置成不可使用
    //beginButton->setEnabled(false);
    beginButton->setText("正在生成...");
    //重新调整按钮的大小
    setButtonFitSize(beginButton);
    //发送开始生成正射影像的信号
    emit generateMessage();
}
