#include "appfrontcontroller.h"
#include "command/openfiberfilecommand.h"
#include "command/opengraymatterimagecommand.h"
#include "command/connectpipelinecommand.h"
#include "command/busydialogcommand.h"
#include "command/rerendercommand.h"
#include "command/resetcameracommand.h"
#include "command/changeinteractorstylecommand.h"
#include "command/opengraymatterparcelcommand.h"
#include "command/changerenderingprefcommand.h"
#include "command/pickcoordinatecommand.h"
#include "command/changebgcolorcommand.h"
#include "command/switchplugincommand.h"
#include "command/reloadpluginrenderercommand.h"
#include "command/openlabelnamescommand.h"

SINGLETON_PATTERN_IMPLIMENT(AppFrontController)

/// command <string,class> pair
static struct
{
    const char* strCommandName;
    const QMetaObject* pMetaObject;
}
s_sCmdTable[] =
{
    { "open_fiber_file",            &OpenFiberFileCommand::staticMetaObject                 },
    { "open_label_names_file",      &OpenLabelNamesCommand::staticMetaObject                },
    { "open_gm_image_file",         &OpenGrayMatterImageCommand::staticMetaObject           },
    { "open_gm_parcel_file",        &OpenGrayMatterParcelCommand::staticMetaObject          },
    { "connect_pipeline",           &ConnectPipelineCommand::staticMetaObject               },
    { "busy_dialog",                &BusyDialogCommand::staticMetaObject                    },
    { "rerender",                   &RerenderCommand::staticMetaObject                      },
    { "reset_camera",               &ResetCameraCommand::staticMetaObject                   },
    { "change_interactor",          &ChangeInteractorStyleCommand::staticMetaObject         },
    { "change_rendering_pref",      &ChangeRenderingPrefCommand::staticMetaObject           },
    { "change_bg_color",            &ChangeBgColorCommand::staticMetaObject                 },
    { "pick_coord",                 &PickCoordinateCommand::staticMetaObject                },
    { "switch_plugin",              &SwitchPluginCommand::staticMetaObject                  },
    { "reload_filter",              &ReloadPluginRendererCommand::staticMetaObject          },
    { "",                           NULL                                                    }    ///end mark
};


AppFrontController::AppFrontController()
{
    xInitCommand();
}

bool AppFrontController::xInitCommand()
{
    /// register commands
    auto* psCMD = s_sCmdTable;
    while( psCMD->pMetaObject != NULL )
    {
        registerCommand(psCMD->strCommandName, psCMD->pMetaObject);
        psCMD++;
    }
    return true;
}


void AppFrontController::run()
{

    forever
    {

        /// do command & exception handling
        try
        {
            GitlFrontController::run();
        }
        catch(...)
        {
             qCritical() << "Error...";
        }

    }


}
