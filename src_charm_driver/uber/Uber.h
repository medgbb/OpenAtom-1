/** addtogroup Uber
 *  @{ 
 *
 * Uber Arrays connect multiple arrays by referring to them by Uber Indexed
 * collections.  So all elements which have Uarr[i][j][k] are one logical
 * instance.
 *
 * Where we formerly had readonly proxies we now have readonly CkVec's of
 * proxies.  Each index tuple (i,j,k) will have an associated proxyOffset (PO).
 * someProxy[PO] will have the proxy for the tuple associated with PO for all
 * proxies.
 *
 * POs will be assigned using a dense 3D enumeration with k the innermost. So
 * the PO is always derivable from the tuple and visa versa, but we keep PO to
 * avoid recalculation overhead.  No race condition for the ID space.  Not that
 * this matters since we don't have a scenario in which we create instances in
 * parallel.
 *
 * PO=(i*Jmax+j) * Kmax +k;
 *
 * In practice, typical array objects only need a minority subset of the
 * proxies in their instance, and perhaps 1 proxy outside their instance.
 * These can be constructed local to the object without a great proliferation
 * of proxies.  They simply copy the proxy from someProxy[proxyOffset] for
 * frequently used proxies.
 */

#ifndef _UBER_H
#define _UBER_H

#include "configure.h"
#include "pup.h"

extern Config config;

class UberIndex {
  public:
    unsigned char x;
    unsigned char y;
    unsigned char z;
    unsigned char s;
    UberIndex(unsigned char _x, unsigned char _y=0, unsigned char _z=0, unsigned char _s=0) : x(_x), y(_y), z(_z), s(_s){}
    UberIndex(){x=y=z=s=0;}
    UberIndex(const UberIndex &obj){x=obj.x;y=obj.y;z=obj.z;s=obj.s;}
    ~UberIndex(){}
    inline UberIndex & operator=(const UberIndex &obj) {
      x=obj.x;
      y=obj.y;
      z=obj.z;
      s=obj.s;
      return *this;
    }
    void pup(PUP::er &p)
    {
      p|x;
      p|y;
      p|z;
      p|s;
    }

    inline bool operator==(const UberIndex &obj) const {
      return(x==obj.x && y==obj.y && z==obj.z && s==obj.s);}
    inline bool operator<(const UberIndex &obj) const {
      return(x<obj.x && y<=obj.y && z<=obj.z && s <=obj.s);}
};

//! holds the UberIndex and the offset for proxies
class UberCollection {
  public:
    UberIndex idxU;
    unsigned char proxyOffset;
    UberCollection(UberIndex _idxU) :  idxU(_idxU) {
      proxyOffset=calcPO();
    }
    UberCollection(unsigned char PO, unsigned char _x, unsigned char _y=0, unsigned char _z=0, unsigned char _s=0) : proxyOffset(PO), idxU(_x,_y,_z,_s) {
    }
    UberCollection(unsigned char _proxyOffset) :proxyOffset(_proxyOffset)
  { // reverse map from the offset onto the index
    idxU.x = proxyOffset % config.UberImax;
    idxU.y = (proxyOffset % (config.UberImax * config.UberJmax)) / config.UberImax;
    idxU.z = (proxyOffset % (config.UberImax * config.UberJmax * config.UberKmax))/(config.UberImax*config.UberJmax);
    idxU.s = proxyOffset / (config.UberImax * config.UberJmax * config.UberKmax);
    //CkAssert(calcPO() == _proxyOffset); ///< @note: Commented out just to avoid including extra headers for CkAssert
  }

    UberCollection() {}
    void pup(PUP::er &p)
    {
      p|idxU;
      p|proxyOffset;
    }
    inline UberCollection & operator=(const UberCollection &obj) {
      idxU=obj.idxU;
      proxyOffset=obj.proxyOffset;
      return *this;
    }
    inline bool operator==(const UberCollection &obj) const {
      return(idxU==obj.idxU);}
    inline bool operator<(const UberCollection &obj) const {
      return(idxU<obj.idxU); }

    inline unsigned char calcPO()
    { return (idxU.x + (idxU.y + (idxU.z + idxU.s*config.UberKmax) * config.UberJmax) * config.UberImax);}
    inline unsigned char getPO() const { return proxyOffset; }
    inline unsigned char setPO(){ proxyOffset=calcPO(); return proxyOffset;}
    inline unsigned char setPO(int inPO){ proxyOffset=inPO; return proxyOffset;}

};
/* @} */
#endif
