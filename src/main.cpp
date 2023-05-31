#include "mainwindow.h"

#include <QApplication>
#include<QWidget>
#include<QGridLayout>
#include<QPalette>
#include<QPushButton>
#include"imageList.hpp"
#include"prepareImage.hpp"
#include<QListWidget>
#include<QDebug>
#include<QTreeView>
#include<QStandardItemModel>
#include"centerWidget.hpp"
#include"extrinsicChangeBoard.hpp"
#include<unordered_map>


//设置基本的窗口布局
void setWindowLayout(MainWindow& window)
{
    //新建一个中心组件，用于添加布局信息
    auto centerWidget=new CenterWidget(&window);
    //把新建的组件设置成网格的中心组件
    window.setCentralWidget(centerWidget);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //设置网格的布局
    setWindowLayout(w);
    //设置窗口最大化显示
    w.showMaximized();
    w.show();
    return a.exec();
}
