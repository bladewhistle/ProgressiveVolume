#include "vtkPVRenderView.h"

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkObject.h>
#include <vtkInteractorStyle.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(vtkPVRenderView);


vtkPVRenderView::vtkPVRenderView(void)
{
	
}


vtkPVRenderView::~vtkPVRenderView(void)
{
}

void vtkPVRenderView::render()
{
	if(_interactor)
		_interactor->Render();
}

void vtkPVRenderView::initialize()
{
	installPipeline();
	installEvents();
}


void vtkPVRenderView::installEvents()
{
	if (this->_renderWindow && this->_renderer)
	{
		this->_renderWindow->AddRenderer(this->_renderer);
	}

	if (this->_interactor)
	{
		if(this->_interactorStyle)
		{
			for(std::set<unsigned long>::iterator iter=_eventSet.begin(); iter != _eventSet.end(); iter++)  
				this->_interactorStyle->AddObserver(*iter, this, &vtkPVRenderView::processEvent);	
		}

		this->_interactor->SetInteractorStyle(this->_interactorStyle);
		this->_interactor->SetRenderWindow(this->_renderWindow);
	}
}

void vtkPVRenderView::installPipeline()
{
	
}

void vtkPVRenderView::processEvent( vtkObject *caller, unsigned long event, void *callData )
{
	defaultEventHandler(caller, event, callData);
}

void vtkPVRenderView::setRenderWindow( vtkRenderWindow* renderWindow )
{
	_renderWindow = renderWindow;
}

vtkRenderWindow* vtkPVRenderView::getRenderWindow()
{
	return _renderWindow;
}

vtkRenderer* vtkPVRenderView::getRenderer()
{
	return _renderer;
}

void vtkPVRenderView::setRenderer( vtkRenderer* renderer )
{
	this->_renderer = renderer;
}

void vtkPVRenderView::setInteractor( vtkRenderWindowInteractor* interactor )
{
	this->_interactor = interactor;
}

vtkInteractorStyle* vtkPVRenderView::getInteractorStyle()
{
	return _interactorStyle;
}

void vtkPVRenderView::setInteractorStyle( vtkInteractorStyle* interactorStyle )
{
	this->_interactorStyle = interactorStyle;
}

vtkRenderWindowInteractor* vtkPVRenderView::getInteractor()
{
	return _interactor;
}

void vtkPVRenderView::defaultEventHandler( vtkObject *caller, unsigned long event, void *callData )
{
	DEFAULT_INVOKE_EVENT
}

void vtkPVRenderView::observeEventOnInteractorStyle( unsigned long event )
{
	_eventSet.insert(event);
}

void vtkPVRenderView::removeObserveEvent( unsigned long event )
{
	_eventSet.erase(event);
}

void vtkPVRenderView::observeEvent( vtkObject* object, unsigned long event )
{
	object->AddObserver(event, this, &vtkPVRenderView::processEvent);
}



