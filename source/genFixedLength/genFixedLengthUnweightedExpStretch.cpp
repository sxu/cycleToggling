#include <iostream>
#include <iomanip>
#include <math.h>
#include <fstream>

int main(int argc, char *argv[]) {

  if(argc < 5) {
    std::cerr << "Please specify filename, n, hop, and seed." << std::endl;
    return -1;
  }
  
  std::string rfile(argv[1]);
  std::string mmfile=rfile+".mtx";
  rfile=rfile+".txt";

  std::ofstream rfileout, mmfileout;
  rfileout.open(rfile);
  mmfileout.open(mmfile);

  size_t n = atol(argv[2]);
  size_t hop = atol(argv[3]);
 
  if(hop > n) {
    std::cerr << "hop length of paths need to be < n" << std::endl;
    return -1;
  }

  size_t precdigits=6;

  double *diag = new double[n];
  for(int i=0; i < n; ++i) {
    diag[i]=0;
  }

  double *rS = new double[n];
  
  int idx=0;

  size_t seed = atol(argv[4]);

  srand(seed);
  int m=n-hop;

  double stretch=0;

  double *printVal = new double[m];

  rS[0]=0;
  for(int i=0; i < n-1; ++i) {
    diag[i]+=1;
    diag[i+1]+=1;
    rS[i+1]=rS[i]+1;
  }

  for(int i=0; i < n-hop; ++i) {
    int u=i;
    int v=i+hop;
    double r=rS[v]-rS[u];
    double newr=r;
    while(double(rand())/double(RAND_MAX) < 0.5) {
      if(newr/2+r-1 < 2*r/newr) {
        break;
      }
      newr/=2;
    }
    stretch+=(r/newr);
    if((newr+r-1) < r/newr) {
      std::cerr << "low stretch tree no longer path" << std::endl;
      return -1;
    }
    printVal[idx]=newr;
    idx++;
  }

  mmfileout << "%%MatrixMarket matrix coordinate real symmetric" << std::endl;
  mmfileout << "%%" << std::endl;
  mmfileout << "%%Structure FixedLength" << std::endl;
  mmfileout << "%%Hop Length " << hop << std::endl;
  mmfileout << "%%Path Weights Unweighted" << std::endl;
  mmfileout << "%%Cycle Stretch ExpStretch" << std::endl;
  mmfileout << "%%Total Stretch " << stretch << std::endl;
  mmfileout << "%%Precision Digits " << precdigits << std::endl;
  mmfileout << n << ' ' << n  << ' ' << m+n-1+n << std::endl;

  rfileout << n << ' ' << m+n-1 << std::endl;

  for(int i=0; i < n-1; ++i) {
    mmfileout << i+1 << ' ' << i+2 << ' ' << -1 << std::endl;
    rfileout << i << ' ' << i+1 << ' ' << 1 << std::endl;
  }

  idx=0;
  for(int i=0; i < n-hop; ++i) {
    int u=i;
    int v=i+hop;
    double roundr=round(pow(10.,precdigits)/printVal[idx])/pow(10.,precdigits);
    if(roundr == 0 || isnan(roundr)) {
      std::cerr << "increase precision beyond " << precdigits
                << " because edge weights too small" << std::endl;
      return -1;
    }
    diag[u]+=roundr;
    diag[v]+=roundr;
    mmfileout << u+1 << ' ' << v+1 << ' '  << std::setprecision(precdigits+1) << -roundr << std::endl;
    rfileout << u << ' ' << v << ' ' << printVal[idx] << std::endl;
    idx++;
  }


  for(int i=0; i < n; ++i) {
    mmfileout << i+1 << ' ' << i+1 << ' ' << std::setprecision(precdigits+1) << diag[i] << std::endl;
  }

  delete diag;
  delete rS;
  delete printVal;

  return 0;
}

