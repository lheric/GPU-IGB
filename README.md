Genome Explorer
===================================

This is the source code of the software tool for paper:

**GPU Accelerated Browser for Neuroimaging Genomics**

Bob Zigon, Huang Li, Xiaohui Yao, Shiaofen Fang, Mohammad Al Hasan, Jingwen Yan, Jason H. Moore, Andrew J. Saykin, Li Shen, for the Alzheimer's Disease Neuroimaging Initiative

How to Run
==========

You can either compile it from the source code or download the binary from [here](http://www.iu.edu/~beca/BECA-GenomicExplorer.zip). 
An example dataset is included in the binary zip file.

[software manual](http://www.iu.edu/~beca/asserts/BECA_Software_Manual_GenomeExplorer.pdf)

If you want to compile it from the source code, follow the 'How to Compile' section.

How to Compile
==============

Dependencies:
* [Qt 5.7 or later](https://www.qt.io/)
* [VTK 7.0.0 or later](https://www.vtk.org/)
* [CUDA 8.0](https://developer.nvidia.com/cuda-toolkit)
* Windows 7 or later

Client:
1. Load BECA.pro with Qt Creator
2. Set `VTK_ROOT` and `VTK_MAIN_VERSION` in file `vtk.pri`
`VTK_ROOT` is the path where VTK is installed
`VTK_MAIN_VERSION` is the version number of VTK
3. Click menu `Build` -> `Build All`

Server:
1. Load `libplugins\libgenomeexplorer\calculation_backend\BServerExperiments.sln` with Visual Studio 2015 or later
2. Build 'BServer'

Follow the instruction in [software manual](http://www.iu.edu/~beca/asserts/BECA_Software_Manual_GenomeExplorer.pdf) to run it.
