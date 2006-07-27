
/********************************************************************************
 * Lambda created by Eric Bohm 2006/7/23
 *
 *
 * 
********************************************************************************/

//============================================================================
//cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
//============================================================================
/** \file lambda.h
 *
 */
//============================================================================

#ifndef _lambda_h_
#define _lambda_h_

#include "CLA_Matrix.h"
#include "ckPairCalculator.h"
extern Config config;
extern int nstates;
extern  PairCalcID pairCalcID1;
extern  PairCalcID pairCalcID2;

class Lambda : public CBase_Lambda{
 public:
  Lambda(){}
  Lambda(CkMigrateMessage *m){}
  Lambda(int lgrain, int ograin, int sgrain) : lambdaGrainSize(lgrain), orthoGrainSize(ograin), sGrainSize(sgrain) 
    {
    }
  
  ~Lambda(){
  }
  // catch lambda for later non_minimization use
  void acceptAllLambda(CkReductionMsg *msg); 

  // catch lbresume reduction
  void lbresume(CkReductionMsg *msg); 

  // catch lambda for later non_minimization use
  void acceptSectionLambda(CkReductionMsg *msg); 

  void makeSections(int indexSize, int *indexZ);

  virtual void pup(PUP::er &p){
//    CBase_Lambda::pup(p);
    ArrayElement2D::pup(p);
    p | pcLambdaProxy;
    p | pcLambdaRedProxy;
  }

 private:
  CProxySection_PairCalculator pcLambdaProxy;
  CProxySection_PairCalculator pcLambdaRedProxy;
  int lambdaGrainSize;
  int orthoGrainSize;
  int sGrainSize;
  
};

/**
 * provide procnum mapping for Lambda
 */
class LambdaMap : public CkArrayMap {
  public:
    LambdaMap(int NN,int _nLambda):N(NN), nLambda(_nLambda)
      {
	offset=0;
	if(nLambda<CkNumPes())
	  offset=1;  //skip proc 0
      }
    virtual int procNum(int arrayHdl, const CkArrayIndex &iIndex){
      int *index=(int *) iIndex.data();
      return (N * index[0] + index[1] + offset) % CkNumPes();
    }
  private:
    int N;
    int nLambda;
    int offset;
};

#endif // #ifndef _lambda_h_
