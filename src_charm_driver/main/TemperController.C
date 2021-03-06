#include "charm++.h"
#include "utility/util.h"
#include "cpaimd.h"
#include "energyGroup.h"
#include "InstanceController.h"
#include "TemperController.h"
#include "main/CPcharmParaInfoGrp.h"
extern double altRandom(long *);

const double kinv=315777.0; // atomic units, inverse of Boltzmann's constant
extern CProxy_InstanceController         instControllerProxy;

TemperController::TemperController(int _simtype, double *_temperatures, int numtemperatures, long _seed, std::string historyfile, std::string dirname) : simType(_simtype), seed(_seed), history(historyfile), output_directory(dirname)
{
  /* insert initialization here */  
  first=true;
  reportedIn=0;
  temperIteration=0;
  numTempers=config.UberKmax;
  numBeads=config.UberImax;
  CkPrintf("TemperController for Tempers %d and beads %d\n",numTempers,numBeads);
  if(numTempers != numtemperatures)  CkAbort("numTempers must be equal to number of temperatures for the TemperController");
  if(numTempers%2 != 0 && numTempers>1)  CkAbort("numTempers must be even");
  temperEnergy=new double[numTempers];
  temperatures=new double[numTempers];
  index = new int[numTempers];
  
  for(int i=0; i<numTempers; i++) index[i]=i;
  memcpy(temperatures, _temperatures, sizeof(double) * numtemperatures);
  switchdir=0;

}


void TemperController::totalEnergy(int temper, int iteration, const EnergyStruct &energies)
{
  sumEnergies(energies, temper);
  CPcharmParaInfo *sim  = CPcharmParaInfo::get(); 
  if((iteration % sim->nscreen_frq)==0)
    {
      CkPrintf("[t=%d] Iter [%d] TOTAL_ENERGY          = %5.8lf\n",temper, iteration, temperEnergy[temper]);
    }
}

void TemperController::acceptData(int temper, int iteration, const EnergyStruct &energies)
{
  reportedIn++;

  if(reportedIn == numTempers*numBeads)
    {
      temperIteration++;
      if(temperIteration % config.temperCycle ==0)
	{ // this is a tempering cycle
	  sumEnergies(energies, temper);
	  acceptData();
	}
      else
	{
	  // this is just a sync to keep the instances in the same round
	  reportedIn=0;
	  //	  CkPrintf("TemperController hanging\n");
	  barrier();
	}
    }
}

void TemperController::sumEnergies(const EnergyStruct &inEnergy, int temper)
{
  temperEnergy[temper]= inEnergy.enl + inEnergy.eext + inEnergy.eke 
    + inEnergy.ehart + inEnergy.egga + inEnergy.eexc + inEnergy.fictEke 
    + inEnergy.eewald_recip + inEnergy.eewald_real;    
  if(simType==1) temperEnergy[temper] += inEnergy.potPIMDChain; 
}

void TemperController::acceptData()
{
  reportedIn=0;

  /* switch energies based on a probability. Take into account nearest neighbor temperatures are moving around. 
     Index has the map.
   */
  static int switchcounter=0;
  CkPrintf("TemperController switch %d\n",switchcounter++);
  /* Glenn wants his global crutch */
  CPcharmParaInfo *sim = CPcharmParaInfo::get();
  for(int i=switchdir; i<numTempers-switchdir; i+=2)
    {
      double t1=temperatures[i];
      double t2=temperatures[i+1];
      double deltaE=temperEnergy[index[i]] - temperEnergy[index[i+1]];
      double deltaB=kinv *( 1.0/t1 - 1.0/t2);
      double p=exp(deltaE*deltaB);
      double randout=altRandom(&seed);
      if(p > randout)
	{
	  // we will send the temperature using the index map
	  int iii=index[i];
	  index[i]=index[i+1];
	  index[i+1]=iii;
	}
    }
  switchdir = (switchdir==0) ? 1 : 0;

  UberCollection uberindex;

  /* putting this in a for loop because we expect this to be personalized */
  for(int i=0; i < numTempers; i++)
  { 
    uberindex.idxU.x=index[i];
    uberindex.setPO();
    //      CkPrintf("sending temp to %d,%d,%d,%d\n",index.idxU.x, index.idxU.y, index.idxU.z, index.idxU.s);
    instControllerProxy[uberindex.proxyOffset].acceptNewTemperature(temperatures[i]);
  }
  output();
}


void TemperController::output()
{
  FILE *tFile =openTemperTrack(output_directory.c_str(),"temperature_order.out","w");
  if(!tFile)
    {
      CkPrintf("unable to open file %s/%s\n",output_directory.c_str(),"temperature_order.out");
	
    }
  FILE *atFile;
  if(first)
    {
      atFile =openTemperTrack(output_directory.c_str() ,history.c_str(),"a");
      if(!atFile)
	{
	  CkPrintf("unable to open file %s/%s\n",output_directory.c_str(),history.c_str());
	  
	}
      fprintf(atFile,"Temper Index Temperature NTemper %d\n", numTempers);
      first=false;
    }
  else
    {
      atFile =openTemperTrack(output_directory.c_str(), history.c_str(), "a");
    }
  fprintf(tFile,"Temper Index Temperature NTemper %d\n", numTempers);
  for(int i=0; i<numTempers; i++)
    {
      fprintf(tFile,"%d %d %g\n",i, index[i], temperatures[i]);
      fprintf(atFile,"%d %d %g\n",i, index[i], temperatures[i]);
      CkPrintf("Temps Now %d %d %g\n",i, index[i], temperatures[i]);
    }
  fclose(tFile);
  fclose(atFile);
}

void TemperController::barrier()
{
  static int barriercounter=0;
  instControllerProxy.resumeFromTemper();
}


TemperController::~TemperController()
{
  delete [] index;
  delete [] temperEnergy;
  delete [] temperatures;
}
#include "temperController.def.h"
