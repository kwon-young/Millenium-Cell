/// \file main.cpp
/// \brief Reachable Form sets within Graph
/// \author A. Sarr
/// \version 1.0
/// \date 24 mai 2013
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/bitset.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/serialization/vector.hpp>

#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

//#include <C:/Users/info/Desktop/Viab-Cell/environment.h>
#include "environment.h"
#include "Graphics.hpp"
#include "GraphManager.hpp"

struct A {
    boost::dynamic_bitset<> x;
  private:
    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int) { 
        ar &x; 
    }
};

namespace boost { namespace serialization {

    template <typename Ar, typename Block, typename Alloc>
        void save(Ar& ar, dynamic_bitset<Block, Alloc> const& bs, unsigned) {
            size_t num_bits = bs.size();
            std::vector<Block> blocks(bs.num_blocks());
            to_block_range(bs, blocks.begin());

            ar & num_bits & blocks;
        }

    template <typename Ar, typename Block, typename Alloc>
        void load(Ar& ar, dynamic_bitset<Block, Alloc>& bs, unsigned) {
            size_t num_bits;
            std::vector<Block> blocks;
            ar & num_bits & blocks;

            bs.resize(num_bits);
            from_block_range(blocks.begin(), blocks.end(), bs);
            bs.resize(num_bits);
        }

    template <typename Ar, typename Block, typename Alloc>
        void serialize(Ar& ar, dynamic_bitset<Block, Alloc>& bs, unsigned version) {
            split_free(ar, bs, version);
        }

} }

int main()
{

  // Count the number of vertices added in each timestep
  unsigned int countVerticesPerTime;

  // record each timestep's number of vertices
  std::vector< unsigned int > verticesPerTimestep;

  // Map regrouping same number of cells vertices
  std::map<unsigned int, std::vector<unsigned int> > verticesPerNbCell;

  Graph g;               // defining a graph
  // dimension of the env
  std::vector<int> dim(3);
  dim[0] = 10;
  dim[1] = 10;
  dim[2] = 1;
  // type of the cell simulated : true if healthy, false if cancerous
  bool healthy = true;
  // initial resources for a cell mitosis
  double initEneLvl=0, initOxyLvl=10, initGluLvl=100, initLacLvl=0;
  // define a graph manager
  GraphManager gm(dim, g,
      initEneLvl, initOxyLvl, initGluLvl, initLacLvl,
      10, 10, 36, 2, 2, 40,
      10, 10, 4, 4, 90,
      20);
  unsigned int timestep; // Records the timesteps

  //  unsigned int bridgeTime4 = 3; //Defining the time of crossing constraints
  //  after three divisions
  //  unsigned int bridgeTime8 = 7; //Defining the time of crossing constraints
  //  after seven divisions

  unsigned int firstPos = 55; // Specify the first cell's position

  unsigned int maxCell = 7; // Defining the max cells to reach for final forms

  // Dimensions of the grid
  unsigned int width = 10;
  unsigned int height = 10;

  // Maximum size of forms
  unsigned int maxSize = width * height;

  Environment *env;
  env = new Environment(maxCell, height, width);

  Graph catalog; // The graph which contains the catalog of forms with which
                 // forms have to be assessed in given timestep

  //  boost::dynamic_bitset<> form1(maxSize);

  // Set the 4 cells form of C-elegans in catalog
  //  form1.set(54);
  //  form1.set(55);
  //  form1.set(64);
  //  form1.set(65);

  //  boost::dynamic_bitset<> form2(width*height);

  // Set the 8 cells form of C-elegans in catalog
  //  form2.set(52);
  //  form2.set(53);
  //  form2.set(54);
  //  form2.set(55);
  //  form2.set(62);
  //  form2.set(63);
  //  form2.set(64);
  //  form2.set(65);

  // The first node(form) of graph is not set
  //  boost::dynamic_bitset<> nodeNul(width*height,0);
  //  boost::add_vertex(nodeNul,catalog);

  // add in catalog all forms to compare with at the first crossing
  // conra/Users/Musky//Dev/Viab-Celli
  //  boost::add_vertex(form1,catalog);

  // contains all forms to compare with at the first crossing conraints
  //  std::vector<unsigned int> formCatalog4;
  //
  //  for(unsigned int i = 0; i < boost::num_vertices(catalog); i++)
  //    formCatalog4.push_back(i);

  ////add in catalog all forms to compare with at the second crossing conraints
  //  boost::add_vertex(form2,catalog);

  // contains all forms to compare with at the second crossing conraints
  //  std::vector<unsigned int> formCatalog8;
  //
  //  for(unsigned int i = formCatalog4.size(); i <
  //  boost::num_vertices(catalog); i++ )
  //    formCatalog8.push_back(i);

  // Output the config of the simulation
  cout << "######## CONFIG ########" << endl << endl;
  cout << "* LENGTH : " << width << " x " << height << endl << endl;
  cout << "* STARTING POSITION : " << firstPos << endl << endl;
  cout << "* MAX CELL NUMBER : " << maxCell << endl << endl << endl;
  cout << "######## RESULTS ########" << endl << endl;

  char directions[4] = {
      'u', 'd', 'r',
      'l'}; // The controls which indicates th direction of mitosis

  boost::dynamic_bitset<> formContainer(maxSize, 0); // The starting form

  formContainer.set(firstPos); // set position


  timestep = 0;

  // Allow to retrieve graph's vertices and iterate on them
  pair< vertex_iter, vertex_iter > vertexPair, vertexPair_prev;
  pair< vertex_iter, vertex_iter > EvertexPair, EvertexPair_prev;
  pair< vertex_iter, vertex_iter > OvertexPair, OvertexPair_prev;
  pair< vertex_iter, vertex_iter > GvertexPair, GvertexPair_prev;
  pair< vertex_iter, vertex_iter > LvertexPair, LvertexPair_prev;

  // Add the starting form to the graph ! This form will represent the root
  //boost::add_vertex(formContainer, g);
  
  std::vector<double> energy(maxSize, initEneLvl);
  std::vector<double> oxygen(maxSize, initOxyLvl);
  std::vector<double> glucose(maxSize, initGluLvl);
  std::vector<double> lactate(maxSize, initLacLvl);
  gm.add_vertexToGForm(formContainer, energy, oxygen, glucose, lactate);

  // Record the reference of the vertex among those of the same number of cell
  verticesPerNbCell[formContainer.count()].push_back(gm.getMaxNbrOfForm() - 1);

  // At initial time 0 we have just one node corresponding to the root
  verticesPerTimestep.push_back(1);

  // Loop until getting all recheable forms with the right number of cells
  while (timestep + 1 < maxCell) {
    std::cout << "timestep :" << timestep << std::endl;
    //std::cout << "countVerticesPerTime : " << countVerticesPerTime << std::endl;
    // Initialize the number of vertices in the current timestep
    //if(countVerticesPerTime == 0) timestep = std::max(((int)timestep)-1, 0);
    countVerticesPerTime = 0;

    vertexPair_prev = vertices(gm.getGForm());
    EvertexPair_prev = vertices(gm.getGEnergy());
    OvertexPair_prev = vertices(gm.getGOxygen());
    GvertexPair_prev = vertices(gm.getGGlucose());
    LvertexPair_prev = vertices(gm.getGLactate());

    timestep++;

    unsigned int nbVertices = 0;

    
    // Loop until each node of previous timestep is processed
    while (nbVertices < verticesPerTimestep[timestep - 1]) {
      //std::cout << "nbVertices  : " << nbVertices<< std::endl;
      // Iterate within the nodes of previous timestep
      --vertexPair_prev.second;
      --EvertexPair_prev.second;
      --OvertexPair_prev.second;
      --GvertexPair_prev.second;
      --LvertexPair_prev.second;

      // Form in the current node to be processed
      boost::dynamic_bitset<> form = gm.getGForm()[*vertexPair_prev.second];
      // initialize resources for a form
      gm.init_ressource(energy, oxygen, glucose, lactate, form);

      unsigned int nbCells = form.count();
      unsigned int formSize = 0;

      unsigned int motherPosition = 0;

      // do healthy or cancerous reaction
      while (motherPosition < maxSize) {
        if (form[motherPosition]) {
          if (healthy)
          {
            gm.healthy_reaction(
                energy,
                oxygen,
                glucose,
                lactate,
                motherPosition);
          }else{
            gm.cancerous_reaction(
                energy,
                oxygen,
                glucose,
                lactate,
                motherPosition);
          }
          //std::cout << "energy at pos " << motherPosition << ":" << energy[motherPosition] << std::endl;
        }
        motherPosition++;
      }
      //gm.getGEnergy()[*EvertexPair_prev.second] = energy;
      //gm.getGOxygen()[*OvertexPair_prev.second] = oxygen;
      //gm.getGGlucose()[*GvertexPair_prev.second] = glucose;
      //gm.getGLactate()[*LvertexPair_prev.second] = lactate;
      motherPosition=0;
      // Process each cell of the form
      while ((motherPosition < maxSize) && (formSize < nbCells)) {
        // If there is a cell in the position c
        if (form[motherPosition]) {
          formSize++;

          // For each cell, try each mitosis control to divide
          for (int d = 0; d < 4; d++) {
            // Try to trigger a mitosis of the current cell with the current
            // control within the current form
            boost::dynamic_bitset<> mitoForm = form;
            bool mitose = env->mitose(mitoForm, motherPosition, directions[d]);

            // Calculating the subset of reachable sets or enforcing the some
            // crossing constraints
            // At the time of crossing constraints, compare in an automated way
            // (rather than by the user) the reached forms to the forms of the
            // catalog defined at the beginning
            // then continuing with the corresponding ones

            // First crossing constraints
            //                      if((timestep == bridgeTime4) && mitose)
            //                      {
            //                          unsigned int inCatalog =
            //                          env->existInGraph(catalog,mitoForm,formCatalog4);
            //                          if(!inCatalog)
            //                          mitose = false;
            //                      }
            //

            // Second crossing constraints
            //                      if((timestep == bridgeTime8) && mitose)
            //                      {
            //                          unsigned int inCatalog =
            //                          env->existInGraph(catalog,mitoForm,formCatalog8);
            //                          if(!inCatalog)
            //                          mitose = false;
            //                      }

            // If a mitosis is achieved
            // check if a mitosis can be done
            if(mitose) {
              mitose = gm.canMitose(
                  motherPosition,
                  directions[d],
                  dim,
                  energy,
                  lactate,
                  healthy);
            }
            
            if (mitose) {
              unsigned int newNbCells = mitoForm.count();

              unsigned int vertex;

              // If there are already forms of same number of cells as the newly
              // generated form,
              // test if there is any redundance, also with geometrical
              // transformation
              if (verticesPerNbCell[newNbCells].size() != 0)
                vertex = env->existInGraph(gm.getGForm(), mitoForm,
                                           verticesPerNbCell[newNbCells]);
              else
                vertex = 0;
              // do not check if form already exist
              //vertex = 0;

              // If there is no redundance
              if (vertex == 0) {
                // Add the wewly created form in the graph
                //Vertex formVertex = boost::add_vertex(mitoForm, g);
                // Add the newly created form in the form graph and save the env
                Vertex formVertex = gm.add_vertexToGForm(mitoForm,
                    energy, oxygen, glucose, lactate);

                // In the map, record the reference of the vertex among those of
                // the same number of cell
                verticesPerNbCell[newNbCells].push_back(gm.getMaxNbrOfForm() - 1);

                // Define an edge that link the two vertices and add properties
                graphEdge firstEdge;

                firstEdge.Control = directions[d];
                firstEdge.Mitoser = motherPosition;
                firstEdge.Temps = timestep;

                // link the two vertices
                //add_edge(*vertexPair_prev.second, formVertex, firstEdge, g);
                gm.add_edgeToGForm(*vertexPair_prev.second, formVertex, firstEdge);

                // Increment the number of added vertices in the current
                // timestep
                countVerticesPerTime++;
              }

              // If the newly created form already exist in the graph
              else {
                // Find among the added vertices which one represent the newly
                // created form
                // If founded, create just a new edge

                graphEdge anotherEdge;
                anotherEdge.Control = directions[d];
                anotherEdge.Mitoser = motherPosition;
                anotherEdge.Temps = timestep;

                //add_edge(*vertexPair_prev.second, vertex, anotherEdge, g);
                gm.add_edgeToGForm(*vertexPair_prev.second, vertex, anotherEdge);
              }
            }
          }
        }

        motherPosition++;
      }

      nbVertices++;
    }

    // record the total number of nodes added at the current timestep
    verticesPerTimestep.push_back(countVerticesPerTime);
  }

  // Output results
  cout << "REACHED SETS : " << endl << endl;
  cout << "* NUMBER = " << verticesPerTimestep[timestep] << endl;
  cout << "* TIMESTEP = " << timestep - 1 << endl;

  vertexPair_prev = vertices(gm.getGForm());

  //  Displaying results on an external file
  for (unsigned int last = 0; last < verticesPerTimestep[timestep]; last++)
    env->display(gm.getGForm()[*--vertexPair_prev.second], last + 1);

  // free allocated memories
  delete env;

  // Output graph

  // 1st method
  // Display on the monitor the list of nodes and edges
  // boost::print_graph(g);

  // 2nd method
  // Output in an external file the list of nodes and that of edges

  //  ofstream graphFile;
  //
  //  graphFile.open("graphFile", ios::out);
  //    if (graphFile.bad()){ cerr << "Impossible d'ouvrir le fichier !" <<
  //    endl;}
  //
  //  boost::write_graphviz(graphFile,g);

  // 3rd method
  // Output all created form according to the graph structure

    ofstream graphFile;
    graphFile.open("graphFile", ios::out);
    if (graphFile.bad()){ cerr << "Impossible d'ouvrir le fichier !" << endl;}
  
    unsigned int t = 0;
    vertexPair = vertices(gm.getGForm());
    while(t <= timestep)
    {
        for(unsigned int step = 0; step < verticesPerTimestep[t]; step++)
        {
            graphFile<<gm.getGForm()[*vertexPair.first]<<"     ";
            ++vertexPair.first;
        }
        t++;
        graphFile<<endl<<endl<<endl;
    }

  // the gm class can be saved with boost::serialize
  const char* fileName = "saved.txt"; 
  {
  // Create an output archive
  std::ofstream ofs(fileName);
  boost::archive::text_oarchive ar(ofs);

  // Write data
  ar & gm;
  }
  std::ifstream ifs(fileName);
  boost::archive::text_iarchive ia(ifs);
  GraphManager gm2;
  ia >> gm2;

  std::vector<double> bgColor(3);
  bgColor[0] = .2;
  bgColor[1] = .3;
  bgColor[2] = .4;
  GraphViewer gv = GraphViewer(gm2, bgColor, dim);
  gv.Render();
  gv.Start();

  return EXIT_SUCCESS;
}
