#pragma once
#include<QWidget>
#include<QGridLayout>
#include<QStandardItemModel>
#include<QLineEdit>
#include<QLabel>
#include<QList>
#include<QString>
#include<QSignalMapper>
#include"editorIdMapper.hpp"
#include<QDebug>
#include<QDoubleValidator>
#include"imageInfo.hpp"


//修改外参的输入框
class ExtrinsicChangeBoard : public QWidget
{
    //标题的字体大小
    static const unsigned HEADER_FONT_SIZE = 15;
    //当前正在处理的图片信息
    ImageInfo* currentImage=nullptr;
    //光心的treeview显示框和旋转的显示框
    QModelIndex centerViewContent;
    QModelIndex rotationViewContent;
    //treeView的主要树干，更新的时候还是要靠它才能找到输入接口
    QStandardItemModel* itemModel;

    //相机光心的输入框
    QLineEdit* centerEditor[3];
    //旋转矩阵的输入框
    QLineEdit* rotationEditor[9];
    //可用的输入数值的有效判别器
    QDoubleValidator* validJudger;
public:

    //初始化相机光心的加入过程
    void prepareCenterUI(QGridLayout* gridLayout,EditorIdMap* mapper)
    {
        //写入相机光心信息
        auto centerLabel=new QLabel("相机光心",this);
        //设置label的字体大小
        auto tempFont=centerLabel->font();
        tempFont.setPointSize(HEADER_FONT_SIZE);
        centerLabel->setFont(tempFont);
        gridLayout->addWidget(centerLabel,0,1,Qt::AlignBottom|Qt::AlignHCenter);
        //相机光心的字符列表
        QList<QString> labelList={"X","Y","Z"};
        //添加3个label
        for(int idXyz=0;idXyz<3;++idXyz)
        {
            //添加label
            auto tempLabel=new QLabel(labelList[idXyz],this);
            //添加到网格的对应位置上
            gridLayout->addWidget(tempLabel,1,idXyz*2,Qt::AlignTop|Qt::AlignRight);
            //添加对应的输入框
            centerEditor[idXyz]=new QLineEdit(this);
            //输入框放在输入框名称的后面
            gridLayout->addWidget(centerEditor[idXyz],1,idXyz*2+1,Qt::AlignTop|Qt::AlignLeft);
            //把文本更新链接到信号响应里面
            connect(centerEditor[idXyz],&QLineEdit::editingFinished,
                    mapper,&EditorIdMap::sendId);
            //设置输入有效性判别器
            centerEditor[idXyz]->setValidator(validJudger);
            //链接当前的输入框的标号
            mapper->recordId(centerEditor[idXyz],idXyz);
        }
    }

    //初始化相机旋转的加入过程
    void prepareRotationUI(QGridLayout* gridLayout,EditorIdMap* mapper)
    {
        //写入相机旋转的信息
        auto rotationLabel=new QLabel("相机旋转",this);
        //设置相机旋转的字体大小
        auto tempFont=rotationLabel->font();
        tempFont.setPointSize(HEADER_FONT_SIZE);
        rotationLabel->setFont(tempFont);
        //添加标题内容
        gridLayout->addWidget(rotationLabel,2,1,Qt::AlignBottom|Qt::AlignHCenter);
        //相机旋转的输入框
        for(int idRow=0;idRow<3;++idRow)
        {
            for(int idCol=0;idCol<3;++idCol)
            {
                //旋转提示信息的字符串
                QString labelStr=QString("r%1%2").arg(idRow+1).arg(idCol+1);
                //旋转信息的输入label
                auto tempLabel=new QLabel(labelStr,this);
                //添加到网格中
                gridLayout->addWidget(tempLabel,3+idRow,idCol*2,Qt::AlignTop|Qt::AlignRight);
                //需要被修改的指针
                auto& targetPtr=rotationEditor[idRow*3 + idCol];
                //旋转矩阵的输入框
                targetPtr=new QLineEdit(this);
                gridLayout->addWidget(targetPtr,3+idRow,idCol*2 + 1,
                        Qt::AlignTop|Qt::AlignLeft);
                //设置有效性判别器
                targetPtr->setValidator(validJudger);
                //链接修改框的id
                connect(targetPtr,&QLineEdit::editingFinished,
                        mapper,&EditorIdMap::sendId);
                //链接当前的旋转输入框的id
                mapper->recordId(targetPtr,idRow*3 + idCol + 3);
            }
        }
    }

    //itemModel 是treeList里面的一个信息，文本框更新后就需要把这里面的数值做相应更改
    ExtrinsicChangeBoard(QWidget* parent=nullptr) : QWidget(parent)
    {
        //新建网格布局
        auto gridLayout=new QGridLayout(this);
        //id的映射器
        auto signalMapper=new EditorIdMap(this);
        //初始化有效判别器
        validJudger=new QDoubleValidator(-9999999,9999999,7,this);
        prepareCenterUI(gridLayout,signalMapper);
        //相机旋转的输入框
        prepareRotationUI(gridLayout,signalMapper);
        //把mapper发送的链接了文本和id的序号链接到具体的修改响应函数上
        connect(signalMapper,&EditorIdMap::idMessage,
                this,&ExtrinsicChangeBoard::handleTextChange);
    }

    //加载当前的旋转信息 响应旋转的时候就会直接修改这里
    void loadExtrinsic(ImageInfo* targetImage,//目标图片的指针
        QStandardItemModel* itemModel, //获取treeview的列表项全靠它
        unsigned idRow //用户点击的行数，用来获取修改的位置
    )
    {
        //记录目前要访问的图片
        currentImage=targetImage;
        //记录目前的光心信息
        centerViewContent=itemModel->index(idRow,1);
        rotationViewContent=itemModel->index(idRow,2);
        //记录获取treeView的model
        this->itemModel=itemModel;
        //设置每个显示框的数值
        for(int i=0;i<3;++i)
        {
            centerEditor[i]->setText(QString::number(targetImage->camCenter[i]));
        }
        for(int i=0;i<9;++i)
        {
            rotationEditor[i]->setText(QString::number(targetImage->camRotation[i]));
        }
    }

public slots:
    //输入框改变时的响应
    void handleTextChange(unsigned id)
    {
        //目标输入框
        QLineEdit* currentInput;
        //光心的情况
        if(id<3)
        {
            currentInput=centerEditor[id];
            currentImage->camCenter[id]=currentInput->text().toDouble();
            //更新光心的显示
            itemModel->setData(centerViewContent,QVariant(
                ImageInfo::arrayToString(currentImage->camCenter,3)));
        }
        else if(id<12)
        {
            currentInput=rotationEditor[id-3];
            //更新数值
            currentImage->camRotation[id-3]=currentInput->text().toDouble();
            //更新旋转的显示
            itemModel->setData(rotationViewContent,QVariant(
                ImageInfo::arrayToString(currentImage->camRotation,9)));
        }
    }
};
