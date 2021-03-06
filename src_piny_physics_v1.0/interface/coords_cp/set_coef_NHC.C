/*==========================================================================*/
/*cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc*/
/*==========================================================================*/
/*                                                                          */
/*                         PI_MD:                                           */
/*             The future of simulation technology                          */
/*             ------------------------------------                         */
/*                     Module: set_atm_coef.c                               */
/*                                                                          */
/* This subprogram sets up the coef NHC for a MD on a                       */
/* LD-classical potential energy surface (LD-PES)                           */
/*                                                                          */
/*==========================================================================*/
/*cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc*/
/*==========================================================================*/

#include "standard_include.h"
#include "../class_defs/allclass_gen.h"
#include "../class_defs/allclass_cp.h"
#include "../class_defs/allclass_mdintegrate.h"
#include "../class_defs/allclass_mdatoms.h"
#include "../class_defs/allclass_mdinter.h"
#include "../class_defs/allclass_mdintra.h"
#include "../class_defs/typedefs_par.h"
#include "../proto_defs/proto_coords_cp_entry.h"
#include "../proto_defs/proto_friend_lib_entry.h"

/*==========================================================================*/
/*cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc*/
/*==========================================================================*/

void set_coef_NHC(CPCOEFFS_INFO *cpcoeffs_info,CPOPTS *cpopts,
    CPTHERM_INFO *cptherm_info,CP_PARSE *cp_parse,
    double *tot_memory)

  /*==========================================================================*/
  /*               Begin subprogram:                                          */
{/*begin routine*/
  /*==========================================================================*/
  /*               Local variables                                            */

  int iii;
  int i,j;                   /* Num: For indicies            */
  double now_memory;         /* Num: Memory allocated now    */ 
  int num_c_nhc;
  double cons_totup;
  double cons_stup;
  double cons_totdn; 
  double cons_stdn; 
  double cp_tau_nhc;

  /* Local pointers */
  int cp_norb                  = cpopts->cp_norb;
  int cp_lda                   = cpopts->cp_lda;
  int cp_lsda                  = cpopts->cp_lsda;
  int ncoef                    = cpcoeffs_info->ncoef;
  int nstate_up                = cpcoeffs_info->nstate_up;
  int nstate_dn                = cpcoeffs_info->nstate_dn;
  int istate_nhc_opt           = cp_parse->istate_nhc_opt;
  int len_c_nhc                = cptherm_info->len_c_nhc;
  double te_ext                = cpopts->te_ext;

  double **c_gkt;
  double **cmass_nhc;
  int *icmapup_nhc;
  int *icmapdn_nhc;

  /*==========================================================================*/
  /* 0) Output to screen */

  PRINTF("\n");PRINT_LINE_STAR;
  PRINTF("Set up plane wave coeff NHC\'s\n");
  PRINT_LINE_DASH;PRINTF("\n");

  /*==========================================================================*/
  /* Set up some useful constants                                             */

  cptherm_info->massiv_flag    = 0;
  if(istate_nhc_opt==4){cptherm_info->massiv_flag = 1;}
  cptherm_info->istate_nhc_opt = istate_nhc_opt;

  /*==========================================================================*/
  /* I) Find the number of NHCs                           */

  if(cp_lda==1){
    switch(istate_nhc_opt){
      case 0: num_c_nhc=0;break;
      case 1: num_c_nhc=1;break;
      case 2: num_c_nhc=1;break;
      case 3: num_c_nhc=nstate_up;break;
      case 4: num_c_nhc=2*nstate_up*ncoef;break;
    }/*end switch*/
  }/*endif*/

  if(cp_lsda==1){
    switch(istate_nhc_opt){
      case 0: num_c_nhc=0;break;
      case 1: num_c_nhc=1;break;
      case 2: num_c_nhc=2;break;
      case 3: num_c_nhc=nstate_up+nstate_dn;break;
      case 4: num_c_nhc=2*(nstate_up+nstate_dn)*ncoef; break;
    }/*end switch*/
  }/*endif*/
  cptherm_info->num_c_nhc      = num_c_nhc;

  PRINTF("you have implemented %d nose-hoover chains ",num_c_nhc);
  PRINTF("each of length %d\n",len_c_nhc);

  /*==========================================================================*/
  /* II) Malloc up the memory                                                 */

  if(istate_nhc_opt!=4){
    now_memory      = (  (num_c_nhc)*(len_c_nhc)*5*(sizeof(double))
        + (nstate_up)*(sizeof(int))+(nstate_dn)*(sizeof(int))
        )*1.0e-6;
  }else{
    now_memory      = ( (num_c_nhc)*(len_c_nhc)*(sizeof(double)) )*1.0e-6;
  }/*endif*/
  *tot_memory += now_memory;

  if(istate_nhc_opt!=4){
    cptherm_info->c_gkt       = cmall_mat(1,len_c_nhc,1,(num_c_nhc+1),"set_coef_NHC");
    cptherm_info->cmass_nhc   = cmall_mat(1,len_c_nhc,1,(num_c_nhc+1),"set_coef_NHC");
    cptherm_info->icmapup_nhc = (int *) cmalloc(nstate_up*sizeof(int),"set_coef_NHC")-1;
    cptherm_info->icmapdn_nhc = (int *) cmalloc(nstate_dn*sizeof(int),"set_coef_NHC")-1;
    c_gkt                     = cptherm_info->c_gkt;
    cmass_nhc                 = cptherm_info->cmass_nhc;
    icmapup_nhc               = cptherm_info->icmapup_nhc;
    icmapdn_nhc               = cptherm_info->icmapdn_nhc;
  }/*endif*/
  cptherm_info->wdti        = (double *)cmalloc((size_t)25*sizeof(double),"set_coef_NHC")-1;
  cptherm_info->wdti2       = (double *)cmalloc((size_t)25*sizeof(double),"set_coef_NHC")-1;
  cptherm_info->wdti4       = (double *)cmalloc((size_t)25*sizeof(double),"set_coef_NHC")-1;
  cptherm_info->wdti8       = (double *)cmalloc((size_t)25*sizeof(double),"set_coef_NHC")-1;
  cptherm_info->wdti16      = (double *)cmalloc((size_t)25*sizeof(double),"set_coef_NHC")-1;

  PRINTF("CP NHC allocation: %g Mbytes; Total memory: %g Mbytes\n",
      now_memory,*tot_memory);

  /*==========================================================================*/
  /*  IV) Calculate degrees of freedom                                        */

  if(cp_norb==0 || cp_norb==1){
    cons_totup  = (double)(nstate_up*(nstate_up+1))/2.0;
    cons_stup   = (double)(nstate_up+1)/2.0;
    cons_totdn  = (double)(nstate_dn*(nstate_dn+1))/2.0;
    cons_stdn   = (double)(nstate_dn+1)/2.0;
  }/*endif*/

  if(cp_norb==2){
    cons_totup  = (double)(nstate_up);
    cons_stup   = 1.0;
    cons_totdn  = (double)(nstate_dn);
    cons_stdn   = 1.0;
  }/*endif*/

  if(cp_norb==3){
    cons_totup  = 0.0;
    cons_stup   = 0.0;
    cons_totdn  = 0.0;
    cons_stdn   = 0.0;
  }/*endif*/

  /*==========================================================================*/
  /* V) Set up the NHCs                                                      */

  switch(istate_nhc_opt){

    /*--------------------------------------------------------------------------*/
    /*  1)No NHC                                                                */

    case 0:
      for(j=1;j<=nstate_up;j++){
        icmapup_nhc[j] = 1;
      }/*endfor*/
      if(cp_lsda==1){
        for(j=1;j<=nstate_dn;j++){
          icmapdn_nhc[j] = 1;
        }/*endfor*/
      }/*endif*/
      break;

      /*--------------------------------------------------------------------------*/
      /*  2)1 NHC                                                                 */

    case 1:
      for(j=1;j<=nstate_up;j++){
        icmapup_nhc[j] = 1;
      }/*endfor*/
      c_gkt[1][1]=((double)(ncoef*nstate_up*2)-cons_totup)*te_ext/BOLTZ;

      if(cp_lsda==1){
        for(j=1;j<=nstate_dn;j++){
          icmapdn_nhc[j] = 1;
        }/*endfor*/
        c_gkt[1][1]+=((double)(ncoef*nstate_dn*2)-cons_totdn)*te_ext/BOLTZ;
      }/*endif*/
      break;

      /*--------------------------------------------------------------------------*/
      /*  2)2 NHC                                                                 */

    case 2:
      for(j=1;j<=nstate_up;j++){
        icmapup_nhc[j] = 1;
      }/*endfor*/
      c_gkt[1][1] = ((double)(ncoef*nstate_up*2)-cons_totup)*te_ext/BOLTZ;
      if(cp_lsda==1){
        for(j=1;j<=nstate_dn;j++){
          icmapdn_nhc[j] = 2;
        }/*endfor*/
        c_gkt[1][2] = ((double)(ncoef*nstate_dn*2)-cons_totdn)*te_ext/BOLTZ;
      }/*endif*/
      break;

      /*--------------------------------------------------------------------------*/
      /*  3)nstate NHCs                                                           */

    case 3:
      for(j=1;j<=nstate_up;j++){
        icmapup_nhc[j] = j;
        c_gkt[1][j] = ((double)(ncoef*2)-cons_stup)*te_ext/BOLTZ;
      }/*endfor*/
      if(cp_lsda==1){
        for(j=1;j<=nstate_dn;j++){
          icmapdn_nhc[j]   = j+nstate_up;
          c_gkt[1][(j+nstate_up)] = ((double)(ncoef*2)-cons_stdn)*te_ext/BOLTZ;
        }/*endfor*/
      }/*endif*/
      break;

      /*--------------------------------------------------------------------------*/
      /*  4)Massive NHCs                                                          */

    case 4:
      cptherm_info->c_gkt_massiv = te_ext/BOLTZ;
      break;

  }/*end switch*/

  /*==========================================================================*/
  /* V) Finish up the gkts and the masses                                   */

  cp_parse->cp_tau_nhc     /= TIME_CONV;
  cp_parse->cp_tau_nhc_def /= TIME_CONV;
  cp_tau_nhc                = cp_parse->cp_tau_nhc;

  if(istate_nhc_opt<4){

    for(j=2;j<=len_c_nhc;j++){
      for(i=1;i<=num_c_nhc;i++){
        c_gkt[j][i] = te_ext/BOLTZ;
      }/*endfor*/
    }/*endfor*/

    for(j=1;j<=len_c_nhc;j++){
      for(i=1;i<=num_c_nhc;i++){
        cmass_nhc[j][i] = c_gkt[j][i]*(cp_tau_nhc)*(cp_tau_nhc);
      }/*endfor*/
    }/*endfor*/

    if(len_c_nhc>2){
      j = (len_c_nhc-1);
      for(i=1;i<=num_c_nhc;i++){
        c_gkt[j][i] += (te_ext/BOLTZ);
      }/*endfor*/
    }/* endif */

  }else{

    cptherm_info->cmass_nhc_massiv = (cptherm_info->c_gkt_massiv)*
      (cp_tau_nhc)*(cp_tau_nhc);

  }/*endif*/

  /*==========================================================================*/
  /* VI) Output */

  PRINTF("\n");PRINT_LINE_DASH
    PRINTF("Completed set up of plane wave coeff NHC\'s\n");
  PRINT_LINE_STAR;PRINTF("\n");

  /*========================================================================*/
}/* end routine */
/*========================================================================*/











