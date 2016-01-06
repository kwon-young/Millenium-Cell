
#include<iostream>
#include "environment.h"



//Create an environment with a wished number of cells and form dimensions
Environment::Environment(unsigned int maxCell, unsigned int height, unsigned int width)
{
    _maxCell = maxCell;
    _width = width;
    _height = height;
}

Environment::~Environment()
{

}

//Get the maximum cells number of sought forms
unsigned int Environment::getMaxCell()
{
    return _maxCell;
}

//Get the maximum height of forms
unsigned int Environment::getHeight()
{
    return _height;
}

//Get the maximum width of forms
unsigned int Environment::getWidth()
{
    return _width;
}

// Allow to make shift and rotate operations on the dynamic bitset for shape translation
boost::dynamic_bitset<> Environment::ror(boost::dynamic_bitset<> transForm, unsigned int nbBits)
{
    unsigned int maxSize = _height*_width;
    return (transForm >> nbBits) | (transForm<< maxSize - nbBits);
}

// return the centroid of a form
unsigned int Environment::findCentroid(boost::dynamic_bitset<> form, boost::dynamic_bitset<>& oneBit, unsigned int& pos)
{
    unsigned int maxSize = _height*_width;
    boost::dynamic_bitset<> noBit(maxSize,0);

    while((form & oneBit) == noBit)
    {
        oneBit >>= 1;
        --pos;
    }

    return pos;

}


// Give the result of all possible translations of the given form in the grid
unsigned int Environment::translationResult(Graph g, vector<unsigned int> vertices, boost::dynamic_bitset<> form)
{
    int maxSize = _height*_width-1;
    boost::dynamic_bitset<> copyForm = form;

    while(maxSize >= 0)
    {
        //compare the successive translation with each node referenced in the vector
        //and return the reference if there is a correspondance, else continue rotating
        for(vector<unsigned int>::iterator it(vertices.begin()); it != vertices.end(); it++)
        {
            if(g[*it] == copyForm)
            return *it;
        }

        copyForm = ror(form,maxSize);

        maxSize--;
    }

    return 0;
}

// The algorithm implementing the formula of form rotation through 270 degrees
unsigned int Environment::rotation270Result(boost::dynamic_bitset<>& form, unsigned int centroidPos)
{
    unsigned int maxSize = _height*_width;
    boost::dynamic_bitset<> rotForm(maxSize);
    unsigned int centroidRow = (int)centroidPos/_width;
    unsigned int centroidCol = (int)centroidPos%_width;
    unsigned int formSize = 0;
    unsigned int nbCells = form.count();
    unsigned int spread, cellNewCol, cellNewRow, cellNewPos;

    for(unsigned int i = 0; i< maxSize; i++)
    {
        if (form[i])
        {
             unsigned int celCol = (int)(i%_width);
             unsigned int celRow = (int)(i/_width);

            if( celCol< centroidCol)
            {
                spread = centroidCol - celCol;
                cellNewRow = centroidRow + spread;
            }

            else
            {
                spread = celCol - centroidCol;
                cellNewRow = centroidRow - spread;
            }

            if(celRow< centroidRow)
            {
                spread = centroidRow - celRow;
                cellNewCol = centroidCol - spread;
            }
            else
            {
                spread = celRow - centroidRow;
                cellNewCol = centroidCol + spread;
            }

            cellNewPos = cellNewRow*_width + cellNewCol;

            if (cellNewPos< maxSize)
            {
                rotForm.set(cellNewPos);
            }
            else
                return formSize;

            formSize++;

        }

        if(formSize == nbCells)
            break;

    }

    //Assert if new position is allocated to all cells or not
    form = rotForm;
    return formSize;
}

// The algorithm implementing the formula of form rotation through 180 degrees
unsigned int Environment::rotation180Result(boost::dynamic_bitset<>& form, unsigned int centroidPos)
{
    unsigned int maxSize = _height*_width;
    boost::dynamic_bitset<> rotForm(maxSize);

    unsigned int formSize = 0;
    unsigned int nbCells = form.count();

    for(unsigned int i = 0; i< maxSize; i++)
    {
        if(form[i])
        {
            if((2*centroidPos-i)< maxSize)
            {
                rotForm.set(2*centroidPos-i);
            }
            else
                return formSize;

            formSize++;

        }

        if(formSize == nbCells)
            break;
    }

    //Assert if new position is allocated to all cells or not
    form = rotForm;
    return formSize;

}

//The algorithm implementing the formula of form rotation through 90 degree
unsigned int Environment::rotation90Result(boost::dynamic_bitset<>& form, unsigned int centroidPos)
{
    unsigned int maxSize = _height*_width;
    boost::dynamic_bitset<> rotForm(maxSize);
    unsigned int centroidPosDiv = (int)(centroidPos/_width)*_width;
    unsigned int centroidRow = (int)centroidPos/_width;
    unsigned int centroidCol = centroidPos%_width;
    unsigned int formSize = 0;
    unsigned int nbCells = form.count();
    unsigned int spread, n, cellNewCol, cellNewRow, cellNewPos;

    for(unsigned int i = 0; i< maxSize; i++)
    {
        if (form[i])
        {
            if(i< centroidPos)
            {
                spread = centroidPosDiv - (int)(i/_width)*_width;
                n = (int)spread/_width;

                cellNewCol = centroidCol + n;
                cellNewRow = centroidRow - centroidCol + (i%_width);
            }

            else
            {
                spread = (int)(i/_width)*_width - centroidPosDiv;
                n = (int)spread/_width;
                cellNewCol = centroidCol - n;
                cellNewRow = centroidRow - centroidCol + i%_width;
            }

            cellNewPos = cellNewRow*_width + cellNewCol;

            if (cellNewPos< maxSize)
            {
                rotForm.set(cellNewPos);
            }
            else
                return formSize;

            formSize++;

        }

        if(formSize == nbCells)
            break;

    }

    //Assert if new position is allocated to all cells or not
    form = rotForm;
    return formSize;
}

//The algorithm implementing the formula which find horizontal symmetry of the form
unsigned int Environment::horSymResult(boost::dynamic_bitset<>& form, unsigned int centroidPos)
{
    unsigned int maxSize = _height*_width;
    boost::dynamic_bitset<> symForm(maxSize);
    unsigned int symInf = (int)(centroidPos/_width)*_width;
    unsigned int symSup = ((int)centroidPos/_width+1)*_width;
    unsigned int formSize = 0;
    unsigned int nbCells = form.count();
    unsigned int spread, cellNewRow, cellNewPos;

    for(unsigned int i = 0; i< maxSize; i++)
    {
        if (form[i])
        {
            if(i< symSup)
            {
                spread = (int)(symSup - i)/_width;

                if((int)(symSup - i)%_width != 0)
                    spread++;

                cellNewRow = (int)(symInf/_width) + spread;
            }

            else
            {
                spread = (int)(i - symSup)/_width;

                if((int)(i - symSup)%_width != 0)
                    spread++;

                cellNewRow = (int)(symSup/_width) - spread;
            }

            cellNewPos = cellNewRow*_width + i%_width;

            if (cellNewPos< maxSize)
            {
                symForm.set(cellNewPos);
            }
            else
                return formSize;

            formSize++;

        }

        if(formSize == nbCells)
            break;

    }

    //Assert if new position is granted to all cells by the symmetry of the form or not
    form = symForm;
    return formSize;
}

//The algorithm implementing the formula which find vertical symmetry of the form
unsigned int Environment::vertSymResult(boost::dynamic_bitset<>& form, unsigned int centroidPos)
{
    unsigned int maxSize = _height*_width;
    boost::dynamic_bitset<> symForm(maxSize);
    unsigned int symInf = (int)(centroidPos%_width);
    unsigned int symSup = ((int)centroidPos%_width+1);
    unsigned int formSize = 0;
    unsigned int nbCells = form.count();
    unsigned int spread, cellNewCol, cellNewPos;

    for(unsigned int i = 0; i< maxSize; i++)
    {
        if (form[i])
        {
            unsigned int celCol = (int)(i%_width);
            unsigned int celRow = (int)(i/_width);

            if(celCol<= symInf)
            {
                spread = symInf - celCol;

                cellNewCol = symSup + spread;
            }

            else
            {
                spread = celCol - symSup;

                cellNewCol = symInf - spread;
            }

            cellNewPos = celRow*_width + cellNewCol;

            if (cellNewPos< maxSize)
            {
                symForm.set(cellNewPos);
            }
            else
                return formSize;

            formSize++;

        }

        if(formSize == nbCells)
            break;

    }

    //Assert if new position is granted to all cells by the symmetry of the form or not
    form = symForm;
    return formSize;
}

// Give the result of the geometrical transformation of the form by specifying the one to apply
unsigned int Environment::geomTransResult(Graph g, vector<unsigned int> vertices, boost::dynamic_bitset<> form, unsigned int (Environment::*geomTrans)(boost::dynamic_bitset<>&, unsigned int))
{
    boost::dynamic_bitset<> rotForm = form;
    unsigned int maxSize = _height*_width;
    boost::dynamic_bitset<> oneBit(maxSize);
    unsigned int nbCells = form.count();
    unsigned int pos = maxSize-1;

    oneBit.set(pos);

    unsigned int centroidPos = findCentroid(form,oneBit,pos);

    unsigned int rotCells = (this->*geomTrans)(rotForm,centroidPos);

    while(rotCells< nbCells)
    {
        rotForm = form;

        oneBit >>= 1;
        --pos;

        centroidPos = findCentroid(rotForm,oneBit,pos);

        rotCells = (this->*geomTrans)(rotForm,centroidPos);

    }

    form = rotForm;

    return translationResult(g, vertices, form);
}

//Verify If a form already exists or its geometrical transformations
unsigned int Environment::existInGraph(Graph g, boost::dynamic_bitset<> form, vector<unsigned int> vertices)
{

    if(!translationResult(g,vertices,form))
    {
        if(!geomTransResult(g, vertices, form,&Environment::rotation270Result))
        {
            if(!geomTransResult(g, vertices, form,&Environment::rotation180Result))
            {
                if(!geomTransResult(g, vertices, form,&Environment::rotation90Result))
                {
                    if(!geomTransResult(g, vertices, form,&Environment::horSymResult))
                    {
                        return geomTransResult(g, vertices, form,&Environment::vertSymResult);
                    }
                    return geomTransResult(g, vertices, form,&Environment::horSymResult);
                }

                return geomTransResult(g, vertices, form,&Environment::rotation90Result);
            }
             return geomTransResult(g, vertices, form,&Environment::rotation180Result);
        }
        return geomTransResult(g, vertices, form,&Environment::rotation270Result);
    }
    return translationResult(g,vertices,form);

}


//Starting the reachable sets generation with a form
void Environment::setForm(boost::dynamic_bitset<> form, vector<unsigned int> positions)
{
    for(unsigned int i = 0; i< positions.size(); i++)
        form.set(positions[i]);
}


//Trigger a mitose
bool Environment::mitose(boost::dynamic_bitset<>& form, unsigned int motherPosition, char direction)
{
      bool mitose = false;
      unsigned int maxSize = _height*_width;

          //Each control(right, up, left and down) has its own mitosis rule
          switch(direction){
          case 'd' :
          {
              //If a up mitosis is required, ensure that there no cell above the mother cell and that the mitosis is possible regards to the grids's upper bounds
              if ((motherPosition >= _width) && !(form[motherPosition-_width]))
              {
                 form.set(motherPosition-_width); //so create the daughter cell above its mother

                 mitose = true;
             }
          } break;

          case 'u' :
          {
              //If a down mitosis is required, ensure that there no cell below the mother cell and that the mitosis is possible regards to the grids's lower bounds
              if ((motherPosition< maxSize-_width) && !(form[motherPosition+_width]))
              {
                  form.set(motherPosition+_width);

                  mitose = true;
              }
          }break;
//
//
          case 'l' :
          {

              //If a right mitosis is required, ensure that there no cell on the right of the mother cell
              if ((motherPosition< maxSize-1) && !(form[motherPosition+1]))
              {
                 if (motherPosition%_width != _width-1)
                 {
                     form.set(motherPosition+1);
                     mitose = true;
                 }

              }
          } break;
//
          case 'r' :
          {
              //If a left mitosis is required, ensure that there no cell on the left of the mother cell
              if ((motherPosition >= 1) && !(form[motherPosition-1]))
              {
                  //If it is
                  if (motherPosition%_width != 0)
                  {
                      form.set(motherPosition-1);
                       mitose = true;
                  }

              }
          }break;
      }

    return mitose;
}


//Display the final forms on a external file
void Environment::display(boost::dynamic_bitset<> form, unsigned int formLabel)
{
    //Open a file to design the grid thanks to the formcontainer and copy down the content of the genome.
    ofstream formFile;

    formFile.open("formFile", ios::app);
    if (formFile.bad()){ cerr<< "Impossible d'ouvrir le fichier !"<< endl;}

        unsigned int l = _height;
        while(!(l == 1))
        {
            unsigned int c=1;
            while(c< _width)
            {
                formFile<<form[l*_width-c]<<"     ";
                c++;
            }
            formFile<<form[l*_width-c];
            formFile<<endl;
            formFile<<endl;
            formFile<<endl;
            l--;
        }

        unsigned int c=1;
        while(c< _width)
        {
            formFile<<form[l*_width-c]<<"     ";
            c++;
        }


        formFile<<form[l*_width-c]<<endl;

        formFile<<"================="<<endl;

        formFile<<"N? : "<<formLabel;

        formFile<<endl;
        formFile<<"================="<<endl;

    formFile.close();

}

