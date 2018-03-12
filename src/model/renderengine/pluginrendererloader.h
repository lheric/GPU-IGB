#ifndef PLUGINRENDERERLOADER_H
#define PLUGINRENDERERLOADER_H

#include "gitldef.h"
#include "gitlmodule.h"
#include "abstractpluginrenderer.h"
#include <QPluginLoader>
#include <QRect>
#include <QVector>


class PluginRendererLoader : public GitlModule
{
public:
    explicit PluginRendererLoader();
    ~PluginRendererLoader();



    /*!
     * Bacially it is a waper of filter interface \see AbstractPluginRenderer
     */
    virtual bool redraw       ();
    virtual bool mousePress (const QPointF* pcPos, Qt::MouseButton eMouseBtn);
    virtual bool keyPress   (int iKeyPressed);

    /*!
     * \brief initAllFilter Init all filters
     * \return
     */
    virtual bool initAllFilters();

    /*!
     * \brief uninitAllFilter Uninit all filters
     * \return
     */
    virtual bool uninitAllFilters();

    /*!
     * \brief reinitAllFilters Re-init all filters
     * \return
     */
    virtual bool reinitAllFilters();

    /*!
     * \brief reloadAllFilters reload filters in the directory
     * \return
     */
    virtual bool reloadAllFilters();

    /*!
     * \brief unloadAllFilters unload all filters
     * \return
     */
    virtual bool unloadAllFilters();

    /*!
     * \brief getFitlerByName get filter by filter name, if there is multiple filters with the same name, the first one will be returned
     * \param strFilterName the filter name
     * \return NULL will be returned if no filter matches
     */
    AbstractPluginRenderer* getPluginByName(const QString& strFilterName);

    /*!
     * \brief sortFilters sort filters according to list
     * \param rcFilterNames
     */
    void sortFilters(const QStringList& rcFilterNames);


    /*!
     * \brief saveFilterOrder Save filter orders to file
     */
    void saveFilterOrder();

    /*!
     * \brief getFilterNames Get all loaded filter names
     * \return
     */
    QStringList getFilterNames();

    /*!
     * \brief getFilterNames Get all loaded filter enable status.
     *                       The filter status orders are the same with getFilterNames
     * \return
     */
    QVector<bool> getEnableStatus();


protected:
    /*!
     * \brief xReadAndSortFilters sort filters according to loaded configurations (previous saved qsettings)
     */
    void xReadAndSortFilters();


    bool xPrepareFilterContext();


    /*!
     * Plugin Directory
     */
    ADD_CLASS_FIELD(QString, strPluginDir, getPluginDir, setPluginDir)


    /*! Plugin loader & instances
     */
    ADD_CLASS_FIELD_NOSETTER(QVector<QPluginLoader*>, apcPluginLoaders, getPluginLoaders)
    ADD_CLASS_FIELD_NOSETTER(QVector<AbstractPluginRenderer*>, apcRenderers, getRenderers)

    /*! Plugin Context \see PluginContext
     */
    ADD_CLASS_FIELD_PRIVATE(PluginContext, cPluginContext)

    /*! The event bus which is used to carry messages between plugins
     */
    ADD_CLASS_FIELD(GitlEventBus*, pcPluginComBus, getPluginComBus, setPluginComBus)

};

#endif // PLUGINRENDERERLOADER_H
