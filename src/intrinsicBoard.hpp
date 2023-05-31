#pragma once
#include<QWidget>
#include<QHBoxLayout>
#include<QLabel>
#include<QLineEdit>
#include<QDoubleValidator>

//内参信息的输入框
class IntrinsicBoard : public QWidget
{
public:

    QLineEdit* focalInput;
    QLineEdit* xInput;
    QLineEdit* yInput;
    QLineEdit* resolutionInput;

    IntrinsicBoard(QWidget* parent=nullptr) : QWidget(parent)
    {
        //输入框有效性判断器
        auto validJudger= new QDoubleValidator(-9999999,9999999,15,this);
        //横向分布的架构
        auto boxLayout=new QHBoxLayout(this);
        //添加焦距
        auto focalLocal=new QLabel("焦距",this);
        //设置字体大小
        auto font=focalLocal->font();
        font.setPointSize(20);
        focalLocal->setFont(font);
        focalLocal->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        boxLayout->addWidget(focalLocal,Qt::AlignCenter);
        //焦距的输入框
        focalInput=new QLineEdit(this);
        focalInput->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        focalInput->setFont(font);
        focalInput->setValidator(validJudger);
        boxLayout->addWidget(focalInput,Qt::AlignCenter);
        //主点的输入框
        auto priLabel=new QLabel("主点坐标X",this);
        priLabel->setFont(font);
        priLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        boxLayout->addWidget(priLabel,Qt::AlignCenter);
        //主点输入框的x
        xInput=new QLineEdit(this);
        xInput->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        xInput->setFont(font);
        xInput->setValidator(validJudger);
        boxLayout->addWidget(xInput,Qt::AlignCenter);
        //主点坐标y
        auto priYLabel=new QLabel("主点坐标Y",this);
        priYLabel->setFont(font);
        priYLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        boxLayout->addWidget(priYLabel,Qt::AlignCenter);
        //主点y的输入框
        yInput=new QLineEdit(this);
        yInput->setFont(font);
        yInput->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        yInput->setValidator(validJudger);
        boxLayout->addWidget(yInput,Qt::AlignCenter);
        //空间分辨率的输入框
        auto resolutionLabel=new QLabel("空间分辨率(米)",this);
        resolutionLabel->setFont(font);
        resolutionLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
        boxLayout->addWidget(resolutionLabel,Qt::AlignCenter);
        resolutionInput= new QLineEdit(this);
        resolutionInput->setFont(font);
        resolutionInput->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        boxLayout->addWidget(resolutionInput,Qt::AlignCenter);
    }
};
