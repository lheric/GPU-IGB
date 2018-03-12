#ifndef VTKTRACKVISTRKREADER_H
#define VTKTRACKVISTRKREADER_H

#include "vtkIOLegacyModule.h"
#include "vtkPolyDataAlgorithm.h"
#include "trkfileio.h"
#include "gitldef.h"

class vtkTrackVisTrkReader : public vtkPolyDataAlgorithm
{

public:
    static vtkTrackVisTrkReader* New();
    vtkTypeMacro(vtkTrackVisTrkReader,vtkPolyDataAlgorithm)
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Set/Get the name of the file from which to read points.
    vtkSetStringMacro(FileName)
    vtkGetStringMacro(FileName)

    //
    ADD_CLASS_FIELD_NOSETTER(TrkFileReader, TrkReader, getTrkReader)


protected:
    vtkTrackVisTrkReader();
    ~vtkTrackVisTrkReader();

    char* FileName;

    int RequestData(vtkInformation*,
                    vtkInformationVector**,
                    vtkInformationVector*);
//private:
//    vtkTrackVisTrkReader(const vtkTrackVisTrkReader&);  // Not implemented.
//    void operator=(const vtkTrackVisTrkReader&);  // Not implemented.
};

#endif // VTKTRACKVISTRKREADER_H
