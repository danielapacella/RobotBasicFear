/*
 * Evorobot* - parameters.cpp
 * Copyright (C) 2009, Stefano Nolfi
 * LARAL, Institute of Cognitive Science and Technologies, CNR, Roma, Italy
 * http://laral.istc.cnr.it

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
/*
 * parameters.c
/*
 */

#include "public.h"

struct Parameter {
    char   *name;                 // the name of the parameter
    char   *folder;               // the folder of the parameter
    int     type;          // the type, 0=int 
    int     n1;                   // the minimum value (for type 0) the number of elements (for type 4 & 7)
    int     n2;                  // the maximum value (for type 0)
    int    *ipointer;             // the pointer to a (int) parameter
    float  *fpointer;             // the pointer to a (float) parameter
    char   *cpointer;             // the pointer to a (char) parameter
    float  **ffpointer;           // the pointer to a pointer to a (float) parameter
    struct Parameter *next;       // the next parameter of the same folder
    struct Parameter *nextfolder; // the first parameter of the next folder
    char   *description;          // parameter description
    int    modified;              // whether the parameter has been modified from its default value 
};

struct Parameter *parlist = NULL; // parameters list
Parameter *ccfolder;              // current folder

int  winparametersx = -1;     // x coordinate of parameters window (-1 = undefined)
int  winparametersy = -1;     // y coordinate of parameters window (-1 = undefined)
int  winparametersdx = 0;     // dx of controller parameters window (0 = undefined)
int  winparametersdy = 0;     // dy of controller parameters window (0 = undefined)

/*
 * This function create a new parameter
 * if the parameter have been already created we cannot modify its properties
 *  with the exception of type 7 (*float) for which we can modify the nuber of available elements up to the maximum (n2)
 * type, 0=int, 1=float, 2=char, 3=int-v, 4=float-v, 5=char-v, 7=*float
 */
void create_parameter(char *name, char *folder, void *vpointer, int type, int n1, int n2, char *description)

{
    struct Parameter *cfolder;
    struct Parameter *cpar;
    struct Parameter *lastfolder;
    struct Parameter *lastpar;
    int folderfound;
    int parfound;
    char emessage[64];

    cfolder = parlist;
    lastfolder = cfolder; 
    folderfound = 0;
    while(cfolder != NULL && folderfound == 0)
     {
       if (strcmp(cfolder->folder, folder) == 0)
        {
         folderfound = 1;
        }
       else
        {
         lastfolder = cfolder;
         cfolder = cfolder->nextfolder;
        }
     }

     parfound = 0;
     if (folderfound == 1)
        {
     cpar = cfolder;
         while(cpar != NULL && parfound == 0)
          {
            if (strcmp(cpar->name, name) == 0)
             {
              parfound = 1;
             }
        else
             {
          lastpar = cpar;
              cpar = cpar->next;
             }
          }
        }

     if (parfound == 0)
       {
        cpar = (struct Parameter  *) malloc (sizeof (struct Parameter));
        cpar->name   = (char *) malloc((unsigned)(strlen(name) + 1)); // +1 for trailing NULL
        strcpy(cpar->name, name);
        cpar->folder = (char *) malloc((unsigned)(strlen(folder) + 1));
        strcpy(cpar->folder, folder);
        cpar->description = (char *) malloc((unsigned)(strlen(description) + 1));
        strcpy(cpar->description, description);
        cpar->type = type;
        cpar->n1 = n1;
        cpar->n2 = n2;
        cpar->next = NULL;
        cpar->nextfolder = NULL;
    cpar->modified = 0;
        if (type == 0 || type == 3)
           cpar->ipointer = (int *) vpointer;
        if (type == 1 || type == 4)
           cpar->fpointer = (float *) vpointer;
        if (type == 2 || type == 5)
           cpar->cpointer = (char *) vpointer;
        if (type == 7)
           cpar->ffpointer = (float **) vpointer;
    if (type == 7)
        cpar->n2 = cpar->n1;
       }

     // if the parameter has already be created we allow to modify
     // only the length of the vector up to the maximun in case of type 7
     if (type == 7 && n1 <= cpar->n2)
        cpar->n1 = n1;

     if (parlist == NULL)      // the first parameter and folder has been created
          parlist = cpar;
       else
         if (folderfound == 0) // an additional folder created
           lastfolder->nextfolder = cpar;
      else
        if (parfound == 0)
          lastpar->next = cpar; // an additional param. created
}

/*
 * create a int parameter
 */
void create_iparameter(char *name, char *folder, void *vpointer, int n1, int n2, char *description)

{
  create_parameter(name, folder, vpointer, 0, n1, n2, description);
}

/*
 * create an int parameter
 */
void create_fparameter(char *name, char *folder, void *vpointer, int n1, int n2, char *description)

{
  create_parameter(name, folder, vpointer, 1, n1, n2, description);
}

/*
 * create a char parameter
 */
void create_cparameter(char *name, char *folder, void *vpointer, int n1, int n2, char *description)

{
  create_parameter(name, folder, vpointer, 2, n1, n2, description);
}

/*
 * create a vector of int parameters
 */
void create_ivparameter(char *name, char *folder, void *vpointer, int n1, int n2, char *description)

{
  create_parameter(name, folder, vpointer, 3, n1, n2, description);
}

/*
 * create a vector of float parameters
 */
void create_fvparameter(char *name, char *folder, void *vpointer, int n1, int n2, char *description)

{
  create_parameter(name, folder, vpointer, 4, n1, n2, description);
}

/*
 * create a vector of float pointer parameters
 */
void create_fvpparameter(char *name, char *folder, void *vpointer, int n1, int n2, char *description)

{
  create_parameter(name, folder, vpointer, 7, n1, n2, description);
}

/*
 * This function modify the value of a previously defined parameter
 */
void set_parameter(char *name, char *folder, int type, int ivalue, float fvalue, char *cvalue)

{
    struct Parameter *cfolder;
    struct Parameter *cpar;
    int folderfound;
    int parfound;
    char emessage[64];
    float *f;

    cfolder = parlist;
    folderfound = 0;
    while(cfolder != NULL && folderfound == 0)
     {
       if (strcmp(cfolder->folder, folder) == 0)
         folderfound = 1;
       else
         cfolder = cfolder->nextfolder;
     }

     if (folderfound == 0)
    {
     sprintf(emessage, "set parameter: parameter %s->%s not found", folder, name);
     display_warning(emessage);
     return;
        }

     parfound = 0;
     if (folderfound == 1)
        {
     cpar = cfolder;
         while(cpar != NULL && parfound == 0)
          {
            if (strcmp(cpar->name, name) == 0)
              parfound = 1;
        else
              cpar = cpar->next;
          }
        }

     if (folderfound == 0)
    {
     sprintf(emessage, "set parameter: parameter %s->%s not found", folder, name);
     display_warning(emessage);
     return;
        }

    if (type == 0 && ((cpar->n1 != 0 || cpar->n2 != 0) && (ivalue < cpar->n1 || ivalue > cpar->n2)))
    {
     sprintf(emessage, "parameter should be within the [%d-%d] range", cpar->n1, cpar->n2);
     display_warning(emessage);
     return;
        }

    if (type == 0)
      *cpar->ipointer = ivalue;
    if (type == 1)
      *cpar->fpointer = fvalue;
    if (type == 2)
      strcpy(cpar->cpointer, cvalue);

    if (type == 4 && ivalue < cpar->n1)
      *(cpar->fpointer + ivalue) = fvalue;

    if (type == 7 && ivalue < cpar->n1)
      {
       f = *(cpar->ffpointer + ivalue);
       *f = fvalue;
      }
    cpar->modified = 1;

}

/*
 * This function modify the value of a previously defined (int) parameter
 */
void set_iparameter(char *name, char *folder, int ivalue)

{

set_parameter(name, folder, 0, ivalue, 0.0, NULL);

}

/*
 * This function modify the value of a previously defined (int) parameter
 */
void set_fparameter(char *name, char *folder, float fvalue)

{

set_parameter(name, folder, 1, 0, fvalue, NULL);

}

/*
 * This function modify the value of a previously defined (int) parameter
 */
void set_cparameter(char *name, char *folder, char *string)

{

set_parameter(name, folder, 2, 0, 0.0, string);

}

/*
 * This function modify the value of a previously defined (float verctor) parameter
 */
void set_fvparameter(char *name, char *folder, int index, float fvalue)

{

set_parameter(name, folder, 4, index, fvalue, NULL);

}

/*
 * This function modify the value of a previously defined vector of pointers to float parameters
 */
void set_fvpparameter(char *name, char *folder, int index, float fvalue)

{

set_parameter(name, folder, 7, index, fvalue, NULL);

}


/*
 * Search and return the pointer to a folder parameter (NULL if not found)
 */
struct Parameter *findfolder(char *folder)

{
    struct Parameter *cfolder;
    int folderfound;

    cfolder = parlist;
    folderfound = 0;
    while(cfolder != NULL && folderfound == 0)
     {
       if (strcmp(folder, cfolder->folder) == 0)
         folderfound = 1;
       else
         cfolder = cfolder->nextfolder;
     }

    return(cfolder);
}


/*
 * Search and return the pointer to a parameter (NULL if not found)
 */
struct Parameter *findparam(Parameter *folder, char *name)

{
    struct Parameter *cpar;
    int parfound;
    char emessage[64];

    parfound = 0;
    cpar = folder;
    while(cpar != NULL && parfound == 0)
        {
          if (strcmp(cpar->name, name) == 0)
              parfound = 1;
        else
              cpar = cpar->next;
        }
    return(cpar);
}


/*
 * parse a parameter description line 
 * if the line does not contains a folder description,
 *  assumes that the parameter belong to the last specified folder (ccfolder)
 * note that rend_param use a sister function with additional graphical features 
 */
void parse_p_line(char *line)
{

    char word[256];
    char message[256];
    Parameter *cpar;
    Parameter *pbuf;

    if (getword(word,line,' ') > 1)       
      {
     pbuf = NULL;
     pbuf = findfolder(word);
     if (pbuf != NULL)
           {
         ccfolder = pbuf;
           }
     else
           {
             if (ccfolder == NULL)
              {
        sprintf(message,"error on %s: folder unspecified", word);
                display_warning(message);
          }
             if (ccfolder != NULL)
              {
               cpar = NULL;
               cpar = findparam(ccfolder, word);
           if (cpar != NULL)
                 {
               if (cpar->type == 0 || cpar->type == 1 || cpar->type == 2)
                    {
                      if (getword(word,line,' ') > 0)
                        {
              if (cpar->type == 0)
                            set_iparameter(cpar->name, cpar->folder, atoi(word));
              if (cpar->type == 1)
                            set_fparameter(cpar->name, cpar->folder, atof(word));
              if (cpar->type == 2)
                            set_cparameter(cpar->name, cpar->folder, word);
                }
             }
           }
          else
                    {
             sprintf(message,"error on '%s': not defined in folder %s", word, ccfolder->folder);
                     display_warning(message);
            }
                }

              }
           }
}



/*
 * load & parse the file evorobot.cf
 */
void loadparameters()

{


     FILE    *fp;
     char       st[1024];
     char       message[128];
     int n;

     create_iparameter("winparametersx", "display",&winparametersx, 0, 0, "x coordinate of parameters window");
     create_iparameter("winparametersy", "display",&winparametersy, 0, 0, "y coordinate of parameters window");
     create_iparameter("winparametersdx", "display",&winparametersdx, 0, 0, "dx of parameters window");
     create_iparameter("winparametersdy", "display",&winparametersdy, 0, 0, "dy of parameters window");

     if ((fp=fopen("evorobot.cf", "r")) != NULL)
    {
      n = 0;
      while (fgets(st,1024,fp) != NULL && n < 1000)
      {
       parse_p_line(st);
       n++;
      }
        fclose(fp);
        sprintf(message,"loaded parameter file: evorobot.cf");
        display_stat_message(message);
        }
       else
       {
         sprintf(message,"Error: the file evorobot.cf could not be opended");
     display_error(message);
       }

}

/*
 * extract a word from a string (the original string is shortened)
 */
int
getword(char *word, char *line, char stop)
{
    int x;
    int y;
    int l;

    x=0;
    for(x=0,l=0;((line[x]) && (line[x] != stop && line[x] != '\n' && line[x] != '\r'));x++,l++)
        word[x] = line[x];

    word[x] = '\0';
    if(line[x]) ++x;
    y=0;

    while(line[y++] = line[x++]);

    return(l);
}

/*
 * extract a line from a string (the original string is shortened)
 * return the length of the string extracted
 */
int
getline(char *word, char *line)
{
    int x;
    int y;
    int l;

    x=0;
    for(x=0,l=0;((line[x]) && (line[x] != '\n'));x++,l++)
        word[x] = line[x];

    word[x] = '\0';
    if(line[x]) ++x;
    y=0;

    while(line[y++] = line[x++]);

    return(l);
}


/*
 * extract a word from a string (the original string is left intact)
 */
int
copyword(char *word, char *line, char stop)
{
    int x;
    int l;

    x=0;
    for(x=0,l=0;((line[x]) && (line[x] != stop && line[x] != '\n'));x++,l++)
        word[x] = line[x];

    word[x] = '\0';

    return(l);
}

/*
 * remove the first space characters from a string
 */
void
clear_from_space(char *stringa)
{

    while(*stringa == ' ')
       stringa++;

}

/*
 * cut a comment ("../...") from a st
 */
void
cat_comment_from_st(char *st)
{

    while(*st != '/' && *st != '\0')
       st++;
    *st = '\0';

}

