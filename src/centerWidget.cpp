#include"centerWidget.hpp"
#include<thread>
#include"zmqServer.hpp"
#include<QImage>
#include<vector>
#include<algorithm>
#include"imageWindow.hpp"

CenterWidget::CenterWidget(QWidget* parent) : QWidget(parent)
{
    //新建一个layout用于表示这个网格单元的布局
    auto layout=new QGridLayout(this);
    //添加顶部的按钮列表
    buttonBar=new TopButtonBar(this);
    //链接载入图片的按钮
    connect(buttonBar->loadImgButton,&QPushButton::clicked,this,&CenterWidget::getImageClick);
    //把它放在网格里
    layout->addWidget(buttonBar,0,0);
    //添加内参输入框
    intrinsic=new IntrinsicBoard(this);
    //放到网格里
    layout->addWidget(intrinsic,1,0);
    //添加用于查看和展示图片的控件
    listContainer=new ImageListContainer(this);
    //放在网格单元的第2行
    layout->addWidget(listContainer,2,0);
    //把按钮从文本载入出来的图片信息列表链接给显示图片列表控件的接收器
    connect(buttonBar,&TopButtonBar::imageInfoListMessage,
            listContainer->leftImgList,&ImageListWidget::receiveImageList);
    //订阅顶部按钮发送的开始生成正射影像的信号
    connect(buttonBar,&TopButtonBar::generateMessage,
            this,&CenterWidget::beginGenerate);
    //发送消息提示框的信号和槽的链接
    connect(this,&CenterWidget::messageBoxMessage,this,
            &CenterWidget::messageBoxEvent);
    //得到正射影像结果时的处理
    connect(this,&CenterWidget::resultSignal,this,
            &CenterWidget::domResultEvent);
}


//选择图片文件的按钮响应函数
void CenterWidget::getImageClick()
{
    auto fileList= QFileDialog::getOpenFileNames(this,"选择图片",QString(),"Images (*.JPG *.jpg *.jpeg *.png)");
    //在列表里面添加每个读取到的图片
    for(auto& eachFile : fileList)
    {
        listContainer->leftImgList->addNewImage(eachFile);
    }
}

//根据图片的列表生成点云的范围
void makeCenterRange(QList<ImageInfo>& infoList,
    float* range)
{
    //遍历每个相机
    for(int idImage=0;idImage<infoList.size();++idImage)
    {
        //当前位置的光心
        auto currCenter=infoList[idImage].camCenter;
        //0的时候初始化范围
        if(idImage==0)
        {
            for(int i=0;i<4;++i) range[i]=currCenter[i/2];
        }
        else
        {
            //比较范围
            if(currCenter[0]<range[0]) range[0]=currCenter[0];
            else if(currCenter[0]>range[1]) range[1]=currCenter[0];
            if(currCenter[1]<range[2]) range[2]=currCenter[1];
            else if(currCenter[1]>range[3]) range[3]=currCenter[1];
        }
    }
}

//发送生成DOM结束的信息
void CenterWidget::sendGenerateEnd()
{
    emit generateFinish();
}

//记录正射影像的结果 这个函数是给非主线程来调用的
//因为QT的界面操作只能由主线程来处理，所以需要给主线程和非主线程之间做一个桥梁
void CenterWidget::recordDomResult(QPixmap pixelMap)
{
    //把算出来的结果图通过信号发送出去，发送出去信号后，这里就会由非主线程转换为主线程的操作
    emit resultSignal(pixelMap);
}

//发送一个消息，产生一个提示框
void CenterWidget::sendMessageBoxMessage(QString title,QString content)
{
    //发送消息框的信号
    emit messageBoxMessage(title,content);
}

//从文本里面读取数据，并且传入一个读取失败的情况下的messageBox
float transNumberWithFlag(QString text,QString warningInfo,
            CenterWidget* widget)
{
    //对数据做临时的转换
    bool transFlag=true;
    float tempValue=text.toDouble(&transFlag);
    //判断是否转换成功
    if(!transFlag)
    {
        //发送信号，产生消息框
        widget->sendMessageBoxMessage("缺少信息",warningInfo);
        throw -1;
    }
    //返回计算出的结果
    return tempValue;
}

//向connection里面写入所有的图片
void writeImageInfo(QList<ImageInfo>& imageInfoList,
    ZmqConnection& connection)
{
    //遍历每个图片
    for(unsigned idImage=0;idImage<imageInfoList.size();++idImage)
    {
        //当前的图片
        auto& currImage=imageInfoList[idImage];
        //发送光心
        connection.write((char*)currImage.camCenter,sizeof(float)*3);
        //发送旋转信息
        connection.write((char*)currImage.camRotation,sizeof(float)*9);
        //图片名称的长度
        std::string strImgPath=currImage.imgPath.toStdString();
        unsigned imgPathLength=strImgPath.size();
        //记录图片长度的信息
        connection.write((char*)&imgPathLength,sizeof(unsigned));
        //写入路径数据
        connection.write((char*)strImgPath.data(),imgPathLength);
    }
}

//把一个uchar数据的bgr转换成rgb 直接在原地转换
//注意这里的dataSize是总长度，但里面的数据是3个一组的，其实是交换每一组里面的r和b的位置
void bgr2rgb(unsigned char* data,unsigned dataSize)
{
    //遍历每一组数据
    for(unsigned idData=0;idData+2<dataSize;idData+=3)
    {
        //交换两个位置的数据
        std::swap(data[idData],data[idData+2]);
    }
}

//从接收的数据流里面读取正射影像的结果
void receiveResultByBuffer(CenterWidget* widget,//传入这个是为了方便直接把结果发送出去
    std::stringstream& resultBuffer
)
{
    //读取宽高
    unsigned imgSize[2];
    resultBuffer.read((char*)imgSize,sizeof(unsigned)*2);
    //如果得到的图片是空的，就可以结束了
    if(imgSize[0]*imgSize[1]==0)
    {
        widget->sendMessageBoxMessage("错误","正射影像生成失败");
        return;
    }
    //初始化uchar数据
    std::vector<unsigned char> imgData(imgSize[0]*imgSize[1]*3);
    resultBuffer.read((char*)imgData.data(),sizeof(unsigned char)*imgData.size());
    //由于收到的是opencv的图片，所以需要把bgr形式的图片转换成rgb
    bgr2rgb(imgData.data(),imgData.size());
    //使用data数据初始化一个image
    QImage image(imgData.data(),imgSize[0],imgSize[1],
            imgSize[0]*3,QImage::Format_RGB888);
    //使用image来初始化一个图片
    QPixmap pixelMap=QPixmap::fromImage(image);
    //把图片发出去，通知它正常运行
    widget->recordDomResult(pixelMap);
}

//综合使用CenterWidget里面的消息，来生成正射影像
//这里需要单独开一个线程来调用，不能让它阻塞正常的线程进行
void callCudaGeneration(CenterWidget* widget)
{
    // 新建zmq链接器
    ZmqConnection connection(5555,true);
    //生成点云的范围
    float range[4];
    makeCenterRange(widget->listContainer->leftImgList->addedImgList,
            range);
    //写入生成范围
    connection.write((char*)range,sizeof(float)*4);
    try {
        float pixelResolution=transNumberWithFlag(
            widget->intrinsic->resolutionInput->text(),"请输入空间分辨率",widget);
        //写入空间分辨率
        connection.write((char*)&pixelResolution,sizeof(float));
        float intrinsic[3];
        //记录焦距
        intrinsic[0]=transNumberWithFlag(
            widget->intrinsic->focalInput->text(),"请输入焦距",widget);
        //记录主点x
        intrinsic[1]=transNumberWithFlag(widget->intrinsic->xInput->text(),
                "请输入主点X",widget);
        //记录主点y
        intrinsic[2]=transNumberWithFlag(widget->intrinsic->yInput->text(),
                                         "请输入主点Y",widget);
        //写入内参数据
        connection.write((char*)intrinsic,sizeof(float)*3);
        //目标点云
        auto& pointcloud=widget->buttonBar->pointcloud;
        //读取点云个数
        unsigned cloudNum=pointcloud.size()/3;
        //写入点云的个数
        connection.write((char*)&cloudNum,sizeof(unsigned));
        //如果点云的数量不够需要报警
        if(cloudNum==0)
        {
            widget->sendMessageBoxMessage("缺少输入信息","请载入点云");
            throw -1;
        }
        //发送点云数据
        connection.write((char*)pointcloud.data(),sizeof(float)*cloudNum*3);
        //获取相机的列表
        auto& imageList=widget->listContainer->leftImgList->addedImgList;
        //图片的个数
        unsigned imageNum=imageList.size();
        //如果图片的数量不够需要报警
        if(imageNum<8)
        {
            widget->sendMessageBoxMessage("缺少输入信息","请输入足够的图片");
            throw -1;
        }
        //记录图片的个数
        connection.write((char*)&imageNum,sizeof(unsigned));
        //向connection里面写入每个图片的信息
        writeImageInfo(imageList,connection);
        //调用connection准备发送了
        connection.send();
    } catch (...) {
        return;
    }
    //准备接收应答
    std::stringstream resultBuffer;
    connection.receive(resultBuffer);
    //发送数据的信号
    receiveResultByBuffer(widget,resultBuffer);
}

//产生消息框的响应
void CenterWidget::messageBoxEvent(QString title,QString content)
{
    //接收到消息后生成消息提示框
    QMessageBox::information(nullptr,title,content);
}

//处理结果的信号，需要把正射影像结果显示在窗口中，并且还要把生成正射影像的按钮重新恢复成可以使用
void CenterWidget::domResultEvent(QPixmap pixelMap)
{
    //新建一个用于显示图片的窗口
    auto imgWindow=new ImageWindow(pixelMap,this);
    //显示图片
    imgWindow->show();
    //把生成正射影像的按钮恢复成正常可用
    buttonBar->restoreGenerationButton();
}

//处理开始生成正射影像的信号
void CenterWidget::beginGenerate()
{
    std::thread generateThread(callCudaGeneration,this);
    //让它慢慢运行吧
    generateThread.detach();
}
