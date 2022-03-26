#include <stdio.h>
#include <math.h>
void analyze(FILE *, double *, double *, int *, double);
int outlier(FILE *, double *, double *, double, int *, double*, double *);

int main(void)
{
  int file_choice, count, status;
  double range_choice, range, m, b, stdev;
  FILE *ifile;
  /*Ask user to select file from menu*/
  printf("Select a data file to analyze: \n");
  for(int i = 1; i<=6; i++){
    printf("[%d] datafile%d.txt\n", i, i);
  }
  scanf("%d", &file_choice);
  /*Open proper file*/
  switch(file_choice){
    case 1: ifile = fopen("datafile1.txt", "r"); break;
    case 2: ifile = fopen("datafile2.txt", "r"); break;
    case 3: ifile = fopen("datafile3.txt", "r"); break;
    case 4: ifile = fopen("datafile4.txt", "r"); break;
    case 5: ifile = fopen("datafile5.txt", "r"); break;
    case 6: ifile = fopen("datafile6.txt", "r"); break;
    default: printf("That file does not exist.\n");
  }
  if (ifile == NULL) return(-1);
  /*Ask user for range to determine outlier*/
  printf("Enter a value to specify the range for determining outliers: ");
  scanf("%lf", &range_choice);
  /*Call function to analyze file as long as the equation changes, aka outliers exist*/
  range = stdev = 0;
  analyze(ifile, &m, &b, &count, range);
  do{
    status = outlier(ifile, &m, &b, range_choice, &count, &stdev, &range);
  } while (status!=1);
  /*Print information about best-fit line of data*/
  printf("The equation of the best-fit line for this file is: y = %.3lfx%+.3lf.\n", m, b);
  fclose(ifile);
  return(0);
}

void analyze(FILE *ifile, double *m, double *b, int *count, double range)
{
  /*Set sigmas to 0*/
  double sigx2, sigxy, sigy, sigx, D, num, num2;
  sigx2 = sigy = sigxy = sigx = 0;
  *count = 0;
  /*Read through file and add appropriate x and y to correct sigma*/
  while(fscanf(ifile, "%lf %lf", &num, &num2)!= EOF){
    if(range==0 ||(num2>=((*m)*num +(*b)-range) && num2<=((*m)*num +(*b)+range))){
      (*count)++;
      sigx2 += (num*num);
      sigxy += (num*num2);
      sigx += num;
      sigy += num2;
    }
  }
  rewind(ifile);
  /*Calculate slope and y-intercept*/
  D = (*count)*sigx2 - sigx*sigx;
  *b = ((sigx2*sigy) - (sigxy*sigx))/D;
  *m = ((*count)*sigxy - sigx*sigy)/D;
  //DEBUG printf("b = %lf\n", *b);
  //DEBUG printf("count = %d\n", *count);
}

int outlier(FILE *ifile, double *m, double *b, double range_choice, int *count, double *stdev, double *range)
{
  double sigr, sigrdif, resid, num, num2;
  sigr = sigrdif = 0;
  /*Hold m value*/
  double temp = *m;
  //DEBUG printf("%lf\n", temp);
  /*Calculate residuals and standard deviation*/
  while(fscanf(ifile, "%lf %lf", &num, &num2)!= EOF){
    if(*stdev==0 ||(num2>=((*m)*num +(*b)-(*range)) && num2<=((*m)*num +(*b)+(*range)))){
      resid = (num2 - ((*m)*num + (*b)));
      sigr += resid;
    }
  }
  rewind(ifile);
  sigr /= (*count);
  while(fscanf(ifile, "%lf %lf", &num, &num2)!= EOF){ 
    if(*stdev==0 ||(num2>=((*m)*num +(*b)-(*range)) && num2<=((*m)*num +(*b)+(*range)))){
      resid = (num2 - ((*m)*num + (*b)));
      sigrdif += (resid - sigr)* (resid - sigr);
    }
  }
  rewind(ifile);
  //DEBUG printf("sigrdif = %lf\n", sigrdif);
  *stdev = sqrt(sigrdif/((*count)-1));
  //DEBUG printf("stdev = %lf\n", *stdev);
  /*Multiply standard deviation by user input*/
  *range = range_choice * (*stdev);
  /*Call analyze function to find new best fit line with less outliers*/
  analyze(ifile, m, b, count, *range);
  /*Check for change and exit if no change or no outliers*/
  if(temp == *m) return(1);
  else return (0);
}