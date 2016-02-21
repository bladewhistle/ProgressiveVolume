#pragma once
#include "BaseViewWidget.h"


class vtkVolumeRayCastingView;

class VolumeRaycastingWidget :
    public BaseViewWidget
{
    Q_OBJECT
public:
    VolumeRaycastingWidget(QWidget* parent, Qt::WFlags f = 0);
    virtual ~VolumeRaycastingWidget(void);

    virtual void initialize(vtkImageData* data);

    void setInput(vtkImageData* data);
    vtkImageData* getInput();
    void setBlendTypeToBone();
    void setBlendTypeToCTSkin();

    void setCameraToFront();
    void setCameraToBack();

    virtual void createView();

    void setProgress(QString title);
    void setState(QString title);

public slots:
    virtual void render();

protected:
    vtkVolumeRayCastingView* _renderView ;
};

