#include "DataBuildTheard.h"
#include <vtkImageData.h>
#include <QMutexLocker>
#include <vtkSmartPointer.h>
#include <vtkImageResample.h>

DataBuildTheard::DataBuildTheard(QObject *parent)
    : QThread(parent)
{

}

DataBuildTheard::~DataBuildTheard()
{

}

void DataBuildTheard::run()
{
    vtkImageData* allData = vtkImageData::New();

    allData->SetDimensions(512, 512, _sliceNum);
    allData->SetSpacing(0.671, 0.671, _currentStepInfo.sliceThickness);
    //allData->SetNumberOfScalarComponents(1);
    //allData->SetScalarType();
    //allData->AllocateScalars();
    allData->AllocateScalars(VTK_SHORT, 1);

    for(int i = 0; i < _dataArray.count(); i ++) {
        vtkImageData* data = _dataArray[i];
        insertCTData(data, allData, _dataArray.count() - i - 1);
    }

    //if(_currentStepInfo.sliceThickness == 1) {
    allData->Register(NULL);
    emit allDataReady(allData);
    //} else {
    //    vtkSmartPointer<vtkImageResample> imageSample = vtkSmartPointer<vtkImageResample>::New();
    //    imageSample->SetInput(allData);
    //    imageSample->SetAxisMagnificationFactor(2, _currentStepInfo.sliceThickness);
    //    imageSample->Update();
    //    vtkImageData* finalData = imageSample->GetOutput();
    //    //finalData->Register(NULL);
    //    emit allDataReady(finalData);
    //}
}

void DataBuildTheard::insertCTData(vtkImageData* srcData, vtkImageData* destData, int z)
{
    if(destData == NULL)
        return;
    //double spacing[3];
    //srcData->GetSpacing(spacing);
    //unsigned int thickness = 1 / spacing[2];
    int* extend = destData->GetExtent();
    if(z > extend[5])
        return;

    short* ptDestData = (short*)destData->GetScalarPointer() + 512 * 512 * z;
    short* ptDSrcData = (short*)srcData->GetScalarPointer();

    //short* pt = (short*)srcData->GetScalarPointer(10, 10, 10);

    int num = srcData->GetNumberOfPoints();
    int bytePerScale = srcData->GetScalarSize();
    memcpy(ptDestData, ptDSrcData, num * bytePerScale);
}
