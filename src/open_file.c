#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "open_file.h"
#include "kml_struct.h"

extern void clean_stdin(void);

/*--------The function opens the kml file, copies it into an array of strings and parses it, forming the kml data structure.------*/
int openkml(char* f_name, folder_t* folder)
{
    char file_name[400];
    strcpy(file_name, f_name);
    strcat(file_name, ".kml");
    FILE* file_stream = fopen(file_name, "r");
    if(file_stream == NULL)
    {
        perror(file_name);
        printf("Or the file name is too long");
        clean_stdin();
        getchar();
        exit(1);
    }
    printf("\nAnalizing file structure..");
    char ch = 'a';
    u32 char_counter = 0;
    u32 char_max = 1;
    u32 str_max = 1;
    while((ch = fgetc(file_stream)) != EOF) // counting the number of characters and the number of lines in a file
    {
        char_max++;
        if(ch == '\n') str_max++;
    } 
    printf("..ok\n%d symbols, %d lines\n", char_max, str_max);

    fseek(file_stream, 0, SEEK_SET);
    printf("Creating and marking memory..");
    u32 mem_vol = str_max * sizeof(char*)+char_max*sizeof(char);
    char** file_array = (char**) malloc(mem_vol); // allocation of memory for an array of pointers to the beginning of each line and for the line themselves
    char* start = (char*)file_array + str_max*sizeof(char*);
    int* str_length = malloc(str_max*sizeof(int)); // the number of characters in each line
    file_array[0] = start;
    for(u32 i = 1; i <= str_max; ++i)  // a cycle of "marking up" lines of allocated memory
    {
        char_counter = 0;
        while((ch = fgetc(file_stream)) != '\n') 
        {
            if(ch == EOF) break;
            char_counter++;
        }
        char_counter++;
        str_length[i-1] = char_counter;
        file_array[i] = file_array[i-1] + char_counter;
    }

/* Copying data from a file stream to an array */
    printf("..ok\nCopying file data..");
    fseek(file_stream, 0, SEEK_SET);
    for(u32 i = 0; i < str_max; i++)
    {   
        int j = 0;
        while((ch = fgetc(file_stream)) != '\n') 
        {   
            if(ch == EOF) break;
            file_array[i][j] = ch;
            j++;
        }
        file_array[i][j] = '\0';
    }
    filedata_t f_data;    
    f_data.farray = file_array;
    f_data.string_length = str_length;
    f_data.strings = str_max;

    int folder_quantity = getfolders(folder, f_data);
    getkmldata(folder, folder_quantity, f_data);

    fclose(file_stream);
    free(f_data.farray);
    free(f_data.string_length);
    
    printf("..ok\nUsing memory - %d KB\n",((mem_vol + str_max*sizeof(int)) / 1024));

    return folder_quantity;
}

/*-------------------------------------The function opens a csv file and creates a kml data structure----------------------------------------- */

int opencsv(char* f_name, folder_t* folder)
{
    char file_name[400];
    strcpy(file_name, f_name);
    char path[400];
    strcpy(path, "./Output_CSV_files/");
    strcat(file_name, ".csv");
    strncat(path, file_name, 400);

    FILE* file_stream = fopen(path, "r");
    if(file_stream == NULL)
    {
        perror(file_name);
        printf("Or the file name is too long");
        clean_stdin();
        getchar();
        exit(1);
    }
    /*-Counting the number of cells in a line and the number of lines in a file (checking for the same number of cells in each line)-*/
    printf("\nAnalizing file structure...");
    char ch = 'a';
    u32 cell_quantity[2] = {0};
    u32 cell_counter = 0;
    u32 str_max = 0;
    while((ch = fgetc(file_stream)) != '\n')
    {
        if(ch == ';')cell_counter++;
    }
    cell_quantity[0] = cell_counter;
    cell_counter = 0;
    while((ch = fgetc(file_stream)) != EOF)
    {   
        if(ch == ';')cell_counter++;
        if(ch == '\n') 
        {
            cell_quantity[1] = cell_quantity[0];
            cell_quantity[0] = cell_counter;
            if(cell_quantity[0] != cell_quantity[1]) 
            {
                        printf("Error! Uncorrect structure of csv file!\n");
                        clean_stdin();
                        getchar();
                        exit(1);
            }
            cell_counter = 0;
            str_max++;
        }
    }
    int folder_quantity = (cell_quantity[0] + 1) / 8;
    if(((cell_quantity[0] + 1) % 8) != 0) 
    {
        printf("Error! Uncorrect structure of csv file!\n");
        clean_stdin();
        getchar();
        exit(1);
    }
    printf("..ok\n%d columns, %d lines\n", cell_quantity[1], str_max);
    fseek(file_stream, 0, SEEK_SET);
    for(u32 i = 0; i < (folder_quantity + 1); i++){
        folder[i].placemark_arr = malloc((str_max + 10) * sizeof(placemark_t));
        if(folder[i].placemark_arr == NULL){
            printf("Memory allocation error!");
            clean_stdin();
            getchar();
            exit(1);
        }
        folder[i].placemark_quantity = str_max;
    }

    char temp_string[200] = {0};
	
	while((ch = fgetc(file_stream)) != '\n'){ch = 0;}
		
    for(u32 i = 0; i < str_max; i++)
    {
        for(u32 j = 0; j < folder_quantity; j++)
        {
            /*----copying the name field---------*/
            u32 k = 0;
            while((ch = fgetc(file_stream)) != ';')
            {
                temp_string[k] = ch;
                k++;
            }
			temp_string[k] = '\0';
            strncpy(folder[j].placemark_arr[i].name, temp_string, (k + 1));
            k = 0;
            /*----copying the measnumber field---*/
            while((ch = fgetc(file_stream)) != ';')
            {
                temp_string[k] = ch;
                k++;
            }
			temp_string[k] = '\0';
            sscanf(temp_string, "%u", &folder[j].placemark_arr[i].measnumber);
            k = 0;
            /*----copying the timestamp field----*/
			while((ch = fgetc(file_stream)) != ';')
            {
                temp_string[k] = ch;
                k++;
            }
			temp_string[k] = '\0';
            sscanf(temp_string, "%u", &folder[j].placemark_arr[i].timestamp);
            k = 0;
            /*----copying the signallevelfield--*/
			while((ch = fgetc(file_stream)) != ';')
            {
                temp_string[k] = ch;
                k++;
            }
			temp_string[k] = '\0';
            sscanf(temp_string, "%lf", &folder[j].placemark_arr[i].signallevel);
            k = 0;
            /*----copying the latitude field---------*/
			while((ch = fgetc(file_stream)) != ';')
            {
                temp_string[k] = ch;
                k++;
            }
			temp_string[k] = '\0';
            sscanf(temp_string, "%lf", &folder[j].placemark_arr[i].latitude);
            k = 0;
            /*----copying the longitude field---------*/
			while((ch = fgetc(file_stream)) != ';')
            {
                temp_string[k] = ch;
                k++;
            }
			temp_string[k] = '\0';
            sscanf(temp_string, "%lf", &folder[j].placemark_arr[i].longitude);
            k = 0;
            /*----copying the altitude field---------*/
			while((ch = fgetc(file_stream)) != ';')
            {
                temp_string[k] = ch;
                k++;
            }
			temp_string[k] = '\0';
            sscanf(temp_string, "%lf", &folder[j].placemark_arr[i].altitude
			);
            k = 0;
            /*----copying the color field---------*/
            if(j == (folder_quantity - 1))
            {
                while((ch = fgetc(file_stream)) != '\n')
                {
                    temp_string[k] = ch;
                    k++;
                }
                temp_string[k] = '\0';
                strncpy(folder[j].placemark_arr[i].color, temp_string, (k + 1));
                k = 0;
            }
            else
            {
                while((ch = fgetc(file_stream)) != ';')
                {
                    temp_string[k] = ch;
                    k++;
                }
                temp_string[k] = '\0';
                strncpy(folder[j].placemark_arr[i].color, temp_string, (k + 1));
                k = 0;
            }
        }
        fgetc(file_stream); // reading \n at the end of a line
    }

/*--------------CORRECTING FOLDER NAMES, adding short names (standart-provider)-----------------*/
    for(u32 i = 0; i < folder_quantity; i++)
    {
        strncpy(folder[i].name, folder[i].placemark_arr[0].name, 300);
        strncpy(folder[i].short_name, folder[i].name, 300);

        char tmp_char[10] = {'\\','/',':','*','?','\"','<','>','|'}; //exclude forbidden characters for file names in Windows
        u32 p = 0;
        while(folder[i].short_name[p] != '\0')
        {
            for(u32 q = 0; q < 9; q++)
            {
                if(folder[i].short_name[p] == tmp_char[q]) 
                {
                    folder[i].short_name[p] = '_';
                }
            }
            p++;
        }
        

        
#if 0
        //--reading the provider's name (located between consecutive characters '[' and '[') 
        //(The name of the standard is also written there, in this case it will be included in the shortname twice.)
        char* tmp_ptr[3];
        tmp_ptr[0] = folder[i].name;
        char subname[100];
        while(1)
        {
            tmp_ptr[0] = strchr(tmp_ptr[0], '[');

            if(tmp_ptr[0] != NULL)
            {
                tmp_ptr[0]++;
                tmp_ptr[1] = strchr(tmp_ptr[0], '[');
                tmp_ptr[2] = strchr(tmp_ptr[0], ']');
                if(tmp_ptr[1] < tmp_ptr[2]) break;
            }  
            else 
            {
                tmp_ptr[0] = folder[i].name;
                tmp_ptr[1] = folder[i].name + strlen(folder[i].name);
                break;
            }
        }
        int max_k = tmp_ptr[1] - tmp_ptr[0];
        for(int k = 0; k < max_k; k++)
        {
            subname[k] = tmp_ptr[0][k];
        }
        subname[max_k] = '\0';
        //---------------------------------------------------------------------------------
        if(strstr(folder[i].name, "GPS") != NULL) 
        {
            strcpy(folder[i].short_name, "GPS");
        }
        if(strstr(folder[i].name, "GSM") != NULL) 
        {
            strcpy(folder[i].short_name, "GSM_");
            strcat(folder[i].short_name, subname);
        }
        if(strstr(folder[i].name, "LTE") != NULL)  
        {
            strcpy(folder[i].short_name, "LTE_");
            strcat(folder[i].short_name, subname);
        }
        if((strstr(folder[i].name, "3G") != NULL) || (strstr(folder[i].name, "UMTS") != NULL))  
        {
            strcpy(folder[i].short_name, "UMTS_");
            strcat(folder[i].short_name, subname);
        }
#endif
    }

    fclose(file_stream);

    if(folder_quantity == 1)
        {
            for(u32 i = 0; i < str_max; i++)
            {
                    folder[1].placemark_arr[i].measnumber = folder[0].placemark_arr[i].measnumber;
                    folder[1].placemark_arr[i].timestamp = folder[0].placemark_arr[i].timestamp;
                    folder[1].placemark_arr[i].signallevel = folder[0].placemark_arr[i].signallevel;
                    folder[1].placemark_arr[i].latitude = folder[0].placemark_arr[i].latitude;
                    folder[1].placemark_arr[i].longitude = folder[0].placemark_arr[i].longitude;
            }
        }
	return folder_quantity;
}