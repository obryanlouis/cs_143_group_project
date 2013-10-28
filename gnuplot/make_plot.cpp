#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

/*
 *  An example of gnuplot plotting.
 */
int main(int argc, const char* argv[]) {
    FILE * fp = popen("gnuplot -persist", "w");    
    FILE * pFile;
    char buffer [100];

    pFile = fopen("gnuplot/plot_test.plt", "r");
    if (pFile == NULL)  
        perror ("Error opening file");
    else
    {
        while (!feof(pFile))
            {
                if (fgets (buffer, 100, pFile) == NULL) 
                    break;
                fputs (buffer, fp);
            }
            fclose (pFile);
    }
    pclose(fp);
    return 0;
}
