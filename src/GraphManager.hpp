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

class GraphManager
{
public:
  GraphManager (
      std::vector<int> dim,
      double initEneLvl,
      double initOxyLvl,
      double initGluLvl,
      double initLacLvl);
  virtual ~GraphManager ();

private:
  /* data */
  Graph _gForm;
  VectorGraph _gEnergy;
  VectorGraph _gOxygen;
  VectorGraph _gGlucose;
  VectorGraph _gLactate;
};

#endif
