
#include <map>
#include <string>

namespace Reaction {

  class Reaction
  {
  public:
    Reaction (const std::map<std::string, unsigned int> &inputs);
    virtual ~Reaction ();

    void set_inputs(const std::map<std::string, unsigned int> &inputs);
    void add_input(const std::string el_name, const unsigned int el_quantity);

    virtual int process();
  
  private:
    
    std::map<std::string, unsigned int> _inputs;
    std::map<std::string, unsigned int> _outputs;
  };
};

class Cell
{
  public:
    Cell ();
    virtual ~Cell ();

    void react();

  private:
    /* data */
    int energy;
};


