#pragma once
#include <vtkSmartPointer.h>
#include <set>
#include <vtkObject.h>

class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkInteractorStyle;
class vtkObject;

class vtkPVRenderView: public vtkObject
{
public:
    vtkTypeMacro(vtkPVRenderView,vtkObject);

    static vtkPVRenderView *New();

    virtual ~vtkPVRenderView(void);

    void render();
    void observeEventOnInteractorStyle(unsigned long event);
    void observeEvent(vtkObject* object, unsigned long event);
    void removeObserveEvent(unsigned long event);
    vtkRenderWindow* getRenderWindow();
    void setRenderWindow(vtkRenderWindow* renderWindow);
    vtkRenderer* getRenderer();
    void setRenderer(vtkRenderer* renderer);
    vtkRenderWindowInteractor* getInteractor();
    void setInteractor(vtkRenderWindowInteractor* interactor);
    vtkInteractorStyle* getInteractorStyle();
    void setInteractorStyle(vtkInteractorStyle* interactorStyle);
    void initialize();
protected:

#define VIEW_MAP_BEGIN	\
	virtual void defaultEventHandler(vtkObject *caller, unsigned long event, void *callData)	\
	{																							\
		/* do nothing if*/																		\
		if(0) ;

#define ON_EVENT(ObserveEvent, EventMethod)			\
		else if(event == ObserveEvent) {			\
			EventMethod(caller, event, callData);	\
			DEFAULT_INVOKE_EVENT					\
			return;									\
		}

#define VIEW_MAP_END	\
	this->Superclass::defaultEventHandler(caller, event, callData);	\
		 }

#define DEFAULT_INVOKE_EVENT	\
	if(HasObserver(event))					\
		 this->InvokeEvent(event, callData);

    vtkPVRenderView(void);

    virtual void installPipeline();
    virtual void installEvents();

    virtual void processEvent(vtkObject *caller, unsigned long event, void *callData);
    virtual void defaultEventHandler(vtkObject *caller, unsigned long event, void *callData);
    vtkSmartPointer<vtkRenderWindow> _renderWindow;
    vtkSmartPointer<vtkRenderer> _renderer;
    vtkSmartPointer<vtkRenderWindowInteractor> _interactor;
    vtkSmartPointer<vtkInteractorStyle>   _interactorStyle;

    std::set<unsigned long> _eventSet;
};

