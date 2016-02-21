#include "VolumeRaycastingWidget.h"
#include "vtkVolumeRayCastingView.h"
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkObjectFactory.h>



VolumeRaycastingWidget::VolumeRaycastingWidget(QWidget* parent, Qt::WFlags f /*= 0*/)
    :BaseViewWidget(parent, f)
{
    _renderView = NULL;
}


VolumeRaycastingWidget::~VolumeRaycastingWidget(void)
{
    if(_renderView)
        _renderView->Delete();
}

void VolumeRaycastingWidget::initialize(vtkImageData* data)
{
    createView();

    setInput(data);

    prepareView(_renderView, vtkInteractorStyleTrackballCamera::New());

    _renderView->initialize();
}


void VolumeRaycastingWidget::setInput(vtkImageData* data)
{
    _renderView->setInput(data);
}


void VolumeRaycastingWidget::setBlendTypeToBone()
{
    _renderView->setBlendTypeToBone();
}

void VolumeRaycastingWidget::setCameraToFront()
{
    _renderView->setCameraToFront();
}

void VolumeRaycastingWidget::setBlendTypeToCTSkin()
{
    _renderView->setBlendTypeToCTSkin();
}

void VolumeRaycastingWidget::setCameraToBack()
{
    _renderView->setCameraToBack();
}

void VolumeRaycastingWidget::createView()
{
    _renderView = vtkVolumeRayCastingView::New();
}

void VolumeRaycastingWidget::setProgress(QString title)
{
    _renderView->setCornerText(1, title.toStdString());
}

void VolumeRaycastingWidget::setState(QString title)
{
    _renderView->setCornerText(0, title.toStdString());
}

void VolumeRaycastingWidget::render()
{
    if(this->isVisible()) {
        _renderView->updateRender();
        _renderView->render();
    }
}

vtkImageData* VolumeRaycastingWidget::getInput()
{
    return _renderView->getInput();
}

