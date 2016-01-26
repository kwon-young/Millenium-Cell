/** 
 * @file GraphManager.hpp
 * @author Kwon-Young Choi
 * @version 1.0
 * @date 2016-01-26
 */

#ifndef GRAPHMANAGER_HPP
#define GRAPHMANAGER_HPP

/* std include */
#include <vector>

/* project include */
#include "environment.h"

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
 * @brief This class is used for managing graphs
 */
/* -----------------------------------------------------------*/
class GraphManager
{
public:
  /* -----------------------------------------------------------*/
  /** 
   * @brief Default constructor
   */
  /* -----------------------------------------------------------*/
  GraphManager();

  /* -----------------------------------------------------------*/
  /** 
   * @brief Constructor
   * 
   * @param dim : dimension of the environment
   * @param g : graph of forms
   * @param initEne : initial energy for a cell
   * @param initOxy : initial oxygen for a cell
   * @param initGlu : initial glucose for a cell
   * @param initLac : initial lactate for a cell
   * @param hInGlu : healthy cell consumption of glucose needed for a healthy reaction
   * @param hInOxy : healthy cell consumption of oxygen needed for a healthy reaction
   * @param hOutEneOxy : healthy cell energy production when breathing
   * @param hOutEneNoOxy : healthy cell energy production when fermenting
   * @param hOutLac : healthy cell lactate production when fermenting
   * @param hLacMitose : healthy cell max threshold of lactate for allowing a mitosis
   * @param cInGlu : cancerous cell consumption of glucose needed for a cancerous reaction
   * @param cInOxy : cancerous cell consumption of oxygen needed for a cancerous reaction
   * @param cOutEne : cancerous cell production of energy when doing a cancerous reaction
   * @param cOutLac : cancerous cell production of lactate when doing a cancerous reaction
   * @param cLacMitose : cancerous cell max threshold of lactate for allowing a mitosis
   * @param eneMitose : energy needed for allowing a mitosis
   */
  /* -----------------------------------------------------------*/
  GraphManager (
      std::vector<int> dim,
      Graph g,
      double initEne, double initOxy, double initGlu, double initLac,
      double hInGlu, double hInOxy, double hOutEneOxy, double hOutEneNoOxy, double hOutLac, double hLacMitose,
      double cInGlu, double cInOxy, double cOutEne, double cOutLac, double cLacMitose,
      double eneMitose);
  virtual ~GraphManager ();

  /* -----------------------------------------------------------*/
  /** 
   * @brief Do a healthy reaction for the cell at position pos
   * 
   * @param[in, out] energy : energy concentration of the env
   * @param[in, out] oxygen : oxygen concentration of the env
   * @param[in, out] glucose : glucose concentration of the env
   * @param[in, out] lactate : lactate concentration of the env
   * @param[in] pos : position of the cell
   * a healthy reaction does either breathing or fermenting
   * when breathing : consumption of oxygen and glucose and produce a lot of energy
   * when fermenting : consumption of glucose and produce little energy and lactate
   */
  /* -----------------------------------------------------------*/
  void healthy_reaction(
      std::vector<double> &energy,
      std::vector<double> &oxygen,
      std::vector<double> &glucose,
      std::vector<double> &lactate,
      int pos);

  /* -----------------------------------------------------------*/
  /** 
   * @brief Do a cancerous reaction for the cell at position pos
   * 
   * @param[in, out ] energy  : energy concentration of the env
   * @param[in, out ] oxygen  : oxygen concentration of the env
   * @param[in, out ] glucose : glucose concentration of the env
   * @param[in, out ] lactate : lactate concentration of the env
   * @param[in] pos : position of the cell
   * a cancerous reaction always ferment and can consume oxygen when available
   * consume glucose and oxygen when available
   * produce energy and lactate
   */
  /* -----------------------------------------------------------*/
  void cancerous_reaction(
      std::vector<double> &energy,
      std::vector<double> &oxygen,
      std::vector<double> &glucose,
      std::vector<double> &lactate,
      int pos);

  /* -----------------------------------------------------------*/
  /** 
   * @brief initialize resources for a form
   * 
   * @param[in, out] energy  : energy concentration of the env
   * @param[in, out] oxygen  : oxygen concentration of the env
   * @param[in, out] glucose : glucose concentration of the env
   * @param[in, out] lactate : lactate concentration of the env
   * @param[in] form : form of cells to initialize
   * Before doing a mitosis, resources of a cell is initialize to
   * the value init* precised in the constructor
   */
  /* -----------------------------------------------------------*/
  void init_ressource(
      std::vector<double> &energy,
      std::vector<double> &oxygen,
      std::vector<double> &glucose,
      std::vector<double> &lactate,
      const boost::dynamic_bitset<> &form);

  /* -----------------------------------------------------------*/
  /** 
   * @brief Check if a mitosis can happen at position pos
   * 
   * @param[in] pos : position of the cell that attempt to do a mitosis
   * @param[in] dir : direction of the mitosis
   * @param[in] dim : dimension of the env, not used
   * @param[in] energy : energy concentration of the env
   * @param[in] lactate : lactate concentration of the env
   * @param[in] healthy : true if healthy, false if cancerous
   * 
   * @return true if mitosis can be done, false otherwise
   * This function check if a mitosis can happen.
   * This check lookup if the energy level is enough (> eneMitose) and if the lactate
   * concentration is not higher than the corresponding threshold *LacMitose
   */
  /* -----------------------------------------------------------*/
  bool canMitose(
      int pos,
      char dir,
      std::vector<int> dim,
      std::vector<double> &energy,
      std::vector<double> &lactate,
      bool healthy);

  /* -----------------------------------------------------------*/
  /** 
   * @brief Getter of the form graph
   * 
   * @return the form graph
   */
  /* -----------------------------------------------------------*/
  Graph getGForm() const;

  /* -----------------------------------------------------------*/
  /** 
   * @brief add a form to the graph form and save the corresponding env
   * 
   * @param[in] form : form to be added
   * @param[in] energy  : energy concentration of the env to be saved
   * @param[in] oxygen  : oxygen concentration of the env to be saved
   * @param[in] glucose : glucose concentration of the env to be saved
   * @param[in] lactate : lactate concentration of the env to be saved
   * 
   * @return the vertex created
   */
  /* -----------------------------------------------------------*/
  Vertex add_vertexToGForm(
      boost::dynamic_bitset<> form,
      const std::vector<double> &energy,
      const std::vector<double> &oxygen,
      const std::vector<double> &glucose,
      const std::vector<double> &lactate);

  /* -----------------------------------------------------------*/
  /** 
   * @brief Add edge to the graph form
   * 
   * @param u
   * @param v
   * @param p
   * Just a wrapper of the boost::add_edge
   */
  /* -----------------------------------------------------------*/
  void add_edgeToGForm(
      Vertex u,
      Vertex v,
      const graphEdge& p);

  /* -----------------------------------------------------------*/
  /** 
   * @brief Getter of the energy graph
   * 
   * @return the energy graph
   */
  /* -----------------------------------------------------------*/
  VectorGraph& getGEnergy();
  VectorGraph& getGOxygen();
  VectorGraph& getGGlucose();
  VectorGraph& getGLactate();

  /* -----------------------------------------------------------*/
  /** 
   * @brief Compute the number of forms contained in the form graph
   * 
   * @return the number of form in graph
   */
  /* -----------------------------------------------------------*/
  int getMaxNbrOfForm() const;

  /* -----------------------------------------------------------*/
  /** 
   * @brief Get a specific form of the form graph and the corresponding env
   * 
   * @param[in] index : index of the form
   * @param[in] g : form graph
   * @param[out] form : corresponding form at index
   * @param[out] energy : corresponding energy env at index
   * @param[out] oxygen : corresponding oxygen env at index
   * @param[out] glucose : corresponding glucose env at index
   * @param[out] lactate : corresponding lactate env at index
   */
  /* -----------------------------------------------------------*/
  void getFormFromGraph(
      int index,
      const Graph &g,
      boost::dynamic_bitset<> &form,
      std::vector<double> &energy,
      std::vector<double> &oxygen,
      std::vector<double> &glucose,
      std::vector<double> &lactate);

  template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar & _gForm;
      ar & _gEnergy;
      ar & _gOxygen;
      ar & _gGlucose;
      ar & _gLactate;
      ar & _initEne;
      ar & _initOxy;
      ar & _initGlu;
      ar & _initLac;
      ar & _hInGlu;
      ar & _hInOxy;
      ar & _hOutEneOxy;
      ar & _hOutEneNoOxy;
      ar & _hOutLac;
      ar & _hLacMitose;
      ar & _cInGlu;
      ar & _cInOxy;
      ar & _cOutEne;
      ar & _cOutLac;
      ar & _cLacMitose;
      ar & _eneMitose;
    }
private:
  /* data */
  Graph _gForm; /*!< form graph*/
  /* env graphs */
  VectorGraph _gEnergy;
  VectorGraph _gOxygen;
  VectorGraph _gGlucose;
  VectorGraph _gLactate;

  /* threshold see constructor */
  double _initEne;
  double _initOxy;
  double _initGlu;
  double _initLac;
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
