

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)

#include "vtkVolumeRayCastingView.h"
#include <vtkObjectFactory.h>
#include <vtkVolume.h>
#include <vtkVolumePicker.h>
#include <vtkVolumeProperty.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkRenderer.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkImageData.h>
#include <vtkCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <vtkCornerAnnotation.h>

#include <vtkRenderWindowInteractor.h>

vtkStandardNewMacro(vtkVolumeRayCastingView);

vtkVolumeRayCastingView::vtkVolumeRayCastingView(void)
{
}


vtkVolumeRayCastingView::~vtkVolumeRayCastingView(void)
{
}

void vtkVolumeRayCastingView::setInput(vtkImageData *in)
{
    this->_imageData = in;
    if(_volumeMapper) {
        _volumeMapper->SetInputData(_imageData);

        double spacing[3];
        in->GetSpacing(spacing);
        double sampleSpacing =
            static_cast<float>((spacing[0] + spacing[1] + spacing[2]) / 6.0);

        _volumeMapper->SetSampleDistance(sampleSpacing);
    }
}

vtkImageData * vtkVolumeRayCastingView::getInput()
{
    return _imageData;
}

void vtkVolumeRayCastingView::updateRender()
{
    //_volume->SetAllocatedRenderTime(0.1, NULL);
}

void vtkVolumeRayCastingView::installPipeline()
{
    // Volume Mapper
    _volumeMapper = vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();

    // _volumeMapper->SetInputData(_imageData);
    _volumeMapper->SetBlendModeToComposite();
    _volumeMapper->SetMaxMemoryInBytes(512 * 1024 * 1024);
    _volumeMapper->SetMaxMemoryFraction(0.95);
    _volumeMapper->AutoAdjustSampleDistancesOff();
    _volumeMapper->SetSampleDistance(-1);
    // Create the volume
    _volume = vtkSmartPointer<vtkVolume>::New();
    _volume->SetMapper(_volumeMapper);

    //_interactor->SetStillUpdateRate(0.01);
    //_interactor->SetDesiredUpdateRate(5);
    //_renderer->AddVolume(_volume);

    double rate = _interactor->GetStillUpdateRate();
    // -----------------------------------------------------------------------------------------------
    // Corner Annotation
    //// -----------------------------------------------------------------------------------------------
    const int maximumFontSize = 14;
    _cornerAnnotation = vtkSmartPointer<vtkCornerAnnotation>::New();
    _cornerAnnotation->SetMaximumFontSize(maximumFontSize);
    _cornerAnnotation->GetTextProperty()->SetColor(1,1,1);
    _cornerAnnotation->GetTextProperty()->SetFontFamilyToArial();
    _cornerAnnotation->GetTextProperty()->SetShadow(1);
    _cornerAnnotation->SetText(3, "Volume Rendering");
    _renderer->AddActor(_cornerAnnotation);

    // Set the initial camera position
    double center[3];
    _imageData->GetCenter(center);
    _renderer->GetActiveCamera()->SetFocalPoint(center);
    _renderer->GetActiveCamera()->SetPosition(center[0], center[1] + 800, center[2]);
    _renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
}

void vtkVolumeRayCastingView::setBlendTypeToCTSkin()
{
    // Get the scalar range
    double range[2];
    _imageData->GetScalarRange(range);

    // Tan
    double r = 255;
    double g = 170;
    double b = 127;

    // Color Function
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    colorTransferFunction->AddRGBPoint(range[0], r / 255.0, g / 255.0, b / 255.0);
    colorTransferFunction->AddRGBPoint(range[1], r / 255.0, g / 255.0, b / 255.0);

    // Opacity Function
    double threshold = -500;
    vtkSmartPointer<vtkPiecewiseFunction> scalarOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
    scalarOpacity->AddPoint(range[0], 0);
    scalarOpacity->AddPoint(threshold, 0);
    scalarOpacity->AddPoint(threshold + 0.1, 1.0);
    scalarOpacity->AddPoint(range[1], 1.0);

    // Volume Property
    vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetColor(colorTransferFunction);
    volumeProperty->SetScalarOpacity(scalarOpacity);
    volumeProperty->SetIndependentComponents(1);
    volumeProperty->ShadeOn();

    _volume->SetProperty(volumeProperty);

    _renderer->AddVolume(_volume);

}

void vtkVolumeRayCastingView::setBlendTypeToBone()
{
    // Get the scalar range
    //double range[2];
    //_imageData->GetScalarRange(range);
    _volumeMapper->SetInputData(_imageData);

    double r = 255;
    double g = 170;
    double b = 127;

    // Color Function
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();

    colorTransferFunction->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
    colorTransferFunction->AddRGBPoint(-16, 0.73, 0.25, 0.30, 0.49, .61);
    colorTransferFunction->AddRGBPoint(641, .90, .82, .56, .5, 0.0);
    colorTransferFunction->AddRGBPoint(3071, 1, 1, 1, .5, 0.0);

    // Opacity Function
    double threshold = -500;
    vtkSmartPointer<vtkPiecewiseFunction> scalarOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();

    scalarOpacity->AddPoint(-3024, 0, 0.5, 0.0);
    scalarOpacity->AddPoint(-16, 0, .49, .61);
    scalarOpacity->AddPoint(641, .72, .5, 0.0);
    scalarOpacity->AddPoint(3071, .71, 0.5, 0.0);

    // Volume Property
    vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetColor(colorTransferFunction);
    volumeProperty->SetScalarOpacity(scalarOpacity);
    volumeProperty->SetIndependentComponents(1);
    volumeProperty->SetInterpolationTypeToLinear();
    volumeProperty->ShadeOn();

    volumeProperty->SetAmbient(0.1);
    volumeProperty->SetDiffuse(0.9);
    volumeProperty->SetSpecular(0.2);
    volumeProperty->SetSpecularPower(10.0);
    volumeProperty->SetScalarOpacityUnitDistance(0.8919);

    _volume->SetProperty(volumeProperty);
    //_volume->Modified();
    _volumeMapper->Update();
    _renderer->AddVolume(_volume);

    /*double rate = _interactor->GetStillUpdateRate();
    int i = 0;*/
}

void vtkVolumeRayCastingView::setCameraToFront()
{
    double center[3];
    _imageData->GetCenter(center);
    _renderer->GetActiveCamera()->SetFocalPoint(center);
    _renderer->GetActiveCamera()->SetPosition(center[0], center[1] - 800, center[2]);
    _renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
    _renderer->ResetCameraClippingRange();
}

/**
 * Show the back of the patient.
 */
void vtkVolumeRayCastingView::setCameraToBack()
{
    double center[3];
    _imageData->GetCenter(center);
    _renderer->GetActiveCamera()->SetFocalPoint(center);
    _renderer->GetActiveCamera()->SetPosition(center[0], center[1] + 800, center[2]);
    _renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
    _renderer->ResetCameraClippingRange();
}

void vtkVolumeRayCastingView::setCornerText(int position, std::string text)
{
    _cornerAnnotation->SetText(position, text.c_str());
}

