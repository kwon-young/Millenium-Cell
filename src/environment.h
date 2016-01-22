#ifndef ENVIRONMENT_H_INCLUDED
#define ENVIRONMENT_H_INCLUDED

#include <string>
#include <vector>

#include <boost/dynamic_bitset.hpp>
#include <iostream>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/array.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>

using namespace std;

// Defining the graph vertices
typedef boost::dynamic_bitset<> graphVertex; // form which can be either a
                                             // starting form or the reached
                                             // form after mitose

// Defining the graph edges properties
struct graphEdge {
  char Control; // control used by mother cell to devide
  unsigned int
      Mitoser; // the index which identify the mother cell that triggers mitose
  int Temps;   // the timestep
  template <typename Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & Control;
    ar & Mitoser;
    ar & Temps;
  }
};

// Definition of the graph
typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::directedS,
                               graphVertex, graphEdge, boost::no_property,
                               boost::listS > Graph;
typedef boost::graph_traits< Graph >::vertex_iterator vertex_iter;
typedef Graph::vertex_descriptor Vertex;

using namespace std;

class Environment
{

public:
  Environment(unsigned int maxCell, unsigned int height,
              unsigned int width); // Create an environment with a wished number
                                   // of cells and sought shape
  ~Environment();
  unsigned int getMaxCell(); // Get the maximum number of cells sought
  unsigned int getWidth();
  unsigned int getHeight();
  boost::dynamic_bitset<> ror(boost::dynamic_bitset<> transForm,
                              unsigned int nbBits); // Allow to make shift and
                                                    // rotate operations on the
                                                    // dynamic bitset for shape
                                                    // translation
  unsigned int
  findCentroid(boost::dynamic_bitset<> form, boost::dynamic_bitset<> &oneBit,
               unsigned int &pos); // return the centroid of the form
  unsigned int translationResult(Graph g, vector< unsigned int > vertices,
                                 boost::dynamic_bitset<> form); // Give the
                                                                // result of all
                                                                // possible
                                                                // translation
                                                                // of the given
                                                                // form in the
                                                                // grid
  unsigned int rotation270Result(
      boost::dynamic_bitset<> &form,
      unsigned int centroidPos); // Rotating the form through 270 degrees
  unsigned int rotation180Result(
      boost::dynamic_bitset<> &form,
      unsigned int centroidPos); // Rotating the form through 180 degrees
  unsigned int
  rotation90Result(boost::dynamic_bitset<> &form,
                   unsigned int centroidPos); // Rotating the form through 90
  unsigned int horSymResult(
      boost::dynamic_bitset<> &form,
      unsigned int centroidPos); // Find the horizontal symmetry of the form
  unsigned int vertSymResult(
      boost::dynamic_bitset<> &form,
      unsigned int centroidPos); // Find the vertical symmetry of the form
  unsigned int geomTransResult(Graph g, vector< unsigned int > vertices,
                               boost::dynamic_bitset<> form,
                               unsigned int (Environment::*geomTrans)(
                                   boost::dynamic_bitset<> &, unsigned int));
  unsigned int
  existInGraph(Graph g, boost::dynamic_bitset<> form,
               vector< unsigned int > vertices); // Verify If a grid
                                                 // of a same number
                                                 // of cells already
                                                 // exists or its
                                                 // translation or
                                                 // rotation
  void setForm(boost::dynamic_bitset<> form,
               vector< unsigned int > positions); // Starting the reachable sets
                                                  // generation with a fo
  bool mitose(boost::dynamic_bitset<> &form, unsigned int motherPosition,
              char direction); // Trigger a mitose
  void
  display(boost::dynamic_bitset<> form,
          unsigned int formLabel); // Display the final grids on a external file
private:
  string _shape;         // the sought shape
  unsigned int _maxCell; // maximum number of cells wished
  unsigned int _height;  // max height of forms
  unsigned int _width;   // max height of forms
};

#endif // ENVIRONMENT_H_INCLUDED
