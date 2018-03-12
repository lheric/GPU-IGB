#ifndef GMISORENDERER_H
#define GMISORENDERER_H
#include "gitldef.h"
#include "model/renderengine/abstractpluginrenderer.h"
#include "genomeexplorermainwin.h"
#include "genomeconfigpanel.h"
#include "model/parcellationlabels.h"


class GenomeExplorer : public QObject, public AbstractPluginRenderer
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PLUGIN_RENDERER_IID)
    Q_INTERFACES(AbstractPluginRenderer)

public:
    explicit GenomeExplorer(QObject *parent = 0);
    ~GenomeExplorer();

    virtual bool init();

    virtual bool uninit();

    virtual bool redraw();

    virtual bool onEnabled();

    virtual bool onDisabled();

    ADD_CLASS_FIELD(GenomeExplorerMainwin*, pcCharts, getCharts, setCharts)
    ADD_CLASS_FIELD_PRIVATE(GenomeConfigPanel*, pcConfigPanel)

signals:

public slots:
    void onIntensityVolumeUpdated();
    void onLabelsUpdated();

};

#endif // GMISORENDERER_H
