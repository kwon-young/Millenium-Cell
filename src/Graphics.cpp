
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
  _cubesSource(),
  _inputs(),
  _cubesAppendFilter(vtkSmartPointer<vtkAppendPolyData>::New()),
  _cubesCleanFilter(vtkSmartPointer<vtkCleanPolyData>::New()),
  _cubesMapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
  _cubesActor(vtkSmartPointer<vtkActor>::New()),
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
  setFormIndex(0);
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
  _cubesCleanFilter->SetInputConnection(_cubesAppendFilter->GetOutputPort());
  _cubesMapper->SetInputConnection(_cubesCleanFilter->GetOutputPort());
  _cubesActor->SetMapper(_cubesMapper);
  _renderer->AddActor(_cubesActor);
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
  if (size > _cubesSource.size())
  {
    // Add cubes
    int diff = size-_cubesSource.size();
    for (int i = 0; i < diff; ++i) {
      _cubesSource.push_back(vtkCubeSource::New());
      _inputs.push_back(vtkPolyData::New());
#if VTK_MAJOR_VERSION <= 5
      _cubesAppendFilter->AddInputConnection((*(_inputs.end()))->GetProducerPort());
#else
      _cubesAppendFilter->AddInputData(_inputs[_inputs.size()-1]);
#endif
    }
  } else if (size < _cubesSource.size())
  {
    // Remove cubes
    int diff = (size-_cubesSource.size());
    diff = std::abs(diff);
    for (int i = 0; i < diff; ++i) {
      if (_inputs.size() > 1)
      {
        _cubesAppendFilter->RemoveInputData(_inputs[_inputs.size()-1]);
        _inputs.pop_back();
        _cubesSource.pop_back();
      }
    }
  }
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

void GraphViewer::drawForm()
{
  // Check the number of cubes, resize if different
  if (_form.count() != _cubesSource.size())
  {
    resizeCubes(_form.count());
  }
  // pos is used for the index of the current cube
  int pos = _form.find_first();
  int x, y, z;

  // Setup scales. This can also be an Int array
  // char is used since it takes the least memory
  vtkSmartPointer<vtkUnsignedCharArray> colors = 
    vtkSmartPointer<vtkUnsignedCharArray>::New();
  colors->SetName("colors");
  colors->SetNumberOfComponents(3);
  unsigned char r[3] = {255,0,0};
  unsigned char g[3] = {0,255,0};
  unsigned char b[3] = {0,0,255};
  colors->InsertNextTupleValue(r);
  colors->InsertNextTupleValue(g);
  colors->InsertNextTupleValue(b);

  for (unsigned int i = 0; i < _cubesSource.size(); ++i) {
    if (pos != -1)
    {
      getXYZ(pos, x, y, z);
      //std::cout << "pos : " << pos << std::endl;
      //std::cout << x << std::endl;
      //std::cout << y << std::endl;
      //std::cout << z << std::endl;
      //std::cout << "end" << std::endl;
      _cubesSource[i]->SetCenter(x, y, z);
      _cubesSource[i]->SetXLength(0.8);
      _cubesSource[i]->SetYLength(0.8);
      _cubesSource[i]->SetZLength(0.8);
      _cubesSource[i]->Update();
      _inputs[i]->ShallowCopy(_cubesSource[i]->GetOutput());
#if VTK_MAJOR_VERSION <= 5
      _cubesAppendFilter->AddInputConnection(_inputs[i]->GetProducerPort());
#else
      _cubesAppendFilter->AddInputData(_inputs[i]);
#endif
    }
    pos = _form.find_next(pos);
  }
  _cubesAppendFilter->Update();
  _cubesCleanFilter->Update();
  _renderWindow->Render();

}
