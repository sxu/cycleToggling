#include <iostream>
#include <iomanip>
#include <math.h>
#include <fstream>

int main(int argc, char *argv[])
{
  if (argc < 3) {
    std::cerr << "Please specify filename and n." << std::endl;
    return -1;
  }

  std::string rfile(argv[1]);
  std::string mmfile=rfile+".mtx";
  rfile=rfile+".txt";

  std::ofstream rfileout, mmfileout;
  rfileout.open(rfile);
  mmfileout.open(mmfile);

  size_t n = atol(argv[2]);	
		
  int width=int(sqrt(n));
  n=width*width;

  size_t precdigits=6;

  if(argc == 4) {
    precdigits=atol(argv[3]);
  }

  double *diag = new double[n];
  for(int i=0; i < n; ++i) {
    diag[i]=0;
  }

  int** a = new int*[width];
  for(int i = 0; i < width; ++i)
    a[i] = new int[width];

  int m=width*(width-1)*2-(n-1);

  double *rS = new double[n];
  double *printVal = new double[m];
  int idx=0;

  double stretch=0;

  int now=0;
  for(int i=0; i <= width-1; ++i) {
    a[0][i]=now++;
  }
  for(int i=1; i <= width-1; ++i) {
    a[i][width-1]=now++;
  }

  int flag=1;
  for(int i=width-1; i >= 1; --i) {
    if(flag) {
      for(int j=width-2; j >= 0; --j) {
        a[i][j]=now++;
      }
    }
    else{
      for(int j=0; j <= width-2; ++j) {
        a[i][j]=now++;
      }
    }
    flag=1-flag;
  }
  

  rS[0]=0;
  for(int i=0; i < n-1; ++i) {
    rS[i+1]=rS[i]+rand()%1000+1;
  }
    
    
  for(int i=0; i <= width-1; ++i) {
    for(int j=0; j <= width-2; ++j) {
      if(abs(a[i][j]-a[i][j+1])!=1) {
        double r=abs(rS[a[i][j]]-rS[a[i][j+1]]);
        double newr=r;
        double maxr=0;
        int dist=a[i][j]-a[i][j+1];
        int start;
        if(dist < 0) {
          start=a[i][j];
        }
        else {
          start=a[i][j+1];
        }
        for(int l=0; l < abs(dist); ++l) {
          double tempr=abs(rS[start+l+1]-rS[start+l]);
          if(tempr > maxr) {
            maxr=tempr;
          }
        }
        while(double(rand())/double(RAND_MAX) < 0.5) {
          if((newr/2+r-maxr)/maxr < 2*r/newr) {
            break;
          }

          newr/=2;
        }
        stretch+=(r/newr);
        if((newr+r-maxr)/maxr < (r/newr)) {
          std::cerr << "low stretch tree no longer path" << std::endl;
          return -1;
        }
        printVal[idx]=newr;
        idx++;
      } 
    }
  }

  for(int i=0; i <= width-1; ++i) {
    for(int j=0; j <= width-2; ++j) {
      if(abs(a[j][i]-a[j+1][i])!=1) {
        double r=abs(rS[a[j][i]]-rS[a[j+1][i]]);
        double newr=r;
        double maxr=0;
        int dist=a[j][i]-a[j+1][i];
        int start;
        if(dist < 0) {
          start=a[j][i];
        }
        else {
          start=a[j+1][i];
        }
        for(int l=0; l < abs(dist); ++l) {
          double tempr=abs(rS[start+l+1]-rS[start+l]);
          if(tempr > maxr) {
            maxr=tempr;
          }
        }

        while (double(rand())/double(RAND_MAX) < 0.5) {
          if((newr/2+r-maxr)/maxr < 2*r/newr) {
            break;
          }
          
          newr/=2;
        }
        stretch+=(r/newr);
        if((newr+r-maxr)/maxr < r/newr) {
          std::cerr << "low stretch tree no longer path" << std::endl;
          return -1;
        }
        printVal[idx]=newr;
        idx++;
      }
    }
  }

  mmfileout << "%%MatrixMarket matrix coordinate real symmetric" << std::endl;
  mmfileout << "%%" << std::endl;
  mmfileout << "%%Structure 2Mesh" << std::endl;
  mmfileout << "%%Path Weights RandomWeighted" << std::endl;
  mmfileout << "%%Cycle Stretch ExpStretch" << std::endl;
  mmfileout << "%%Total Stretch " << stretch << std::endl;
  mmfileout << "%%Precision Digits " << precdigits << std::endl;
  mmfileout << n << ' ' << n  << ' ' << m+n-1+n << std::endl;

  rfileout << n << ' ' << m+n-1 << std::endl;
  


  for (int i=0; i < n-1; ++i) {
    double roundr=round(pow(10.,precdigits)/(rS[i+1]-rS[i]))/pow(10.,precdigits);
    if(roundr == 0 || isnan(roundr)) {
      std::cerr << "increase precision beyond " << precdigits
                << " because edge weights too small" << std::endl;
      return -1;
    }
    mmfileout << i+1 << ' ' << i+2 << ' ' << std::setprecision(precdigits+1) << -roundr << std::endl;
    rfileout << i << ' ' << i+1 << ' ' << rS[i+1]-rS[i] << std::endl;
    diag[i]+=roundr;
    diag[i+1]+=roundr;
  }
  
  idx=0;

  for(int i=0; i <= width-1; ++i) {
    for(int j=0; j <= width-2; ++j) {
      if(abs(a[i][j]-a[i][j+1])!=1) {
        double roundr=round(pow(10.,precdigits)/printVal[idx])/pow(10.,precdigits);
        if(roundr == 0 || isnan(roundr)) {
          std::cerr << "increase precision beyond " << precdigits
                    << " because edge weights too small" << std::endl;
          return -1;
        }
        diag[a[i][j]]+=roundr;
        diag[a[i][j+1]]+=roundr;
        mmfileout << a[i][j]+1 << ' ' << a[i][j+1]+1 << ' ' << std::setprecision(precdigits+1) << -roundr << std::endl;
        rfileout << a[i][j] << ' ' << a[i][j+1] << ' ' << printVal[idx] << std::endl;
        idx++;
      }
    }
  }

  for(int i=0; i <= width-1; ++i) {
    for(int j=0; j <= width-2; ++j) {
      if(abs(a[j][i]-a[j+1][i])!=1) {
        double roundr=round(pow(10.,precdigits)/printVal[idx])/pow(10.,precdigits);
        if(roundr == 0 || isnan(roundr)) {
          std::cerr << "increase precision beyond " << precdigits
                    << " because edge weights too small" << std::endl;
          return -1;
        }
        diag[a[j][i]]+=roundr;
        diag[a[j+1][i]]+=roundr;
        mmfileout << a[j][i]+1 << ' ' << a[j+1][i]+1 << ' ' << std::setprecision(precdigits+1) << -roundr << std::endl;
        rfileout << a[j][i] << ' ' << a[j+1][i] << ' ' << printVal[idx] << std::endl;
        idx++;
      }
    }
  }
  

  for(int i=0; i < n; ++i) {
    mmfileout << i+1 << ' ' << i+1 << ' ' << std::setprecision(precdigits+1) << diag[i] << std::endl;
  }
  
  for(int i=0; i < width; ++i) {
    delete a[i];
  }
  delete a;
  delete diag;
  delete rS;
  delete printVal;

  mmfileout.close();
  rfileout.close();
  
  return 0;
}
