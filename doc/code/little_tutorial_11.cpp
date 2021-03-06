/** little_tutorial_11.cpp
 *
 * (C) Copyright 2011 François Mauger, Christian Pfligersdorffer
 *
 * Use, modification and distribution is subject to the Boost Software
 * License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 */

/**
 * The intent of this program is to serve as a tutorial for
 * users of the little archive in the framework of 
 * the Boost/Serialization library. 
 *
 * This example program compares the times needed to serialize
 * and deserialize some large amount of data using little archive and 
 * text archives.
 *
 */

#include <string>
#include <fstream>
#include <vector>

#include <boost/archive/little_oarchive.hpp>
#include <boost/archive/little_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/timer.hpp>

using namespace std;

class data_type
{
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize (Archive & ar, const unsigned int version);
public:
  void print (ostream & out, const string & title) const;
public:
  vector<double> values;
  data_type ();
};

data_type::data_type () : values ()
{
  return;
}

void data_type::print (ostream & out, const string & title) const
{
  out << endl;
  out << title << " :" << endl;
  bool skip = false;
  for (int i = 0; i < this->values.size (); ++i)
    {
      if ((i >= 12) && (i < (int) this->values.size () - 8)) 
	{
	  if (! skip) out << " ..." << endl;
	  skip = true;
	  continue;
	}
      out.precision (16);
      out.width (18);
      out << this->values [i] << ' ' ;
      if ((i%4) == 3) clog << endl;
    }
  out << endl;
  return;
}
  
template<class Archive>
void data_type::serialize (Archive & ar, const unsigned int version)
{
  ar & values;
  return;
}

double do_little_out (const data_type & a_data)
{
  string filename = "little_11.data"; 
  ofstream fout (filename.c_str (), ios_base::binary);
  boost::timer io_timer;    
  {
    boost::archive::little_oarchive olittle (fout);
    olittle & a_data;
  }
  return io_timer.elapsed ();
}

double do_little_in (data_type & a_data)
{
  string filename = "little_11.data"; 
  ifstream fin (filename.c_str (), ios_base::binary);
  boost::timer io_timer;    
  {
    boost::archive::little_iarchive ilittle (fin);
    ilittle & a_data;
  }
  return io_timer.elapsed ();
}

double do_text_out (const data_type & a_data)
{
  string filename = "little_11.txt"; 
  ofstream fout (filename.c_str ());
  boost::timer io_timer;    
  {
    boost::archive::text_oarchive ota (fout);
    ota & a_data;
  }
  return io_timer.elapsed ();
}

double do_text_in (data_type & a_data)
{
  string filename = "little_11.txt"; 
  ifstream fin (filename.c_str ());
  boost::timer io_timer;    
  {
    boost::archive::text_iarchive ita (fin);
    ita & a_data;
  }
  return io_timer.elapsed ();
}

int main (void)
{
  double elapsed_time_little_out; 
  double elapsed_time_text_out; 
  double elapsed_time_little_in; 
  double elapsed_time_text_in; 
  data_type my_data; // A data structure to be stored then loaded.

  {
    // Fill the vector with random values :
    size_t dim = 10000000;
    my_data.values.reserve (dim);
    boost::random::mt19937 rng;
    boost::random::uniform_real_distribution<> flat (0.0, 100.0);
    for (int i = 0; i < dim; ++i)
      {      
	double val = flat (rng);
	my_data.values.push_back (val);
      }
    my_data.print (clog, "Stored data in little archive and text archive");
  }

  {
    // Store in little archive :
    elapsed_time_little_out = do_little_out (my_data);
  }

  {
    // Store in text archive :
    elapsed_time_text_out = do_text_out (my_data);
  }     

  {
    my_data.values.clear ();
    // Load from little archive :
    elapsed_time_little_in = do_little_in (my_data);
    my_data.print (clog, "Loaded data from little archive");
  }

  {
    my_data.values.clear ();
    // Load from text archive :
    elapsed_time_text_in = do_text_in (my_data);
    my_data.print (clog, "Loaded data from text archive");
  }
  
  clog << "little archive  store I/O elapsed time : " << elapsed_time_little_out  << " (second)" << endl;
  clog << "Text store I/O elapsed time : " << elapsed_time_text_out << " (second)" << endl;  
  clog << "little archive  load  I/O elapsed time : " << elapsed_time_little_in   << " (second)" << endl;
  clog << "Text load  I/O elapsed time : " << elapsed_time_text_in  << " (second)" << endl;

  return 0;
}

// end of little_tutorial_11.cpp
