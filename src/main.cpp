#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkInteractionStyle)


#include "view/mainwindow.h"
#include <QDebug>
#include <QApplication>
#include <QWindow>
#include <command/appfrontcontroller.h>
#include "model/modellocator.h"


static void xMessageOutput(QtMsgType type, const QMessageLogContext &rCcontext, const QString &strMsg)
{
    Q_UNUSED(rCcontext)

    QByteArray localMsg = strMsg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stdout, "[Debug]: %s\n", localMsg.constData());
        break;
    case QtInfoMsg:
        fprintf(stdout, "[Info]: %s\n",  localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "[Warn]: %s\n",  localMsg.constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "[Crit]: %s\n",  localMsg.constData());
        break;
    case QtFatalMsg:
        fprintf(stderr, "[Fata]: %s\n",  localMsg.constData());
    }
    fflush(stdout);
    fflush(stderr);

    GitlUpdateUIEvt cEvt;
    cEvt.setParameter("msg_detail", strMsg);
    cEvt.setParameter("msg_level",(int)type);
    cEvt.dispatch();

    if(type == QtFatalMsg)
        abort();

}


#include <QProcess>
QProcess g_cbusyWinProc;
void testBusyWin(QObject* p)
{
    g_cbusyWinProc.setParent(p);
    //g_cbusyWinProc.start("E:\\Learning\\swap_with_vm\\BECA-2015-05-14\\appProgressDialog.exe");
}



int main(int argc, char *argv[])
{

    ModelLocator::getInstance();

    // front controller
    AppFrontController::getInstance();

    QApplication a(argc, argv);


    //testBusyWin(&a);
    /// intall message handler
    qInstallMessageHandler(xMessageOutput);

    MainWindow w;
    w.show();
    return a.exec();
}
