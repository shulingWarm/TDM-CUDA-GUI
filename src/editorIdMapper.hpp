#pragma once

#include <QWidget>
#include <QString>
#include<unordered_map>

class EditorIdMap : public QWidget
{
    Q_OBJECT

public:
    explicit EditorIdMap(QWidget *parent);

private:

    //从控件到id的映射表
    std::unordered_map<QObject*,unsigned> widgetIdMap;
    //默认的未定义id
    static const unsigned UNDEFINED_ID = 9999999;
public:
    //记录新的id
    void recordId(QObject* obj,unsigned id);

signals:
    //连带string和id的消息
    void strIdMessage(QString text,int id);
    //一个单独的id消息
    void idMessage(unsigned id);

public slots:
    //从单个的string到连带id的映射
    void sendStringWithId(const QString& text);

    //单独发送一个id
    void sendId();
};
