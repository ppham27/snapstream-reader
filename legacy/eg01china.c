
#include <stdio.h>
#include <string.h>

/* globally defined variables */

#define MAXHEAD 800	/* maximum number of characters in header */
#define MAXCHARS 18000000 /* maximum number of characters in daily log */
#define MAXWORDS 20000	/* maximum number of words in program */
#define MAXPROGS 250	/* maximum number of programs in a day */
#define PAD 50		/* number of words before and after excerpt */
#define MAXTITLE 50	/* maximum number of characters in title */
#define NUMTITLES 400	/* maximum number of unique show titles */
#define MAXLEN 14	/* maximum length of word or string */
#define MAXMATCH 200	/* maximum number of matches of word within program */

char *filename="test9";  /* name of the daily file  */
char *tag ="globaliz*";
char *taga="globaliz*";
char *tagb="trade";

char bigstring[MAXCHARS];        /* daily log with word terminators */
int letter=0;                      /* position inside bigstring */
int numprogs=0;                    /* number of programs */
char headers[MAXPROGS][MAXHEAD]; /* meta-data headers in 1-D array of strings */
char titles[MAXPROGS][MAXTITLE]; /* titles in 1-D array of strings */
int wordnum[MAXPROGS];		/* number of words in ith program */
int titlens[MAXPROGS];            /* title lengths in a 1-D array */
int titlenums[MAXPROGS];         /* title ID's in 1-D int array */
char *table[MAXPROGS][MAXWORDS]; /* starts of words not in headers, stored 
				    as 2-D array of pointers into bigstring */
int LEN;      		/* length of big string */
int hlen;      		/* header length */
char title[MAXTITLE];
char titlearray[NUMTITLES][MAXTITLE];
int maxid;		/* one more than the largest title ID */
char date[25];		/* not currently used */
int titlen, datlen;
int n1=0;			/* how many programs contain matches */

int pri[MAXMATCH+1];     /* array of pointers to table that stores 
			       current ingredients in boolean match */

int sec[MAXMATCH+1];     /* second array of pointers to table, to combine
			       in boolean fashion with primary array */

int ter[MAXMATCH+1];     /* third array of pointers to table, to combine
			       in boolean fashion with the others */


/* subroutine declarations */

int getstring(char *);	/* inputs daily log, returns length of bigstring */

void loadtitles();		/* loads title strings into titlearray */

int striptitle(int i);  /* strips title and date out of header and */
                        /* places title in titles[i], returning titlen[i] */

int idmatch(int i);	/* finds the program ID for the title #i */

int parseprogs();       /* parses header, stripping data, then parses body, */
                        /* storing pointers to beginnings of words */
int readheader(int i);

int readbody(int i);   /* finds beginning of word pointers to next program */
                       /* block (where letter is pointing) and writes */
                       /* starting locations of words into array */

char text(char cc);	/* returns -1 if EOF, returns 0 if not text, */
			/* apostrophe or hyphen, else returns lower case  */
			/* letter value or apostrophe or hyphen  */

int wildcmp(char *s, char *t);  /* like library strcmp but allows wild card */

int find(int i, char *ma, int s[]); /* finds locations of *ma in
				       ith program, stores in s[] */

int near(int i, int d, char *ma, char *mb, int s[]);
						/* finds *ma within d of *mb in
						   program i, stores in s[] */

int not(int s[]);		      /* negates s[] */

int and(int s[],int t[]); /* stores (s[] AND t[]) in s[] */

int or(int s[],int t[]);	/* stores (s[] OR t[]) in s[] */

void display(int i, int s[]);	/* prints out +-PAD about each word
				   in ith program at locations in s[] */

void printtitle(int i);		/* prints out title #i */

void printcaps(int i, int j);   /* prints table[i][j] all in caps */

/* subroutines start here */

/* getstring inputs the big string "bigstring", inserting \0 characters
   between each transision from a text to a non-text character.  It returns
   the length of bigstring */

int getstring(char* file_name)
{
  int i=0;
  char y;
  char z='\0';
  FILE *fp;
  
  fp = fopen(file_name,"r");
  while ((y = getc(fp)) != EOF)  {
    if (y == -61) {/* bigstring[i++]=13 */; numprogs++;}
    if ( (i > 0) && (text(y) == 0) && (text(z) != 0) ) {bigstring[i++]='\0';}
    bigstring[i++]=y;
    z = y;
    if (i >= MAXCHARS) {
      printf("\nWarning: file exceeded %d chars\n",i); 
      break;
    }
  }
  bigstring[i++] = -61;
  bigstring[i++]=EOF; 
  fclose(fp);
  return(i);
}

/* parseprogs is the control flow for readheader(i) and readbody(i),
   which loop i through the programs in the daily log, filling the
   metadata array with header data and the words array with the
   body data.  It returns the number of programs. */

int parseprogs(int i)
{
  int t, prog=0;
  letter = i;
  for(prog=0 ; (prog < numprogs) && (t != -1) ; prog++)
  {
    /* printf("\nReading header #%d",prog); */
    /* printf("\nStarting header at letter #%d",letter); */
    t = readheader(prog);
    /* printf("\nEnding header at letter #%d",letter); */
    if (hlen != -1) 
        {
        /* printf("\nReading program #%d",prog); */
        /* printf("\nStarting program at letter #%d",letter); */
        wordnum[prog]=readbody(prog);
        /* printf("\nEnding program at letter #%d",letter); */
        /* printf("\nNumber of words in program #%d is %d\n", prog,wordnum[prog]);  */
     } 
  }
  return(prog);
}

/* reads the ith header, stores the title and title ID in 
   titles[i] and titlenums[i] respectively and returns the length 
   of the title, or -1 if reaches end marker for file or daily log */

int readheader(int i)
{
  int hlen; 
  int hstart;
  int j;
  char y;

  hstart = letter;
  while( ((y=bigstring[letter]) != EOF) && (y != 61) 
                                     && (letter < hstart + MAXHEAD))
    {headers[i][letter++ - hstart] = y;} 

  if (y == EOF) 
     {printf("\nEOF at letter %d",letter); hlen = -1;}
  else if (letter++ - hstart >= MAXHEAD) 
    {
    printf("\nWarning: maximum header length exceeded at letter = %d and hstart = %d\n",letter,hstart);
    hlen=MAXHEAD; 
    }
  else 
    {
    hlen = letter - hstart;
    headers[i][hlen] = '\0';
    titlens[i] = striptitle(i);
    titlenums[i] = idmatch(i);
    /* printtitle(i); */
    }
  return(hlen);
}

/* printtitle prints out the title stored in titles[i] of length titlens[i] */

void printtitle(int i)
{
   int j;
   char y;

   /* printf("\nTitle of program #%d has ID #%d and reads:\n",i, titlenums[i]); */
   for (j=0; j<titlens[i]; j++) 
      {if ( (y=titles[i][j]) != '\0') printf("%c",y);}
}

/* striptitle extracts title from headers[i] and puts it in 
   titles[i], returning titlen to put in titlens[i] */

int striptitle(int i)
{
  int s,t;
  int j=0;
  int titlen;
  char y;

  while(headers[i][j++] != ':'); 
  while(text(headers[i][j]) == 0) {j++;}
  s=j; /* printf("\ns=%d, j=%d, header[i][j]=%d\n",s,j,headers[i][j]); */
  while((y = headers[i][j]) != 10) 
      {titles[i][j-s] = y; j++;
      }
  titles[i][j-s] = '\0';
  titlen = j-s;
  return(titlen);
}

/* readbody (i) reads the body from the current pointer (letter) into
   the ith row of the array "words".  It returns the number of words
   in the ith program. */

int readbody(int i)
{
  int wctr=0;
  int j;
  char y,z;

  z = bigstring[letter]; letter++; /* printf("\n(%d)",letter) */;

  while (((y=bigstring[letter]) != -61) && (y != EOF)) 
     {if ((text(y) != 0) && (text (z) == 0)) 
         {table[i][wctr] = &(bigstring[letter]); wctr++;}
      z=y; letter++;
      }
  return(wctr);
}

/* text converts upper case to lower case, leaves hypens and apostrophes */
/* alone, and converts everything else to 0 except EOF goes to -1 */

char text(char cc)
{ char dd;
  {
  if ( ((cc >= 'A') && (cc <= 'Z')) || ((cc >= 'a') && (cc <= 'z')) )
                        {dd = cc + ('a'-'A')*(cc < 'a');}
  else if (cc == 39) dd = 39; else if (cc == 45) dd = 45;
  else if (cc == EOF) dd = -1; 
  else if ((cc >= '0') && (cc <= '9')) dd=cc; 
  else dd = 0;
  return(dd);
  }
}

int wildcmp(char *s, char *t)
{
int i;
for (i=0; ; i++)
  {
  if (s[i] == 42) return (0);
  if ((s[i] == '\0') && ((t[i] == '\0') || (t[i] == '\n'))) return (0);
  if (s[i] != text(t[i])) return (s[i]-t[i]);
  }
}


/* find finds all occurrences of a string in program i, 
   storing the locations (pointers into table) in s, starting
   at location 1.  Location 0 gets the count, modified so that
   0 is replaced by -1; the count is also the output.  The 
   search is case-insensitive. */

int find(int i, char *ma, int s[])
{
int j=0, k=0;

while ((j < wordnum[i]) && (k < MAXMATCH+1))
  {
  if (wildcmp(ma,table[i][j]) == 0)
    {
    s[++k] = j; 
    }
  j++;
  }
if (k==0) {k = -1;}
s[0] = k;
return(k);
}

/* near finds all occurrences of *ma and *mb within d of each other in
   program i and stores the locations of either into the table s.
   Location s[0] gets the count, modified so that 0 is replaced by -1.
   The count is also the output.  The search is case-insensitive.  */

int near(int i, int d, char *ma, char *mb, int s[])
{
int flag=0, marker=0;
int j=0, k=0;
int a, b;
while ((j < wordnum[i]) && (k < MAXMATCH))
  {
  a=wildcmp(ma,table[i][j]); b=wildcmp(mb,table[i][j]);
  if (flag==0)
    {
      if (a==0) {flag=1; marker=j;}
      else if (b==0) {flag=2; marker=j;}
    }
  else if (flag==1)
    {
      if (b==0) {s[++k] = marker; s[++k] = j; flag=0;}
      else if (a==0) {marker=j;}
      else if (j - marker == d) {flag=0;}
    }
  else if (flag==2)
    {
      if (a==0) {s[++k] = marker; s[++k] = j; flag=0;}
      else if (b==0) {marker=j;}
      else if (j - marker == d) {flag=0;}
    }
  j++;
  }
if (k==0) {k = -1;}
s[0] = k;
return(k);
}

/* not negates a location array s: if the count is 0 or more, it simply
   replace the 0 in s[0] by a -1, leaving junk in the rest of s[]; if 
   s[0] is alrady -1, then it becomes 0, that is, when not changes
   false to true, it leaves a null true array.  */ 

int not(int s[])
{
if (s[0] >= 0) {s[0] = -1;} 
else s[0]=0;
return(s[0]);
}

/* and takes the union of two arrays s[] and t[] and stores it in the 
   first array, unless one is negative in which case it simply overwrites
   the count of the first by -1.  */

int and(int s[],int t[])
{
int j;

if ((s[0] >= 0) && (t[0] >= 0))
   {
   if (s[0] + t[0] > MAXMATCH) {printf("\nWarning: too many matches");}
   else {
        for (j=0;j<t[0];j++) {s[s[0]+j+1]=t[j+1];}
        s[0] = s[0]+t[0];
        }
   }
else {s[0] = -1;}
return(s[0]);
}

/* or takes the union of two arrays s[] and t[] and stores it in the 
   first array, unless both are negative in which case it does nothing. */

int or(int s[],int t[])
{
int j;

if ((s[0] >= 0) || (t[0] >= 0))
   {
   if (s[0]==-1) s[0]=0;
   if (t[0]==-1) t[0]=0;
   if (s[0] + t[0] > MAXMATCH) {printf("\nWarning: too many matches");}
   else {
        for (j=0;j<t[0];j++) {s[s[0]+j+1]=t[j+1];}
        s[0] = s[0]+t[0];
        }
   }
else {s[0] = -1;}
return(s[0]);
}

/* printcaps prints the jth word of program i in all CAPS */

void printcaps(i,j)
{
int k=0;
char y;
while((y=table[i][j][k++]) != '\0') {printf("%c",text(y) + 'A' - 'a');}
printf(" ");
}

/* display selects the +-PAD chunks of program marked by the keyword indices
   in the array s[] (as indices of table[i][]).  After printing one chunk
   it suppresses chunks until there is no longer more than 50% overlap
   with the last printed chunk.  Once it decides to print a chunk,
   it catalogues all keywords in the chunk and reserves them for CAPS
   printing.  It uses character by character printing from table[] rather
   than word printing from s[] in order to get punctuation; it also
   suppresses the \0 characters that got inserted in getstring.  */

void display(int i, int s[])
{
int j, ini, fin;
int v,j0,j1;
char *u;
char *mark1, *mark2, *mark3, *mark4, *mark1b, *mark1c;

if (s[0]==0) {printf("\nWarning: tried to display empty matches.");}
        /* else printf("\nDisplaying %d matches at %d.",s[0],i); */
for (j=0;j<s[0];j++)
  {
  if (s[j+1] < PAD) {ini = 0;} else {ini = s[j+1]-PAD;}
  if (s[j+1] >= wordnum[i] - PAD) {fin = wordnum[i];} 
      else fin = s[j+1]+PAD;
  /*
  DEBUGGING LINE:
  printf("\n\nSegment containing word #%d spanning character %d to %d:\n", 
      s[j+1],ini,fin);
  */

  printf("\n\n");
  j0 = j+1; while ((j0 > 1) && (s[j0-1] >= ini)) j0--;
  j1 = j+1; while ((j1 < s[0]) && (s[j1+1] <= fin)) j1++;

  for(u=table[i][ini];u<table[i][s[j0]];u++) 
     {if (*u != '\0') printf("%c",*u);}
  for(v=j0;v<j1;v++)
    {
    printcaps(i,s[v]);
    for(u=table[i][s[v]+1];u<table[i][s[v+1]];u++) 
       {if (*u != '\0') printf("%c",*u);}
    }
  printcaps(i,s[j1]);
  for(u=table[i][s[j1]+1];u<table[i][fin];u++) 
     {if (*u != '\0') printf("%c",*u);}
  }
}


/* Loads titles from "titles.dat" to the array "titlearray[NUMTITLES][PAD]" */

void loadtitles()
{
int i=3;
int j;
FILE *fp;
char fname[] = "titles.dat";
char progtitle[MAXTITLE];

fp = fopen("titles.dat", "r");
while(fgets(progtitle,MAXTITLE,fp) != NULL)
   {j=0; 
      while(progtitle[j] != 0) {titlearray[i][j]=progtitle[j]; j++;} 
   i++;
   if (i == NUMTITLES) {printf("\n\nToo many titles!"); break;}
   }
fclose(fp);

maxid=i; /* printf("\n%d title ID's ending at %d\n",maxid - 3,maxid-1); */
/* for (j=3;j<i;j++) printf("\nTitle #%d = %s",j,titlearray[j]); */
}


/* Finds the program ID for character string in tit */
/* It uses the lookup file "title.dat" */
/* It is not case sensitive */
/* It finds the first entry of "title.dat" which is an initial substring */
/* of tit, so a unique initial identifier is all that's required */

int idmatch(int i)
{
int j,k,t,mismatch;
int id=0;
char y;

for (t=3; t<maxid; t++) 
  {
  j=0; mismatch=0; k=0;
  while( ((y=titlearray[t][j]) != 10) && (y != '*'))
    {
    while (titles[i][k] == '\0') {k++;}
    if (text(y) != text(titles[i][k])) {mismatch=1; break;} 
    /* else printf("\nMatched %d[%d] and %d[%d], equaling %d",t,j,i,k,y); */
    j++; k++;
    }
  if (mismatch==0) {id=t;}
  }
return(id);
}



/* MAIN PROGRAM STARTS HERE */

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "a file name is expected as the first argument");
    return -1;
  }
  int i,j,t;
  loadtitles();
  LEN = getstring(argv[1]);
   
  printf("\nlength of main string = %d",LEN); 
  printf("\nNumber of programs = %d\n", numprogs);
  printf("\n----------------------------------------------------------\n");
   
  parseprogs(0); 
  int total_matches = 0;
  int selected_programs = 0;
  for (i=0;i<numprogs;i++) {
    if (titlenums[i] != 0) {
      ++selected_programs;
      /* SAMPLE formatting - do not delete */
      /* find(i,"presid*",pri); */
      /* find(i,"trad*",sec); */
      /* t = near(i,50,"has","won",pri); */
      /* not(sec); */
      /* t = and(pri,sec); */
      /* near(i,50,"trad*","china*",pri); */
      /* near(i,50,"trad*","chinese*",sec); */
      /* t = or(pri,sec); */
      t = find(i,"chin*",pri);
      /* find(i,"trad*",pri); */
      /* find(i,"nafta",sec); */
      /* t = or(pri,sec); */
      /* t = or(pri,sec); */
      /* find(i,"tradition*",ter); */
      /* not(ter); */
      if (t != -1) {     
        printf("\n--------------------------------------------\n\nTitle: ");
        printtitle(i);
        printf("\nTitle ID #%d",idmatch(i)); printf("\n");
        display(i,pri);
        n1++;
        total_matches += pri[0];
      }
    }
  }
  printf("\n--------------------------------------------\n");
  printf("\n     Number of total matches: %d.\n",total_matches);
  printf("     Number of programs with matches: %d.\n",n1);
  printf("     Number of total programs: %d.\n",numprogs);
  printf("     Number of selected programs: %d.\n",selected_programs);
  printf("%d\t%d\t%d\t%d\n", total_matches, n1, numprogs, selected_programs);
  return 0;
}

