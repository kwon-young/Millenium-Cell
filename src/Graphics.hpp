
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>
#include <vtkXMLStructuredGridWriter.h>
#include <vtkMath.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkCubeSource.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkPoints.h>

#include <boost/dynamic_bitset.hpp>

#include <vector>

class Form
{
public:
  Form (const boost::dynamic_bitset<> &form, const std::vector<unsigned int> dim);
  virtual ~Form ();

  void convert();
  vtkSmartPointer<vtkActor> getPointsActor() const;
  vtkSmartPointer<vtkActor> getCubesActor() const;

private:

  void convertPointGrid();
  void convertRectangleList();
  void getXYZ(
      const unsigned int pos,
      unsigned int &x,
      unsigned int &y,
      unsigned int &z);

  /* data */
  boost::dynamic_bitset<> _form;
  std::vector<unsigned int> _dim;

  vtkSmartPointer<vtkStructuredGrid> _pointsStructuredGrid;
  vtkSmartPointer<vtkPoints> _points;
  vtkSmartPointer<vtkStructuredGridGeometryFilter> _pointsGeometryFilter;
  vtkSmartPointer<vtkPolyDataMapper> _pointsMapper;
  vtkSmartPointer<vtkActor> _pointsActor;

  vtkPolyData **_inputs;
  vtkCubeSource **_cubesSource;
  vtkSmartPointer<vtkAppendPolyData> _cubesAppendFilter;
  vtkSmartPointer<vtkCleanPolyData> _cubesCleanFilter;
  vtkSmartPointer<vtkPolyDataMapper> _cubesMapper;
  vtkSmartPointer<vtkActor> _cubesActor;
};

void KeypressCallbackFunction (
  vtkObject* caller,
  long unsigned int eventId,
  void* clientData,
  void* callData );

class Env
{
public:
  Env (std::vector<double> bgColor);
  virtual ~Env ();

  void addForm(Form *myForm);

  //void updateForms();

  void renderStart();

private:
  /* data */
  std::vector<double> _bgColor;
  std::vector<Form*> _forms;

  vtkSmartPointer<vtkRenderer> _renderer;
  vtkSmartPointer<vtkRenderWindow> _renderWindow;
  vtkSmartPointer<vtkRenderWindowInteractor> _renderWindowInteractor;
  vtkSmartPointer<vtkCallbackCommand> _keypressCallback;
};
