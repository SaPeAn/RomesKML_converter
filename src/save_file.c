#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "kml_struct.h"
#include "save_file.h"


static char kml_header[300] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://www.opengis.net/kml/2.2\" "
"xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">\n";

static char signal_folder_header[200] = "<Folder>\n	<name>R&amp;S %s\n	</name>\n	<open>0\n	</open>\n";

static char placemark_template[2500] =
"   <Placemark>\n"
"		<name>R&amp;S %s Measurement #%d\n"
"		</name>\n"
"		<snippet>Timestamp: %d &lt;br/&gt; %.2f0000 dBm\n"
"		</snippet>\n"
"		<visibility>0\n"
"		</visibility>\n"
"		<Point>\n"
"			<altitudeMode>relativeToGround\n"
"			</altitudeMode>\n"
"			<extrude>1\n"
"			</extrude>\n"
"			<coordinates>%f,%f,%f\n"
"			</coordinates></Point>\n"
"		<styleUrl>#StdPlacemarkStyle\n"
"		</styleUrl>\n"
"		<Style ID=\"StdPlacemarkStyle\">\n"
"			<LineStyle>\n"
"				<color>%s\n"
"				</color>\n"
"				<width>4\n"
"				</width></LineStyle>\n"
"			<IconStyle>\n"
"				<Icon>\n"
"					<href>http://maps.google.com/mapfiles/kml/pal2/icon26.png\n"
"					</href></Icon>\n"
"				<color>%s\n"
"				</color>\n"
"				<colorMode>normal\n"
"				</colorMode>\n"
"				<scale>0.35\n"
"				</scale>\n"
"			</IconStyle>\n"
"			<LabelStyle>\n"
"				<scale>0\n"
"				</scale>\n"
"				<color>%s\n"
"				</color></LabelStyle>\n"
"		</Style>\n"
"		<description><![CDATA[<i>Timestamp:</i> %d <table border=\"0\"><tr><td nowrap><i><u> R&S %s: </u></i></td><td nowrap> %.2f0000 dBm </td></tr>"
"<tr><td nowrap><i> R&S GPS Device[1] - Latitude: </i></td><td nowrap> %f ° </td></tr>"
"<tr><td nowrap><i> R&S GPS Device[1] - Longitude: </i></td><td nowrap> %f ° </td></tr> </table> <br/>]]>\n"
"		</description>\n"
"   </Placemark>\n";

extern void clean_stdin(void);

void savecsv(char* f_name, folder_t* folder, int folder_quantity, int fillinflag)
{    
    printf("\nSaving csv-file..");
    char file_name[400];
    strcpy(file_name, f_name);
    char path[400];
    strcpy(path, "./Output_CSV_files/");
    strcat(file_name, ".csv");
    strncat(path, file_name, 400);
    FILE* csv_file = fopen(path, "w");
    if(csv_file == NULL)
    {
        perror(file_name);
        printf("Or the file name is too long");
        clean_stdin();
        getchar();
        exit(1);
    }
    for(int i = 0; i < folder_quantity; i++)
    { 
        fprintf(csv_file, "name;measnumber;timestamp;signallevel;latitude;longitude;altitude;color"); // name[120], measnumber, timestamp, signallevel, latitude, longitude, altitude, color[20]
        if(i < (folder_quantity - 1)) fprintf(csv_file, ";");
    }
    fprintf(csv_file, "\n");

    int k[20] = {0};

    for(int j = 0; j < (folder[0].placemark_quantity); j++)
    {
        for(int i = 0; i < folder_quantity; i++)
        { 
            if(i == 0)
            {
                k[i] = j;
                fprintf(csv_file, "%s;%d;%d;%f;%f;%f;%f;%s", folder[i].placemark_arr[k[i]].name, folder[i].placemark_arr[k[i]].measnumber, folder[i].placemark_arr[k[i]].timestamp,
                    folder[i].placemark_arr[k[i]].signallevel, folder[i].placemark_arr[k[i]].latitude, folder[i].placemark_arr[k[i]].longitude, folder[i].placemark_arr[k[i]].altitude,
                    folder[i].placemark_arr[k[i]].color); // name[120], measnumber, timestamp, signallevel, latitude, longitude, altitude, color[20]
                if(i < (folder_quantity - 1)) fprintf(csv_file, ";");
            }
            else
            {
                if(folder[0].placemark_arr[k[0]].timestamp == folder[i].placemark_arr[k[i]].timestamp)
                {
                    if(fillinflag)
                    {
                        fprintf(csv_file, "%s;%d;%d;%f;%f;%f;%f;%s", folder[i].placemark_arr[k[i]].name, folder[0].placemark_arr[k[0]].measnumber, folder[i].placemark_arr[k[i]].timestamp,
                            folder[i].placemark_arr[k[i]].signallevel, folder[i].placemark_arr[k[i]].latitude, folder[i].placemark_arr[k[i]].longitude, folder[i].placemark_arr[k[i]].altitude,
                            folder[i].placemark_arr[k[i]].color); // name[120], measnumber, timestamp, signallevel, latitude, longitude, altitude, color[20]
                        if(i < (folder_quantity - 1)) fprintf(csv_file, ";");
                    }
                    else
                    {
                        fprintf(csv_file, "%s;%d;%d;%f;%f;%f;%f;%s", folder[i].placemark_arr[k[i]].name, folder[i].placemark_arr[k[i]].measnumber, folder[i].placemark_arr[k[i]].timestamp,
                            folder[i].placemark_arr[k[i]].signallevel, folder[i].placemark_arr[k[i]].latitude, folder[i].placemark_arr[k[i]].longitude, folder[i].placemark_arr[k[i]].altitude,
                            folder[i].placemark_arr[k[i]].color); // name[120], measnumber, timestamp, signallevel, latitude, longitude, altitude, color[20]
                        if(i < (folder_quantity - 1)) fprintf(csv_file, ";");
                    }
                    k[i]++;
                }
                else 
                {
                    if(fillinflag)
                    {
                        fprintf(csv_file, "%s;%d;%d;%f;%f;%f;%f;%s", folder[i].placemark_arr[k[i]].name, folder[0].placemark_arr[k[0]].measnumber, folder[0].placemark_arr[k[0]].timestamp,
                        folder[i].placemark_arr[k[i]].signallevel, folder[0].placemark_arr[k[0]].latitude, folder[0].placemark_arr[k[0]].longitude, folder[i].placemark_arr[k[i]].altitude,
                        folder[i].placemark_arr[k[i]].color); // name[120], measnumber, timestamp, signallevel, latitude, longitude, altitude, color[20]
                        if(i < (folder_quantity - 1)) fprintf(csv_file, ";");
                    }
                    else
                    {
                        fprintf(csv_file, ";;;;;;;");
                        if(i < (folder_quantity - 1)) fprintf(csv_file, ";");
                    }
                }
            }
        }
        fprintf(csv_file, "\n");
    }
    fclose(csv_file);
    printf("..ok\n");
}

void savekml(char* f_name, folder_t* folder, int folder_quantity)
{
    printf("\nSaving kml-file(s)\n");
    char file_name[400];
    char subname[400];
    char path[400];

    for(u32 i = 0; i < folder_quantity; i++)
    {   
        if(strstr(folder[i].name, "GPS Data") != NULL) continue;

        strcpy(path, "./Output_KML_files/");
        strncpy(file_name, f_name, 400);
        strncat(path, file_name, 400);
        strcat(path, "_");
        strcpy(subname, folder[i].short_name);
        strcat(subname, ".kml");
        strncat(path, subname, 400);

        FILE* kml_file = fopen(path, "w");
        if(kml_file == NULL)
        {
            perror(path);
            printf("Or the file name is too long");
            clean_stdin();
            getchar();
            exit(1);
        }

        fprintf(kml_file, kml_header);
        fprintf(kml_file, signal_folder_header, folder[i].name);

        for(u32 j = 0; j < folder[i].placemark_quantity; j++)
        {
            fprintf(kml_file, placemark_template, 
                folder[i].placemark_arr[j].name,        folder[i].placemark_arr[j].measnumber, folder[i].placemark_arr[j].timestamp, 
                folder[i].placemark_arr[j].signallevel, folder[i].placemark_arr[j].longitude,  folder[i].placemark_arr[j].latitude, 
                folder[i].placemark_arr[j].altitude,    folder[i].placemark_arr[j].color,      folder[i].placemark_arr[j].color, 
                folder[i].placemark_arr[j].color,       folder[i].placemark_arr[j].timestamp,  folder[i].placemark_arr[j].name, 
                folder[i].placemark_arr[j].signallevel, folder[i].placemark_arr[j].latitude,   folder[i].placemark_arr[j].longitude);
        }
        fprintf(kml_file, "</Folder>\n");
        fprintf(kml_file, "</kml>");
        fclose(kml_file);
        printf("%s file saved.\n", path);
    }
}

void savekml_test(char* f_name, folder_t* folder, int folder_quantity)
{
    printf("\nSaving raw coverage kml-file\n");
    char file_name[400];
    char subname[400];
    char path[400];

    strcpy(path, "./Output_KML_files/");
    strncpy(file_name, f_name, 400);
    strncat(path, file_name, 400);
    strcat(path, "_rawcov.kml");

    FILE* kml_file = fopen(path, "w");
    if(kml_file == NULL)
    {
        perror(path);
        printf("Or the file name is too long");
        clean_stdin();
        getchar();
        exit(1);
    }

    fprintf(kml_file, kml_header);
    fprintf(kml_file, signal_folder_header, strcat(file_name, "_rawcov"));

    for(u32 j = 0; j < folder[0].placemark_quantity; j++)
    {
        fprintf(kml_file, placemark_template, 
            file_name,        folder[0].placemark_arr[j].measnumber, folder[0].placemark_arr[j].timestamp, 
            0.0, folder[0].placemark_arr[j].longitude,  folder[0].placemark_arr[j].latitude, 
            0.0,    ((folder[0].placemark_arr[j].coverfl) ? "AB00E660" : "AB000000"),      ((folder[0].placemark_arr[j].coverfl) ? "AB00E660" : "AB000000"), 
            ((folder[0].placemark_arr[j].coverfl) ? "AB00E660" : "AB000000"),       folder[0].placemark_arr[j].timestamp,  "RAW coverage data", 
            0.0, folder[0].placemark_arr[j].latitude,   folder[0].placemark_arr[j].longitude);
    }
    fprintf(kml_file, "</Folder>\n");
    fprintf(kml_file, "</kml>");
    fclose(kml_file);
    printf("%s file saved.\n", path);


    printf("\nSaving averaging coverage kml-file\n");

    strcpy(path, "./Output_KML_files/");
    strncpy(file_name, f_name, 400);
    strncat(path, file_name, 400);
    strcat(path, "_avgcov.kml");

    kml_file = fopen(path, "w");
    if(kml_file == NULL)
    {
        perror(path);
        printf("Or the file name is too long");
        clean_stdin();
        getchar();
        exit(1);
    }

    fprintf(kml_file, kml_header);
    fprintf(kml_file, signal_folder_header, strcat(file_name, "_avgcov"));

    for(u32 j = 0; j < folder[1].placemark_quantity; j++)
    {
        fprintf(kml_file, placemark_template, 
            file_name,        folder[1].placemark_arr[j].measnumber, folder[1].placemark_arr[j].timestamp, 
            0.0, folder[1].placemark_arr[j].longitude,  folder[1].placemark_arr[j].latitude, 
            100.0,    ((folder[1].placemark_arr[j].coverfl) ? "AB00E660" : "AB000000"),      ((folder[1].placemark_arr[j].coverfl) ? "AB00E660" : "AB000000"), 
            ((folder[1].placemark_arr[j].coverfl) ? "AB00E660" : "AB000000"),       folder[1].placemark_arr[j].timestamp,  "RAW coverage data", 
            0.0, folder[1].placemark_arr[j].latitude,   folder[1].placemark_arr[j].longitude);
    }
    fprintf(kml_file, "</Folder>\n");
    fprintf(kml_file, "</kml>");
    fclose(kml_file);
    printf("%s file saved.\n", path);
}


void savecsvcovchart(char *filename, folder_t *folder, int folder_quantity)
{
    char path[400];
    sprintf(path, "./Output_CSV_files/%s_coverchart.csv", filename);
    FILE* csv_file = fopen(path, "w");
    if(csv_file == NULL)
    {
        perror(path);
        printf("Or the file name is too long");
        clean_stdin();
        getchar();
        exit(1);
    }

    for(u32 i = 0; i < folder_quantity; i ++)
    {
        if(strstr(folder[i].name, "GPS") != NULL) continue;
        fprintf(csv_file, "%s;;;;;", folder[i].short_name);
    }
    fprintf(csv_file, "\n");

    for(int i = 0; i < folder[0].placemark_quantity; i ++)
    {        
        for(u32 l = 0; l < folder_quantity; l++)
        {
            if(strstr(folder[l].name, "GPS Data") != NULL) continue;
            fprintf(csv_file, ";%d;%f;%f;%f;", folder[l].placemark_arr[i].measnumber, folder[l].threshold, folder[l].placemark_arr[i].signallevel, folder[l].placemark_arr[i].averagelevel);
        }
        fprintf(csv_file, "\n");
    }
    fclose(csv_file);
}

void savecsvcovtab(char *filename, folder_t *folder, int folder_quantity, init_t settings)
{
    char path[400];
    if(settings.covercalctype == separate) 
    {
        for(u32 i = 0; i < folder_quantity; i++)
        {
            if(strstr(folder[i].name, "GPS Data") != NULL) continue;
            strcpy(path, "./Output_CSV_files/Coverage_");
            strcat(path, filename);
            strcat(path, "_");
            strcat(path, folder[i].short_name);
            strcat(path, ".csv");
            FILE* csvcovtab = fopen(path, "w");
            if(csvcovtab == NULL)
            {
                perror(path);
                printf("Or the file name is too long");
                clean_stdin();
                getchar();
                exit(1);
            }
            fprintf(csvcovtab, "distance;start latitude;start longitude;end latitude;end longitude;\n");
            for(u32 j = 0; j < folder[i].reg_quantity; j++)
            {
                if(!folder[i].covreg[j].coverfl)
                {
                    fprintf(csvcovtab, "%f;%f;%f;%f;%f\n", folder[i].covreg[j].distance, folder[i].covreg[j].startlat, 
                    folder[i].covreg[j].startlong, folder[i].covreg[j].endlat, folder[i].covreg[j].endlong);
                }
            }
            fprintf(csvcovtab, "\ntotal distance;%f;\ncovered distance;%f;\nuncovered distance;%f;\n", folder[i].totdist, folder[i].covtotdist, folder[i].uncovtotdist);
            fclose(csvcovtab);
        }
    }
    
    if(settings.covercalctype == total) 
    {
        strcpy(path, "./Output_CSV_files/Coverage_");
        strcat(path, filename);
        strcat(path, ".csv");
        FILE* csvcovtab = fopen(path, "w");
        if(csvcovtab == NULL)
        {
            perror(path);
            printf("Or the file name is too long");
            clean_stdin();
            getchar();
            exit(1);
        }
        fprintf(csvcovtab, "distance;start latitude;start longitude;end latitude;end longitude;\n");
        for(u32 j = 0; j < folder[1].reg_quantity; j++)
        {
            if(!folder[1].covreg[j].coverfl)
            {
                fprintf(csvcovtab, "%f;%f;%f;%f;%f\n", folder[1].covreg[j].distance, folder[1].covreg[j].startlat, 
                    folder[1].covreg[j].startlong, folder[1].covreg[j].endlat, folder[1].covreg[j].endlong);
            }
        }
        fprintf(csvcovtab, "\ntotal distance;%f;\ncovered distance;%f;\nuncovered distance;%f;\n", folder[0].totdist, folder[0].covtotdist, folder[0].uncovtotdist);
        fclose(csvcovtab);
#if 0       
        strcpy(path, "./Output_CSV_files/Coverage_NOdistaver_");
        strcat(path, filename);
        strcat(path, ".csv");
        csvcovtab = fopen(path, "w");
        if(csvcovtab == NULL)
        {
            perror(path);
            printf("Or the file name is too long");
            clean_stdin();
            getchar();
            exit(1);
        }
        fprintf(csvcovtab, "distance;start latitude;start longitude;end latitude;end longitude;\n");
        for(u32 j = 0; j < folder[0].reg_quantity; j++)
        {
            fprintf(csvcovtab, "%f;%f;%f;%f;%f\n", folder[0].covreg[j].distance, folder[0].covreg[j].startlat, 
                folder[0].covreg[j].startlong, folder[0].covreg[j].endlat, folder[0].covreg[j].endlong);
        }
        fprintf(csvcovtab, "\ntotal distance;%f;\ncovered distance;%f;\nuncovered distance;%f;\n", folder[1].totdist, folder[1].covtotdist, folder[1].uncovtotdist);
        fclose(csvcovtab);
#endif
    }
}