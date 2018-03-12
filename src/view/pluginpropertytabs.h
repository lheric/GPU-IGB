#ifndef PLUGINPROPERTYTABS_H
#define PLUGINPROPERTYTABS_H

#include <QObject>
#include <QWidget>
#include <QTabWidget>
#include "gitlview.h"

class PluginPropertyTabs: public QTabWidget, public GitlView
{
    Q_OBJECT
public:
    explicit PluginPropertyTabs(QWidget *parent = 0);
    ~PluginPropertyTabs();


protected:
    virtual void tabInserted(int index);
};

#endif // PLUGINPROPERTYTABS_H
