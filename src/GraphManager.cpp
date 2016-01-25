#include <numeric>

#include "GraphManager.hpp"


GraphManager::GraphManager (
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
      double eneMitose):
  _gForm(g),
  _hInGlu(hInGlu),
  _hInOxy(hInOxy),
  _hOutEneOxy(hOutEneOxy),
  _hOutEneNoOxy(hOutEneNoOxy),
  _hOutLac(hOutLac),
  _hLacMitose(hLacMitose),
  _cInGlu(cInGlu),
  _cInOxy(cInOxy),
  _cOutEne(cOutEne),
  _cOutLac(cOutLac),
  _cLacMitose(cLacMitose),
  _eneMitose(eneMitose)
{
}

GraphManager::~GraphManager()
{}

void GraphManager::healthy_reaction(
    std::vector<double> &energy,
    std::vector<double> &oxygen,
    std::vector<double> &glucose,
    std::vector<double> &lactate,
    int pos)
{
  if(glucose[pos] >= _hInGlu)
  {
    glucose[pos] -= _hInGlu;
    if(oxygen[pos] >= _hInOxy)
    {
      oxygen[pos] -= _hInOxy;
      energy[pos] += _hOutEneOxy;
    } else {
      lactate[pos] += _hOutLac;
      energy[pos] += _hOutEneNoOxy;
    }
  }
}

void GraphManager::cancerous_reaction(
    std::vector<double> &energy,
    std::vector<double> &oxygen,
    std::vector<double> &glucose,
    std::vector<double> &lactate,
    int pos)
{
  if(glucose[pos] >= _cInGlu)
  {
    glucose[pos] -= _cInGlu;
    if(oxygen[pos] >= _cInOxy)
    {
      oxygen[pos] -= _cInOxy;
    }
    lactate[pos] += _cOutLac;
    energy[pos] += _cOutEne;
  }
}

bool GraphManager::canMitose(
    int pos,
    char dir,
    std::vector<int> dim,
    std::vector<double> &energy,
    std::vector<double> &lactate,
    bool healthy)
{
  double lacMitose;
  int poseMitose = pos;
  switch (dir) {
    case 'd': {
       poseMitose -= dim[0];
    } break;

    case 'u': {
        poseMitose += dim[0];
    } break;
    case 'l': {
        poseMitose += 1;
    } break;
    case 'r': {
        poseMitose -= 1;
    } break;
  }
  if (healthy)
  {
    lacMitose = _hLacMitose;
  } else {
    lacMitose = _cLacMitose;
  }
  if (energy[pos] > _eneMitose && lactate[pos] < lacMitose)
  {
    energy[pos] -= _eneMitose;
    energy[poseMitose] += _eneMitose;
    return true;
  } else {
    return false;
  }
}

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

VectorGraph GraphManager::getGOxygen() const
{
  return _gOxygen;
}

VectorGraph GraphManager::getGGlucose() const
{
  return _gGlucose;
}

VectorGraph GraphManager::getGLactate() const
{
  return _gLactate;
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

