#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>//isdigit
#include <algorithm>
#include <numeric>

const int MAX_L = 4;
const double delta_g = 0.25; //g-factor tollerance

//******************************************************************************
double spin (double twoSplus1)
// Outputs spin given the multiplicity
{
  return double(twoSplus1-1)/2;
}

//******************************************************************************
double gFactor_nr (double twoSplus1, int L, double J)
/*non-relativistic g-factor*/
{
  if(J==0) return 0;
  double S = spin(twoSplus1);
  return 1.5 + (S*(S+1)-L*(L+1))/(2*J*(J+1));
}

//******************************************************************************
std::string termL (int x)
{
  static std::string spectroscopic_list = "SPDFGHIKLMNOQRTUV";
  std::string T;
  if( x < (int)spectroscopic_list.size() )
    T = spectroscopic_list.at(x);
  else
    T = std::to_string(x);
  return T;
}

//******************************************************************************
int l_atoi (char c)
{
  static std::string spectroscopic_list = "spdfghiklmnoqrtuv";
  int il = 0;
  for(auto& cl : spectroscopic_list){
    if (c==cl)
      return il;
    else
      ++il;
  }
  return -1;
}

//******************************************************************************
std::vector<int> make_l_list(std::string el_list)
{
  std::vector<int> l_list;
  if(el_list.empty()) return l_list;
  int n = 1;
  for(auto &c : el_list){
    if (isdigit(c)){
      n = c - '0';
    }
    else{
      int il = l_atoi(c);
      for(int i=0; i<n; i++)
        l_list.push_back(il);
    }
  }
  return l_list;
}

//******************************************************************************
int get_maxL(std::vector<int> l_list)
{
  if(l_list.size()==0) return MAX_L;
  return std::accumulate(l_list.begin(), l_list.end(), 0);
}
//******************************************************************************
int get_minL(std::vector<int> l_list)
/*
min_L = max_l - sum(rest)
      = 2*max_l - sum(all)
*/
{
  if(l_list.size()==0) return 0;
  auto max_l = *std::max_element(l_list.begin(), l_list.end());
  auto sum_all = std::accumulate(l_list.begin(), l_list.end(), 0);
  auto tmp =  2*max_l - sum_all;
  if(tmp<0) return 0; //XXX this not actually correct... can fix later
  else return tmp;
}


//******************************************************************************
void instructions()
{
  std::cout << "----------------------------------\n";
  std::cout<< "Enter as: J (as decimal), g-factor, then "
    <<" either the leading electron config, or just the number of electrons.\n"
    <<"e.g., for J=3/2, g=1.5, and two-electrons in p,d orbitals, enter:\n"
    <<" 1.5 1.5 pd\n"
    <<"Or:\n 1.5 1.5 2\n"
    <<"Put g=0 for any g-factor (wildcard). Put '0' for any orbitals\n";
  std::cout<<"(Nb: you may have to ommit orbitals, not leading config not "
    <<"always good enough\n\n";
}


//******************************************************************************
int main(){

  instructions();

  while(true){
    double J;
    double in_g;
    std::string in_str;
    std::cout<<"Enter: J g config: \n <";
    std::cin >> J;
    std::cin >> in_g;
    std::cin >> in_str;
    if (std::cin.fail())
    {
    	std::cout<<"I don't understand. Try again."<<"\n";
      instructions();
    	break;
    }

    int in_num_el = 0;
    if(in_str.size()==1){
      for(auto &c : in_str){
        if (isdigit(c)) in_num_el = c - '0';
        in_str = "";
      }
    }

    auto l_list = make_l_list(in_str);
    auto num_el = (int) l_list.size();
    if(num_el==0) num_el = in_num_el;

    auto min_L = get_minL(l_list);
    auto max_L = get_maxL(l_list);

    int max_mp = 6;
    int min_mp = 1;
    int d_mp = 1;
    if (num_el == 0){
      //unknown num electrons. Allowed values: 1,2,3,4,...
      min_mp = 1;
      d_mp = 1;
    }else if (num_el % 2 == 0){
      //Even  num electrons. Allowed values: 1,3,5,...
      min_mp = 1;
      d_mp = 2;
    }else{
      //odd num electrons. Allowed values: 2,4,6,...
      min_mp = 2;
      d_mp = 2;
    }


    int count = 0;
    std::cout<<"\n Possible states: (2S+1)L_J  -  g_factor\n";
    for(int mp = min_mp; mp <= max_mp; mp += d_mp){
      for(int L = min_L; L <= max_L; L++){
        double nr_g = gFactor_nr(mp,L,J);
        double delta = fabs((in_g - nr_g)/nr_g);
        if (in_g==0 || delta<delta_g){
          //good state!
          std::string strJ = (int(2*J) % 2 == 0)? std::to_string((int)J)
            : std::to_string((int)(2*J))+"/2";
          std::string strL = termL(L);
          std::cout<<mp<<"-"<<strL<<"_"<<strJ<<"  -  "<<nr_g<<"\n";
          ++count;
        }
      }
    }
    std::cout<<"Found "<<count<<" states.\n\n";
    if(count==0){
      std::cout<<"Perhaps g-factor too tight?"" Currently del(g)/g < "
        <<delta_g<<"\n\n";
    }

  }


}
