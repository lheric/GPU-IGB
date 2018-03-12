#include "pluginpropertytabs.h"
#include <QDebug>
PluginPropertyTabs::PluginPropertyTabs(QWidget *parent):
    QTabWidget(parent)

{
}

PluginPropertyTabs::~PluginPropertyTabs()
{
}

void PluginPropertyTabs::tabInserted(int index)
{
    QTabWidget::tabInserted(index);
}
