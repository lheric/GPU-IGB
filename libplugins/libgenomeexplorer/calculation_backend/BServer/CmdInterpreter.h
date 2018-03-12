//
// CmdInterpreter.h
//
#include "..\Common\Sockets.h"

// ==================================================================

class CBaseCmdInterpreter
{
public:
	CBaseCmdInterpreter();
	virtual ~CBaseCmdInterpreter();
	void virtual Init(void) = 0;
	void Run(CSocket* S);

protected:
	void virtual DoMalloc1D(CSocket* pCS, CMD_t *pCmd) = 0;
	void virtual DoMalloc2D(CSocket* pCS, CMD_t *pCmd) = 0;
	void virtual DoFree(CSocket* pCS, CMD_t *pCmd) = 0;
	void virtual DoShutDown(CSocket *pCS);
	void virtual DoCopyHost2Device(CSocket* pCS, CMD_t *pCmd, char* payload, int payload_size) = 0;
	void virtual DoCopyDevice2Host(CSocket* pCS, CMD_t *pCmd) = 0;
	void virtual DoFunctionAnova(CSocket* pCS, CMD_t *pCmd);
	void virtual DoFunctionVegasTest(CSocket* pCS, CMD_t *pCmd);
};

// ==================================================================

class CGPUCmdInterpreter : public CBaseCmdInterpreter
{
public:
	CGPUCmdInterpreter();
	virtual ~CGPUCmdInterpreter();
	void Init(void);

protected:
	void DoMalloc1D(CSocket* pCS, CMD_t *pCmd);
	void DoMalloc2D(CSocket* pCS, CMD_t *pCmd);
	void DoFree(CSocket* pCS, CMD_t *pCmd);
	void DoCopyHost2Device(CSocket* pCS, CMD_t *pCmd, char* payload, int payload_size);
	void DoCopyDevice2Host(CSocket* pCS, CMD_t *pCmd);
	void DoFunctionAnova(CSocket* pCS, CMD_t *pCmd);
	void DoFunctionVegasTest(CSocket* pCS, CMD_t *pCmd);

	char *m_XferBuf;
};

// ==================================================================

class CCPUCmdInterpreter : public CBaseCmdInterpreter
{
public:
	CCPUCmdInterpreter();
	virtual ~CCPUCmdInterpreter();
	void Init(void);

protected:
	void DoMalloc1D(CSocket* pCS, CMD_t *pCmd);
	void DoMalloc2D(CSocket* pCS, CMD_t *pCmd);
	void DoFree(CSocket* pCS, CMD_t *pCmd);
	void DoCopyHost2Device(CSocket* pCS, CMD_t *pCmd, char* payload, int payload_size);
	void DoCopyDevice2Host(CSocket* pCS, CMD_t *pCmd);
	void DoFunctionAnova(CSocket* pCS, CMD_t *pCmd);
	void DoFunctionVegasTest(CSocket* pCS, CMD_t *pCmd);
};
