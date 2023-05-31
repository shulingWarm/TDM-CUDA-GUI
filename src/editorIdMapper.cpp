#include"editorIdMapper.hpp"

EditorIdMap::EditorIdMap(QWidget* parent) : QWidget(parent)
{

}

//从单个的string到连带id的映射
void EditorIdMap::sendStringWithId(const QString& text)
{
    //获取当前的发送者
    auto sender=QWidget::sender();
    //默认的未定义的id
    unsigned id=UNDEFINED_ID;
    //判断是否记录过它的id
    if(widgetIdMap.count(sender))
    {
        id=widgetIdMap.at(sender);
    }
    //发送对应的消息
    emit strIdMessage(text,id);
}

//单独发送一个id
void EditorIdMap::sendId()
{
    //获取当前的发送者
    auto sender=QWidget::sender();
    //默认的未定义的id
    unsigned id=UNDEFINED_ID;
    //判断是否记录过它的id
    if(widgetIdMap.count(sender))
    {
        id=widgetIdMap.at(sender);
    }
    //发送对应的消息
    emit idMessage(id);
}

//记录新的id
void EditorIdMap::recordId(QObject* obj,unsigned id)
{
    //直接记录对应的id
    widgetIdMap[obj]=id;
}
