#ifndef _INSTANCECONTROLLER_H_
#define _INSTANCECONTROLLER_H_
//#include "InstanceController.decl.h"
class ICCookieMsg : public CkMcastBaseMsg, public CMessage_ICCookieMsg {
  public:
    int junk;
};

/****************************************************************************
 * Handles launch synchronization for each instance.
 * 
 * Catch all for per instance synchronization of any kind.  
 *
 ****************************************************************************/
class InstanceController: public CBase_InstanceController {
  public:
    InstanceController_SDAG_CODE
    double Timer;
    InstanceController();
    ~InstanceController(){}
    InstanceController(CkMigrateMessage *m){}
    void init();
    void doneInit(CkReductionMsg *msg);
    void initCookie(ICCookieMsg *msg);
    void callPrintEnergyHart(CkReductionMsg *msg);
    void callPrintEnergyEexc(CkReductionMsg *msg);
    void callPrintFictEke(CkReductionMsg *msg);
    void callPrintEnergyEke(CkReductionMsg *m);
    void callAllDoneCPForces(int);
    void allDoneCPForcesAllKPoint(int);
    void cleanExit(CkReductionMsg *m);
    void cleanExitAll(CkReductionMsg *m);
    void acceptNewTemperature(double temp);
    void useNewTemperature(double temp);
    void atomsDoneNewTemp(CkReductionMsg *m);
    void gspDoneNewTemp(CkReductionMsg *m);
    void fmagMinTest(CkReductionMsg *m);
    CProxySection_CP_State_GSpacePlane gTemperBeadProxy;
  private:

    int done_init;
    int numKpointforces;
    bool atomsTempDone;
    bool gspTempDone;
    bool printToScreen;
    CkSectionInfo allKPcookie;
};

#endif
