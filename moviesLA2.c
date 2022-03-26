#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define TGMAX 30
#define RMAX 8
/*Type definition*/
  typedef struct{
    char title[TGMAX];
    char genre[TGMAX];
    char rating[RMAX];
    int year;
    float score;
    int rented;
  } movie_t;
/*Functions*/
void menu(int *);
movie_t * loadfile(int *);
int displaylist(movie_t *, int);
int searchlist(movie_t *, int);
movie_t * sortlist(movie_t *, int);
movie_t * deletemovie(movie_t *, int *);
movie_t * addmovie(movie_t *, int *);
int savelist(movie_t *, int);
movie_t * createarray(int);

int main(void){
  int menu_choice, count;
  count = 0;
  movie_t *inventory = NULL;
  /*Call menu and scan user option, store option in variable menu_choice*/
  do{
    menu(&menu_choice);
    switch(menu_choice){
      case 1: inventory = loadfile(&count); break;
      case 2: displaylist(inventory, count); break;
      case 3: inventory = sortlist(inventory, count); break;
      case 4: searchlist(inventory, count); break;
      case 5: inventory = deletemovie(inventory, &count); break;
      case 6: inventory = addmovie(inventory, &count); break;
      case 7: savelist(inventory, count); break;
      case 8: printf("Quitting program...\n"); free(inventory); break;
      default: printf("Invalid selection.\n");
    }
  } while(menu_choice != 8);
  return(0);
}

void menu(int *input){
  //Create menu//
  printf("Select an action below: \n");
  printf("[1] Load list of movies from a file\n");
  printf("[2] Display currently loaded list of movies\n");
  printf("[3] Sort list of movies\n");
  printf("[4] Search for a movie\n");
  printf("[5] Delete a movie from the list\n");
  printf("[6] Add a movie to the list\n");
  printf("[7] Save the list to a file\n");
  printf("[8] Quit program\n");
  scanf("%d", input);
}

movie_t * loadfile(int *count){
  FILE *ifile;
  *count=0;
  char filename[TGMAX];
  movie_t dummy;
  //Open file that user enters//
  printf("Enter a file to load: ");
  scanf("%s", filename);
  ifile = fopen(filename, "r");
  if (ifile==NULL){
    printf("Could not open that file.\n");
    return(NULL);
  }
  //Count number of movies in file//
  while(fscanf(ifile, "%s %s %s %d %f %d", dummy.title, dummy.genre, dummy.rating, &dummy.year, &dummy.score, &dummy.rented)!=EOF) (*count)++;
  rewind(ifile);
  //Create array of that size//
  movie_t *list;
  list = createarray(*count);
  //Load file into list array//
  int i=0;
  while (fscanf(ifile, "%s %s %s %d %f %d", list[i].title, list[i]. genre, list[i].rating, &list[i].year, &list[i].score, &list[i].rented)!=EOF && i<*count){
    i++;
  }
  printf("Done.\n");
  fclose(ifile);
  return(list);
}

int displaylist(movie_t list[], int count){
  //Check if list is loaded//
  if (list==NULL){
    printf("No file loaded. Select [1] first.\n");
    return(-1);
  }
  //Print list//
  for(int i = 0; i<count;i++){
    printf("TITLE: %-28s GENRE: %-20s RATING: %-8s YEAR: %-5d SCORE: %-4.1f TIMES RENTED: %-5d\n", list[i].title, list[i]. genre, list[i].rating, list[i].year, list[i].score, list[i].rented);
  }
  return(0);
}

int searchlist(movie_t list[], int count){
  int status = 0; /*Flag for if found*/
  //Check if list is loaded//
  if (list==NULL){
    printf("No file loaded. Select [1] first.\n");
    return(-1);
  }
  char title_choice[TGMAX];
  //Ask user for movie title//
  printf("Enter movie title to find: ");
  scanf("%s", title_choice);
  //Search for title and print if found//
  for(int i = 0; i<count; i++){
    if(strcmp(list[i].title, title_choice)==0){
      printf("TITLE: %-28s GENRE: %-20s RATING: %-8s YEAR: %-5d SCORE: %-4.1f TIMES RENTED: %-5d\n", list[i].title, list[i]. genre, list[i].rating, list[i].year, list[i].score, list[i].rented);
      status++;
    }
  }
  //Tell user if not found//
  if(status==0) printf("\nNot in list.\n");
  return(0);
}

int savelist(movie_t list[], int count){
  char newfile[TGMAX];
  FILE *afile;
  //Check if list is loaded//
  if (list==NULL){
    printf("No file loaded. Select [1] first.\n");
    return(-1);
  }
  //Ask user file name to save to and open file//
  printf("Enter name of new file to save current loaded list: ");
  scanf("%s", newfile);
  afile = fopen(newfile, "w");
  if(afile==NULL) return(-1);
  //Print into file//
  for (int i=0; i<count; i++){
    fprintf(afile, "%s %s %s %d %f %d\n", list[i].title, list[i].genre, list[i].rating, list[i].year, list[i].score, list[i].rented);
  }
  printf("Done.\n");
  fclose(afile);
  return(0);
}

movie_t * sortlist(movie_t list[], int count){
  int sortfield_choice, sortorder_choice, temp_index;
  //Create type and array to hold fields//
  typedef struct{
    char string[TGMAX];
  } sortfield_t;
  sortfield_t *sortarray; 
  sortfield_t temp;
  sortarray = (sortfield_t *) malloc(count*sizeof(sortfield_t)); //Array for strings//
  float *n_sorting;
  float tempf;
  n_sorting = (float *) malloc(count*sizeof(float)); //Array for numbers//
  int *index;
  index = (int *)malloc(count*sizeof(int)); //Array for index sorting//
  for(int i = 0; i<count; i++) index[i]= i; //Load index array with index from 0 to count//
  //Check if list is loaded//
  if (list==NULL){
    printf("No file loaded. Select [1] first.\n");
    return(NULL);
  }
  //Ask user how to sort//
  do{
    printf("Enter a number to choose which field to sort by:\n");
    printf("[1] Title [2] Genre [3] Rating [4] Year [5] Score [6] Times Rented\n");
    scanf("%d", &sortfield_choice);
  } while(sortfield_choice<1 && (sortfield_choice)>6);
  do{
    printf("Enter a number to choose the order to sort:\n");
    printf("[1] Ascending [2] Descending\n");
    scanf("%d", &sortorder_choice);
  } while(sortorder_choice !=2 && sortorder_choice!=1);
  //Copy user choice field into array for sorting//
  switch(sortfield_choice){
    case 1: for(int i=0; i<count; i++) strcpy(sortarray[i].string, list[i].title); break;
    case 2: for(int i=0; i<count; i++) strcpy(sortarray[i].string, list[i].genre); break;
    case 3: for(int i=0; i<count; i++) strcpy(sortarray[i].string, list[i].rating); break;
    case 4: for(int i=0; i<count; i++) n_sorting[i] = list[i].year; break;
    case 5: for(int i=0; i<count; i++) n_sorting[i] = list[i].score; break;
    case 6: for(int i=0; i<count; i++) n_sorting[i] = list[i].rented; break;
  }
  //Ascending sorting//
  if(sortorder_choice ==1){
    if(sortfield_choice>=1 && sortfield_choice<=3){
      for(int a = 0; a<count; a++){
        for(int b = a+1; b<count; b++){
          if(strcmp(sortarray[a].string, sortarray[b].string)>0){
            temp_index = index[a];
            index[a] = index[b];
            index[b] = temp_index;
            strcpy(temp.string, sortarray[a].string);
            strcpy(sortarray[a].string, sortarray[b].string);
            strcpy(sortarray[b].string, temp.string);
          }}}
    }
    else{
      for(int a = 0; a<count; a++){
        for(int b = a+1; b<count; b++){
          if(n_sorting[a]>n_sorting[b]){
            temp_index = index[a];
            index[a] = index[b];
            index[b] = temp_index;
            tempf = n_sorting[a];
            n_sorting[a] = n_sorting[b];
            n_sorting[b] = tempf;
          }}}}
  }
  //Descending sorting//
  if(sortorder_choice ==2){
    if(sortfield_choice>=1 && sortfield_choice<=3){
      for(int a = 0; a<(count-1); a++){
        for(int b = a+1; b<count; b++){
          if(strcmp(sortarray[a].string, sortarray[b].string)<0){
            temp_index = index[a];
            index[a] = index[b];
            index[b] = temp_index;
            strcpy(temp.string, sortarray[a].string);
            strcpy(sortarray[a].string, sortarray[b].string);
            strcpy(sortarray[b].string, temp.string);
          }}}}
    else{
      for(int a = 0; a<(count-1); a++){
        for(int b = a+1; b<count; b++){
          if(n_sorting[a]<n_sorting[b]){
            temp_index = index[a];
            index[a] = index[b];
            index[b] = temp_index;
            tempf = n_sorting[a];
            n_sorting[a] = n_sorting[b];
            n_sorting[b] = tempf;
          }}}}
  }
  //Use index to copy sorted list into new list and return new list//
  movie_t *sortedlist;
  sortedlist = createarray(count);
  for(int i = 0; i<count; i++) sortedlist[i] = list[index[i]];
  //Free sorting arrays//
  printf("Done sorting list.\n");
  free(n_sorting);
  free(sortarray);
  free(list);
  free(index);
  return(sortedlist);
}

movie_t * deletemovie(movie_t list[], int *count){
  //Check if list is loaded//
   if (list==NULL){
    printf("No file loaded. Select [1] first.\n");
    return(NULL);
  }
  //Ask user which movie to delete//
  char delete_choice[TGMAX];
  printf("Enter movie title to delete: ");
  scanf("%s", delete_choice);
  //Search list and find title//
  int marker = -1;
  for(int i = 0; i<(*count); i++){
    if(strcmp(list[i].title, delete_choice)==0) marker = i;
  }
  if(marker>=0){
    //Reduce count//
    (*count)-=1;
    //Create new array one less bigger than current count//
    movie_t *deleted;
    deleted = createarray(*count);
    for(int j = 0; j<(*count); j++){
      if (j<marker){
        deleted[j] = list[j];
      }
      if (j>=marker){
        deleted[j]= list[j+1];
      }
    }
    printf("Done deleting.\n");
    free(list);
    return(deleted);
  }
  //Not found//
  else{
    printf("\nNot in list.\n");
    return(list);
  }
}

movie_t * addmovie(movie_t list[], int *count){
  int i;
  (*count)++;
  //Ask user what to add//
  movie_t new_movie;
  printf("Enter title: ");
  scanf("%s", new_movie.title);
  printf("\nEnter genre: ");
  scanf("%s", new_movie.genre);
  printf("\nEnter rating: ");
  scanf("%s", new_movie.rating);
  printf("\nEnter release year: ");
  scanf("%d", &new_movie.year);
  printf("\nEnter score: ");
  scanf("%f", &new_movie.score);
  printf("\nEnter number of times rented: ");
  scanf("%d", &new_movie.rented);
  //Create larger array//
  movie_t * added;
  added = createarray(*count);
  //Copy original list//
  for(i = 0; i<(*count-1); i++){
    added[i] = list[i];
  }
  added[i] = new_movie; //Add new movie//
  printf("Done adding.\n");
  free(list);
  return(added);
}

movie_t * createarray(int num){
  movie_t* new = NULL;
  if(num>0) new = (movie_t *) malloc(num* sizeof(movie_t));
  return(new);
}
