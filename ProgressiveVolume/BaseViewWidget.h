#pragma once
#include <QVTKWidget.h>


class vtkPVRenderView;
class vtkInteractorStyle;

#define COMMON_RENDER

class BaseViewWidget :
	public QVTKWidget
{
public:
	BaseViewWidget(QWidget* parent, Qt::WFlags f = 0);
	virtual ~BaseViewWidget(void);

protected:
	virtual void prepareView(vtkPVRenderView* view, vtkInteractorStyle* style);
	virtual void createView() = 0;
	virtual void render() = 0;
};

