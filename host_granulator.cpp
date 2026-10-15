// Test Granulator (x64) : dry exact, niveau wet, pitch +12 (x2 passages par zéro), silence, tailles de bloc 1/8192, éditeur. SSE strict si FPUTRAP.
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vst2_min.h"
static intptr_t cb(AEffect*, int32_t op, int32_t, intptr_t, void*, float){ return op==1?2300:0; }
typedef AEffect* (__cdecl *MainFn)(audioMasterCallback);
static AEffect* e; static int bad=0;
static void chk(int ok,const char* m){ printf("  %s %s\n",ok?"ok   ":"ERREUR",m); bad+=!ok; }
static void setp(float s,float d,float p,float a,float m){ e->setParameter(e,0,s);e->setParameter(e,1,d);e->setParameter(e,2,p);e->setParameter(e,3,a);e->setParameter(e,4,m); }
static const int N=44100*2; static float X[N],Y[N],Z[N];
static void run(int blk){ e->dispatcher(e,12,0,0,0,0); e->dispatcher(e,12,0,1,0,0);
  for(int o=0;o<N;o+=blk){ int n=(N-o<blk)?N-o:blk; float* I[2]={X+o,X+o}; float* O[2]={Y+o,Z+o}; e->processReplacing(e,I,O,n);} }
static double rms(const float*v,int a,int b){ double s=0; for(int i=a;i<b;i++) s+=v[i]*v[i]; return sqrt(s/(b-a)); }
static int zc(const float*v,int a,int b){ int c=0; for(int i=a+1;i<b;i++) if((v[i-1]<0)!=(v[i]<0)) c++; return c; }
int main(int argc,char**argv){
  setvbuf(stdout,NULL,_IONBF,0);
  if(getenv("FPUTRAP")){ unsigned m=0; __asm__ volatile("stmxcsr %0":"=m"(m)); m&=~0x0680u; __asm__ volatile("ldmxcsr %0"::"m"(m)); puts("SSE strict (exceptions invalid/zerodiv/overflow demasquees)"); }
  HMODULE h=LoadLibraryA(argv[1]); if(!h){puts("FAIL LoadLibrary");return 1;}
  e=((MainFn)GetProcAddress(h,"VSTPluginMain"))(cb); if(!e){puts("FAIL main NULL");return 1;}
  e->dispatcher(e,0,0,0,0,0); e->dispatcher(e,10,0,0,0,44100.f);
  for(int i=0;i<N;i++) X[i]=0.5f*(float)sin(2*3.14159265358979*440.0*i/44100.0);
  // 1) wet=0 : sortie = entrée
  setp(0.5f,0.65f,0.5f,0.3f,0.0f); run(512); double md=0; for(int i=0;i<N;i++){ double d=fabs(Y[i]-X[i]); if(d>md) md=d; } printf("  dry: ecart max %.2e\n",md); chk(md<1e-4,"Dry/Wet=0 : sortie = entree");
  // 2) wet=1, pitch 0, alea 0, densite 20/s, taille 50 ms : niveau proche de l'entrée, pas de NaN
  setp(0.5f,0.65f,0.5f,0.0f,1.0f); run(512); double r0=rms(X,44100,N), r1=rms(Y,44100,N); double db=20*log10(r1/r0); int nan=0; for(int i=0;i<N;i++) if(!(fabs(Y[i])<10)) nan++;
  printf("  wet=100%%: niveau %.2f dB, valeurs invalides %d\n",db,nan); chk(db>-6&&db<3&&!nan,"wet 100% : niveau coherent (-6..+3 dB), pas de NaN");
  // 3) pitch +12 : ~2x plus de passages par zéro
  setp(0.5f,0.65f,1.0f,0.0f,1.0f); run(512); double ratio=(double)zc(Y,44100,N)/(double)zc(X,44100,N); printf("  pitch +12 : rapport de frequence %.2f\n",ratio); chk(ratio>1.7&&ratio<2.3,"pitch +12 st : frequence x2");
  setp(0.5f,0.65f,0.0f,0.0f,1.0f); run(512); ratio=(double)zc(Y,44100,N)/(double)zc(X,44100,N); printf("  pitch -12 : rapport de frequence %.2f\n",ratio); chk(ratio>0.4&&ratio<0.6,"pitch -12 st : frequence /2");
  // 4) réglages extrêmes, tailles de bloc 1 et 8192 : stable, borné
  int ext[3][5]={{100,100,100,100,100},{0,0,0,100,100},{100,0,50,100,100}};
  for(int k=0;k<3;k++){ setp(ext[k][0]/100.f,ext[k][1]/100.f,ext[k][2]/100.f,ext[k][3]/100.f,ext[k][4]/100.f); int blks[3]={1,333,8192};
    for(int b=0;b<3;b++){ run(blks[b]); double pk=0; int inv=0; for(int i=0;i<N;i++){ if(!(fabs(Y[i])<100)) inv++; else if(fabs(Y[i])>pk) pk=fabs(Y[i]); }
      char m[96]; sprintf(m,"extreme %d, bloc %d : borne (pic %.2f)",k,blks[b],pk); chk(!inv&&pk<8,m); } }
  // 5) silence
  for(int i=0;i<N;i++) X[i]=0; setp(0.5f,0.65f,0.5f,0.5f,1.0f); run(512); chk(rms(Y,0,N)<1e-9,"silence -> silence");
  // 6) affichage des paramètres + éditeur
  setp(0.5f,0.65f,0.75f,0.3f,0.5f); for(int i=0;i<5;i++){ char n[16]={0},d[16]={0},l[16]={0}; e->dispatcher(e,8,i,0,n,0); e->dispatcher(e,7,i,0,d,0); e->dispatcher(e,6,i,0,l,0); printf("  %s = %s %s\n",n,d,l); }
  ERect* r=0; int ed=(int)e->dispatcher(e,13,0,0,&r,0); HWND p=CreateWindowExA(0,"STATIC","p",WS_OVERLAPPEDWINDOW,0,0,300,300,0,0,GetModuleHandleA(0),0);
  int open=(int)e->dispatcher(e,14,0,0,p,0); e->dispatcher(e,15,0,0,0,0); chk(ed&&open,"editeur s'ouvre et se ferme");
  e->dispatcher(e,1,0,0,0,0); puts(bad?"FAIL":"PASS"); return bad; }
