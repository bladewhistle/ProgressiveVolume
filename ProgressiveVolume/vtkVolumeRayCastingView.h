#pragma once
#include "vtkPVRenderView.h"

#include <vtkActor.h>
#include <vtkCornerAnnotation.h>

#include <map>
#include <vector>


class vtkImageData;
class vtkVolume;
class vtkGPUVolumeRayCastMapper;

class vtkVolumeRayCastingView :
    public vtkPVRenderView
{
public:
    vtkTypeMacro(vtkVolumeRayCastingView, vtkPVRenderView);
    static vtkVolumeRayCastingView *New();

    virtual ~vtkVolumeRayCastingView(void);

    void setBlendTypeToCTSkin();
    void setBlendTypeToBone();

    void setCameraToFront() ;
    void setCameraToBack();

    void setCornerText(int position, std::string text);

    virtual void setInput(vtkImageData *in);
    virtual vtkImageData *getInput();

    void updateRender();

protected:
    vtkSmartPointer<vtkImageData> _imageData;

protected:

    vtkSmartPointer<vtkVolume> _volume;
    vtkSmartPointer<vtkGPUVolumeRayCastMapper> _volumeMapper;

    vtkSmartPointer<vtkCornerAnnotation> _cornerAnnotation;

    vtkVolumeRayCastingView(void);

    virtual void installPipeline();




};
