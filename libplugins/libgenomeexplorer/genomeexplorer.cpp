#include "genomeexplorer.h"
#include "model/graymatter.h"
#include "model/subject.h"
#include <QDebug>


GenomeExplorer::GenomeExplorer(QObject *parent)
{
    Q_UNUSED(parent);

    setName("Genome Explorer");
    m_pcConfigPanel = new GenomeConfigPanel(this);


}

GenomeExplorer::~GenomeExplorer()
{
}

bool GenomeExplorer::init()
{
    /// create genome explorer chart window
    m_pcCharts = new GenomeExplorerMainwin();
    m_pcCharts->attach(getContext()->pcEventBus);

    /// add it to mainwin
    QSplitter* pcSplitter = getContext()->pcCenterSplitter;
    pcSplitter->addWidget(m_pcCharts);

    /// add property panel to mainwin
    m_pcConfigPanel->attach(getContext()->pcEventBus);
    getContext()->pcPluginPropTabs->addTab(m_pcConfigPanel, this->getName());

    /// subscribe to siganls
    GrayMatter& rcGM = getContext()->pcSubject->getGrayMat();
    this->connect(&rcGM.getIntensityVol(), SIGNAL(updated()), this, SLOT(onIntensityVolumeUpdated()));
    this->connect(&rcGM.getLabelNames(), SIGNAL(updated()), this, SLOT(onLabelsUpdated()));


    return true;
}

bool GenomeExplorer::uninit()
{
    getContext()->pcCenterSplitter->hide();
    delete m_pcCharts;
    return true;
}

bool GenomeExplorer::redraw()
{
    return true;
}



bool GenomeExplorer::onEnabled()
{
    m_pcCharts->show();
    return true;
}

bool GenomeExplorer::onDisabled()
{

    m_pcCharts->hide();
    return true;
}

void GenomeExplorer::onIntensityVolumeUpdated()
{
    this->m_pcCharts->setBGData(getContext()->pcSubject->getGrayMat().getIntensityVol().getVolume().GetPointer());
}

void GenomeExplorer::onLabelsUpdated()
{

}


