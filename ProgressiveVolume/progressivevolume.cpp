#include "progressivevolume.h"
#include "VolumeRaycastingWidget.h"
#include <vtkPNGReader.h>
#include <vtkMetaImageReader.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkDICOMImageReader.h>
#include <vtkMetaImageWriter.h>
//#include <vtkGDCMImageReader.h>

ProgressiveVolume::ProgressiveVolume(QWidget *parent, Qt::WFlags flags)
    : QWidget(parent, flags), _engine(this)
{
    ui.setupUi(this);

    _volumeWidget = new VolumeRaycastingWidget(this);
    ui.verticalLayout_2->addWidget(_volumeWidget);

    //prepareData();
    _engine.setWidget(_volumeWidget);
    _engine.setSliceNum(320);
    _engine.setProgressive(false);
    _engine.prepare();
    _engine.start();

    int i = 0;
}



ProgressiveVolume::~ProgressiveVolume()
{

}

void ProgressiveVolume::prepareData()
{
    //int zCount = 320;

    //vtkSmartPointer<vtkImageData> allData = vtkSmartPointer<vtkImageData>::New();
    //int thickness = 1;
    //allData->SetDimensions(512, 512, zCount / thickness);
    //allData->SetSpacing(0.671, 0.671, thickness);
    //allData->SetNumberOfScalarComponents(1);
    //allData->SetScalarTypeToShort();
    //allData->AllocateScalars();


    //for(int i = 0; i < zCount; i += thickness) {
    //    QString path = QString("../../dcm/image_11-17-2015_%1.dcm").arg(i);

    //    vtkSmartPointer<vtkGDCMImageReader> reader = vtkSmartPointer<vtkGDCMImageReader>::New();
    //    reader->SetFileName(path.toUtf8().constData());
    //    reader->Update();

    //    vtkImageData* data = reader->GetOutput();

    //    /* vtkSmartPointer<vtkMetaImageWriter> writer = vtkSmartPointer<vtkMetaImageWriter>::New();
    //    writer->SetFileName("d:\\testOut.mhd");
    //    writer->SetInput(data);
    //    writer->Write();
    //    const char* type = data->GetScalarTypeAsString();*/
    //    insertCTData(data, allData, (zCount - i) / thickness - 1);
    //}

    //_volumeWidget->initialize(allData);
    //_volumeWidget->setCameraToBack();
    //_volumeWidget->setBlendTypeToBone();
}

void ProgressiveVolume::insertCTData(vtkImageData* srcData, vtkImageData* destData, int z)
{
    short* ptDestData = (short*)destData->GetScalarPointer() + 512 * 512 * z;
    short* ptDSrcData = (short*)srcData->GetScalarPointer();
    int size = srcData->GetNumberOfPoints();
    memcpy(ptDestData, ptDSrcData, size * 2);
}
