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

  size_t n=atol(argv[2]);
  
  int width=int(pow(n,1.0/3.0));
  n=width*width*width;
  
  size_t precdigits=6;

  if(argc == 4) {
    precdigits=atol(argv[3]);
  }

  int*** a = new int**[width];
  for(int i=0; i < width; ++i) {
    a[i] = new int*[width];
    for(int j=0; j < width; ++j) {
      a[i][j] = new int[width];
    }
  }

  double *diag = new double[n];
  for(int i=0; i < n; ++i) {
    diag[i]=0;
  }

  int m=width*width*(width-1)*3-(n-1);
  
  double *rS = new double[n];

  double *printVal = new double[m];
  int idx=0;

  double stretch=0;

  int now=0;
  int flag=1, flag2=1;

  for(int k=0; k <= width-1; ++k) {
    if(flag2) {
      for(int i=0; i <= width-1; ++i) {
        if(flag) {
          for(int j=width-1; j >= 0; j--) {
            a[k][i][j]=now++;
          }
        }
        else {
          for(int j=0; j <= width-1; ++j) {
            a[k][i][j]=now++;
          }
        }
        flag=1-flag;
      }
    }
    else {
      for(int i=width-1; i >= 0; --i) {
        if(flag) {
          for(int j=width-1; j >= 0; --j) {
            a[k][i][j]=now++;
          }
        }
        else {
          for(int j=0; j <= width-1; ++j) {
            a[k][i][j]=now++;
          }
        }
        flag=1-flag;
      }
    }
    flag2=1-flag2;
  }

  rS[0]=0;
  for(int i=0; i < n-1; ++i) {
    rS[i+1]=rS[i]+rand()%1000+1;
  }


  for(int i=0; i <= width-1; ++i) {
    for(int j=0; j <= width-1; ++j) {
      for(int k=0; k <= width-2; ++k) {
        if(abs(a[i][j][k]-a[i][j][k+1])!=1) {
          double r=abs(rS[a[i][j][k]]-rS[a[i][j][k+1]]);
          double newr=r;
          double maxr=0;
          int dist=a[i][j][k]-a[i][j][k+1];
          int start;
          if(dist < 0) {
            start=a[i][j][k+1];
          }
          else {
            start=a[i][j][k];
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
          if((newr+r-maxr)/maxr < r/newr) {
            std::cerr << "low stretch tree no longer path" << std::endl;
            return -1;
          }
          printVal[idx]=newr;
          idx++;
        }
        if(abs(a[i][k][j]-a[i][k+1][j])!=1) {
          double r=abs(rS[a[i][k][j]]-rS[a[i][k+1][j]]); 
          double newr=r;
          double maxr=0;
          int dist=a[i][k][j]-a[i][k+1][j];
          int start;
          if(dist < 0) {
            start=a[i][k][j];
          }
          else {
            start=a[i][k+1][j];
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
          if((newr+r-maxr)/maxr < r/newr) {
            std::cerr << "low stretch tree no longer path" << std::endl;
            return -1;          
          }
          printVal[idx]=newr;
          idx++;
        }
        if(abs(a[k][i][j]-a[k+1][i][j])!=1) {
          double r=abs(rS[a[k][i][j]]-rS[a[k+1][i][j]]);
          double newr=r;
          double maxr=0;
          int dist=a[k][i][j]-a[k+1][i][j];
          int start;
          if(dist < 0) {
            start=a[k][i][j];
          }
          else {
            start=a[k+1][i][j];
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
          if((newr+r-maxr)/maxr < r/newr) {
            std::cerr << "low stretch tree no longer path" << std::endl;
            return -1;
          }
          printVal[idx]=newr;
          idx++;
        }
      }
    }
  }

  idx=0;

  mmfileout << "%%MatrixMarket matrix coordinate real symmetric" << std::endl;
  mmfileout << "%%" << std::endl;
  mmfileout << "%%Structure 3Mesh" << std::endl;
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
    mmfileout << i+1 << ' ' << i+2 << ' '  << std::setprecision(precdigits+1) << -roundr << std::endl;
    rfileout << i << ' ' << i+1 << ' ' << rS[i+1]-rS[i] << std::endl;
    diag[i]+=roundr;
    diag[i+1]+=roundr;
  }
  
  for(int i=0; i <= width-1; ++i) {
    for(int j=0; j <= width-1; ++j) {
      for(int k=0; k <= width-2; ++k) {
        if(abs(a[i][j][k]-a[i][j][k+1])!=1) {
          double roundr=round(pow(10.,precdigits)/printVal[idx])/pow(10.,precdigits);
          if(roundr == 0 || isnan(roundr)) {
            std::cerr << "increase precision beyond " << precdigits
                      << " because edge weights too small" << std::endl;
            return -1;
          }
          diag[a[i][j][k]]+=roundr;
          diag[a[i][j][k+1]]+=roundr;
          mmfileout << a[i][j][k]+1 << ' ' << a[i][j][k+1]+1 << ' '  << std::setprecision(precdigits+1) << -roundr << std::endl;
          rfileout << a[i][j][k] << ' ' << a[i][j][k+1] << ' ' << printVal[idx] << std::endl;
          idx++;
        }
        if(abs(a[i][k][j]-a[i][k+1][j])!=1) {
          double roundr=round(pow(10.,precdigits)/printVal[idx])/pow(10.,precdigits);
          if(roundr == 0 || isnan(roundr)) {
            std::cerr << "increase precision beyond " << precdigits
                      << " because edge weights too small" << std::endl;
            return -1;
          }
          diag[a[i][k][j]]+=roundr;
          diag[a[i][k+1][j]]+=roundr;
          mmfileout << a[i][k][j]+1 << ' ' << a[i][k+1][j]+1 << ' '  << std::setprecision(precdigits+1) << -roundr << std::endl;
          rfileout << a[i][k][j] << ' ' << a[i][k+1][j] << ' ' << printVal[idx] << std::endl;
          idx++;
        }
        if(abs(a[k][i][j]-a[k+1][i][j])!=1) {
          double roundr=round(pow(10.,precdigits)/printVal[idx])/pow(10.,precdigits);
          if(roundr == 0 || isnan(roundr)) {
            std::cerr << "increase precision beyond " << precdigits
                      << " because edge weights too small" << std::endl;
            return -1;
          }
          diag[a[k][i][j]]+=roundr;
          diag[a[k+1][i][j]]+=roundr;
          mmfileout << a[k][i][j]+1 << ' ' << a[k+1][i][j]+1 << ' '  << std::setprecision(precdigits+1) << -roundr << std::endl;
          rfileout << a[k][i][j] << ' ' << a[k+1][i][j] << ' ' << printVal[idx] << std::endl;
          idx++;
        }
      }
    }
  }


  for(int i=0; i < n; ++i) {
    mmfileout << i+1 << ' ' << i+1 << ' ' << std::setprecision(precdigits+1) << diag[i] << std::endl;
  }

  for(int i=0; i < width; ++i) {
    for(int j=0; j < width; j++) {
      delete a[i][j];
    }
    delete a[i];
  }

  delete a;
  delete diag;
  delete rS;
  delete printVal;

  rfileout.close();
  mmfileout.close();

  return 0;
}
