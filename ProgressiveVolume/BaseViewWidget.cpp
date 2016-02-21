#include "BaseViewWidget.h"
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include "vtkPVRenderView.h"
#include <vtkRenderWindow.h>
#include <vtkInteractorStyle.h>


BaseViewWidget::BaseViewWidget(QWidget* parent, Qt::WFlags f)
    :QVTKWidget(parent, f)
{
}


BaseViewWidget::~BaseViewWidget(void)
{
}

void BaseViewWidget::prepareView(vtkPVRenderView* view, vtkInteractorStyle* style)
{
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    view->setRenderWindow(GetRenderWindow());
    view->setRenderer(renderer);
    view->setInteractor(GetRenderWindow()->GetInteractor());
    GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
    view->setInteractorStyle(style);

    GetRenderWindow()->SetDesiredUpdateRate(5);
}
