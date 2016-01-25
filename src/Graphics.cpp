
#include "Graphics.hpp"

#include <numeric>
#include <cassert>
#include <string>
#include <iostream>
#include <cstdlib>
#include <algorithm>

/* -----------------------------------------------------------*/
/** 
 * @brief Callback used to react to keypress in the vtk windows
 * 
 * @param caller
 * @param eventId
 * @param clientData
 * @param callData
 */
/* -----------------------------------------------------------*/
void KeypressCallbackFunction (
  vtkObject* caller,
  long unsigned int vtkNotUsed(eventId),
  void* clientData,
  void* vtkNotUsed(callData) )
{
 
  // Get the current window interactor
  vtkRenderWindowInteractor *iren = 
    static_cast<vtkRenderWindowInteractor*>(caller);
 
  // Get the keysym pressed
  std::string key = iren->GetKeySym();
  //std::cout << "Pressed: " << key << std::endl;

  /**
   * For now, you can press right or left to iterate through all forms in the graph
   */
  GraphViewer *gv = static_cast<GraphViewer*>(clientData);
  if (!key.compare("Left"))
    gv->setFormIndex(gv->getFormIndex()-1);
  else if (!key.compare("Right"))
    gv->setFormIndex(gv->getFormIndex()+1);
  //Env *env = static_cast<Env*>(clientData);
  //env->updateForms(iren->GetKeySym());

  //env->Render();
  //// Close the window
  //iren->GetRenderWindow()->Finalize();
 
  //// Stop the interactor
  //iren->TerminateApp();
  //std::cout << "Closing window..." << std::endl;
  
}

GraphViewer::GraphViewer (
    Graph &g,
    std::vector<int> verticesPerTimestep,
    std::vector<double> bgColor,
    std::vector<int> dim) :
  _g(g),
  _verticesPerTimestep(verticesPerTimestep),
  _bgColor(bgColor),
  _dim(dim),
  _renderer(vtkSmartPointer<vtkRenderer>::New()),
  _renderWindow(vtkSmartPointer<vtkRenderWindow>::New()),
  _renderWindowInteractor(vtkSmartPointer<vtkRenderWindowInteractor>::New()),
  _keypressCallback(vtkSmartPointer<vtkCallbackCommand>::New()),
  _pointsStructuredGrid(vtkSmartPointer<vtkStructuredGrid>::New()),
  _points(vtkSmartPointer<vtkPoints>::New()),
  _pointsGeometryFilter(vtkSmartPointer<vtkStructuredGridGeometryFilter>::New()),
  _pointsMapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
  _pointsActor(vtkSmartPointer<vtkActor>::New()),
  _cubePoints(vtkSmartPointer<vtkPoints>::New()),
  _cubeColors(vtkSmartPointer<vtkUnsignedCharArray>::New()),
  _cubePolyData(vtkSmartPointer<vtkPolyData>::New()),
  _cubeSource(vtkSmartPointer<vtkCubeSource>::New()),
  _cubeGlyph3D(vtkSmartPointer<vtkGlyph3D>::New()),
  _cubeMapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
  _cubeActor(vtkSmartPointer<vtkActor>::New()),
  _formIndex(0),
  _form(),
  _vertexPair(vertices(_g))
{
  // Set the object flow for the vtk window
  _renderWindow->AddRenderer(_renderer);
  _renderWindowInteractor->SetRenderWindow(_renderWindow);
  _renderer->SetBackground(_bgColor[0], _bgColor[1], _bgColor[2]);

  // Setup the keypress interactor
  _keypressCallback->SetCallback ( KeypressCallbackFunction );
  _keypressCallback->SetClientData(this);
  _renderWindowInteractor->AddObserver (
      vtkCommand::KeyPressEvent,
      _keypressCallback );

  convertPointGrid();

  initCubes();
  setFormIndex(10);
}

GraphViewer::~GraphViewer ()
{
}

void GraphViewer::Render()
{
  _renderWindow->Render();
}

void GraphViewer::Start()
{
  _renderWindowInteractor->Start();
}

void GraphViewer::convertPointGrid()
{
  if (_dim[2] == 1) _dim[2]++;


  // Iterate through all dimension to set point
  for(int k = 0; k < _dim[2]; k++)
  {
    for(int j = 0; j < _dim[1]; j++)
    {
      for(int i = 0; i < _dim[0]; i++)
      {
        _points->InsertNextPoint(i, j, k);
      }
    }
  }

  // Specify the dimensions of the grid
  _pointsStructuredGrid->SetDimensions(_dim[0], _dim[1], _dim[2]);
  _pointsStructuredGrid->SetPoints(_points);

#if VTK_MAJOR_VERSION <= 5
  _pointsGeometryFilter->SetInputConnection(_pointsStructuredGrid->GetProducerPort());
#else
  _pointsGeometryFilter->SetInputData(_pointsStructuredGrid);
#endif
  _pointsGeometryFilter->Update();
  // Create a mapper and actor
  _pointsMapper->SetInputConnection(_pointsGeometryFilter->GetOutputPort());

  _pointsActor->SetMapper(_pointsMapper);
  _pointsActor->GetProperty()->SetPointSize(1);
  _renderer->AddActor(_pointsActor);

}

void GraphViewer::initCubes()
{
  _cubeColors->SetName("_cubeColors");
  // Combine into a _cubePolyData
  _cubePolyData->SetPoints(_cubePoints);
  _cubePolyData->GetPointData()->SetScalars(_cubeColors);
  _cubeGlyph3D->SetColorModeToColorByScalar();
  _cubeGlyph3D->SetSourceConnection(_cubeSource->GetOutputPort());
#if VTK_MAJOR_VERSION <= 5
  _cubeGlyph3D->SetInput(_cubePolyData);
#else
  _cubeGlyph3D->SetInputData(_cubePolyData);
#endif
  _cubeGlyph3D->ScalingOff();
  // Create a _cubeMapper and _cubeActor
  _cubeMapper->SetInputConnection(_cubeGlyph3D->GetOutputPort());
  _cubeActor->SetMapper(_cubeMapper);
 
  _renderer->AddActor(_cubeActor);
}

int GraphViewer::getFormIndex() const
{
  return _formIndex;
}

void GraphViewer::setFormIndex(int newFormIndex)
{
  // Count the total number of form in the graph
  int maxForm = std::accumulate(
      _verticesPerTimestep.begin(),
      _verticesPerTimestep.end(),
      0);
  // Count the difference from the current form index and the new form index
  int inc = std::min(std::max(0, newFormIndex), maxForm-1) - _formIndex;
  // Get the new form
  _vertexPair.first += inc;
  _form = _g[*_vertexPair.first];
  // Count the maximum number of cell that the space can contains
  int maxCell = std::accumulate(_dim.begin(), _dim.end(), 1, std::multiplies<int>());
  // Verify if the number of cell in the form is correct
  if (_form.size() != maxCell) _form.resize(maxCell, 0);
  // Set the new index
  _formIndex += inc;

  drawForm();
}

void GraphViewer::resizeCubes(int size)
{
  _cubePoints->SetNumberOfPoints(size);
  _cubeColors->SetNumberOfComponents(3);
  _cubeColors->SetNumberOfTuples(size);
}

void GraphViewer::getXYZ(
    const int pos,
    int &x,
    int &y,
    int &z)
{
  x = (pos % (_dim[1]*_dim[0])) % _dim[1];
  y = (pos % (_dim[1]*_dim[0])) / _dim[1];
  z = pos / (_dim[1]*_dim[0]);
}
void OutputPoints(vtkSmartPointer<vtkPoints> points)
{
  for(vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
    {
    double p[3];
    points->GetPoint(i,p);
    cout << p[0] << " " << p[1] << " " << p[2] << endl;
    }
}
void GraphViewer::drawForm()
{
  unsigned char r[3] = {255,0,0};
 
   //Check the number of cubes, resize if different
  if (_form.count() != _cubePoints->GetNumberOfPoints())
  {
    resizeCubes(_form.size());
  }
  // pos is used for the index of the current cube
  int pos = _form.find_first(), firstPos = 0;
  int x, y, z;

  std::cout << "New Form at index : " << _formIndex << std::endl;
  unsigned char color[3] = {255, 0, 0};
  for (vtkIdType i = 0; i < _cubePoints->GetNumberOfPoints(); ++i) {
    //std::cout << "find 1 at " << pos << std::endl;
    if (pos != -1)
    {
      if (i == 0) firstPos = pos;
      getXYZ(pos, x, y, z);
      _cubePoints->SetPoint(i, x, y, z);
      _cubeColors->SetTupleValue(i, color);
    } else {
      getXYZ(firstPos, x, y, z);
      _cubePoints->SetPoint(i, x, y, z);
      _cubeColors->SetTupleValue(i, color);
    }

    pos = _form.find_next(pos);
  }
  OutputPoints(_cubePoints);
  _cubeGlyph3D->Update();
  _renderWindow->Render();
  //std::cout << _cubePoints->GetNumberOfPoints() << std::endl;
  //std::cout << _cubeColors->GetNumberOfPoints() << std::endl;

}
