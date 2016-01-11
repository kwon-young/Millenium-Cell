
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
#include <string>

#include "environment.h"

/*

class Form
{
public:
  Form (boost::dynamic_bitset<> &form, std::vector<unsigned int> &dim);
  virtual ~Form ();

  void setForm(vtkSmartPointer<vtkAppendPolyData> cubeAppendFilter);
private:

  void getXYZ(
      const unsigned int pos,
      unsigned int &x,
      unsigned int &y,
      unsigned int &z);

  boost::dynamic_bitset<> _form;
  std::vector<unsigned int> _dim;

  vtkPolyData **_inputs;
  vtkCubeSource **_cubesSource;
};

void KeypressCallbackFunction (
  vtkObject* caller,
  long unsigned int eventId,
  void* clientData,
  void* callData );

class Env
{
public:
  Env (
      std::vector<double> bgColor,
      std::vector<unsigned int> dim,
      Graph &g,
      std::vector<unsigned int> verticesPerTimestep);
  virtual ~Env ();

  //void addForm(Form *myForm);

  void getFormFromGraph();
  void updateForms(std::string keySym);

  void Render();
  void Start();

private:

  void convertPointGrid();

  std::vector<double> _bgColor;
  std::vector<unsigned int> _dim;
  Graph &_g;
  std::vector<unsigned int> _verticesPerTimestep;
  pair< vertex_iter, vertex_iter > _vertexPair;
  std::vector<Form*> _forms;
  unsigned int _renderIndex;

  vtkSmartPointer<vtkStructuredGrid> _pointsStructuredGrid;
  vtkSmartPointer<vtkPoints> _points;
  vtkSmartPointer<vtkStructuredGridGeometryFilter> _pointsGeometryFilter;
  vtkSmartPointer<vtkPolyDataMapper> _pointsMapper;
  vtkSmartPointer<vtkActor> _pointsActor;

  vtkSmartPointer<vtkAppendPolyData> _cubesAppendFilter;
  vtkSmartPointer<vtkCleanPolyData> _cubesCleanFilter;
  vtkSmartPointer<vtkPolyDataMapper> _cubesMapper;
  vtkSmartPointer<vtkActor> _cubesActor;

  vtkSmartPointer<vtkRenderer> _renderer;
  vtkSmartPointer<vtkRenderWindow> _renderWindow;
  vtkSmartPointer<vtkRenderWindowInteractor> _renderWindowInteractor;
  vtkSmartPointer<vtkCallbackCommand> _keypressCallback;
};

*/

class GraphViewer
{
public:
  GraphViewer (
      Graph &g,
      std::vector<int> verticesPerTimestep,
      std::vector<double> bgColor,
      std::vector<int> dim);
  virtual ~GraphViewer ();

  int getFormIndex() const;
  void setFormIndex(int newFormIndex);

  void Render();
  void Start();
private:

  void convertPointGrid();
  void initCubes();
  void getXYZ(
      const int pos,
      int &x,
      int &y,
      int &z);
  void resizeCubes(int size);
  void drawForm();
  void updateCubes();

  /* data */
  Graph &_g;
  std::vector<int> _verticesPerTimestep;
  std::vector<double> _bgColor;
  std::vector<int> _dim;

  vtkSmartPointer<vtkRenderer> _renderer;
  vtkSmartPointer<vtkRenderWindow> _renderWindow;
  vtkSmartPointer<vtkRenderWindowInteractor> _renderWindowInteractor;
  vtkSmartPointer<vtkCallbackCommand> _keypressCallback;

  vtkSmartPointer<vtkStructuredGrid> _pointsStructuredGrid;
  vtkSmartPointer<vtkPoints> _points;
  vtkSmartPointer<vtkStructuredGridGeometryFilter> _pointsGeometryFilter;
  vtkSmartPointer<vtkPolyDataMapper> _pointsMapper;
  vtkSmartPointer<vtkActor> _pointsActor;

  std::vector<vtkCubeSource*> _cubesSource;
  std::vector<vtkPolyData*> _inputs;
  vtkSmartPointer<vtkAppendPolyData> _cubesAppendFilter;
  vtkSmartPointer<vtkCleanPolyData> _cubesCleanFilter;
  vtkSmartPointer<vtkPolyDataMapper> _cubesMapper;
  vtkSmartPointer<vtkActor> _cubesActor;

  int _formIndex;
  boost::dynamic_bitset<> _form;
  pair< vertex_iter, vertex_iter > _vertexPair;
};
