#ifndef ABSTRACTPLUGINRENDERER_H
#define ABSTRACTPLUGINRENDERER_H

#include <QtPlugin>
#include <QWidget>
#include <QTableWidget>
#include <QSplitter>
#include "gitleventbus.h"
#include "gitldef.h"

#define PLUGIN_RENDERER_IID "edu.iupui.BECA.AbstractPluginRenderer"

#define COMMAND_MAP(COMMAND_NAME, CLASSNAME, CALLBACK_MEMBER_FUNCTION) \
    this->subscribeToEvtByName(COMMAND_NAME, MAKE_CALLBACK(CLASSNAME::CALLBACK_MEMBER_FUNCTION));

class vtkRenderer;
class Subject;
class vtkAlgorithmOutput;
class PluginRendererLoader;

/*!
 * \brief The PluginContext class
 * Parameters which will be sent to the filter plgins in AbstractPluginRenderer Interface
 * Basically it is just pointers to the objects in model, indicating the state of the system.
 */
struct PluginContext
{
    PluginRendererLoader* pcRendererLoader = NULL;
    vtkRenderer* pcRenderer = NULL;
    Subject* pcSubject = NULL;
    QTabWidget* pcPluginPropTabs = NULL;
    QSplitter* pcCenterSplitter = NULL;
    GitlEventBus* pcEventBus = NULL;
};


class AbstractPluginRenderer: public GitlModule
{
public:

    AbstractPluginRenderer()
    {
        m_bEnable = false;
        m_strName = "UNKNOWN";
        m_pcContext = NULL;
    }

    virtual ~AbstractPluginRenderer()
    {
    }

    /*!
     * \brief init is called as soon as loaded in
     * \return
     */
    virtual bool init()
    {
        return true;
    }

    /*!
     * \brief uninit is called before unload
     * \return
     */
    virtual bool uninit()
    {
        return true;
    }

    /*!
     * \brief redraw is called for each rendered frame
     * \return
     */
    virtual bool redraw()
    {
        return true;
    }

    /*!
     * \brief update Refresh the status
     */
    virtual void update()
    {
        if(getEnable() == true)
            this->onEnabled();
        else
            this->onDisabled();
    }

    /*!
     * \brief onEnabled called when renderer is enabled
     * \return
     */
    virtual bool onEnabled()
    {
        return true;
    }

    /*!
     * \brief onDisabled called when renderer is disabled
     * \return
     */
    virtual bool onDisabled()
    {
        return true;
    }

    /*!
     * \brief mousePress when user press mouse button on the dispalyed frame
     * \param pcPos position in the window
     * \param eMouseBtn mouse button that is pressed
     * \return
     */
    virtual bool mousePress(const QPointF* pcPos,
                            Qt::MouseButton eMouseBtn)
    {
        Q_UNUSED(pcPos)
        Q_UNUSED(eMouseBtn)
        return true;
    }

    /*!
     * \brief keyPress when user press a key on keyboard
     * \param iKeyPressed keycode
     * \return
     */
    virtual bool keyPress(int iKeyPressed)
    {
        Q_UNUSED(iKeyPressed)
        return true;
    }


    /*! The plugin name displayed in the user interface
     */
    ADD_CLASS_FIELD(QString, strName, getName, setName)


    /*! The context for running this plugin
     */
    ADD_CLASS_FIELD(PluginContext*, pcContext, getContext, setContext)


    /*! This is the switch for turn off or on this plugin
     *  false - this filter will not be applied
     *  true  - this filter will be applied
     */
    ADD_CLASS_FIELD_NOSETTER(bool, bEnable, getEnable)
public:
    void setEnable(bool bEnable)
    {
        this->m_bEnable = bEnable;
        if(bEnable == true)
            this->onEnabled();
        else
            this->onDisabled();
    }



};

/// This is required by the Qt plugin system.
Q_DECLARE_INTERFACE(AbstractPluginRenderer, PLUGIN_RENDERER_IID)
Q_DECLARE_METATYPE(AbstractPluginRenderer*)

#endif // ABSTRACTPLUGINRENDERER_H
