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
      double hInGlu,
      double hInOxy,
      double hOutEneOxy,
      double hOutEneNoOxy,
      double hOutLac,
      double hLacMitose,
      double cInGlu,
      double cInOxy,
      double cOutEne,
      double cOutLac,
      double cLacMitose,
      double eneMitose);
  virtual ~GraphManager ();

  void healthy_reaction(
      std::vector<double> &energy,
      std::vector<double> &oxygen,
      std::vector<double> &glucose,
      std::vector<double> &lactate,
      int pos);
  void cancerous_reaction(
      std::vector<double> &energy,
      std::vector<double> &oxygen,
      std::vector<double> &glucose,
      std::vector<double> &lactate,
      int pos);
  bool canMitose(
      int pos,
      char dir,
      std::vector<int> dim,
      std::vector<double> &energy,
      std::vector<double> &lactate,
      bool healthy);
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
  VectorGraph getGOxygen() const;
  VectorGraph getGGlucose() const;
  VectorGraph getGLactate() const;
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

  /* threshold */
  double _hInGlu;
  double _hInOxy;
  double _hOutEneOxy;
  double _hOutEneNoOxy;
  double _hOutLac;
  double _hLacMitose;
  double _cInGlu;
  double _cInOxy;
  double _cOutEne;
  double _cOutLac;
  double _cLacMitose;
  double _eneMitose;
};

#endif
