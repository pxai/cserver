#include <stdio.h>
#define MAX 512


int readLine(FILE * fich, char * line, int len);
int processLine(char * line, int len, char * name, char * value);

// Lerro bat irakurtzeko funtzioa
int readLine(FILE * fich, char * line, int len)
{
       char c;
       int i = 0;

       while ( (c = fgetc(fich)) != EOF  && (i++)<len && c != '\n')
       {
               (*line++) = c;
       }

       (*line) = '\0';

       return (feof(fich))?EOF:1;
}


int processLine(char * line, int len, char * name, char * value)
{
       int beginning = 1;
       int state = 1;
       int i = -1;

       name[0] = '\0';
       value[0] = '\0';

       while((*line) != '\0' && (i++)<len)
       {
               if (beginning && ((*line)==' ' || (*line) == '\t'))
               {
                       (*line++);
                       continue;
               }
               if (beginning && (*line) == '#') break;

               if ((*line) != '=')
               {
                       beginning = 0;
                       if (state) (*name++) = (*line);
                       else (*value++) = (*line);
               }
               else
               {
                       state = 0;
                       (*name) = '\0';
               }
               (*line++);
       }

       (*value) = '\0';

       return !beginning;
}