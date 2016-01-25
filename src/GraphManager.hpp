#ifndef GRAPHMANAGER_HPP
#define GRAPHMANAGER_HPP

#include "environment.h"
#include <vector>

// Defining the graph vertices
typedef std::vector<double> vectorGraphVertex; // form which can be either a
                                             // starting form or the reached
                                             // form after mitose
// Definition of the graph
typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::directedS,
                               vectorGraphVertex, graphEdge, boost::no_property,
                               boost::listS > VectorGraph;
typedef boost::graph_traits< VectorGraph >::vertex_iterator vectorVertex_iter;
typedef VectorGraph::vertex_descriptor VectorVertex;

/* -----------------------------------------------------------*/
/** 
 * @brief GraphManager class
 */
/* -----------------------------------------------------------*/
class GraphManager
{
public:
  GraphManager (
      std::vector<int> dim,
      Graph g,
      double initEneLvl,
      double initOxyLvl,
      double initGluLvl,
      double initLacLvl);
  virtual ~GraphManager ();

  Graph getGForm() const;
  Vertex add_vertexToGForm(
      boost::dynamic_bitset<> form,
      const std::vector<double> &energy,
      const std::vector<double> &oxygen,
      const std::vector<double> &glucose,
      const std::vector<double> &lactate);
  void add_edgeToGForm(
      Vertex u,
      Vertex v,
      const graphEdge& p);
  VectorGraph getGEnergy() const;
  int getMaxNbrOfForm() const;
  void getFormFromGraph(
      int index,
      const Graph &g,
      boost::dynamic_bitset<> &form,
      std::vector<double> &energy,
      std::vector<double> &oxygen,
      std::vector<double> &glucose,
      std::vector<double> &lactate);
private:
  /* data */
  Graph _gForm;
  VectorGraph _gEnergy;
  VectorGraph _gOxygen;
  VectorGraph _gGlucose;
  VectorGraph _gLactate;
};

#endif
