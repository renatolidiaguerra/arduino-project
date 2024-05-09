//ArduinoMega Chess 2.0
//Sergey Urusov, ususovsv@gmail.com
#include  "wac.h"
#include "logic.h"
#include <stdint.h>
#include <avr/pgmspace.h>
#include  <EEPROM.h> 

#define MAXSTEPS 600 //600*7=4200 bytes, 700 - works too
#define  MAXDEPTH 20 //28*20=560 bytes

typedef struct {
  signed char f1,f2; //figure  moved and taken
  signed char c1,c2; //from and to coordinates
  signed char  typecheck; //check after move &B1000, move type &B111 1-En Passant,2-king castle,3-quween  castle,4-5-6-7-promotion to khight,bishop,rook,queen
  short weight; //position  weight
} step_t;

step_t steps[MAXSTEPS]; //moves massive include solving  tree
short ply; //number of halfstep in match, started with 0

struct position_t  {  
  byte w; //white to move 1, black - 0
  byte wrk, wrq, brk, brq; //castle  ability
  byte pp; //number of field 1..63 of last two squares pawn move
  short b_step; //number of levels first move
  short l_step; //quantity of moves  on level
  short cur_step; //current move on level
  short best_step; //best  move on level
  step_t cut; //move for sorting (cut alpha-beta)
  byte check_on_table;  //moving side under check    
  short weight_w; //weight of white figures
  short weight_b; //weight of black figures
  short weight_s; //statistic estimation  of position, in favor of whites +
} pos[MAXDEPTH];


struct startpos_t  {
  byte w; 
  byte wrk, wrq, brk, brq; 
  byte pp; 
  char pole[64];  
  int ply;  
} startpos;

byte poswk=0; //white king field
byte  posbk=0; //black king field

int level; //number of current level of solving  tree
unsigned long timelimith=5000; //limith of thinking time in ms
const  int limits[10]={5,10,30,60,120,180,300,600,1800,3600};
const char* limitstrings[10]={"5  sec","10 sec","30 sec","1 min","2 min","3 min","5 min","10 min","30  min","1 hr"};
byte limit=0;
unsigned long starttime;
boolean endspiel=0;
boolean  halt=0;
unsigned long count=0;
int depth;
boolean TRACE=0;
int maxstep;
boolean  solving=0;
boolean gameover=0;
boolean autow=0;
boolean autob=1;
boolean  debute;

String str_step(step_t);
#include "gui.h"

//****************************
void  new_game(String fen="") {
  if (fen=="") fen=F("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR  w KQkq -");  // 
  fenload(fen);
  for (int i=0;i<64;i++) startpos.pole[i]=pole[i];
  startpos.w=pos[0].w;                               
  startpos.wrk=pos[0].wrk;                               
  startpos.wrq=pos[0].wrq;                               
  startpos.brk=pos[0].brk;                               
  startpos.brq=pos[0].brq;                               
  startpos.w=pos[0].w;
  startpos.pp=pos[0].pp;
  startpos.ply=ply;  
  kingpositions();    
}
//****************************
void  setup() {   
  Serial.begin(115200);  
  while (!Serial) ;
  Serial.println(F("Start"));   
  new_game();  
  cleardisplayboard();
  if (!rotate) { autow=0; autob=1;  } else { autow=1; autob=0; }   
  guistart();      
  clearstatus();    
  //wactest();
  Serial.println(F("Command:"));  
}  
//****************************
void  start_game() {
  for (int i=0;i<64;i++) pole[i]=startpos.pole[i];
  pos[0].w=startpos.w;                               
  pos[0].wrk=startpos.wrk;                               
  pos[0].wrq=startpos.wrq;                               
  pos[0].brk=startpos.brk;                               
  pos[0].brq=startpos.brq;                               
  pos[0].w=startpos.w;
  pos[0].pp=startpos.pp;  
  ply=startpos.ply;    
  kingpositions();    
}
//**********************************   
boolean  fenload(String ss) {
 char s='x',i=0,j=0;  boolean load=false;   
  for (int  i=0;i<64;i++) pole[i]=0;                               
  pos[0].w=1; 
  pos[0].wrk=0;  pos[0].wrq=0;  pos[0].brk=0; pos[0].brq=0;
  pos[0].pp=0; 
  pos[0].cur_step=0;  pos[0].b_step=MAXDEPTH; pos[0].l_step=0;
  int spaces=0;      
  for (int  c=0; c<ss.length(); c++) {
    s=ss[c];       
    if (i>7) { i=0; j++; }        
    if (spaces==3&&int(s)>='a'&&int(s)<='h') {      
      c++;
      char s1=ss[c];
      if (int(s1)>='1'&&int(s1)<='8') {
        pos[0].pp=8*(7-(int(s1)-int('1')))+int(s)-int('a');        
      }
    } else {
    int l=j*8+i;  
    switch (s) {      
    case '/': i=0; break;  
    case 'p': pole[l]=-fp; i++; break;
    case  'P': pole[l]=fp; i++; break;
    case 'n': pole[l]=-fn; i++; break;
    case  'N': pole[l]=fn; i++; break;
    case 'b': if (spaces==0) { pole[l]=-fb; i++;  } else
              if (spaces==1) { pos[0].w=0; load=true; }              
              break;    
    case 'B': pole[l]=fb; i++; break;
    case 'r':  pole[l]=-fr; i++; break;
    case 'R': pole[l]=fr; i++; break;
    case 'q':  if (spaces==0) { pole[l]=-fq; i++; } else pos[0].brq=1; 
              break;
    case 'Q': if (spaces==0) { pole[l]=fq;  i++; } else pos[0].wrq=1;
              break;
    case 'k': if (spaces==0) { pole[l]=-fk;  i++; } else pos[0].brk=1; 
              break;
    case 'K': if (spaces==0) { pole[l]=fk;  i++; } else pos[0].wrk=1;
              break;
    case '1': i++; break;
    case '2': i+=2; break;
    case '3': i+=3; break;
    case '4': i+=4; break;
    case '5': i+=5; break;
    case '6': i+=6;  break;
    case '7': i+=7; break;
    case '8': i=0; j++; break;    
    case  ' ': spaces++; break;    
    case 'w': if (spaces==1) { pos[0].w=1; load=true;  } break;        
    }       
    }    
    if (spaces==4) break;    
  }  
  if (!load) Serial.println(F("Error"));
  if (pos[0].w) ply=0; else  ply=1;
  steps[0].f1=0; steps[1].f1=0;
  return load;  
}
//****************************
void  show_position() {    
  for (int i=0;i<8;i++) {
     Serial.println(F("-----------------------------------------"));       
      Serial.print("|");       
      for (int j=0;j<8;j++)    {        
        signed char f=pole[i*8+j];
        if (f>=0) { Serial.print("  "); Serial.print(fig_symb1[f]); Serial.print(" |"); }                     
         else { Serial.print(" -"); Serial.print(fig_symb1[-f]); Serial.print("  |"); }                     
      }   
      Serial.print("  ");              
      Serial.println(8-i);              
  }    
  Serial.println(F("-----------------------------------------"));       
  Serial.print(F("   a    b    c    d    e    f    g    h    "));       
  if (pos[0].w==0) Serial.print(F("Black")); else Serial.print(F("White"));       
  Serial.println();
}
//****************************
String  str_pole(int i) {
  return String(char('a'+i%8)+String(8-i/8));  
}
//****************************
String  str_step(step_t st) {
  String s="";
   if (st.f1==0) return s;
   if  ((st.typecheck&B111)==2) s="0-0";
    else if ((st.typecheck&B111)==3) s="0-0-0";
   else  {    
    if (abs(st.f1)>1) s=s+fig_symb[abs(st.f1)];     
    s=s+str_pole(st.c1);      
    if (st.f2==0) s=s+"-";      
    if (st.f2!=0) s=s+"x";    
    s=s+str_pole(st.c2);     
   } 
   if ((st.typecheck&B111)>3) s=s+"="+fig_symb[(st.typecheck&B111)-2];
   if (st.typecheck&B1000) s=s+"+";   
   return s;
}
//****************************
void  show_steps(int l) {
 Serial.print(F("Steps=")); Serial.println(String(pos[l].l_step-pos[l].b_step));
  for (int i=pos[l].b_step;i<pos[l].l_step;i++) { 
  Serial.print(str_step(steps[i]));  Serial.print(" "); Serial.print(steps[i].weight); Serial.print(F("    "));
  if ((i+1)%3==0) Serial.println(); 
 }
 Serial.println();  
} 
//****************************  
//****************************  
//****************************  
//****************************  
//****************************  
//****************************
//****************************  
void movestep(int l, step_t &s) {             
  pole[s.c1]=0;
  pole[s.c2]=s.f1;          
  if (pos[l].w) { //white to move
   if (s.f1==fk) poswk=s.c2;  
   switch (s.typecheck&B111) {   
   case 0:     
    return;
   case  1: //En Passant
    pole[s.c2+8]=0;  
    break;
   case 2: //short castle
    pole[60]=0;
    pole[61]=fr;
    pole[62]=fk;
    pole[63]=0;   
    poswk=62;    
    break;
   case 3: //long castle
    pole[60]=0;
    pole[59]=fr;
    pole[58]=fk;
    pole[57]=0;
    pole[56]=0;  
    poswk=58;     
    break;
   default: 
    pole[s.c2]=(s.typecheck&B111)-2;   
   } 
  } else { //black to move   
   if (s.f1==-fk) posbk=s.c2; 
   switch ((s.typecheck&B111)) {   
   case 0:     
    return; 
   case  1: //En Passant
    pole[s.c2-8]=0;
    break;
   case 2: //short castle
    pole[4]=0;
    pole[5]=-fr;
    pole[6]=-fk;
    pole[7]=0;
    posbk=6;       
    break;
   case 3: //long castle
    pole[4]=0;
    pole[3]=-fr;
    pole[2]=-fk;
    pole[1]=0;
    pole[0]=0;   
    posbk=2;    
    break;
   default: 
    pole[s.c2]=2-(s.typecheck&B111);
   }
  }      
}
//****************************  
void backstep(int l, step_t &s)  {     
  pole[s.c1]=s.f1;
  pole[s.c2]=s.f2;  
  if (pos[l].w) { //white  to move
   if (s.f1==fk) poswk=s.c1; 
   switch (s.typecheck&B111) {
   case  0: return;
   case 1: //En Passant
    pole[s.c2]=0;
    pole[s.c2+8]=-fp;
    break;
   case 2: //short castle
    pole[60]=fk;
    pole[61]=0;
    pole[62]=0;
    pole[63]=fr;
    poswk=60;    
    break;
   case  3: //long castle
    pole[60]=fk;
    pole[59]=0;
    pole[58]=0;
    pole[57]=0;
    pole[56]=fr;
    poswk=60;    
    break;
   }
  } else { //black  to move 
  if (s.f1==-fk) posbk=s.c1;
   switch (s.typecheck&B111) {
   case  0: return;
   case 1: //En Passant
    pole[s.c2]=0;
    pole[s.c2-8]=fp;
    break;
   case 2: //short castle
    pole[4]=-fk;
    pole[5]=0;
    pole[6]=0;
    pole[7]=-fr;
    posbk=4;    
    break;
   case  3: //long castle
    pole[4]=-fk;
    pole[3]=0;
    pole[2]=0;
    pole[1]=0;
    pole[0]=-fr;
    posbk=4;    
    break;
   }
  }             
}
//****************************  
void movepos(int l, step_t &s) {     
  pos[l+1].wrk=pos[l].wrk; pos[l+1].wrq=pos[l].wrq;  
  pos[l+1].brk=pos[l].brk; pos[l+1].brq=pos[l].brq;
  pos[l+1].pp=0;    
  if (pos[l].w) { // 
   if (pos[l].wrk||pos[l].wrq) {
    if (s.c1==60) {
      pos[l+1].wrk=0; pos[l+1].wrq=0;
    } else if (s.c1==63) pos[l+1].wrk=0;
    else if (s.c1==56) pos[l+1].wrq=0;
   }
   if ((s.typecheck&B111)==0&&s.f1==fp&&s.c2==s.c1-16)     
    if (column[s.c2]>1&&pole[s.c2-1]==-fp||column[s.c2]<8&&pole[s.c2+1]==-fp)  pos[l+1].pp=s.c1-8;      
   pos[l+1].weight_w=pos[l].weight_w; pos[l+1].weight_b=pos[l].weight_b;
   if (s.f2!=0) pos[l+1].weight_b-=fig_weight[-s.f2];
   if ((s.typecheck&B111)>3)  pos[l+1].weight_w+=fig_weight[(s.typecheck&B111)-2]-100;          
   if (s.f1==fk&&endspiel)  
    pos[l+1].weight_s=pos[l].weight_s+(int)pgm_read_word(&stat_weightw[6][s.c2])-(int)pgm_read_word(&stat_weightw[6][s.c1]);
   else  
    pos[l+1].weight_s=pos[l].weight_s+(int)pgm_read_word(&stat_weightw[s.f1-1][s.c2])-(int)pgm_read_word(&stat_weightw[s.f1-1][s.c1]);
   if (s.f2!=0) pos[l+1].weight_s+=(int)pgm_read_word(&stat_weightb[-s.f2-1][s.c2]);   
  } else { // 
   if (pos[l].brk||pos[l].brq) {
    if (s.c1==4) {
      pos[l+1].brk=0; pos[l+1].brq=0;
    } else if (s.c1==7) pos[l+1].brk=0;
    else if (s.c1==0) pos[l+1].brq=0;
   }   
   if ((s.typecheck&B111)==0&&s.f1==-fp&&s.c2==s.c1+16)  
    if (column[s.c2]>1&&pole[s.c2-1]==fp||column[s.c2]<8&&pole[s.c2+1]==fp)  pos[l+1].pp=s.c1+8;    
   pos[l+1].weight_w=pos[l].weight_w; pos[l+1].weight_b=pos[l].weight_b;
   if (s.f2!=0) pos[l+1].weight_w-=fig_weight[s.f2];
   if ((s.typecheck&&B111)>3)  pos[l+1].weight_b+=fig_weight[(s.typecheck&B111)-2]-100;         
   if (s.f1==-fk&&endspiel)  
    pos[l+1].weight_s=pos[l].weight_s-(int)pgm_read_word(&stat_weightb[6][s.c2])+(int)pgm_read_word(&stat_weightb[6][s.c1]);
   else
    pos[l+1].weight_s=pos[l].weight_s-(int)pgm_read_word(&stat_weightb[-s.f1-1][s.c2])+(int)pgm_read_word(&stat_weightb[-s.f1-1][s.c1]);
   if (s.f2!=0) pos[l+1].weight_s-=(int)pgm_read_word(&stat_weightw[s.f2-1][s.c2]);   
  }    
  count++; //estimated positions count
}
//****************************  
boolean check_w() {
  signed char f2=0;
  int j=0;
  if (pole[poswk]!=fk)  {        
    for (int i=0;i<64;i++) if (pole[i]==fk) {  poswk=i; break; }      
  }  
  while (pgm_read_byte(&diag_step[poswk][j])!=99) {                
    if (pgm_read_byte(&diag_step[poswk][j])==88) f2=0; 
    else if (f2==0) {     
     f2=pole[pgm_read_byte(&diag_step[poswk][j])];
     if (f2==-fb||f2==-fq)  return(true);
    }
    j++;
  }
  f2=0;  j=0;
  while (pgm_read_byte(&stra_step[poswk][j])!=99)  {                     
    if (pgm_read_byte(&stra_step[poswk][j])==88) f2=0;  
    else if (f2==0) {         
     f2=pole[pgm_read_byte(&stra_step[poswk][j])];
     if (f2==-fr||f2==-fq) return(true);   
     if (j==0||pgm_read_byte(&stra_step[poswk][j])==88)
      if (f2==-fk) return(true);    
    }
    j++;
  }
  j=0;
  while  (pgm_read_byte(&knight_step[poswk][j])!=99) {                
    if (pole[pgm_read_byte(&knight_step[poswk][j])]==-fn)  return(true);                 
    j++;
  }
  if (row[poswk]<7) {
    if  (column[poswk]>1&&pole[poswk-9]==-fp) return(true);
    if (column[poswk]<8&&pole[poswk-7]==-fp)  return(true);
  }
  j=0;
  while (pgm_read_byte(&king_step[poswk][j])!=99)  {                
    if (pole[pgm_read_byte(&king_step[poswk][j])]==-fk) return(true);                 
    j++;
  }  
  return(false);  
}
//****************************
boolean  check_b() {  
  signed char f2=0;
  int j=0;
  if (pole[posbk]!=-fk) {        
    for (int i=0;i<64;i++) if (pole[i]==-fk) {  posbk=i; break; }      
  }    
  while (pgm_read_byte(&diag_step[posbk][j])!=99) {                
    if (pgm_read_byte(&diag_step[posbk][j])==88) f2=0; 
    else if (f2==0) {     
     f2=pole[pgm_read_byte(&diag_step[posbk][j])];
     if (f2==fb||f2==fq)  return(true);    
    }
    j++;
  }
  f2=0;  j=0;
  while (pgm_read_byte(&stra_step[posbk][j])!=99)  {                     
    if (pgm_read_byte(&stra_step[posbk][j])==88) f2=0;  
    else if (f2==0) {         
     f2=pole[pgm_read_byte(&stra_step[posbk][j])];
     if (f2==fr||f2==fq) return(true);    
     if (j==0||pgm_read_byte(&stra_step[posbk][j])==88)
      if (f2==fk) return(true);    
    }
    j++;
  }
  j=0;
  while  (pgm_read_byte(&knight_step[posbk][j])!=99) {                
    if (pole[pgm_read_byte(&knight_step[posbk][j])]==fn)  return(true);                 
    j++;
  }    
  if (row[posbk]>2) {
    if (column[posbk]>1&&pole[posbk+7]==fp) return(true);
    if (column[posbk]<8&&pole[posbk+9]==fp)  return(true);
  }
  j=0;
  while (pgm_read_byte(&king_step[posbk][j])!=99)  {                
    if (pole[pgm_read_byte(&king_step[posbk][j])]==fk) return(true);                 
    j++;
  }    
  return(false);  
}
//****************************

//****************************
void  add_king(int l, int i) {
  signed char f1=pole[i];
  signed char f2;
  int  j=0;
  while (pgm_read_byte(&king_step[i][j])!=99) {                
    f2=pole[pgm_read_byte(&king_step[i][j])];
    if (f2==0||f2<0&&f1>0||f2>0&&f1<0) {
     steps[pos[l].l_step].typecheck=0;      
     steps[pos[l].l_step].c1=i;      
     steps[pos[l].l_step].c2=pgm_read_byte(&king_step[i][j]);      
     steps[pos[l].l_step].f1=f1;      
     steps[pos[l].l_step].f2=f2;           
     pos[l].l_step++;            
    }             
    j++;
  }    
}
//****************************
void add_knight(int l, int i)  {
  signed char f1=pole[i];
  signed char f2;
  int j=0;  
  while (pgm_read_byte(&knight_step[i][j])!=99)  {                    
    f2=pole[pgm_read_byte(&knight_step[i][j])];
    if  (f2==0||f2<0&&f1>0||f2>0&&f1<0) {
     steps[pos[l].l_step].typecheck=0;      
     steps[pos[l].l_step].c1=i;      
     steps[pos[l].l_step].c2=pgm_read_byte(&knight_step[i][j]);      
     steps[pos[l].l_step].f1=f1;      
     steps[pos[l].l_step].f2=f2;           
     pos[l].l_step++;                
    }             
    j++;
  }    
}
//****************************
void add_stra(int l, int i) {
  signed char f1=pole[i];
  signed char f2=0;
  int j=0;
  while (pgm_read_byte(&stra_step[i][j])!=99)  {      
    if (pgm_read_byte(&stra_step[i][j])==88) f2=0;       
    else  if (f2==0) {      
     f2=pole[pgm_read_byte(&stra_step[i][j])];
     if  (f2==0||f2<0&&f1>0||f2>0&&f1<0) {
      steps[pos[l].l_step].typecheck=0;      
      steps[pos[l].l_step].c1=i;      
      steps[pos[l].l_step].c2=pgm_read_byte(&stra_step[i][j]);      
      steps[pos[l].l_step].f1=f1;      
      steps[pos[l].l_step].f2=f2;           
      
      pos[l].l_step++;            
     }          
    }
    j++;
  }
}
//****************************
void add_diag(int  l, int i) {
  signed char f1=pole[i];
  signed char f2=0;
  int j=0;
  while (pgm_read_byte(&diag_step[i][j])!=99) {                
    if (pgm_read_byte(&diag_step[i][j])==88)  f2=0; 
    else if (f2==0) {     
     f2=pole[pgm_read_byte(&diag_step[i][j])];
     if (f2==0||f2<0&&f1>0||f2>0&&f1<0) {
      steps[pos[l].l_step].typecheck=0;      
      steps[pos[l].l_step].c1=i;      
      steps[pos[l].l_step].c2=pgm_read_byte(&diag_step[i][j]);      
      steps[pos[l].l_step].f1=f1;      
      steps[pos[l].l_step].f2=f2;           
      pos[l].l_step++;            
     }         
    }
    j++;
  }    
}
//****************************
void add_one(int l,  int c1, int c2) {
  steps[pos[l].l_step].typecheck=0;      
  steps[pos[l].l_step].c1=c1;      
  steps[pos[l].l_step].c2=c2;      
  steps[pos[l].l_step].f1=pole[c1];      
  steps[pos[l].l_step].f2=pole[c2];           
  pos[l].l_step++;                
}
//****************************
void  sort_steps(int l) { //moves massive sort by weight
step_t buf;  
   for (int  i=pos[l].b_step;i<pos[l].l_step-1;i++) { 
    int maxweight=steps[i].weight;
    int maxj=i;
    for (int j=i+1;j<pos[l].l_step;j++)  {
     if (steps[j].weight>maxweight)  {
      maxweight=steps[j].weight; maxj=j;
     }      
    }    
    if  (maxweight==0&&l>0) return;    
    if (maxj==i) continue;    
    buf=steps[i];
    steps[i]=steps[maxj];
    steps[maxj]=buf;    
   }    
}
//****************************  
void generate_steps(int l) { 
 if (l>0) pos[l].b_step=pos[l-1].l_step;  else pos[l].b_step=ply+MAXDEPTH;
 pos[l].cur_step=pos[l].b_step; pos[l].l_step=pos[l].b_step;  
 int check;
 signed char f;  
 if (pole[poswk]!=fk) {      
   for  (int i=0;i<64;i++) if (pole[i]==fk) {  poswk=i; break; }     
 } 
 if (pole[posbk]!=-fk)  {      
     for (int i=0;i<64;i++) if (pole[i]==-fk) {  posbk=i; break; }        
  } 
 if (l>0) {
   if ((steps[pos[l-1].cur_step].typecheck&B1000)==0) {               
    if (pos[l].w) pos[l].check_on_table=check_w(); else pos[l].check_on_table=check_b();        
    if (pos[l].check_on_table) steps[pos[l-1].cur_step].typecheck=steps[pos[l-1].cur_step].typecheck|B1000;
   } else pos[l].check_on_table=1; 
  } else if (pos[0].w) pos[0].check_on_table=check_w();  else pos[0].check_on_table=check_b();         
 for (int ii=0;ii<64;ii++) {
  int i;
  if (pos[l].w) i=ii; else i=63-ii;
  f=pole[i];      
  if (f==0||f<0&&pos[l].w||f>0&&!pos[l].w)  continue;  
  if (pos[l].l_step>MAXSTEPS-30) break;
  switch (abs(f)) {   
   case fn: add_knight(l,i);break;   
   case fb: add_diag(l,i); break;
   case  fr: add_stra(l,i); break;   
   case fq: add_stra(l,i); add_diag(l,i); break;   
  }
 }  
  for (int ii=0;ii<64;ii++) {
   int i;
   if (pos[l].w)  i=ii; else i=63-ii;
   f=pole[i];      
   if (f==0||f<0&&pos[l].w||f>0&&!pos[l].w)  continue;   
   if (pos[l].l_step>MAXSTEPS-30) break;
   switch (abs(f)) {   
   case fk: add_king(l,i); break;           
   case fp:
      if (f==fp)  {
       if (row[i]<7&&pole[i-8]==0) add_one(l,i,i-8); 
       if (row[i]==2&&pole[i-8]==0&&pole[i-16]==0)  add_one(l,i,i-16);        
       if (row[i]==7) {
        if (pole[i-8]==0)  {
         add_one(l,i,i-8); steps[pos[l].l_step-1].typecheck=4;      
         add_one(l,i,i-8);  steps[pos[l].l_step-1].typecheck=5;      
         add_one(l,i,i-8); steps[pos[l].l_step-1].typecheck=6;      
         add_one(l,i,i-8); steps[pos[l].l_step-1].typecheck=7;               
        } 
        if (column[i]>1&&pole[i-9]<0) {
         add_one(l,i,i-9);  steps[pos[l].l_step-1].typecheck=4;      
         add_one(l,i,i-9); steps[pos[l].l_step-1].typecheck=5;      
         add_one(l,i,i-9); steps[pos[l].l_step-1].typecheck=6;      
         add_one(l,i,i-9); steps[pos[l].l_step-1].typecheck=7;               
        }
        if (column[i]<8&&pole[i-7]<0) {
         add_one(l,i,i-7);  steps[pos[l].l_step-1].typecheck=4;      
         add_one(l,i,i-7); steps[pos[l].l_step-1].typecheck=5;      
         add_one(l,i,i-7); steps[pos[l].l_step-1].typecheck=6;      
         add_one(l,i,i-7); steps[pos[l].l_step-1].typecheck=7;      
        }     
      } else {
       if (column[i]>1&&pole[i-9]<0) add_one(l,i,i-9);  
       if (column[i]<8&&pole[i-7]<0) add_one(l,i,i-7);        
      }
    } else if (f==-fp) {
      if (row[i]>2&&pole[i+8]==0) add_one(l,i,i+8);  
      if (row[i]==7&&pole[i+8]==0&&pole[i+16]==0) add_one(l,i,i+16); 
      if  (row[i]==2) {
        if (pole[i+8]==0) {
         add_one(l,i,i+8); steps[pos[l].l_step-1].typecheck=4;      
         add_one(l,i,i+8); steps[pos[l].l_step-1].typecheck=5;      
         add_one(l,i,i+8); steps[pos[l].l_step-1].typecheck=6;      
         add_one(l,i,i+8);  steps[pos[l].l_step-1].typecheck=7;               
        } 
        if (column[i]>1&&pole[i+7]>0)  {
         add_one(l,i,i+7); steps[pos[l].l_step-1].typecheck=4;      
         add_one(l,i,i+7);  steps[pos[l].l_step-1].typecheck=5;      
         add_one(l,i,i+7); steps[pos[l].l_step-1].typecheck=6;      
         add_one(l,i,i+7); steps[pos[l].l_step-1].typecheck=7;               
        }
        if (column[i]<8&&pole[i+9]>0) {
         add_one(l,i,i+9);  steps[pos[l].l_step-1].typecheck=4;      
         add_one(l,i,i+9); steps[pos[l].l_step-1].typecheck=5;      
         add_one(l,i,i+9); steps[pos[l].l_step-1].typecheck=6;      
         add_one(l,i,i+9); steps[pos[l].l_step-1].typecheck=7;               
        }     
      } else {
       if (column[i]>1&&pole[i+7]>0) add_one(l,i,i+7);  
       if (column[i]<8&&pole[i+9]>0) add_one(l,i,i+9);            
      }
     } 
    } //switch
  } //   
  
  if (pos[l].pp!=0&&pole[pos[l].pp]==0)  { //en passant  !!!
   if (pos[l].w==1) {
    if (column[pos[l].pp]>1&&pole[pos[l].pp+7]==fp)  { add_one(l,pos[l].pp+7,pos[l].pp); steps[pos[l].l_step-1].typecheck=1; steps[pos[l].l_step-1].f2=-fp;}
    if (column[pos[l].pp]<8&&pole[pos[l].pp+9]==fp) { add_one(l,pos[l].pp+9,pos[l].pp);  steps[pos[l].l_step-1].typecheck=1; steps[pos[l].l_step-1].f2=-fp;}
   } else  {
    if (column[pos[l].pp]>1&&pole[pos[l].pp-9]==-fp) { add_one(l,pos[l].pp-9,pos[l].pp);  steps[pos[l].l_step-1].typecheck=1; steps[pos[l].l_step-1].f2=fp;}
    if (column[pos[l].pp]<8&&pole[pos[l].pp-7]==-fp)  { add_one(l,pos[l].pp-7,pos[l].pp); steps[pos[l].l_step-1].typecheck=1; steps[pos[l].l_step-1].f2=fp;}
   }
  }

 if (pos[l].w==1&&!pos[l].check_on_table) { //white catle
  if (pos[l].wrk) { //short     
   if (pole[60]==fk&&pole[61]==0&&pole[62]==0&&pole[63]==fr)  {
    pole[60]=0; pole[61]=fk; poswk=61; check=check_w(); pole[60]=fk; poswk=60;  pole[61]=0;    
    if (!check) {    
     steps[pos[l].l_step].typecheck=2;      
     steps[pos[l].l_step].c1=60;      
     steps[pos[l].l_step].c2=62;      
     steps[pos[l].l_step].f1=fk;      
     steps[pos[l].l_step].f2=0;           
     pos[l].l_step++;                      
    } 
   }
  }  
  if (pos[l].wrq) { //long
   if (pole[60]==fk&&pole[59]==0&&pole[58]==0&&pole[57]==0&&pole[56]==fr)  {
    pole[60]=0; pole[59]=fk; poswk=59; check=check_w(); pole[60]=fk; poswk=60;  pole[59]=0;    
    if (!check) {        
     steps[pos[l].l_step].typecheck=3;      
     steps[pos[l].l_step].c1=60;      
     steps[pos[l].l_step].c2=58;      
     steps[pos[l].l_step].f1=fk;      
     steps[pos[l].l_step].f2=0;           
     pos[l].l_step++;                      
    } 
   }
  }
 } else if (pos[l].w==0&&!pos[l].check_on_table) { //black castle  
  if  (pos[l].brk) { //short
   if (pole[4]==-fk&&pole[5]==0&&pole[6]==0&&pole[7]==-fr)  {
    pole[4]=0; pole[5]=-fk; posbk=5; check=check_b(); pole[4]=-fk; posbk=4;  pole[5]=0;   
    if (!check) {    
     steps[pos[l].l_step].typecheck=2;      
     steps[pos[l].l_step].c1=4;      
     steps[pos[l].l_step].c2=6;      
     steps[pos[l].l_step].f1=-fk;      
     steps[pos[l].l_step].f2=0;           
     pos[l].l_step++;                      
    } 
   }
  }
  if (pos[l].brq) { //long
   if (pole[4]==-fk&&pole[3]==0&&pole[2]==0&&pole[1]==0&&pole[0]==-fr)  {    
    pole[4]=0; pole[3]=-fk; posbk=3; check=check_b(); pole[4]=-fk; posbk=4;  pole[3]=0;   
    if (!check) {    
     steps[pos[l].l_step].typecheck=3;      
     steps[pos[l].l_step].c1=4;      
     steps[pos[l].l_step].c2=2;      
     steps[pos[l].l_step].f1=-fk;      
     steps[pos[l].l_step].f2=0;           
     pos[l].l_step++;                      
    }
   }  
  }
 }  
 for (int i=pos[l].b_step;i<pos[l].l_step;i++) {       
  steps[i].typecheck=steps[i].typecheck&B111;
  steps[i].weight=abs(steps[i].f2);  
  if (abs(steps[i].typecheck&&B111)>3)  steps[i].weight+=fig_weight[abs(steps[i].typecheck&&B111)-2];    
  steps[i].weight<<=2;
  if (l>0) { //add to weight of best level move 
       if (pos[l].cut.c2==steps[i].c2&&pos[l].cut.c1==steps[i].c1)  steps[i].weight+=5; //best move
       if (steps[i].c2==steps[pos[l-1].cur_step].c2)  steps[i].weight+=8; //taking last moves figure                    
  } 
 }  //i     
 sort_steps(l);  
 
}
//****************************
int  evaluate(int l) { //calculating position weight proportional
int res;  
  if  (pos[l].w) res=5000L*(pos[l].weight_w-pos[l].weight_b+pos[l].weight_s)/(pos[l].weight_w+pos[l].weight_b+2000);  
   else res=5000L*(pos[l].weight_b-pos[l].weight_w-pos[l].weight_s)/(pos[l].weight_w+pos[l].weight_b+2000);          
  if (debute) res+=random(30); 
  return res; 
} 
//****************************  
boolean checkd_w() {
  signed char f2=0;
  int j=0;
  while (pgm_read_byte(&diag_step[poswk][j])!=99)  {                
    if (pgm_read_byte(&diag_step[poswk][j])==88) f2=0; 
    else if (f2==0) {     
     f2=pole[pgm_read_byte(&diag_step[poswk][j])];
     if (f2==-fb||f2==-fq) return(true);
    }
    j++;
  }
  f2=0;  j=0;
  while (pgm_read_byte(&stra_step[poswk][j])!=99) {                     
    if (pgm_read_byte(&stra_step[poswk][j])==88) f2=0; 
    else if (f2==0) {         
     f2=pole[pgm_read_byte(&stra_step[poswk][j])];
     if (f2==-fr||f2==-fq)  return(true);   
     if (j==0||pgm_read_byte(&stra_step[poswk][j])==88)
      if (f2==-fk) return(true);    
    }
    j++;
  }  
  return(false);  
}
//****************************
boolean checkd_b() {  
  signed  char f2=0;
  int j=0;
  while (pgm_read_byte(&diag_step[posbk][j])!=99) {                
    if (pgm_read_byte(&diag_step[posbk][j])==88) f2=0; 
    else if (f2==0) {     
     f2=pole[pgm_read_byte(&diag_step[posbk][j])];
     if (f2==fb||f2==fq) return(true);    
    }
    j++;
  }
  f2=0;  j=0;
  while (pgm_read_byte(&stra_step[posbk][j])!=99) {                     
    if (pgm_read_byte(&stra_step[posbk][j])==88) f2=0; 
    else if (f2==0) {         
     f2=pole[pgm_read_byte(&stra_step[posbk][j])];
     if (f2==fr||f2==fq)  return(true);    
     if (j==0||pgm_read_byte(&stra_step[posbk][j])==88)
      if (f2==fk) return(true);    
    }
    j++;
  }  
  return(false);  
}
//****************************
int active(step_t& s) { //determines  if move active before move
int j;      
  if (s.f2!=0||(s.typecheck&B111)>3)  return 1; //taking or promotion
  if (abs(s.f2)==fk) return -1; //king moves  not active, castling too
  switch (s.f1) {  
  case fp: 
     if (row[s.c2]>5)  return 1; //en passant
     if (column[s.c2]>1&&posbk==s.c2-9||column[s.c2]<8&&posbk==s.c2-7)  return 1; //check by pawn
     return -1;     
  case -fp: 
     if (row[s.c2]<4)  return 1; //en passant
     if (column[s.c2]>1&&poswk==s.c2+7||column[s.c2]<8&&posbk==s.c2+9)  return 1; //check by pawn
     return -1;          
  case fn:      
    j=0;
    while (pgm_read_byte(&knight_step[s.c2][j])!=99) {                    
     if  (pole[pgm_read_byte(&knight_step[s.c2][j])]==-fk) return 1; //check by knight
     j++;
    }      
    return 0;     
  case -fn:         
    j=0;
    while (pgm_read_byte(&knight_step[s.c2][j])!=99) {                    
     if  (pole[pgm_read_byte(&knight_step[s.c2][j])]==fk) return 1; //check by knight
     j++;
    }      
    return 0;     
  case fb: 
    if (diag1[s.c2]!=diag1[posbk]&&diag2[s.c2]!=diag2[posbk])  return -1; //not king diagonal
    return 0;
  case -fb: 
    if (diag1[s.c2]!=diag1[poswk]&&diag2[s.c2]!=diag2[poswk])  return -1; //not king diagonal
    return 0;
  case fr: 
    if (row[s.c2]!=row[posbk]&&column[s.c2]!=column[posbk])  return -1; //not horiz-vert of king
    return 0;
  case -fr: 
    if (row[s.c2]!=row[poswk]&&column[s.c2]!=column[poswk])  return -1; //not horiz-vert of king
    return 0;
  case fq: 
    if (diag1[s.c2]!=diag1[posbk]&&diag2[s.c2]!=diag2[posbk]&&        //not king diagonal  
        row[s.c2]!=row[posbk]&&column[s.c2]!=column[posbk])  return -1; //not horiz-vert of king
    return 0;
  case -fq: 
    if (diag1[s.c2]!=diag1[poswk]&&diag2[s.c2]!=diag2[poswk]&&        //not king diagonal
        row[s.c2]!=row[poswk]&&column[s.c2]!=column[poswk])  return -1; //not horiz-vert of king
    return 0;
  } //switch     
  return  0; 
}
//****************************  
int quiescence(int l, int alpha,  int beta, int depthleft) {       
  if (depthleft<=0||l>MAXDEPTH-1) { 
    if  (l>depth) depth=l;
    return evaluate(l);
  }
 // Serial.println(l);
  int score=-20000;             
  generate_steps(l);  
  if (pos[l].l_step>MAXSTEPS-30)  return(evaluate(l));   
  //if (pos[l].l_step>maxstep) maxstep=pos[l].l_step;
  
  if (!pos[l].check_on_table) {   
    int weight=evaluate(l); 
    if  (weight >= score) score=weight; 
    if (score>alpha) alpha=score;    
    if  (alpha>=beta) return alpha;                
  }                      
  int  check,checked,act; 
  for (int i=pos[l].b_step;i<pos[l].l_step;i++) {       
   act=1;
   if (!pos[l].check_on_table) {
    act=active(steps[i]);    
    if (act==-1)  continue;    
   }
   movestep(l,steps[i]);        
   check=0;   
   if (act==0) {
    if (pos[l].w) check=checkd_b(); else check=checkd_w();       
    if (check) steps[i].typecheck=steps[i].typecheck|B1000;
     if  (!check) { backstep(l,steps[i]); continue; }      
   }   
   if (pos[l].w)  checked=check_w(); else checked=check_b();     
   if (checked) { backstep(l,steps[i]);  continue; } //if check opens - move restricted
   
   if (check&&depthleft==1&&l<MAXDEPTH-1)  depthleft++;     
   
   pos[l].cur_step=i;                       
   
   movepos(l,steps[i]);   
   steps[ply+l]=steps[i];
   int tmp=-quiescence(l+1,-beta,-alpha,depthleft-1);   
   backstep(l,steps[i]);   
   if (tmp>score) score=tmp;
   if (score>alpha)  {
    alpha=score;      
    pos[l].best_step=i;                        
    pos[l].cut=steps[i];                            
   }
   if (alpha>=beta  ) return alpha;    
  } 
  if (score==-20000) {
    if (pos[l].check_on_table)  {
      score=-10000+l; 
    }
  }       
  return score;
}
//****************************  
void print_best(step_t& s) {
  String wei=String(getcoeff()*s.weight/100.,2);
  if (s.weight>9000) wei="+M"+String((10001-s.weight)/2); 
  int tim=(millis()-starttime)/1000;   
  Serial.print(str_step(pos[0].cut)+" ("+wei+") "+get_time(tim)+" ");  
} 
//****************************  
int draw_repeat(int l) { 
 if  (l+ply<=12) return 0;
 for (int i=0;i<4;i++) {
  int li=ply+l-i; 
  if  (steps[li].c1!=steps[li-4].c1||steps[li].c2!=steps[li-4].c2||
      steps[li].c1!=steps[li-8].c1||steps[li].c2!=steps[li-8].c2)  return 0;
 }
 if (TRACE>0) Serial.println("repeat!"); 
 return 1;
}
//****************************  
int alphaBeta(int l, int alpha, int beta, int depthleft) {
 int score=-20000,  check, tmp;      
   if (depthleft<=0||l>=MAXDEPTH) {
      int fd=4; //4    
      if (l<5&&steps[pos[0].cur_step].typecheck&B1000) fd+=2;   //   
      if (steps[pos[l-1].cur_step].f2!=0) fd+=2;  //  
      return quiescence(l,alpha,beta,fd);                    
   }   
   if (l>0) generate_steps(l);         
   if  (pos[l].l_step>MAXSTEPS-30) return(evaluate(l));   
   //if (pos[l].l_step>maxstep)  maxstep=pos[l].l_step;
   for (int i=pos[l].b_step;i<pos[l].l_step;i++) {       
    if (l==0) depth=depthleft;                      
    movestep(l,steps[i]);        
    if (pos[l].w) check=check_w(); else check=check_b();        
    if (check) { backstep(l,steps[i]);  continue; } //if check opens - move restricted    
    pos[l].cur_step=i;
    movepos(l,steps[i]);  
    steps[ply+l]=steps[i];
    if (TRACE>l) {
     if (l==0) {              
       Serial.print(str_step(steps[i]));  Serial.print("  ");       
     } else {         
       Serial.println();             
       for (int ll=0;ll<l;ll++) Serial.print("      "); 
       Serial.print(String(l+1)+"- "+str_step(steps[i]));       
     }
    } //TRACE               
    tmp=-alphaBeta(l+1,-beta,-alpha,depthleft-1);   
    backstep(l,steps[i]);   
    if (draw_repeat(l)) tmp=0;     
    if  (tmp>score) score=tmp;
    steps[i].weight=tmp;        
    if (score>alpha&&!halt)  {
      alpha=score;      
      pos[l].best_step=i;                        
      pos[l].cut=steps[i];                        
      if (l==0) {        
        if (depthleft<10) Serial.print(" ");
        if (depth<10) Serial.print("  ");
        Serial.print(String(depthleft)+"/"+String(depth)+"  ");
        print_best(steps[i]);                
        Serial.println();
      }
    }
    if (TRACE>l) {
     if (l==0) {         
        Serial.println("        "+String(tmp)+"   "+String(depth));                    
     } else  {         
        Serial.print(" = "+String(tmp));                
     }
    }              
    if (alpha>=beta) return alpha;                    
    if (halt||(l<3&&millis()-starttime>timelimith)) {  //time out or stop
      halt=1;
      return score;                        
    }
    if (millis()-guitime>100)  gui();
   } //all moves   
   if (score==-20000) {
    if (pos[l].check_on_table)  {
      score=-10000+l; steps[pos[l-1].cur_step].typecheck=steps[pos[l-1].cur_step].typecheck|B1000;  
    } else score=0;
   }       
   return score;
}
//****************************  
//****************************  
boolean is_draw() { //determines if draw  on desk
  boolean draw=false;
  int cn=0,cbw=0,cbb=0,co=0,cb=0,cw=0;
  for  (int i=0;i<64;i++) {  
    if (abs(pole[i])==1) co++;  
    if (abs(pole[i])>3&&abs(pole[i])<6)  co++; //calculationg pawns, rooks, queens
    if (abs(pole[i])==6) continue;  //without kings
    if (abs(pole[i])==2) cn++; //number of knights
    if  (abs(pole[i])==3&&(column[i]+row[i])%2==0) cbb++; //number of whitefields bishops
    if (abs(pole[i])==3&&(column[i]+row[i])%2==1) cbw++; //number of blackfields  bishops
    if (pole[i]==3) cw++; //number of white bishops
    if (pole[i]==-3)  cb++; //number of black bishops
  }
  if (cn==1&&co+cbb+cbw==0) draw=true;  //one knight only
  if (cbb+cbw==1&&co+cn==0) draw=true;  //one bishop only
  if (co+cn+cbb==0||co+cn+cbw==0) draw=true; //    
  if (co+cn==0&&cb==1&&cw==1)  draw=true; //two bihops diffrent color
  if (draw) return draw;
  int lastmove=0;
  for (int i=0;i<ply;i++) //     
   if (abs(steps[i].c1)==fp||steps[i].f2!=0||(steps[i].typecheck&B111)!=0)  lastmove=i;  
  if (lastmove<ply-100) return 1; //50 moves rule draw
  if  (lastmove>ply-11) return 0; //     
  return 0; //!!!!!!!!!!!!!!!!!!!!      
  int ply0=ply;
  char poleb[64]; //
  while (lastmove<ply0-11) {
   start_game();           
   for (int i=0;i<=lastmove;i++) { //       
     pos[0].cur_step=i;
     movestep(0,steps[i]);   
     movepos(0,steps[i]);   
     pos[1].w=!pos[0].w;        
     pos[0]=pos[1];   
   }   
   for (int i=0;i<64;i++) poleb[i]=pole[i];  //   
   int repeats=0;

   lastmove++;
  }   
  start_game();           
  for (int i=0;i<ply0;i++) { //  
    pos[0].cur_step=i;
    movestep(0,steps[i]);   
    movepos(0,steps[i]);   
    pos[1].w=!pos[0].w;        
    pos[0]=pos[1];   
  }
  ply=ply0;
  
  return draw;  
}
//****************************
boolean  is_debute() {
 int diff=0; 
 for (int i=0;i<64;i++) if (pole[i]!=(char) pgm_read_byte(&polestart[i]))  diff++;
 debute=0;
 if (diff<13) debute=1;
 return debute;                                
}
//****************************  
int generate_legal() {
  kingpositions();
  generate_steps(0);
  int  legal=0;
  int check;
  for (int i=pos[0].b_step;i<pos[0].l_step;i++) {     
    pos[0].cur_step=i;
    movestep(0,steps[i]);   
    if (pos[0].w) check=check_w();  else check=check_b();           
    //pos[0].check_on_table=check;    
    if (!check) legal++;         
    if (!check) steps[i].weight=0; else steps[i].weight=-30000;
    backstep(0,steps[i]);
  }    
  sort_steps(0);    
  pos[0].l_step=pos[0].b_step+legal;  //illegal moves delete
  return(legal);  
}
//****************************  
void kingpositions() {
 for (int i=0;i<64;i++) { //kings positions definition
   if (pole[i]==fk) poswk=i;  
   if (pole[i]==-fk) posbk=i;
  }         
}  
//****************************  
int solve() {
step_t best;    
  starttime=millis();  
  if (is_draw()) { Serial.println(" DRAW!"); return  0; } //draw
  kingpositions();
  pos[0].weight_w=0;pos[0].weight_b=0; 
  for (int i=0;i<64;i++) { //enspile and figures weights definition 
    if (pole[i]<0)  {
      pos[0].weight_b+=fig_weight[-pole[i]];      
    } else if (pole[i]>0)  { 
      pos[0].weight_w+=fig_weight[pole[i]]; //full start weight 8000           
    }        
  }    
  if (pos[0].weight_w+pos[0].weight_b<3500) endspiel=true;  else endspiel=false;    //3500?  
  is_debute();  
  //Serial.println(pos[0].weight_w);
  //Serial.println(pos[0].weight_b);
  pos[0].weight_s=0;
  pos[0].cut.f1=0;  pos[0].cut.weight=0; 
  for (int i=0;i<64;i++) { //get statistic estimation
   int f=pole[i];
   if (!f) continue;
   if (abs(f)==fk&&endspiel) {
     if (f<0) 
       pos[0].weight_s-=(int)pgm_read_word(&stat_weightb[6][i]);                               
     else 
       pos[0].weight_s+=(int)pgm_read_word(&stat_weightw[6][i]);                                 
   } else {                
     if (f<0)  
      pos[0].weight_s-=(int)pgm_read_word(&stat_weightb[-f-1][i]);                               
     else 
      pos[0].weight_s+=(int)pgm_read_word(&stat_weightw[f-1][i]);                                     
   }      
  }  
  int startscore=evaluate(0);
  count=0; //maxstep=0;   
  for (int i=1;i<MAXDEPTH;i++) {
   if (i%2) pos[i].w=!pos[0].w;  else pos[i].w=pos[0].w;    
   pos[i].pp=0;
  }   
  best.f1=0;
  int  samebest=0;  
  int legal=generate_legal(); 
  if (legal==0) 
   if (pos[0].check_on_table)  { Serial.println(" CHECKMATE!"); return -2; } // 
    else { Serial.println("  PAT!"); return 0; } // 
    
  if (legal==1)  { //ove answere
    pos[0].cut=steps[pos[0].b_step];
    movestep(0,pos[0].cut);   
    int v=evaluate(0);
    backstep(0,pos[0].cut);
    pos[0].cut.weight=v; 
    return 1;  
  }
  
  halt=0;
  BAction.Show("STOP");     
  BMenu.Hide();  
  randomSeed(millis());
  erasestatus();  
  //status_c1=-1; status_c2=-1;
  solving=1;  
  int ALPHA=-20000; 
  int  BETA=20000;   
  level=2; //2 
  for (int x=0;x<MAXDEPTH;x++) { 
    pos[x].cut.f1=0;  pos[x].cut.c2=-1;     
  } //            
  int score;      
  while (level<=10)  {
   if (TRACE>0) {
    Serial.print(F("******* LEVEL=")); Serial.print(level);
    Serial.println();       
   }      
   for (int x=1;x<MAXDEPTH;x++) {  
    pos[x].cut.f1=0; pos[x].cut.c2=-1;     
   } //    0 
   for (int  i=pos[0].b_step;i<pos[0].l_step;i++) {     //       
    pos[0].cur_step=i;
    movestep(0,steps[i]);   
    int check=0;
    if (pos[0].w) check=check_b();  else check=check_w();           
    if (check) steps[i].typecheck=steps[i].typecheck|B1000;  
    steps[i].weight+=evaluate(0)+(steps[i].typecheck&B1000)*30;    
    if  (steps[i].f2!=0) steps[i].weight-=steps[i].f1;        
    backstep(0,steps[i]);
   }  
   steps[pos[0].b_step].weight+=10000; //first - always first
   sort_steps(0);        
   for (int i=pos[0].b_step;i<pos[0].l_step;i++) steps[i].weight=-8000;  //weights down
   
//   show_steps(0);
//   delay(100000000000000000000);  
   
   score=alphaBeta(0,ALPHA,BETA,level);   
   unsigned long tim=millis()-starttime;        
   bool out=0;
   ALPHA=score-100;
   BETA=score+100;      
// RENATOB corrigir out++ (acho que aqui a ideia é o out ficar sempre True, mas não sei)
  //  if (score>=BETA) { out++; BETA=20000; }         
  //  if (score<=ALPHA) { out++;  ALPHA=-20000; }   
   if (score>=BETA) { out= !out; BETA=20000; }         
   if (score<=ALPHA) { out= !out;  ALPHA=-20000; }   
   sort_steps(0);    //                      
   if (best.f1==steps[pos[0].b_step].f1&&best.c1==steps[pos[0].b_step].c1
       &&best.c2==steps[pos[0].b_step].c2&&best.typecheck==steps[pos[0].b_step].typecheck)  {
    samebest++;
    if (samebest>=2&&score>=startscore+200&&tim>timelimith/4)  break;     
   } else { best=steps[pos[0].b_step]; samebest=0; }
   if (score>9900||pos[0].cut.weight<-9900||tim>timelimith||halt)  break;         
   
   if (!out) level++;      
  }
  Serial.print("Move=");
  print_best(pos[0].cut);  
  Serial.println(" count="+String(count)+" nps="+String(1000*count/(millis()-starttime)));
  //Serial.println(" maxstep="+String(maxstep));  
  halt=1;
  show_status();  
  solving=0;    
  if (pos[0].cut.weight<-9900) return -1; 
  return  1;  
}
//****************************
void wactest() {
char w[150];  String ss=""; 
  timelimith=120000;
  TRACE=0;
  for (int i=155;i<=300;i++)  {
    strcpy_P(w, (char*)pgm_read_word(&wacs[i-1])); 
    ss=w;
    new_game(ss);       
    show_position();  
    clearstatus();
    show_board();  
    Serial.println(ss);
    Serial.println();
  }  
}
//**********************************
float  getcoeff() {
  float coeff=(pos[0].weight_b+pos[0].weight_w+2000)/5000.0;
  //Serial.println(coeff,2);
  return coeff;
}
//**********************************
void  show_status() {  
  String wei;
  
  if (pos[0].cut.weight>9000) wei="M"+String((10001-pos[0].cut.weight)/2);  
  else if (pos[0].cut.weight<-9000) wei="-M"+String((10001+pos[0].cut.weight)/2);  
   else wei=String(getcoeff()*pos[0].cut.weight/100.0,2); 
  if (pos[0].cut.weight>0)  wei="+"+wei;   
  uint16_t c=GRAY;
  if (halt) { 
    wei=wei+" "+String(1000*count/(millis()-starttime));  
    showstatus(get_time((millis()-starttime)/1000)+" "+String(level)+"/"+String(depth),str_step(pos[0].cut),wei,GRAY,WHITE);  
  } else {   
   showstatus(get_time((millis()-starttime)/1000)+"  "+String(level),str_step(pos[0].cut),wei);    
  } 
}
//********************************** 
void closemenu() {
  menu=0; 
  BMenu.Show("MENU");                         
  BBack.Hide();               
  BTime.Hide();                    
  BNew.Hide();                    
  BAuto.Hide();                    
  BRotate.Hide();                    
  BLoad.Hide();                    
  BSave.Hide();                    
  clearmenu();
  show_steps();
  clearstatus();     
  BAction.Show("START");  
}
//**********************************  
void check_gameend() {
   kingpositions();        
    String st;
    if (is_draw()) { gameover=1; st=F("draw!"); }
     else if (generate_legal()==0)  
     if (pos[0].check_on_table) { gameover=1; st=F("checkmate!"); } // 
      else { gameover=1; st=F("pat!"); } // 
    if (gameover)  showstatus(statusbuf1,st,statusbuf3,GRAY,RED);  
}
//********************************** 
void eepromsave() {
 EEPROM.put(0,limit);
  EEPROM.put(1,autow);
 EEPROM.put(2,autob);
 EEPROM.put(3,rotate);
 EEPROM.put(4,sound);
  EEPROM.put(5,ply);
 EEPROM.put(10,startpos);
 for (int i=0;i<ply;i++) 
  EEPROM.put(100+i*sizeof(step_t),steps[i]);
}
//**********************************  
void eepromload() {
 EEPROM.get(0,limit);
 EEPROM.get(1,autow);
 EEPROM.get(2,autob);
  EEPROM.get(3,rotate);
 EEPROM.get(4,sound); 
 EEPROM.get(10,startpos);
  start_game(); 
 EEPROM.get(5,ply); 
 for (int i=0;i<ply;i++) 
  EEPROM.get(100+i*sizeof(step_t),steps[i]);
  int pl=ply;
 for (int i=0;i<pl;i++) {
   pos[0].cur_step=i;
   if (pos[0].w==1)  pos[1].w=1; else pos[1].w=0;    
   if (steps[i].f1!=0) {     
     movestep(0,steps[i]);   
     movepos(0,steps[i]);               
    } 
   pos[0]=pos[1];         
  } 
 ply=pl;
 if (ply%2==0) pos[0].w=1; else pos[0].w=0; 
}
//**********************************  
void gui() {
char w[150]; 
 if (Serial.available()) {
      String  s="";
      while (Serial.available()) s=s+char(Serial.read());
      s.trim();     
      if (solving) {
       if (!halt) { 
        s.toUpperCase();   
        if (s=="STOP") { halt=1; Serial.println("STOPPED"); }     
       } 
      } else { //!solving
       if (s.indexOf("TIME")==0||s.indexOf("time")==0)  {
        long tim=s.substring(4).toInt();
        if (tim!=0) timelimith=tim*1000;
        if (timelimith/60000>0) {
         Serial.print("timelimith = "+String(timelimith/60000)+"  min ");
         int sec=(timelimith%60000)/1000;
         if (sec>0) Serial.println(String(sec)+"  sec"); else Serial.println();  
        } else  Serial.println("timelimith  = "+String(timelimith/1000)+" sec");    
       } else {
        if (s.indexOf("/")==-1)  { 
         int n=s.toInt();
         if (n>0&&n<=300) {
          strcpy_P(w,  (char*)pgm_read_word(&wacs[n-1])); 
          s=w;
         } else if (s=="0")  s=F("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"); 
         else  return;
        }
        new_game(s); 
        gameover=0;      
        if  (pos[0].w) { autow=1; autob=0; }  else { autob=1; autow=0; }
        initboard();
        show_position();  
        clearstatus();
        show_board();  
        Serial.println(s);        
       }      
      } 
      Serial.println(F("Command:"));
  } //serial
 if (gettouch()) {
  if (solving) {
   if (BAction.IsPressed())  { //calculation stop
     beep(100); 
     if (!halt) {
      halt=1;
      pause=1;
      BAction.Hide();        
     } 
   }   
  } else  { //solving 
   if (BAction.IsPressed()) {        
     beep(100); 
     if  (status_c1!=-1&&status_c2!=-1) { //player move try
      generate_legal();
      int g=0, gn=-1;      
      for (int i=pos[0].b_step;i<pos[0].l_step;i++)  {
       if (steps[i].c1==status_c1&&steps[i].c2==status_c2) { g++; gn=i; }                
      }      
      if (!g) {
       beep(300);
       erasestatus();
      } else {
        pos[0].cur_step=gn;
        movestep(0,steps[gn]);   
        movepos(0,steps[gn]);   
        steps[ply]=steps[gn];        
        pos[1].w=!pos[0].w;        
        pos[0]=pos[1];   
        ply++;
        //show_position();
        show_board();       
        show_steps();    
        erasestatus();
        check_gameend();        
        BAction.Show("START");
        BMenu.Show("MENU");               
        pause=0;      
      }
     } else {
      pause=0;      
      BAction.Show("START");
     }
   } else if (BMenu.IsPressed()) {         
    beep(100); 
    if (!menu)  {
     menu=1;   
     BAction.Hide();
     erasestatus();      
     clearmenu();     
     BMenu.Show("EXIT");
     if (ply>1) BBack.Show("BACK"); else  BBack.Hide();                              
     BTime.Show(limitstrings[limit]);
     BNew.Show("NEW");                         
     String sa="";
     if  (autow) sa="W";
     if (autob) sa=sa+"B";
     if (sa=="") sa="-";
     sa="AUTO:"+sa;
     BAuto.Show(sa);                         
     BRotate.Show("ROTATE");                         
     BLoad.Show();                         
     BSave.Show();                         
    } else { //menu
     closemenu();      
    }   
  } else if (BTime.IsPressed()) {        
     beep(100); 
     limit+=1;  if (limit>9) limit=0;
     timelimith=limits[limit]*1000;
     //Serial.println(timelimith);
     BTime.Show(limitstrings[limit]);     
  } else if (BBack.IsPressed()) {        
     beep(100); 
     int pl=ply-2;
     if (pl<0) pl=0;           
     start_game();     
     for (int i=0;i<pl;i++) {
      pos[0].cur_step=i;
      if (pos[0].w==1) pos[1].w=1; else pos[1].w=0;    
      if (steps[i].f1!=0)  {     
       movestep(0,steps[i]);   
       movepos(0,steps[i]);               
      } 
      pos[0]=pos[1];         
     } 
     ply=pl;
     if  (ply%2==0) pos[0].w=1; else pos[0].w=0;
     closemenu();
     erasestatus();
     show_board();
     show_color();
     show_steps();          
  }  else if (BNew.IsPressed()) {        
     beep(100);      
     new_game();
     cleardisplayboard();      
     show_board();
     border();
     closemenu();             
     erasestatus();     
     gameover=0;     
  } else if  (BAuto.IsPressed()) {        
    beep(100);      
    if (!autow&&!autob)  autob=1;   
     else if (!autow&&autob) { autow=1; autob=0; }  
      else  if (autow&&!autob) { autow=1; autob=1; }  
       else { autow=0; autob=0; }  
    String sa="";
    if (autow) sa="W";
    if (autob) sa=sa+"B";
    if (sa=="") sa="-";
    sa="AUTO:"+sa;
    BAuto.Show(sa);                            
    show_color();
  } else if (BRotate.IsPressed()) {        
    beep(100);        
    BRotate.Show();                         
    if (rotate) rotate=0;  else rotate=1;    
    erasestatus(); 
    border();
    cleardisplayboard();    
    show_board();
    show_color();
  } else if (BSave.IsPressed())  {        
    beep(100);        
    eepromsave();    
    BSave.Show();
    closemenu();             
    erasestatus();     
  } else if (BLoad.IsPressed())  {        
    beep(100);        
    eepromload();        
    BLoad.Show();
    closemenu();
    erasestatus();
    cleardisplayboard();      
    show_board();
    show_color();
    show_steps();          
    gameover=0;      
  }  else {  
    signed char c=field_pressed();
    if (!menu&&c!=-1) {     
     if (status_c1==-1) { 
      status_c1=c; status_step(status_c1,1,getColor(220,0,0));  
     }  else if (status_c2==-1) { 
      status_c2=c; status_step(status_c2,1,getColor(0,180,0));       
     }   
     if (status_c1!=-1&&status_c2!=-1&&c!=status_c2) erasestatus();      
    } 
   }      
  
  } //solving  
 } //gettouch 
 if  (solving&&millis()-statustime>999) { show_status(); statustime=millis(); }
 if  (!pause&&!menu&&!solving&&!gameover) {  
  if (pos[0].w&&autow||!pos[0].w&&autob)  {
   halt=0;
   TRACE=0;  
   gameover=0;      
   int res=solve();  
   //Serial.println(res);
   if (res==1) {    
    movestep(0,pos[0].cut);   
    movepos(0,pos[0].cut);   
    steps[ply]=pos[0].cut;
    pos[0]=pos[1];   
    ply++;
    //show_position();
    show_board();       
    show_steps();     
    erasestatus();
    check_gameend();   
   } else { 
    String  st=F("draw!");
    if (res==-1)  st=F("give up!"); 
     else if (res==-2)  st=F("checkmate!"); 
    for (int k=0;k<11;k++) statusbuf2[k]='-'; 
    showstatus(statusbuf1,st,statusbuf3,GRAY,RED);  
    gameover=1;
   }
   BAction.Show("START");
   BMenu.Show("MENU");    
  } else { // 
  }
 }
 guitime=millis();
} 
//****************************
void  loop() {
 gui();
 delay(100);
}
//****************************
