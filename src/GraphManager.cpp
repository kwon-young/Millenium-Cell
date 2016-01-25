#include <numeric>

#include "GraphManager.hpp"


GraphManager::GraphManager (
      std::vector<int> dim,
      Graph g,
      double initEneLvl,
      double initOxyLvl,
      double initGluLvl,
      double initLacLvl):
  _gForm(g)
{
  int maxCell = std::accumulate(dim.begin(), dim.end(), 1, std::multiplies<int>());
  std::cout << "max nbr of form : " << getMaxNbrOfForm() << std::endl;
  for (int i = 0; i < getMaxNbrOfForm(); ++i) {
    std::vector<double> energy(maxCell, initEneLvl-i);
    std::vector<double> oxygen(maxCell, initEneLvl-i);
    std::vector<double> glucose(maxCell, initEneLvl-i);
    std::vector<double> lactate(maxCell, initEneLvl-i);
    boost::add_vertex(energy, _gEnergy);
    boost::add_vertex(oxygen, _gOxygen);
    boost::add_vertex(glucose, _gGlucose);
    boost::add_vertex(lactate, _gLactate);
  }
}

GraphManager::~GraphManager()
{}

Graph GraphManager::getGForm() const
{
  return _gForm;
}

Vertex GraphManager::add_vertexToGForm(
    boost::dynamic_bitset<> form,
    const std::vector<double> &energy,
    const std::vector<double> &oxygen,
    const std::vector<double> &glucose,
    const std::vector<double> &lactate)
{
  boost::add_vertex(energy, _gEnergy);
  boost::add_vertex(oxygen, _gOxygen);
  boost::add_vertex(glucose, _gGlucose);
  boost::add_vertex(lactate, _gLactate);
  return boost::add_vertex(form, _gForm);
}

void GraphManager::add_edgeToGForm(
    Vertex u,
    Vertex v,
    const graphEdge& p)
{
  add_edge(u, v, p, _gForm);
}

VectorGraph GraphManager::getGEnergy() const
{
  return _gEnergy;
}

int GraphManager::getMaxNbrOfForm() const
{
  return boost::num_vertices(_gForm);
}

void GraphManager::getFormFromGraph(
    int index,
    const Graph &g,
    boost::dynamic_bitset<> &form,
    std::vector<double> &energy,
    std::vector<double> &oxygen,
    std::vector<double> &glucose,
    std::vector<double> &lactate)
{
  // Get the new form
  pair< vertex_iter, vertex_iter > vertexPair = vertices(_gForm);
  vertexPair.first += index;
  form = _gForm[*vertexPair.first];

  vertexPair = vertices(_gEnergy);
  vertexPair.first += index;
  energy = _gEnergy[*vertexPair.first];

  vertexPair = vertices(_gOxygen);
  vertexPair.first += index;
  oxygen = _gOxygen[*vertexPair.first];

  vertexPair = vertices(_gGlucose);
  vertexPair.first += index;
  glucose = _gGlucose[*vertexPair.first];

  vertexPair = vertices(_gLactate);
  vertexPair.first += index;
  lactate = _gLactate[*vertexPair.first];
}

