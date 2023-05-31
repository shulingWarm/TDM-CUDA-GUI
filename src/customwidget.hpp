// customwidget.h
#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QWidget>
#include <QString>

class EditorIdMap : public QWidget
{
    Q_OBJECT

public:
    explicit EditorIdMap(QWidget *parent);

signals:
    void customSignal(const QString& message);
};

#endif // CUSTOMWIDGET_H
