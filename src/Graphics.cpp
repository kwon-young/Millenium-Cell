
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
  else if (!key.compare("Up"))
    gv->setFormIndex(gv->getFormIndex()+10);
  else if (!key.compare("Down"))
    gv->setFormIndex(gv->getFormIndex()-10);
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
    GraphManager &gm,
    std::vector<double> bgColor,
    std::vector<int> dim) :
  _gm(gm),
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
  _concPoints(vtkSmartPointer<vtkPoints>::New()),
  _concColors(vtkSmartPointer<vtkUnsignedCharArray>::New()),
  _concScales(vtkSmartPointer<vtkFloatArray>::New()),
  _concPolyData(vtkSmartPointer<vtkPolyData>::New()),
  _concSource(vtkSmartPointer<vtkCubeSource>::New()),
  _concGlyph3D(vtkSmartPointer<vtkGlyph3D>::New()),
  _concMapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
  _concActor(vtkSmartPointer<vtkActor>::New()),
  _formIndex(0),
  _form(),
  _EForm(),
  _OForm(),
  _GForm(),
  _LForm()
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
  _cubeColors->SetName("_cubeColors");
  // Combine into a _cubePolyData
  _cubePolyData->SetPoints(_cubePoints);
  _cubePolyData->GetPointData()->SetScalars(_cubeColors);
  _cubeGlyph3D->SetColorModeToColorByScalar();
  _cubeSource->SetXLength(0.8);
  _cubeSource->SetYLength(0.8);
  _cubeSource->SetZLength(0.8);
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

  _concColors->SetName("_concColors");
  _concScales->SetName("_concScales");
  // Combine into a _concPolyData
  _concPolyData->SetPoints(_concPoints);
  _concPolyData->GetPointData()->SetScalars(_concColors);
  //_concPolyData->GetPointData()->SetScalars(_concScales);
  _concGlyph3D->SetColorModeToColorByScalar();
  //_concGlyph3D->SetScaleModeToScaleByScalar();
  _concSource->SetXLength(0.4);
  _concSource->SetYLength(0.4);
  _concSource->SetZLength(0.4);
  _concGlyph3D->SetSourceConnection(_concSource->GetOutputPort());
#if VTK_MAJOR_VERSION <= 5
  _concGlyph3D->SetInput(_concPolyData);
#else
  _concGlyph3D->SetInputData(_concPolyData);
#endif
  _concGlyph3D->ScalingOff();
  // Create a _concMapper and _concActor
  _concMapper->SetInputConnection(_concGlyph3D->GetOutputPort());
  _concActor->SetMapper(_concMapper);
 
  _renderer->AddActor(_concActor);
}

int GraphViewer::getFormIndex() const
{
  return _formIndex;
}

void GraphViewer::setFormIndex(int newFormIndex)
{
  // Count the total number of form in the graph
  int maxForm = _gm.getMaxNbrOfForm();
  // Count the difference from the current form index and the new form index
  newFormIndex = std::min(std::max(0, newFormIndex), maxForm-1);
  _formIndex = newFormIndex;
  _gm.getFormFromGraph(newFormIndex, _gm.getGForm(), _form, _EForm, _OForm, _GForm, _LForm);

  drawForm();
}

void GraphViewer::resizeCubes(int size)
{
  _cubePoints->SetNumberOfPoints(size);
  _cubeColors->SetNumberOfComponents(3);
  _cubeColors->SetNumberOfTuples(size);

  _concPoints->SetNumberOfPoints(size*4);
  _concColors->SetNumberOfComponents(3);
  _concColors->SetNumberOfTuples(size*4);
  //_concScales->SetNumberOfValues(size*4);
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

void printColor(const std::vector<unsigned char> &color)
{
  std::cout << "color[r, g, b] : " << std::dec << (int)color[0] << " "
                                   << std::dec << (int)color[1] << " "
                                   << std::dec << (int)color[2] << std::endl;
}

void GraphViewer::linearColorGradient(
    const std::vector<double> &compConcentration,
    int pos,
    std::vector<unsigned char> &color)
{
  assert(color.size() == 3);
  //std::cout << "cc : " << compConcentration.size() << std::endl;
  //std::cout << "pos" << pos << std::endl;
  assert(compConcentration.size() > pos);
  //double maxi = *(std::max_element(compConcentration.begin(), compConcentration.end()));
  double maxi = 30;
  maxi = std::max(maxi, (double)0);
  double currentCon = compConcentration[pos];
  //std::cout << "maxi : " << maxi << std::endl;
  color[0] = (unsigned char)std::min((std::max(currentCon, (double)0)*255/maxi), (double)255);
  color[1] = 0;
  color[2] = 255 - color[0];
}

float GraphViewer::linearScaleGradient(
    const std::vector<double> &compConcentration,
    int pos)
{
  //std::cout << "cc : " << compConcentration.size() << std::endl;
  //std::cout << "pos" << pos << std::endl;
  assert(compConcentration.size() > pos);
  double maxi = *(std::max_element(compConcentration.begin(), compConcentration.end()));
  maxi = std::max(maxi, (double)0);
  double currentCon = compConcentration[pos];
  //std::cout << "maxi : " << maxi << std::endl;
  return std::max(currentCon, (double)0)/maxi;
}
void GraphViewer::drawForm()
{
   //Check the number of cubes, resize if different
  if (_form.count() != _cubePoints->GetNumberOfPoints())
  {
    resizeCubes(_form.size());
  }
  // pos is used for the index of the current cube
  int pos = _form.find_first(), firstPos = 0;
  int x, y, z;

  std::cout << "New Form at index : " << _formIndex << std::endl;
  int maxNbrPoints = _cubePoints->GetNumberOfPoints();
  for (vtkIdType i = 0; i < maxNbrPoints; ++i) {
    //std::cout << "find 1 at " << pos << std::endl;
    
    std::vector<unsigned char> color(3, 0);
    color[0] = 0;
    color[1] = 255;
    color[2] = 0;
    float f;
    
    if (pos != -1)
    {
      if (i == 0) firstPos = pos;
      getXYZ(pos, x, y, z);
      //printColor(color);
    } else {
      getXYZ(firstPos, x, y, z);
    }
    _cubePoints->SetPoint(i, x, y, z);
    _cubeColors->SetTupleValue(i, &color[0]);
    pos = _form.find_next(pos);

    linearColorGradient(_EForm, i, color);
    //f = linearScaleGradient(_EForm, i);
    getXYZ(i+2*maxNbrPoints, x, y, z);
    _concPoints->SetPoint(i, x, y, z);
    _concColors->SetTupleValue(i, &color[0]);
    //_concScales->SetValue(i, f);

    linearColorGradient(_OForm, i, color);
    //f = linearScaleGradient(_OForm, i);
    getXYZ(i+3*maxNbrPoints, x, y, z);
    _concPoints->SetPoint(i+1*maxNbrPoints, x, y, z);
    _concColors->SetTupleValue(i+1*maxNbrPoints, &color[0]);
    //_concScales->SetValue(i+1*maxNbrPoints, f);

    linearColorGradient(_GForm, i, color);
    //f = linearScaleGradient(_GForm, i);
    getXYZ(i+4*maxNbrPoints, x, y, z);
    _concPoints->SetPoint(i+2*maxNbrPoints, x, y, z);
    _concColors->SetTupleValue(i+2*maxNbrPoints, &color[0]);
    //_concScales->SetValue(i+2*maxNbrPoints, f);

    linearColorGradient(_LForm, i, color);
    //f = linearScaleGradient(_LForm, i);
    getXYZ(i+5*maxNbrPoints, x, y, z);
    _concPoints->SetPoint(i+3*maxNbrPoints, x, y, z);
    _concColors->SetTupleValue(i+3*maxNbrPoints, &color[0]);
    //_concScales->SetValue(i+2*maxNbrPoints, f);
  }
  //OutputPoints(_cubePoints);
  _cubeGlyph3D->Update();
  _concGlyph3D->Update();
  _renderWindow->Render();
  //std::cout << _cubePoints->GetNumberOfPoints() << std::endl;
  //std::cout << _cubeColors->GetNumberOfPoints() << std::endl;

}
