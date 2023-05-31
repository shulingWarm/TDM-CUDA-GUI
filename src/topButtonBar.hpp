#pragma once
#include<QWidget>
#include<QPushButton>
#include<QHBoxLayout>
#include"imageInfo.hpp"
#include<vector>

//按钮的列表 最上面的添加图片的按钮，会另外添加一个从文本载入的按钮
class TopButtonBar : public QWidget
{
    Q_OBJECT
public:

    //载入图片用的button
    QPushButton* loadImgButton;

    //点云数据，因为后面这个数据要参与传播，为了提高一些掌控感，这里还是用自己熟悉的stl
    std::vector<float> pointcloud;

    //用于启动生成正射影像的按钮，最开始的时候是没有它的
    QPushButton* beginButton;

    //把按钮设置成适当的大小
    static void setButtonFitSize(QPushButton* button);

    //从文本里面读取图片信息的列表
    void readImgInfoListFromFile(QList<ImageInfo>& infoList,
                const QString& filePath);

    TopButtonBar(QWidget* parent=nullptr);

signals:
    void imageInfoListMessage(QList<ImageInfo> imgInfoList);
    //开始生成正射影像的信号
    void generateMessage();

public slots:
    //从文本载入相机pose的响应函数
    void loadFromTextFile();

    //载入点云的响应函数
    void loadPointcloud();

    //开始生成正射影像的按钮
    void beginGenerate();
};
