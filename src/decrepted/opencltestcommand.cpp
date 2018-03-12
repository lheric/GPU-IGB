#include "opencltestcommand.h"
#include "model/modellocator.h"
#include "algorithm/vtkutils.h"
#include "algorithm/cl.hpp"

OpenCLTestCommand::OpenCLTestCommand(QObject *parent) :
    GitlAbstractCommand(parent)
{
    setInWorkerThread(false);
}

static bool xCheckOpenCLError(cl_int iCode, QString& strMessage)
{
    if(iCode != CL_SUCCESS)
    {
        qWarning() << "Error!" << strMessage << "\nCode:" << iCode;
        return false;
    }
    return true;
}

bool OpenCLTestCommand::execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
{

    ModelLocator* pcModel = ModelLocator::getInstance();
    pcModel->getRayCastOpenCLRenderer().render();
    return true;

    ///
    vtkImageData* pcIntensityVolume = pcModel->getSubject().getGrayMat().getIntensityVol().getVolume();
    if(VTKUtils::isEmptyDimension(pcIntensityVolume->GetDimensions()))
    {
        qWarning() << "Invalid Intensity Volume";
        return false;
    }


    //retrieve all available platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    if(platforms.size() == 0) {
        std::cerr << "No platforms available" <<std::endl;
        exit(-1);
    }

    //list all available platforms
    for(auto& platform : platforms) {
        std::cout << "Available platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
    }

    //choose the first available platform (I'll assume we only have one installed)
    cl::Platform chosen_platform = platforms[1];    /// TODO BUG auto GPU choose needed
    std::cout << "Using platform: " << chosen_platform.getInfo<CL_PLATFORM_NAME>() << std::endl;


    //retrieve all devices usable by the selected platform
    std::vector<cl::Device> devices;
    chosen_platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    if(devices.size() == 0) {
        std::cerr << "No device found" << std::endl;
        return false;
    }

    //list all available devices
    for(cl::Device& device : devices) {
        std::cout << "Available device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        std::cout << "Vendor: " << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
        std::cout << "Device Version: " << device.getInfo<CL_DEVICE_VERSION>() << std::endl;
        std::cout << "Driver Version: " << device.getInfo<CL_DRIVER_VERSION>() << std::endl;


    }

    //choose the first available devices
    cl::Device chosen_device = devices[0];
    std::cout << "Using device: " << std::string(chosen_device.getInfo<CL_DEVICE_NAME>()) << std::endl;

    cl::Context context({chosen_device});

    cl::Program::Sources sources;

    // kernel calculates for each element C=A+B
//    std::string kernel_code=
//            "   void kernel simple_add(global const int2* A, global const int2* B, global int2* C){       "
//            "       C[get_global_id(0)]=A[get_global_id(0)]+B[get_global_id(0)];                 "
//            "       printf(\"%d\", get_global_id(0));"
//            "   }                                                                               ";

    QString strSourcePath = "E:\\Learning\\Github\\BrainExplorerForConnectomicAnalysis\\src\\assets\\opencl_kernel\\kernel.cl";
    QFile cSourceFile(strSourcePath);
    if(!cSourceFile.open(QIODevice::ReadOnly))
    {
        qCritical() << "Fail to read" << strSourcePath;
        return false;
    }



    QString strKernelCode = QTextStream(&cSourceFile).readAll();
    cSourceFile.close();

    sources.push_back({strKernelCode.toStdString().c_str(), strKernelCode.length()});

    cl::Program program(context,sources);
    if(program.build({chosen_device})!=CL_SUCCESS){
        std::cout<<" Error building: "<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(chosen_device)<<"\n";
        exit(1);
    }


    // create buffers on the device
    int* iDim = pcIntensityVolume->GetDimensions();
    float* afIntensityVolume = new float[iDim[0]*iDim[1]*iDim[2]];
    float* afOutVolume = new float[iDim[0]*iDim[1]];
    for(int x = 0; x < iDim[0]; x++)
    {
        for(int y = 0; y < iDim[1]; y++)
        {
            for(int z = 0; z < iDim[2]; z++)
            {
                int iOffset = z*iDim[0]*iDim[1] + y*iDim[0] + x;
                afIntensityVolume[iOffset] = pcIntensityVolume->GetScalarComponentAsFloat(x,y,z,0);
            }
        }
    }

    cl::ImageFormat format;
    format.image_channel_data_type = CL_FLOAT;
    format.image_channel_order = CL_LUMINANCE;

    ///
//    iDim[0] = iDim[1] = iDim[2] = 2;
//    for(int i = 0; i < 8; i++)
//        afIntensityVolume[i] = 2;

    cl::Image3D img(context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR , format, iDim[0], iDim[1], iDim[2], 0, 0, afIntensityVolume);
    cl::Image2D out(context,CL_MEM_WRITE_ONLY , format, iDim[0], iDim[1]);

//    cl::Buffer buffer_A(context,CL_MEM_READ_WRITE,sizeof(cl_int2)*10);
//    cl::Buffer buffer_B(context,CL_MEM_READ_WRITE,sizeof(cl_int2)*10);
//    cl::Buffer buffer_C(context,CL_MEM_READ_WRITE,sizeof(cl_int2)*10);

//    cl_int2 A[] = {{0,0}, {1,1}, {2,2}};
//    cl_int2 B[] = {{1,1}, {2,3}, {4,4}};

    //create queue to which we will push commands for the device.
    cl::CommandQueue queue(context,chosen_device);



    std::vector<cl::ImageFormat> supported_format;

    qDebug() << context.getSupportedImageFormats (	CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                                CL_MEM_OBJECT_IMAGE3D,
                                                &supported_format);
    for(cl::ImageFormat fo: supported_format)
    {
        qDebug() << QString("%1").arg(fo.image_channel_order , 0, 16)
                 << QString("%1").arg(fo.image_channel_data_type , 0, 16);
        qDebug() << "";
    }

    //write arrays A and B to the device
//    queue.enqueueWriteBuffer(buffer_A,CL_TRUE,0,sizeof(cl_int2)*3,A);
//    queue.enqueueWriteBuffer(buffer_B,CL_TRUE,0,sizeof(cl_int2)*3,B);


//    //run the kernel
//    cl::make_kernel simple_add(cl::Kernel(program,"simple_add"),queue,cl::NullRange,cl::NDRange(10),cl::NullRange);
//    simple_add(buffer_A,buffer_B,buffer_C);

    //alternative way to run the kernel
    cl::Kernel kernel_add=cl::Kernel(program,"image_copy");
    kernel_add.setArg(0,img);
    kernel_add.setArg(1,out);
//    kernel_add.setArg(2,buffer_C);
    queue.enqueueNDRangeKernel(kernel_add,cl::NullRange,cl::NDRange(iDim[0],iDim[1]),cl::NullRange);
    queue.finish();


    //read result
    cl::size_t<3> cOrigin; cOrigin[0] = cOrigin[1] = cOrigin[2] = 0;
    cl::size_t<3> cRegion; cRegion[0] = iDim[0]; cRegion[1] = iDim[1]; cRegion[2] = 1;
    cl_int iResult = queue.enqueueReadImage(out, CL_TRUE,
                                            cOrigin,
                                            cRegion,
                                            0, 0, afOutVolume);
    if( iResult != CL_SUCCESS)
    {
        qWarning() << "Error! Code:" << iResult;
        return false;
    }




    Matrix<double> cMatrix;
    cMatrix.allocate(iDim[0], iDim[1]);
    for(int x = 0; x < iDim[0]; x++)
    {
        for(int y = 0; y < iDim[1]; y++)
        {
            int iOffset = y*iDim[0] + x;
            cMatrix[x][y] = afOutVolume[iOffset];
//            if(cMatrix[x][y] > 0.01)
//                qDebug() << cMatrix[x][y];
//            qDebug() << x << y << afOutVolume[iOffset];
        }
    }
    cMatrix.writeToCSV("opencl.csv");
    return true;
}

