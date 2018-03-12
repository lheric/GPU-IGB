#include "pluginrendererloader.h"
#include "model/rom.h"
#include "model/modellocator.h"
#include <QDebug>
#include <QDir>

#define PLUGIN_DIRECTORY "plugins" ///< plugin directory

PluginRendererLoader::PluginRendererLoader()
{
    this->m_strPluginDir = PLUGIN_DIRECTORY;
    this->setPluginComBus(GitlEventBus::create());
}

PluginRendererLoader::~PluginRendererLoader()
{
}

bool PluginRendererLoader::initAllFilters()
{
    xPrepareFilterContext();
    for(int i = 0; i < m_apcRenderers.size(); i++)
    {
        if(m_apcRenderers[i]->init() == false )
            qWarning() << QString("Plugin Filter %1 Init Failed!").arg(m_apcRenderers[i]->getName());
        m_apcRenderers[i]->setEnable(m_apcRenderers[i]->getEnable());
    }
    return true;
}

bool PluginRendererLoader::uninitAllFilters()
{
    xPrepareFilterContext();
    for(int i = 0; i < m_apcRenderers.size(); i++)
    {
        if(m_apcRenderers[i]->uninit() == false )
            qWarning() << QString("Plugin Filter %1 Uninit Failed!").arg(m_apcRenderers[i]->getName());
    }
    return true;
}




bool PluginRendererLoader::reinitAllFilters()
{
    uninitAllFilters();
    initAllFilters();
    return true;
}

bool PluginRendererLoader::reloadAllFilters()
{
    unloadAllFilters();

    xPrepareFilterContext();
    QDir cPluginsDir(m_strPluginDir);
    QStringList cFilters;
    cFilters << "*.dll" << "*.so" << "*.dylib";
    /// try to load each file as plugin in the directory
    foreach(QString strPluginFileName, cPluginsDir.entryList(cFilters, QDir::Files))
    {
        QPluginLoader* pLoader = new QPluginLoader(cPluginsDir.absoluteFilePath(strPluginFileName));
        QObject* pcClass = pLoader->instance();
        AbstractPluginRenderer *pcPlugin =  qobject_cast<AbstractPluginRenderer*>(pcClass);
        ///  loading success or fail
        if (pcPlugin)
        {
            ///< ensure filter name is unique
            if( getPluginByName(pcPlugin->getName()) == NULL )
            {
                m_apcPluginLoaders.push_back(pLoader);
                pcPlugin->setContext(&m_cPluginContext);    /// set context of this filter
                pcPlugin->attach(this->getPluginComBus());  /// attach this filter to inter communication bus
                m_apcRenderers.push_back(pcPlugin);
                qDebug() << QString("Plugin Filter %1 Loading Succeeded!").arg(strPluginFileName);
            }
            else
            {
                qWarning() << QString("Plugin Filter %1 Loading Failed!").arg(strPluginFileName);
                qWarning() << QString("Filter Name '%1' Already Existed!").arg(pcPlugin->getName());
                delete pcPlugin;
                delete pLoader;
            }
        }
        else
        {
            qWarning() << QString("Plugin Filter %1 Loading Failed!").arg(strPluginFileName);
            delete pLoader;
        }
    }

    /// sort
    xReadAndSortFilters();

    /// init
    initAllFilters();
    return true;
}


bool PluginRendererLoader::unloadAllFilters()
{
    /// uninit each filter
    for(int i = 0; i < m_apcRenderers.size(); i++)
    {
        m_apcRenderers[i]->uninit();
    }


    /// traverse the list and free all
    while( !m_apcRenderers.empty() )
    {
        AbstractPluginRenderer* pFilter = m_apcRenderers.back();
        delete pFilter;
        m_apcRenderers.pop_back();

    }

    while( !m_apcPluginLoaders.empty() )
    {
        QPluginLoader* pLoader = m_apcPluginLoaders.back();
        delete pLoader;
        m_apcPluginLoaders.pop_back();
    }

    return true;
}

bool PluginRendererLoader::redraw()
{
    // prepare filter context
    xPrepareFilterContext();

    for(int i = 0; i < m_apcRenderers.size(); i++)
    {
        AbstractPluginRenderer* pRenderer = m_apcRenderers[i];
        pRenderer->redraw();
    }
    return true;
}

bool PluginRendererLoader::mousePress(const QPointF* pcPos, Qt::MouseButton eMouseBtn)
{
    // prepare filter context
    xPrepareFilterContext();

    for(int i = 0; i < m_apcRenderers.size(); i++)
    {
        AbstractPluginRenderer* pRenderer = m_apcRenderers[i];
        if( pRenderer->getEnable() )
        {
            pRenderer->mousePress(pcPos, eMouseBtn);
        }
    }
    return true;
}

bool PluginRendererLoader::keyPress(int iKeyPressed)
{
    // prepare filter context
    xPrepareFilterContext();

    for(int i = 0; i < m_apcRenderers.size(); i++)
    {
        AbstractPluginRenderer* pFilter = m_apcRenderers[i];
        if( pFilter->getEnable() )
        {
            pFilter->keyPress(iKeyPressed);
        }
    }
    return true;
}


AbstractPluginRenderer* PluginRendererLoader::getPluginByName(const QString &strFilterName)
{
    AbstractPluginRenderer* pRenderer = NULL;
    for(int i = 0; i < m_apcRenderers.size(); i++)
    {
        if( m_apcRenderers[i]->getName() == strFilterName )
        {
            pRenderer = m_apcRenderers[i];
            break;
        }
    }
    return pRenderer;
}


void PluginRendererLoader::xReadAndSortFilters()
{
    ///read filter order
    QStringList cLastOrder;
    g_cAppSetting.beginGroup("PluginRenderers");
    int iSize = g_cAppSetting.beginReadArray("renderer_order");
    for (int i = 0; i < iSize; ++i)
    {
        g_cAppSetting.setArrayIndex(i);
        const QString& strFilterName = g_cAppSetting.value("renderer_name").toString();
        cLastOrder.push_back(strFilterName);
    }
    g_cAppSetting.endArray();
    g_cAppSetting.endGroup();

    /// sorting
    sortFilters(cLastOrder);
}

void PluginRendererLoader::sortFilters(const QStringList &rcFilterNames)
{
    QStringList cFilterNamesCopy = rcFilterNames;

    for (int i = 0; i < cFilterNamesCopy.size(); ++i)
    {
        bool bFound = false;
        for(int j = 0; j < m_apcRenderers.size(); j++)
        {
            if(m_apcRenderers.at(j)->getName() == cFilterNamesCopy.at(i))
            {
                bFound = true;
                /// swap position i & j
                AbstractPluginRenderer* tempFilter = m_apcRenderers.at(i);
                m_apcRenderers.replace(i, m_apcRenderers.at(j));
                m_apcRenderers.replace(j, tempFilter);

                QPluginLoader* tempLoader = m_apcPluginLoaders.at(i);
                m_apcPluginLoaders.replace(i, m_apcPluginLoaders.at(j));
                m_apcPluginLoaders.replace(j, tempLoader);
            }

        }
        if(bFound == false) /// earse the filters that are missing
        {
            cFilterNamesCopy.erase(cFilterNamesCopy.begin()+i);
            i--;
        }
    }
}

bool PluginRendererLoader::xPrepareFilterContext()
{
    /// prepare filter context
    ModelLocator* pModel = ModelLocator::getInstance();

    /// loader
    m_cPluginContext.pcRendererLoader = this;

    /// renderer
    m_cPluginContext.pcRenderer = pModel->getRenderer();
    if(m_cPluginContext.pcRenderer == NULL)
    {
        qCritical() << "RendererLoader Context Renderer invalid";
        return false;
    }

    /// subject
    m_cPluginContext.pcSubject = &pModel->getSubject();

    /// property panel
    m_cPluginContext.pcPluginPropTabs = pModel->getPluginPropTabs();

    /// center splitter in mainwin
    m_cPluginContext.pcCenterSplitter = pModel->getCenterWidgetSplitter();

    /// event bus
    m_cPluginContext.pcEventBus = pModel->getEventBus();
    return true;
}


void PluginRendererLoader::saveFilterOrder()
{
    ///save filter order
    g_cAppSetting.beginGroup("PluginRenderers");
    g_cAppSetting.beginWriteArray("renderer_order");
    for (int i = 0; i < m_apcRenderers.size(); ++i)
    {
        g_cAppSetting.setArrayIndex(i);
        g_cAppSetting.setValue("renderer_name", m_apcRenderers.at(i)->getName());
    }
    g_cAppSetting.endArray();
    g_cAppSetting.endGroup();
    g_cAppSetting.sync();
}

QStringList PluginRendererLoader::getFilterNames()
{
    QStringList cNameList;
    foreach(AbstractPluginRenderer* pcFilter, m_apcRenderers )
        cNameList << pcFilter->getName();
    return cNameList;
}

QVector<bool> PluginRendererLoader::getEnableStatus()
{
    QVector<bool> cEnableStatus;
    foreach(AbstractPluginRenderer* pcFilter, m_apcRenderers )
        cEnableStatus.push_back(pcFilter->getEnable());
    return cEnableStatus;
}
