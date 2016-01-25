#include <numeric>

#include "GraphManager.hpp"


GraphManager::GraphManager (
      std::vector<int> dim,
      double initEneLvl,
      double initOxyLvl,
      double initGluLvl,
      double initLacLvl)
{
  int maxCell = std::accumulate(dim.begin(), dim.end(), 1, std::multiplies<int>());
  std::vector<double> energy(maxCell, initEneLvl);
  std::vector<double> oxygen(maxCell, initOxyLvl);
  std::vector<double> glucose(maxCell, initGluLvl);
  std::vector<double> lactate(maxCell, initLacLvl);
  boost::add_vertex(energy, _gEnergy);
  boost::add_vertex(oxygen, _gOxygen);
  boost::add_vertex(glucose, _gGlucose);
  boost::add_vertex(lactate, _gLactate);
}

GraphManager::~GraphManager()
{}

