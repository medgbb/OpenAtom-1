#include "main/cpaimd.h"

#ifndef ORTHO_MAP_H
#define ORTHO_MAP_H

#define USE_INT_MAP

/// Centroid based ortho map (actual map creation in MapTable.C)
class OrthoMap : public CkArrayMapTable2
{
    public:
        OrthoMap(MapType2 map, UberCollection _instance)
        {
            thisInstance=_instance;
            #ifdef USE_INT_MAP
                maptable = new MapType2(map);
            #else
                maptable= &Orthomaptable;
            #endif
        }

        ~OrthoMap() { }

        void pup(PUP::er &p)
        {
            CkArrayMapTable2::pup(p);
            #ifdef USE_INT_MAP
                p|*maptable;
            #else
                maptable= &Orthomaptable;
            #endif
        }

        inline int procNum(int, const CkArrayIndex &iIndex)
        {
            int *index=(int *) iIndex.data();
            int proc;
            #ifdef USE_INT_MAP
                proc=maptable->get(index[0],index[1]);
            #else
                proc=maptable->get(intdual(index[0],index[1]));
            #endif
            CkAssert(proc>=0);
            if(numPes != CkNumPes())
                return(proc%CkNumPes());
            else
                return(proc);
        }
};

#endif // ORTHO_MAP_H

