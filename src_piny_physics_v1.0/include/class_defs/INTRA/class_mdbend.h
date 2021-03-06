//==========================================================================
//cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
//==========================================================================
//                                                                          
//                          class_mdbend.h                                  
//                                                                          
//                Class definition for classical bends                      
//                                                                          
//==========================================================================
//cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
//==========================================================================

#ifndef _MDBEND_
#define _MDBEND_


class MDBEND {
  public:
    int npow;                    /* Num: # pow series bends             */
    int ntyp_pow;                /* Num: # pow series bend typs         */ 
    int *j1_pow,*j2_pow,*j3_pow; /* Lst: Indices of atms in pow bends;   
Lth: npow                           */
    int *jtyp_pow;               /* Map: index of bend -> bend type;
Lth: npow                           */
    double *eq_pow;              /* Lst: List of eq. bend angs;    
Lth: ntyp_pow                       */
    double *c_0,*c_1,*c_2,*c_3,*c_4,*c_5,*c_6;
    double *s_0,*s_1,*s_2,*s_3,*s_4,*s_5,*s_6;            
    double *dc_0,*dc_1,*dc_2,*dc_3,*dc_4,*dc_5,*dc_6;            
    double *ds_0,*ds_1,*ds_2,*ds_3,*ds_4,*ds_5,*ds_6;
    /* Lst: Bend power series coefficients
Lth: ntyp_pow                       */

    //=============================================================================
    // Default Constructor/Destructor

    MDBEND(){
      npow = 0;                
      ntyp_pow = 0;            
    }
    ~MDBEND(){}

    //=============================================================================
    // Pack/Unpack

#ifdef PUP_ON
    void pup(PUP::er &p){

      // Pupping ints

      p | npow;          
      p | ntyp_pow;      

      // Integer Arrays 

      if(npow>0){
        pup1d_int(p,&j1_pow,npow);
        pup1d_int(p,&j2_pow,npow);
        pup1d_int(p,&j3_pow,npow);
        pup1d_int(p,&jtyp_pow,npow);
      }

      // Double Arrays 

      if(npow>0){
        pup1d_dbl(p,&eq_pow,ntyp_pow);    

        pup1d_dbl(p,&c_0,ntyp_pow);
        pup1d_dbl(p,&c_1,ntyp_pow);
        pup1d_dbl(p,&c_2,ntyp_pow);
        pup1d_dbl(p,&c_3,ntyp_pow);
        pup1d_dbl(p,&c_4,ntyp_pow);
        pup1d_dbl(p,&c_5,ntyp_pow);
        pup1d_dbl(p,&c_6,ntyp_pow);          

        pup1d_dbl(p,&s_0,ntyp_pow);
        pup1d_dbl(p,&s_1,ntyp_pow);
        pup1d_dbl(p,&s_2,ntyp_pow);
        pup1d_dbl(p,&s_3,ntyp_pow);
        pup1d_dbl(p,&s_4,ntyp_pow);
        pup1d_dbl(p,&s_5,ntyp_pow);
        pup1d_dbl(p,&s_6,ntyp_pow);          

        pup1d_dbl(p,&dc_0,ntyp_pow);
        pup1d_dbl(p,&dc_1,ntyp_pow);
        pup1d_dbl(p,&dc_2,ntyp_pow);
        pup1d_dbl(p,&dc_3,ntyp_pow);
        pup1d_dbl(p,&dc_4,ntyp_pow);
        pup1d_dbl(p,&dc_5,ntyp_pow);
        pup1d_dbl(p,&dc_6,ntyp_pow);      

        pup1d_dbl(p,&ds_0,ntyp_pow);
        pup1d_dbl(p,&ds_1,ntyp_pow);
        pup1d_dbl(p,&ds_2,ntyp_pow);
        pup1d_dbl(p,&ds_3,ntyp_pow);
        pup1d_dbl(p,&ds_4,ntyp_pow);
        pup1d_dbl(p,&ds_5,ntyp_pow);
        pup1d_dbl(p,&ds_6,ntyp_pow);      
      }/*endif*/
#ifdef _PARALLEL_DEBUG_        
      if (p.isUnpacking())
        state_class_out ();
#endif 
    } /* Pack/Unpack */
#endif
    //============================================================================
    // Print out state of class

    void state_class_out(){

      int i;
      char fileName [255];
      sprintf (fileName, "%d_mdbend.state", CkMyPe());
      FILE *fp;  fp = fopen(fileName,"w");

      fprintf(fp,"mdbend:  npow %d\n",npow);
      fprintf(fp,"mdbend:  ntyp_pow %d\n",ntyp_pow);

      // Print Integer Arrays 

      for(i=1;i<=npow;i++){fprintf(fp,"mdbend: j1_pow[%d] %d\n",i,j1_pow[i]);}
      for(i=1;i<=npow;i++){fprintf(fp,"mdbend: j2_pow[%d] %d\n",i,j2_pow[i]);}
      for(i=1;i<=npow;i++){fprintf(fp,"mdbend: j3_pow[%d] %d\n",i,j3_pow[i]);}

      for(i=1;i<=npow;i++){fprintf(fp,"mdbend: jtyp_pow[%d] %d\n",
          i,jtyp_pow[i]);}
      // Print Double Arrays 

      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: eq_pow[%d] %.12g\n",
          i,eq_pow[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: c_0[%d] %.12g\n",i,c_0[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: c_1[%d] %.12g\n",i,c_1[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: c_2[%d] %.12g\n",i,c_2[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: c_3[%d] %.12g\n",i,c_3[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: c_4[%d] %.12g\n",i,c_4[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: c_5[%d] %.12g\n",i,c_5[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: c_6[%d] %.12g\n",i,c_6[i]);}

      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: s_0[%d] %.12g\n",i,s_0[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: s_1[%d] %.12g\n",i,s_1[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: s_2[%d] %.12g\n",i,s_2[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: s_3[%d] %.12g\n",i,s_3[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: s_4[%d] %.12g\n",i,s_4[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: s_5[%d] %.12g\n",i,s_5[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: s_6[%d] %.12g\n",i,s_6[i]);}

      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: dc_0[%d] %.12g\n",i,dc_0[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: dc_1[%d] %.12g\n",i,dc_1[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: dc_2[%d] %.12g\n",i,dc_2[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: dc_3[%d] %.12g\n",i,dc_3[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: dc_4[%d] %.12g\n",i,dc_4[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: dc_5[%d] %.12g\n",i,dc_5[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: dc_6[%d] %.12g\n",i,dc_6[i]);}

      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: ds_0[%d] %.12g\n",i,ds_0[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: ds_1[%d] %.12g\n",i,ds_1[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: ds_2[%d] %.12g\n",i,ds_2[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: ds_3[%d] %.12g\n",i,ds_3[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: ds_4[%d] %.12g\n",i,ds_4[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: ds_5[%d] %.12g\n",i,ds_5[i]);}
      for(i=1;i<=ntyp_pow;i++){fprintf(fp,"mdbend: ds_6[%d] %.12g\n",i,ds_6[i]);}

      fclose(fp);
    }/* end member function */


}; /* end class definition */
//==========================================================================

#ifdef PUP_ON
PUPmarshall(MDBEND);
#endif


#endif
//==========================================================================
