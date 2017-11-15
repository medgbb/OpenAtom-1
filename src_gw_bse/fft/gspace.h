#ifndef GSPACE_H
#define GSPACE_H


class GSPACE{
  public:

    int ng;  // number of g vectors 

    int *ig;
    int *jg;
    int *kg;
    
    // size of FFT box for density
    //int nfftDense[3];
    int *ig_diff;
    int *jg_diff;
    int *kg_diff;
    
};

#endif
