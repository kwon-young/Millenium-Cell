
#include "Graphics.hpp"

#include <numeric>
#include <cassert>
#include <string>
#include <iostream>
#include <cstdlib>
#include <algorithm>

/*

Form::Form (boost::dynamic_bitset<> &form, std::vector<unsigned int> &dim):
  _form(form),
  _dim(dim),
  _pointsStructuredGrid(vtkSmartPointer<vtkStructuredGrid>::New()),
  _points(vtkSmartPointer<vtkPoints>::New()),
  _pointsGeometryFilter(vtkSmartPointer<vtkStructuredGridGeometryFilter>::New()),
  _pointsMapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
  _pointsActor(vtkSmartPointer<vtkActor>::New()),
  _inputs(new vtkPolyData*[form.size()]),
  _cubesSource(new vtkCubeSource*[form.count()]),
  _cubesAppendFilter(vtkSmartPointer<vtkAppendPolyData>::New()),
  _cubesCleanFilter(vtkSmartPointer<vtkCleanPolyData>::New()),
  _cubesMapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
  _cubesActor(vtkSmartPointer<vtkActor>::New())
{
  _cubesCleanFilter->SetInputConnection(_cubesAppendFilter->GetOutputPort());
  //Create a mapper and actor
  _cubesMapper->SetInputConnection(_cubesCleanFilter->GetOutputPort());
  _cubesActor->SetMapper(_cubesMapper);

#if VTK_MAJOR_VERSION <= 5
  _pointsGeometryFilter->SetInputConnection(_pointsStructuredGrid->GetProducerPort());
#else
  _pointsGeometryFilter->SetInputData(_pointsStructuredGrid);
#endif
  // Create a mapper and actor
  _pointsMapper->SetInputConnection(_pointsGeometryFilter->GetOutputPort());

  _pointsActor->SetMapper(_pointsMapper);
  _pointsActor->GetProperty()->SetPointSize(1);
  convertPointGrid();
  convertRectangleList();
}

Form::~Form ()
{
  if (_inputs)
    delete[] _inputs;
  if (_cubesSource)
    delete[] _cubesSource;
}

void Form::convertPointGrid()
{
  if (_dim.size() == 2) {
    for(unsigned int k = 0; k < _dim[2]; k++)
    {
      for(unsigned int j = 0; j < _dim[1]; j++)
      {
        _points->InsertNextPoint(j, k, 1);
      }
    }
  } else if (_dim.size() == 3) {
    for(unsigned int k = 0; k < _dim[2]; k++)
    {
      for(unsigned int j = 0; j < _dim[1]; j++)
      {
        for(unsigned int i = 0; i < _dim[0]; i++)
        {
          _points->InsertNextPoint(i, j, k);
        }
      }
    }
  }

  // Specify the dimensions of the grid
  if(_dim.size()==2)
    _pointsStructuredGrid->SetDimensions(_dim[0], _dim[1], 1);
  else if(_dim.size() == 3)
    _pointsStructuredGrid->SetDimensions(_dim[0], _dim[1], _dim[2]);
  _pointsStructuredGrid->SetPoints(_points);

  _pointsGeometryFilter->Update();

}

void Form::getXYZ(
    const unsigned int pos,
    unsigned int &x,
    unsigned int &y,
    unsigned int &z)
{
  x = (pos % (_dim[1]*_dim[0])) % _dim[1];
  y = (pos % (_dim[1]*_dim[0])) / _dim[1];
  z = pos / (_dim[1]*_dim[0]);
}

void Form::convertRectangleList()
{
  //std::cout << _form.size() << std::endl;
  //std::cout << std::accumulate(_dim.begin(), _dim.end(), 1, std::multiplies<int>()) << std::endl;
  assert(_form.size()==std::accumulate(_dim.begin(), _dim.end(), 1, std::multiplies<int>()));
  assert(_dim.size() == 2 || _dim.size() == 3);

  if (_form.count() > 0)
  {
    int pos = _form.find_first();
    unsigned int x, y, z;
    for (unsigned int i = 0; i < _form.count(); ++i) {
      if (pos != -1)
      {
        getXYZ(pos, x, y, z);
        //std::cout << "pos : " << pos << std::endl;
        //std::cout << x << std::endl;
        //std::cout << y << std::endl;
        //std::cout << z << std::endl;
        //std::cout << "end" << std::endl;
        _cubesSource[i] = vtkCubeSource::New();
        _cubesSource[i]->SetCenter(x, y, z);
        _cubesSource[i]->SetXLength(0.8);
        _cubesSource[i]->SetYLength(0.8);
        _cubesSource[i]->SetZLength(0.8);
        _cubesSource[i]->Update();
        _inputs[i] = vtkPolyData::New();
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

    // Remove any duplicate points.
    _cubesCleanFilter->Update();
  }
}

vtkSmartPointer<vtkActor> Form::getPointsActor() const
{
  return _pointsActor;
}

vtkSmartPointer<vtkActor> Form::getCubesActor() const
{
  return _cubesActor;
}

void KeypressCallbackFunction (
  vtkObject* caller,
  long unsigned int vtkNotUsed(eventId),
  void* clientData,
  void* vtkNotUsed(callData) )
{

  vtkRenderWindowInteractor *iren = 
    static_cast<vtkRenderWindowInteractor*>(caller);
 
  std::cout << "Pressed: " << iren->GetKeySym() << std::endl;

  Env *env = static_cast<Env*>(clientData);
  env->updateForms(iren->GetKeySym());

  env->Render();
  //// Close the window
  //iren->GetRenderWindow()->Finalize();
 
  //// Stop the interactor
  //iren->TerminateApp();
  //std::cout << "Closing window..." << std::endl;
  
}

Env::Env (
      std::vector<double> bgColor,
      std::vector<unsigned int> dim,
      Graph &g,
      std::vector<unsigned int> verticesPerTimestep) :
  _bgColor(bgColor),
  _dim(dim),
  _g(g),
  _verticesPerTimestep(verticesPerTimestep),
  _vertexPair(vertices(_g)),
  _forms(),
  _renderIndex(0),
  _renderer(vtkSmartPointer<vtkRenderer>::New()),
  _renderWindow(vtkSmartPointer<vtkRenderWindow>::New()),
  _renderWindowInteractor(vtkSmartPointer<vtkRenderWindowInteractor>::New()),
  _keypressCallback(vtkSmartPointer<vtkCallbackCommand>::New())
{
  _renderWindow->AddRenderer(_renderer);
  _renderWindowInteractor->SetRenderWindow(_renderWindow);
  _renderer->SetBackground(_bgColor[0], _bgColor[1], _bgColor[2]);

  _keypressCallback->SetCallback ( KeypressCallbackFunction );
  _keypressCallback->SetClientData(this);
  _renderWindowInteractor->AddObserver (
      vtkCommand::KeyPressEvent,
      _keypressCallback );
  
  int verticesNbr = std::accumulate(_verticesPerTimestep.begin(), _verticesPerTimestep.end(), 0);
  _forms.resize(verticesNbr, NULL);

  updateForms("");
  _renderer->AddActor(_forms[_renderIndex]->getCubesActor());
  _renderer->AddActor(_forms[_renderIndex]->getPointsActor());
}

Env::~Env ()
{
  for (int i = 0; i < _forms.size(); ++i) {
    if(_forms[i] != NULL) delete _forms[i];
  }
}

//void Env::addForm(Form *myForm)
//{
  //_forms.push_back(myForm);
  //_renderer->AddActor(myForm->getCubesActor());
  //_renderer->AddActor(myForm->getPointsActor());
//}

void Env::getFormFromGraph()
{
  _vertexPair = vertices(_g);
  for (int i = 0; i < _renderIndex; ++i) {
    ++_vertexPair.first;
  }
  boost::dynamic_bitset<> gform = _g[*_vertexPair.first];
  int maxCell = std::accumulate(_dim.begin(), _dim.end(), 1, std::multiplies<int>());
  if (gform.size() == (maxCell / 2)) gform.resize(gform.size()*2, 0);
  std::cout << gform.size() << std::endl;
  _forms[_renderIndex] = new Form(gform, _dim);
}

void Env::updateForms(std::string keySym)
{
  int inc = 0;
  if (!(keySym.compare("Right")))
    inc = 1;
  else if (!(keySym.compare("Left")))
    inc = -1;

  if (_renderIndex < 0 && _renderIndex >= _forms.size())
  {
    _renderIndex = 0;
  }
  if (_renderIndex+inc >= 0 && _renderIndex+inc < _forms.size())
  {
    _renderIndex+=inc;
    std::cout << "update : " << _renderIndex << std::endl;
  }
  _renderer->Clear();
  if(_forms[_renderIndex] == NULL) getFormFromGraph();

}

void Env::Render()
{
  _renderWindow->Render();
}

void Env::Start()
{
  _renderWindowInteractor->Start();
}

*/

void KeypressCallbackFunction (
  vtkObject* caller,
  long unsigned int vtkNotUsed(eventId),
  void* clientData,
  void* vtkNotUsed(callData) )
{
 
  vtkRenderWindowInteractor *iren = 
    static_cast<vtkRenderWindowInteractor*>(caller);
 
  std::string key = iren->GetKeySym();
  //std::cout << "Pressed: " << key << std::endl;

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
  _renderWindow->AddRenderer(_renderer);
  _renderWindowInteractor->SetRenderWindow(_renderWindow);
  _renderer->SetBackground(_bgColor[0], _bgColor[1], _bgColor[2]);

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
  if (_dim.size() == 2) {
    for(int k = 0; k < _dim[2]; k++)
    {
      for(int j = 0; j < _dim[1]; j++)
      {
        _points->InsertNextPoint(j, k, 1);
      }
    }
  } else if (_dim.size() == 3) {
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
  }

  // Specify the dimensions of the grid
  if(_dim.size()==2)
    _pointsStructuredGrid->SetDimensions(_dim[0], _dim[1], 2);
  else if(_dim.size() == 3)
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
  int maxForm = std::accumulate(
      _verticesPerTimestep.begin(),
      _verticesPerTimestep.end(),
      0);
  int inc = std::min(std::max(0, newFormIndex), maxForm-1) - _formIndex;
  _vertexPair.first += inc;
  _form = _g[*_vertexPair.first];
  int maxCell = std::accumulate(_dim.begin(), _dim.end(), 1, std::multiplies<int>());
  if (_form.size() != maxCell) _form.resize(maxCell, 0);
  _formIndex += inc;
  drawForm();
}

void GraphViewer::resizeCubes(int size)
{
  if (size > _cubesSource.size())
  {
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
  if (_form.count() != _cubesSource.size())
  {
    resizeCubes(_form.count());
  }
  int pos = _form.find_first();
  int x, y, z;
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
