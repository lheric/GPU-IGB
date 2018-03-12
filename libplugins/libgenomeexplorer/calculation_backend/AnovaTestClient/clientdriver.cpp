#include "clientdriver.h"
using namespace std;


// =======================================================================
// DumpMatrix -- for float matrices.
// =======================================================================
void DumpMatrix(CArray2D_TCP<float> &M, const char *filename, const char *extension, int seqnumber, bool writetext)
{
    stringstream ss;

    ss << filename << '-';
    ss.fill('0');
    ss.width(5);
    ss << seqnumber << extension;

    ofstream file;
    if (writetext)
    {
        file.open(ss.str(), ofstream::out);

        if (file.is_open())
        {
            for (unsigned int i = 0; i < M.GetHeight(); i++)\
            {
                file.width(5);
                file << i << " : ";
                for (unsigned int j = 0; j < M.GetWidth(); j++)
                {
                    file << fixed << setprecision(4) << setw(10) << M(i, j) << ", ";
                }
                file << endl;
            }

            file.close();
        }
    }
    else
    {
        file.open(ss.str(), ofstream::out | ofstream::binary);

        if (file.is_open())
        {
            file.write((char*)M.GetHostDataPtr(), M.SizeOfMemory());
            file.close();
        }
    }
}

// =======================================================================
// DumpMatrix -- for integer matrices.
// =======================================================================
void DumpMatrix(CArray2D_TCP<int> &M, char *filename, char *extension, int seqnumber, bool writetext)
{
    stringstream ss;

    ss << filename << '-';
    ss.fill('0');
    ss.width(5);
    ss << seqnumber << extension;

    ofstream file;
    if (writetext)
    {
        file.open(ss.str(), ofstream::out);

        if (file.is_open())
        {
            for (unsigned int i = 0; i < M.GetHeight(); i++)\
            {
                file.width(5);
                file << i << " : ";
                for (unsigned int j = 0; j < M.GetWidth(); j++)
                {
                    file << fixed << setw(3) << M(i, j) << ", ";
                }
                file << endl;
            }

            file.close();
        }
    }
    else
    {
        file.open(ss.str(), ofstream::out | ofstream::binary);

        if (file.is_open())
        {
            file.write((char*)M.GetHostDataPtr(), M.SizeOfMemory());
            file.close();
        }
    }
}

// =======================================================================
// DoFunctionANova -- this is the client side function that communicates
// to a server to request that the ANOVA be computed.
// =======================================================================

bool DoFunctionANova(float *pVoxelSubject,
    int *pSNPSubject,
    float *pVoxelSNP,
    int NumberOfVoxels,
    int NumberOfSubjects,
    int NumberOfSNPs,
    int *pVoxelList,
    int VoxelListCount,
    int *pSNPList,
    int SNPListCount,
    CSocket* CliSock)
{
    CMD_t Cmd;

    Cmd.opcode = OPC_FunctionAnova;
    Cmd.C.FunctionAnova.VoxelSubject = pVoxelSubject;
    Cmd.C.FunctionAnova.SNPSubject = pSNPSubject;
    Cmd.C.FunctionAnova.VoxelSNP = pVoxelSNP;

    Cmd.C.FunctionAnova.NumberOfSubjects = NumberOfSubjects;
    Cmd.C.FunctionAnova.NumberOfVoxels = NumberOfVoxels;
    Cmd.C.FunctionAnova.NumberOfSNPs = NumberOfSNPs;

    Cmd.C.FunctionAnova.VoxelList = pVoxelList;
    Cmd.C.FunctionAnova.VoxelListCount = VoxelListCount;
    Cmd.C.FunctionAnova.SNPList = pSNPList;
    Cmd.C.FunctionAnova.SNPListCount = SNPListCount;

    int iResult = CliSock->SendTCPMessage(&Cmd, NULL);
    if (iResult != 0)
    {
        cout << "DoFunctionANova -- send error" << endl;
        return false;
    }

    CMD_t RetCmd;
    char *payload = NULL;
    unsigned int payload_size = 0;

    iResult = CliSock->ReceiveTCPMessage(&RetCmd, &payload, &payload_size);
    if (iResult != 0)
    {
        cout << "DoFunctionANova -- recv error" << endl;
        return false;
    }

    return true;
}

float FRand(void)
{
    return (float)rand() / (float)RAND_MAX;
}

// =======================================================================
// AnovaTestOne -- this is the driver function that runs tests.
// =======================================================================
bool AnovaTestOne(const int NumberOfSubjects, int NumberOfVoxels, int NumberOfSNPs, CSocket* ClientSocket)
{
    cout << "\n*** Running AnovaTestOne ***" << endl;

    //
    // Allocate some vectors that can be used on the host.
    //
    CArray2D_TCP<float> VoxelSubject(ClientSocket, NumberOfVoxels, NumberOfSubjects, false);
    CArray2D_TCP<int> SNPSubject(ClientSocket, NumberOfSNPs, NumberOfSubjects, false);
    CArray2D_TCP<float> VoxelSNP(ClientSocket, NumberOfVoxels, NumberOfSNPs, true, 0.0f);
    CArray1D_TCP<int> VoxelList(ClientSocket, NumberOfVoxels, true, 0);
    CArray1D_TCP<int> SNPList(ClientSocket, NumberOfSNPs, true, 0);

    //
    // Initialize the VoxelSubject and SNPSubject matrices.
    //
    for (unsigned int i = 0; i < VoxelSubject.GetHeight(); i++)
    {
        for (unsigned int j = 0; j < VoxelSubject.GetWidth(); j++)
        {
            VoxelSubject(i, j) = FRand() * 100.0f;
        }
    }

    for (unsigned int i = 0; i < SNPSubject.GetHeight(); i++)
        for (unsigned int j = 0; j < SNPSubject.GetWidth(); j++)
        {
            float R = FRand();

            if (R < 0.33f) SNPSubject(i, j) = 0;
            else
                if (R < 0.67f) SNPSubject(i, j) = 1;
                else
                    SNPSubject(i, j) = 2;
        }

    //
    // Intialize the 2 vectors that describe which voxels and which SNPs
    // to process on the server.
    //
    for (unsigned int i = 0; i < VoxelList.GetNumElements(); i++)
        VoxelList(i) = i;

    for (unsigned int i = 0; i < SNPList.GetNumElements(); i++)
        SNPList(i) = i;


    VoxelSubject.CopyToServer();
    SNPSubject.CopyToServer();
    VoxelList.CopyToServer();
    SNPList.CopyToServer();

    bool Result = true;

    for (int c = 0; c < 1; c++)
    {
        cout << "Iteration " << c << endl;

        Timer tmr;

        Result |= DoFunctionANova(
            VoxelSubject.GetServerDataPtr(),
            SNPSubject.GetServerDataPtr(),
            VoxelSNP.GetServerDataPtr(),
            NumberOfVoxels,
            NumberOfSubjects,
            NumberOfSNPs,
            VoxelList.GetServerDataPtr(),
            VoxelList.GetNumElements(),
            SNPList.GetServerDataPtr(),
            SNPList.GetNumElements(),
            ClientSocket);

        double DeltaTime = tmr.elapsed();

        cout << "Elapsed time " << DeltaTime << " seconds." << endl;

        if (Result)
        {
            VoxelSNP.CopyFromServer();

            // Dump the matrices to a file. Since the client app doesn't know
            // if it is talking to a CPU or GPU server, just give the file a generic
            // name. Then rename it when the program exits and compare the CPU version
            // of the file against the GPU version of the file using a DIFF tool.
            //
            // DumpMatrix(VoxelSNP, "VoxelSNP", ".txt", c);
            //			DumpMatrix(VoxelSNP, "VoxelSNP", ".bin", c, false);
            //			DumpMatrix(VoxelSubject, "VoxelSubject", ".txt", c);
            //			DumpMatrix(SNPSubject, "SNPSubject", ".txt", c);
        }

    }

    return Result;
}



bool AnovaTestTwo(const int NumberOfSubjects, int NumberOfVoxels, int NumberOfSNPs, CSocket* ClientSocket, CArray2D_TCP<float>* VoxelSNP)
{
    cout << "\n*** Running AnovaTestOne ***" << endl;

    //
    // Allocate some vectors that can be used on the host.
    //
    CArray2D_TCP<float> VoxelSubject(ClientSocket, NumberOfVoxels, NumberOfSubjects, false);
    CArray2D_TCP<int> SNPSubject(ClientSocket, NumberOfSNPs, NumberOfSubjects, false);
    CArray1D_TCP<int> VoxelList(ClientSocket, NumberOfVoxels, true, 0);
    CArray1D_TCP<int> SNPList(ClientSocket, NumberOfSNPs, true, 0);

    //
    // Initialize the VoxelSubject and SNPSubject matrices.
    //
    for (unsigned int i = 0; i < VoxelSubject.GetHeight(); i++)
    {
        for (unsigned int j = 0; j < VoxelSubject.GetWidth(); j++)
        {
            VoxelSubject(i, j) = FRand() * 100.0f;
        }
    }

    for (unsigned int i = 0; i < SNPSubject.GetHeight(); i++)
        for (unsigned int j = 0; j < SNPSubject.GetWidth(); j++)
        {
            float R = FRand();

            if (R < 0.33f) SNPSubject(i, j) = 0;
            else
                if (R < 0.67f) SNPSubject(i, j) = 1;
                else
                    SNPSubject(i, j) = 2;
        }

    //
    // Intialize the 2 vectors that describe which voxels and which SNPs
    // to process on the server.
    //
    for (unsigned int i = 0; i < VoxelList.GetNumElements(); i++)
        VoxelList(i) = i;

    for (unsigned int i = 0; i < SNPList.GetNumElements(); i++)
        SNPList(i) = i;


    VoxelSubject.CopyToServer();
    SNPSubject.CopyToServer();
    VoxelList.CopyToServer();
    SNPList.CopyToServer();

    bool Result = true;

    for (int c = 0; c < 1; c++)
    {
        cout << "Iteration " << c << endl;

        Timer tmr;

        Result |= DoFunctionANova(
            VoxelSubject.GetServerDataPtr(),
            SNPSubject.GetServerDataPtr(),
            VoxelSNP->GetServerDataPtr(),
            NumberOfVoxels,
            NumberOfSubjects,
            NumberOfSNPs,
            VoxelList.GetServerDataPtr(),
            VoxelList.GetNumElements(),
            SNPList.GetServerDataPtr(),
            SNPList.GetNumElements(),
            ClientSocket);

        double DeltaTime = tmr.elapsed();

        cout << "Elapsed time " << DeltaTime << " seconds." << endl;

        if (Result)
        {
            VoxelSNP->CopyFromServer();

            // Dump the matrices to a file. Since the client app doesn't know
            // if it is talking to a CPU or GPU server, just give the file a generic
            // name. Then rename it when the program exits and compare the CPU version
            // of the file against the GPU version of the file using a DIFF tool.
            //
            // DumpMatrix(*VoxelSNP, "VoxelSNP", ".txt", c);
            //			DumpMatrix(VoxelSNP, "VoxelSNP", ".bin", c, false);
            //			DumpMatrix(VoxelSubject, "VoxelSubject", ".txt", c);
            //			DumpMatrix(SNPSubject, "SNPSubject", ".txt", c);
        }

    }

    return Result;
}
