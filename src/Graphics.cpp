
#include "Graphics.hpp"

#include <numeric>
#include <cassert>
#include <string>
#include <iostream>

Form::Form (const boost::dynamic_bitset<> &form, const std::vector<unsigned int> dim):
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
  convert();
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
  _cubesCleanFilter->SetInputConnection(_cubesAppendFilter->GetOutputPort());
  _cubesCleanFilter->Update();

  //Create a mapper and actor
  _cubesMapper->SetInputConnection(_cubesCleanFilter->GetOutputPort());

  _cubesActor->SetMapper(_cubesMapper);
}

void Form::convert()
{
  //std::cout << _form.size() << std::endl;
  //std::cout << std::accumulate(_dim.begin(), _dim.end(), 1, std::multiplies<int>()) << std::endl;
  assert(_form.size()==std::accumulate(_dim.begin(), _dim.end(), 1, std::multiplies<int>()));
  assert(_dim.size() == 2 || _dim.size() == 3);
  
  convertPointGrid();
  if (_form.count() > 0)
    convertRectangleList();

}

vtkSmartPointer<vtkActor> Form::getPointsActor() const
{
  return _pointsActor;
}

vtkSmartPointer<vtkActor> Form::getCubesActor() const
{
  return _cubesActor;
}

Env::Env (std::vector<double> bgColor):
  _bgColor(bgColor),
  _forms(),
  renderer(vtkSmartPointer<vtkRenderer>::New()),
  renderWindow(vtkSmartPointer<vtkRenderWindow>::New()),
  renderWindowInteractor(vtkSmartPointer<vtkRenderWindowInteractor>::New())
{
  renderWindow->AddRenderer(renderer);
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderer->SetBackground(_bgColor[0], _bgColor[1], _bgColor[2]);
}

Env::~Env ()
{
}

void Env::addForm(Form *myForm)
{
  _forms.push_back(myForm);
  renderer->AddActor(myForm->getCubesActor());
  renderer->AddActor(myForm->getPointsActor());
}

//void Env::updateForms()
//{
  //renderer->Clear();
  //for (unsigned int i = 0; i < _forms.size(); ++i) {
    //_forms[i].convert();
    //renderer->AddActor(_forms[i].getCubesActor());
    //renderer->AddActor(_forms[i].getPointsActor());
  //}
//}

void Env::renderStart()
{
  renderWindow->Render();
  renderWindowInteractor->Start();
}
