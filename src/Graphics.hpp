/** 
 * @file Graphics.hpp
 * @author Kwon-Young Choi
 * @version 0.1
 * @date 2016-01-21
 */

/**
 * VTK includes
 */
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
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkUnsignedCharArray.h>
#include <vtkGlyph3D.h>
#include <vtkPointData.h>

/**
 * Boost include
 */
#include <boost/dynamic_bitset.hpp>

/**
 * Standard library include
 */
#include <vector>
#include <string>

/**
 * Project include
 */
#include "environment.h"

/* -----------------------------------------------------------*/
/** 
 * @brief Class allowing to view a Graph object with VTK
 */
/* -----------------------------------------------------------*/
class GraphViewer
{
public:
  /* -----------------------------------------------------------*/
  /** 
   * @brief Constructor
   * 
   * @param g : Graph object to view, contains Boost::dynamic_bitset<> of forms
   * @param verticesPerTimestep : number of forms for each level of the graph
   * @param bgColor : background color for the vtk windows
   * @param dim : 3D space size
   */
  /* -----------------------------------------------------------*/
  GraphViewer (
      Graph &g,
      std::vector<int> verticesPerTimestep,
      std::vector<double> bgColor,
      std::vector<int> dim);
  /* -----------------------------------------------------------*/
  /** 
   * @brief Destructor
   */
  /* -----------------------------------------------------------*/
  virtual ~GraphViewer ();

  /* -----------------------------------------------------------*/
  /** 
   * @brief Getter for _formIndex
   * 
   * @return _formIndex
   */
  /* -----------------------------------------------------------*/
  int getFormIndex() const;
  /* -----------------------------------------------------------*/
  /** 
   * @brief Setter for _formIndex
   * 
   * @param newFormIndex : new index for _formIndex
   */
  /* -----------------------------------------------------------*/
  void setFormIndex(int newFormIndex);

  /* -----------------------------------------------------------*/
  /** 
   * @brief Perform render action for vt
   */
  /* -----------------------------------------------------------*/
  void Render();
  /* -----------------------------------------------------------*/
  /** 
   * @brief Start the windows for vtk
   */
  /* -----------------------------------------------------------*/
  void Start();
private:

  /* -----------------------------------------------------------*/
  /** 
   * @brief Create 3D grid of VTK point
   */
  /* -----------------------------------------------------------*/
  void convertPointGrid();
  /* -----------------------------------------------------------*/
  /** 
   * @brief Set connections for the VTK object flow
   */
  /* -----------------------------------------------------------*/
  void initCubes();
  /* -----------------------------------------------------------*/
  /** 
   * @brief Convert a 1D coordinate to 3D coordinates
   * 
   * @param[in] pos : 1D coordinate that represent a 3D coordinate
   * @param[out] x : x parameter of the 3D coordinate
   * @param[out] y : y parameter of the 3D coordinate
   * @param[out] z : z parameter of the 3D coordinate
   */
  /* -----------------------------------------------------------*/
  void getXYZ(
      const int pos,
      int &x,
      int &y,
      int &z);
  /* -----------------------------------------------------------*/
  /** 
   * @brief Resize the number of cubes to be drawn by vtk
   * 
   * @param[in] size : new number of cubes to be drawn
   */
  /* -----------------------------------------------------------*/
  void resizeCubes(int size);
  /* -----------------------------------------------------------*/
  /** 
   * @brief Convert a form into cubes to be drawn by vtk
   */
  /* -----------------------------------------------------------*/
  void drawForm();

  /* data */
  Graph &_g; /*!< graph of form to be drawn*/
  std::vector<int> _verticesPerTimestep; /*!< number of forms for each level of the graph*/
  std::vector<double> _bgColor; /*!< background color for the vtk window*/
  std::vector<int> _dim; /*!< Dimension of the 3D space*/

  /**
   * VTK objects concerning the windows and its interactor
   */
  vtkSmartPointer<vtkRenderer> _renderer;
  vtkSmartPointer<vtkRenderWindow> _renderWindow;
  vtkSmartPointer<vtkRenderWindowInteractor> _renderWindowInteractor;
  vtkSmartPointer<vtkCallbackCommand> _keypressCallback;

  /**
   * VTK objects for drawing a grid of points
   */
  vtkSmartPointer<vtkStructuredGrid> _pointsStructuredGrid;
  vtkSmartPointer<vtkPoints> _points;
  vtkSmartPointer<vtkStructuredGridGeometryFilter> _pointsGeometryFilter;
  vtkSmartPointer<vtkPolyDataMapper> _pointsMapper;
  vtkSmartPointer<vtkActor> _pointsActor;

  /**
   * VTK objects for drawing multiple cubes
   * Used for drawing a form in the vtk windows
   */
  vtkSmartPointer<vtkPoints> _cubePoints;
  vtkSmartPointer<vtkUnsignedCharArray> _cubeColors;
  vtkSmartPointer<vtkPolyData> _cubePolyData;
  vtkSmartPointer<vtkCubeSource> _cubeSource;
  vtkSmartPointer<vtkGlyph3D> _cubeGlyph3D;
  vtkSmartPointer<vtkPolyDataMapper> _cubeMapper;
  vtkSmartPointer<vtkActor> _cubeActor;

  int _formIndex; /*!< index of the current form*/
  boost::dynamic_bitset<> _form; /*!< dynamic_bytset of the current form*/
  pair< vertex_iter, vertex_iter > _vertexPair; /*!< iterator for iterating the graph*/
};
