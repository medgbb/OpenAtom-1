//==========================================================================
//cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
//==========================================================================
//
//  This program converts doublePack states (Gamma Pt) and singlePack states.
//  That means the code restores the bottom half of gpsace to allow
//  use of Gamma pt KS states to initialize Kpt computations
//
//  To run this program : executable inputfile
//
//==========================================================================
//cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
//==========================================================================
// Standard include files

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <ctime>

//==========================================================================
// In-line functions, typedefs and structures

typedef struct complex{
  double  re;
  double  im;   
};
#define PRINTF printf
#define EXIT(N) {exit(N);}

//==========================================================================
// Function prototypes

int main (int , char *[]);
void flip_data_set(int , int *, int *, int *,complex *);
void readtoendofline(FILE *);

//==========================================================================


//==========================================================================
//cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
//==========================================================================
int main (int argc, char *argv[]){
  //==========================================================================

  int nktot,nktot2,n1,n2,n3;
  int *kx,*ky,*kz;
  complex *data;
  char fname[1024];
  char directory[1024];
  FILE *fp;

  //=========================================================================
  //             Check for input file                                 

  if(argc < 2) {
    PRINTF("@@@@@@@@@@@@@@@@@@@@_error_@@@@@@@@@@@@@@@@@@@@\n");
    PRINTF("No input file specified\n");
    PRINTF("@@@@@@@@@@@@@@@@@@@@_error_@@@@@@@@@@@@@@@@@@@@\n");
    EXIT(1);
  }/*endif*/

  //==========================================================================
  // Read the input file

  int nstate,ibinary;
  PRINTF("\nReading input parameters from %s\n",argv[1]);
  fp = fopen(argv[1],"r");
  fscanf(fp,"%d %d",&nstate,&ibinary); readtoendofline(fp);
  fscanf(fp,"%s ",directory); readtoendofline(fp);
  fclose(fp);
  PRINTF("Finished reading input parameters from %s\n\n",argv[1]);

  PRINTF("I am converting %d doublePack states to singlePack form\n",nstate);
  PRINTF("I am assuming the doublePack states are stored in the directory %s\n",directory);
  PRINTF("I will write the singlePack states in the directory %s_flipped\n",directory);
  PRINTF("I am using the binary option %d : (0/1) = (ascii/binary)\n\n",ibinary);

  if(nstate<0 || ibinary<0 || ibinary>1){
    PRINTF("@@@@@@@@@@@@@@@@@@@@_error_@@@@@@@@@@@@@@@@@@@@\n");
    PRINTF("Bad input data \n");
    PRINTF("@@@@@@@@@@@@@@@@@@@@_error_@@@@@@@@@@@@@@@@@@@@\n");
    EXIT(1);
  }//endif

  //==========================================================================
  // Read in the header information and malloc some memory

  if(ibinary==0){
    sprintf(fname,"%s/state1.out",directory);
    fp = fopen(fname,"r");
    fscanf(fp,"%d %d %d %d",&nktot,&n1,&n2,&n3);
  }else{
    fp = fopen(fname,"rb");
    int n=1;
    fread(&(nktot),sizeof(int),n,fp);
  }//endif
  PRINTF("nktot = %d\n",nktot);
  fclose(fp);

  nktot2 = 2*nktot-1;
  data = (complex *)malloc(nktot2*sizeof(complex));    
  kx   = (int *)malloc(nktot2*sizeof(int));
  ky   = (int *)malloc(nktot2*sizeof(int));
  kz   = (int *)malloc(nktot2*sizeof(int));

  //==========================================================================
  // Read in the each state, flip it, and write the flipped guy back out

  for(int is=0;is<nstate;is++){
    //---------------------------------------------------------------------------
    // Read in the state
    sprintf(fname,"%s/state%d.out",directory,is + 1);
    if(ibinary==0){
      fp = fopen(fname,"r");
      PRINTF("Reading file: %s\n",fname);
      fscanf(fp,"%d %d %d %d",&nktot,&n1,&n2,&n3);
      for(int i =0;i<nktot;i++){
        fscanf(fp,"%lf %lf %d %d %d",&data[i].re,&data[i].im,&kx[i],&ky[i],&kz[i]);    
      }//endfor
    }else{
      fp = fopen(fname,"rb");
      int n=1;
      fread(&(nktot),sizeof(int),n,fp);
      fread(&(n1),sizeof(int),n,fp);
      fread(&(n2),sizeof(int),n,fp);
      fread(&(n3),sizeof(int),n,fp);
      for(int i=0;i<nktot;i++) {
        int x,y,z;
        double re,im;
        fread(&(re),sizeof(double),n,fp);
        fread(&(im),sizeof(double),n,fp);
        fread(&(x),sizeof(int),n,fp);
        fread(&(y),sizeof(int),n,fp);
        fread(&(z),sizeof(int),n,fp);
        data[i].re = re; data[i].im=im;
        kx[i]    = x;
        ky[i]    = y;
        kz[i]    = z; 
      }//endfor 
    }//endif
    fclose(fp);
    //-----------------------------------------------------------------------------------
    // Flip the state
    flip_data_set(nktot,kx,ky,kz,data);

    sprintf(fname,"%s_flipped/state%d.out",directory,is + 1);
    //---------------------------------------------------------------------------
    // Write the flipped state out
    if(ibinary==0){
      fp = fopen(fname,"w");
      PRINTF("Writing file: %s\n",fname);
      fprintf(fp,"%d %d %d %d\n",nktot2,n1,n2,n3);
      for(int i =0;i<nktot2;i++){
        fprintf(fp,"%lf %lf %d %d %d\n",data[i].re,data[i].im,kx[i],ky[i],kz[i]);    
      }//endfor
    }else{
      fp = fopen(fname,"wb");
      int n=1;
      fwrite(&(nktot2),sizeof(int),n,fp);
      fwrite(&(n1),sizeof(int),n,fp);
      fwrite(&(n2),sizeof(int),n,fp);
      fwrite(&(n3),sizeof(int),n,fp);
      double sum=0.0;
      for(int i=0;i<nktot2;i++) {
        int x,y,z;
        double re,im;
        re = data[i].re; im = data[i].im; 
        sum += re*re + im*im;
        x = kx[i];
        y = ky[i];
        z = kz[i];
        fwrite(&(re),sizeof(double),n,fp);
        fwrite(&(im),sizeof(double),n,fp);
        fwrite(&(x),sizeof(int),n,fp);
        fwrite(&(y),sizeof(int),n,fp);
        fwrite(&(z),sizeof(int),n,fp);
      }//endfor 
      //PRINTF("%.10g \n",sum);
    }//endif
    fclose(fp);
  }//endfor; is=states

  //==========================================================================

  return 1;

  //--------------------------------------------------------------------------
}//end main
//==========================================================================



//==========================================================================
//cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
//==========================================================================
// Take the output and reorder it to full complex beast
//  kx,ky,kz had better be malloced nktot*2 - 1 but contain nktot data
//  at input
//==========================================================================

void flip_data_set(int nktot, int *kx, int *ky, int *kz,complex *data)

  //==========================================================================
{//begin routine 
  //==========================================================================
  // Count half plane kx=0 of piny data : check piny data

  int nplane0 = 0;
  for(int i=0;i<nktot;i++){
    if(kx[i]==0){nplane0++;}
  }//endfor
  int *kxt = (int *)malloc(nplane0*sizeof(int));
  int *kyt = (int *)malloc(nplane0*sizeof(int));
  int *kzt = (int *)malloc(nplane0*sizeof(int));

  complex *datat;
  datat = (complex *)malloc(nplane0*sizeof(complex));

  for(int i=0;i<nplane0-1;i++){
    kxt[i]= kx[i];
    kyt[i]= ky[i];
    kzt[i]= kz[i];
    datat[i]= data[i];
    if(kx[i]!=0){
      PRINTF("@@@@@@@@@@@@@@@@@@@@_Error_@@@@@@@@@@@@@@@@@@@@\n");
      PRINTF("Error.1 while flipping piny dblpack data set\n");
      PRINTF("@@@@@@@@@@@@@@@@@@@@_Error_@@@@@@@@@@@@@@@@@@@@\n");
      EXIT(1);
    }//endif
  }//endfor
  kxt[(nplane0-1)]   = kx[(nktot-1)];
  kyt[(nplane0-1)]   = ky[(nktot-1)];
  kzt[(nplane0-1)]   = kz[(nktot-1)];
  datat[(nplane0-1)] = data[(nktot-1)];

  if(kx[(nktot-1)]!=0 || ky[(nktot-1)] || kz[(nktot-1)]){
    PRINTF("@@@@@@@@@@@@@@@@@@@@_Error_@@@@@@@@@@@@@@@@@@@@\n");
    PRINTF("Error.2 while flipping piny dblpack data set\n");
    PRINTF("@@@@@@@@@@@@@@@@@@@@_Error_@@@@@@@@@@@@@@@@@@@@\n");
    EXIT(1);
  }//endif

  //==========================================================================
  // Expand the data set

  for(int i=nktot-2;i>=0;i--){
    kx[(i+nplane0)]   = kx[i];
    ky[(i+nplane0)]   = ky[i];
    kz[(i+nplane0)]   = kz[i];
    data[(i+nplane0)] = data[i];
  }//endfor

  //==========================================================================
  // Create the bottom half of plane zero by symmetry : 

  int i1 = 0;
  for(int i=0;i<nplane0-1;i++){
    int ind = nplane0-i-2;
    kx[i]      =  kxt[ind];
    ky[i]      = -kyt[ind];
    kz[i]      = -kzt[ind];
    data[i].re =  datat[ind].re;
    data[i].im = -datat[ind].im;
    if(kx[i]!=0 || ky[i]<ky[i1]){
      PRINTF("@@@@@@@@@@@@@@@@@@@@_Error_@@@@@@@@@@@@@@@@@@@@\n");
      PRINTF("Error.3 while flipping piny dblpack data set %d %d %d %d %d\n",kx[i],ky[i],kz[i],i,ind);
      PRINTF("@@@@@@@@@@@@@@@@@@@@_Error_@@@@@@@@@@@@@@@@@@@@\n");
      EXIT(1);
    }//endif
    i1 = i;
  }//endfor
  kx[(nplane0-1)]   = kxt[(nplane0-1)];
  ky[(nplane0-1)]   = kyt[(nplane0-1)];
  kz[(nplane0-1)]   = kzt[(nplane0-1)];
  data[(nplane0-1)] = datat[(nplane0-1)];

  if(nktot>=nplane0+1){
    if(kx[nplane0]!= 0 || ky[nplane0]!=0 || kz[nplane0]!=1){
      PRINTF("@@@@@@@@@@@@@@@@@@@@_Error_@@@@@@@@@@@@@@@@@@@@\n");
      PRINTF("Error.4 while flipping piny dblpack data set\n");
      PRINTF("@@@@@@@@@@@@@@@@@@@@_Error_@@@@@@@@@@@@@@@@@@@@\n");
      EXIT(1);
    }//endif
  }//endfor

#ifdef _GLENN_DBG_FLIP
  int nnn = MIN(nplane0+3,nktot);
  for(int i=0;i<nnn;i++){
    PRINTF(" %d : %d %d %d \n",i,kx[i],ky[i],kz[i]);
  }
#endif

  //==========================================================================
  // Exit

  free(kxt);
  free(kyt);
  free(kzt);
  free(datat);

  //==========================================================================
  // Now we have full planes in the upper half space. We can flip them over

  int nnow = nktot + nplane0 - 1;
  int nktot2 = 2*nktot - 1;

  datat = (complex *)malloc(nnow*sizeof(complex));
  kxt = (int *)malloc(nnow*sizeof(int));
  kyt = (int *)malloc(nnow*sizeof(int));
  kzt = (int *)malloc(nnow*sizeof(int));
  for(int i=0;i<nnow;i++){
    kxt[i]   = kx[i];
    kyt[i]   = ky[i];
    kzt[i]   = kz[i];
    datat[i] = data[i];
  }//endfor

  // The top half + plane0 is now in place!!!
  int ioff = nktot2 - nnow;
  for(int i=0,j=ioff;i<nnow;i++,j++){
    kx[j]    = kxt[i];
    ky[j]    = kyt[i];
    kz[j]    = kzt[i];
    data[j]  = datat[i];
  }

  // Get the bottom half using hermitian conjg sym!
  for(int i=0,j=nktot2-1;i<nnow-2*nplane0+1;i++,j--){
    kx[i]       =  -kx[j];
    ky[i]       =  -ky[j];
    kz[i]       =  -kz[j];
    data[i].re =  data[j].re;
    data[i].im = -data[j].im;
  }

  //==========================================================================
}//end routine
//==========================================================================


//==========================================================================
//cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
//==========================================================================
// readtoendofline: Function to read to end of line in read_coord files     
//==========================================================================
void readtoendofline(FILE *fp){
  int eol,ch;
  eol = (int )'\n';
  ch = eol+1;
  while(ch!=eol){ch=fgetc(fp);}
  if(ch==EOF){
    PRINTF("@@@@@@@@@@@@@@@@@@@@_error_@@@@@@@@@@@@@@@@@@@@\n");
    PRINTF("ERROR: Unexpected end of file reached          \n");
    PRINTF("@@@@@@@@@@@@@@@@@@@@_error_@@@@@@@@@@@@@@@@@@@@\n");
    EXIT(1);
  }//endif
}// end routine 
//==========================================================================
