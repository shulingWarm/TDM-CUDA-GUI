#pragma once
#include<QDialog>
#include<QLabel>
#include<QVBoxLayout>
#include<QDesktopWidget>
#include<QPushButton>
#include<QFileDialog>

//一个用于显示图片的临时窗口
class ImageWindow : public QDialog
{
public:
    //总的图片
    QPixmap totalImage;
    //用于显示图片的label
    QLabel* imageLabel;

    ImageWindow(const QPixmap& pixelMap,QWidget* parent=nullptr) : QDialog(parent)
    {
        //新建layout
        auto boxLayout=new QVBoxLayout(this);
        boxLayout->setDirection(QVBoxLayout::TopToBottom);
        //新建用于放图片的label
        imageLabel=new QLabel(this);
        //记录总的图片
        totalImage=pixelMap;
        //把窗口设置成最大化显示
        //this->showMaximized();
        //把label添加到layout里面
        boxLayout->addWidget(imageLabel,Qt::AlignCenter);
        //把label设置成中心的模式
        imageLabel->setAlignment(Qt::AlignCenter);
        //强行设置label的大小
        imageLabel->resize(1000,1000);
        //调整图片的大小
        auto scaleImg=totalImage.scaled(imageLabel->size(),
            Qt::KeepAspectRatio,Qt::SmoothTransformation);
        //添加图片
        imageLabel->setPixmap(scaleImg);
        //添加保存图片的按钮
        auto saveImgButton=new QPushButton("保存图片",this);
        //这个按钮的理想大小
        auto idealSize=saveImgButton->sizeHint();
        //让按钮的大小固定为这个大小
        saveImgButton->setFixedSize(idealSize);
        //添加到layout里面
        boxLayout->addWidget(saveImgButton,Qt::AlignHCenter);
        //链接按钮和保存图片的响应
        connect(saveImgButton,&QPushButton::clicked,this,
                &ImageWindow::saveImageResponse);
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

public slots:
    //保存图片的响应函数
    void saveImageResponse()
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"),
                                   "/home",
                                   tr("Images (*.png *.xpm *.jpg)"));
        if (!fileName.isEmpty()) {
            totalImage.save(fileName);
        }
    }
};
