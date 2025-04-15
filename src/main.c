
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include "open_file.h"
#include "save_file.h"
#include "kml_struct.h"
#include "main.h"

static const char instruction[5000] = "\n"
"    ������� RomesKML_converter ������������� ��� ��������� �������� kml-������ �� �� R&S Romes\n"
"    ���������� ���������� ��������� ������� �������� ����� ����� �� ������������� �������.\n"
"\n"    
"    ��� ���������� ������ � �������� � �������� kml-����� ������ ���������� �������� �� ������� ��������� � ��������� �����,\n"
"    ������ ����������� ������ �� GPS: latitude, longitude (����������� ��� �������������� ��������� ��� �������� � �� Romes).\n"
"\n"    
"\n"                   
"                   ����������\n"
"\n"    
"    ����� ������� ������� ������� � ����� � ����������� ������ ��������� �������������� ��������:\n"
"\n"    
"    - ����� Output_CSV_files - ��� ���������� ����������� ��������� �������� ������ � ������� csv\n"
"    - ����� Output_KML_files - ��� ���������� ����������� ��������� �������� ������ � ������� kml\n"
"    - ���� settings.ini � ����������� �������, ���������� ������ �������� ���������� �� ���������, \n"
"      ������� ����� ������������ ��������, ��� ����������� ��������� ����� ����� ��������� �� �����.\n"
"    - ����������.\n"
"\n"   
"    � ������� ����������� ��� �������� ������: \n"
"    1. ����������� � ��������� ������ �� ��������� kml-�����\n"
"    2. ��������� ������ �� csv-����� �������������� ���������� �������� �� ��������� kml-�����. \n"
"\n"    
"    �������� kml-���� ���������� ��������� � ����� � ��������\n"
"\n"
"    � ������ ������ �������� ���������� ��������� ��������:\n"
"\n"    
"    - �������������� � ���������� � ����� csv-������� ���� ����������� ������ �� ��������� kml-�����;\n"
"    - ������������ � ���������� csv-������ ���������� ���������� ������� �� �������� ������ ��� (������� ��������, ����� � ������ \"Coverage_\" � ��������);\n"
"    - ���������� �������������� csv-������ ��� ��������� � Excel �������� �������������� ���������� ������ ��� ������������ ������ �������� \n"
"      (��������������� ������, ����� ���� ������������ ��� ��������� ���������� ����������, ���� � ������ \"_coverchart\" � ��������);\n"
"    - �������������� ������� � ������ kml �������� ������ �� ����� csv-������� � ��������� �� ��������� ����� ��������-�������� (�� ������ ������ ������ ����������,\n"
"      ��� ��������� ������� ��������� �� � �� GoogleEarth ��� ��������, � ����������� � �� GoogleEarth ����� ���������� �� ������� � ���� kml-����).\n"
"\n"      
"    �� ������ ������ ���������� ��� �� �� ����� ������ �� �� ��������� kml-�����, � �� �������������� �������������� ����� csv-�������,\n"
"    ��� ���� ����������� ������������� �������� ������ � ����� csv-������� ����� ���������� ����������.\n"
"\n"      
"    ��������� ����� settings.ini:\n"
"\n"    
"    filflag=1                 - ����������� ������ ������ ���� ����� \"1\", �������� ������������ ��� ���������� ������������� � �������� kml-����� ������ �� ������� (����������� ����������� �� Romes v.21.2)\n"
"    GSMlevel=-85.0            - ��������� ������� �������, dBm, ��� ����������� �������� ���������� GSM\n"
"    UMTSlevel=-90.0           - ��������� ������� �������, dBm, ��� ����������� �������� ���������� UMTS\n"
"    LTElevel=-83.5            - ��������� ������� �������, dBm, ��� ����������� �������� ���������� LTE\n"
"    defaultlvl=-85.0          - ��������� ������� �������, dBm, ��� ����������� �������� � ������, ���� ������� �� ������ ���������� �������� ����� (�������� �� ������ ������)\n"
"    calctype=TOT            - ��� ������������ ������ ��������: \"SEP\" - ��������� ������, ������ ������ ��������� ����� ������ ���������; \"TOT\" - ���� ����� ������� �������� ����������\n"
"    avertype=MED           - ��� ������� �������� ������ ��� ������������ ������ ��������: \"SMA\" - ������� �������, \"MED\" - ��������� �������\n"
"    averdepth=20              - ���������� ����� ��� ������� �������� (�� 2 �� 100);\n"
"    maxskipdistance=0.15      - �������� ��������������� ��������� ����������� ������� �� ����������, ��, ����������� ��� ������������ ����� ������� �������� (����� calctype=TOTAL)\n"
"    \n"
"    ��� ������ ���� ������������ ������ �������� - ����� ������� �������� (calctype=TOTAL) - � ����� Output_KML_files ����� ��������� ��� kml-�����,\n"
"    ���� � \"������\" (���� � ������ _rawcov.kml � ��������), ������ � ������������ �� ���.��������� (maxskipdistance) ������� (���� � ������ _avgcov.kml � ��������).\n"
"    � ������ ���������� ����� �������� ���������� ������� ������.\n"
"    � �� ������� ����� ��������� �������� ���������� �������� ���.��������� ���������� (maxskipdistance) ��� ���������� �������� ������\n"
"\n\n"
"�������, ��������, ����������� - p.a.samsonov@ya.ru";

void clean_stdin(void)
{
	int c;
	do {
	c = getchar();
	} while (c != '\n' && c != EOF);
}

void printkmldata(folder_t* folder, int folder_quantity, u32 start_plm, u32 end_plm)
{
    printf("\n");
    for(u32 i = 0; i < folder_quantity; i++)
    {
        printf("Folder %d, %s, %d �����\n", i, folder[i].short_name, folder[i].placemark_quantity);
        for(u32 j = start_plm; j < end_plm; j++)
        {
            printf("    � %d, ���: %s, �����: %d ��, �������: %f, ���: %f, ���: %f, ���: %f, ����: %s\n", folder[i].placemark_arr[j].measnumber, 
                folder[i].placemark_arr[j].name, folder[i].placemark_arr[j].timestamp, folder[i].placemark_arr[j].signallevel, 
                folder[i].placemark_arr[j].latitude, folder[i].placemark_arr[j].longitude, folder[i].placemark_arr[j].altitude,
                folder[i].placemark_arr[j].color);
        }
    }
}

void printcoveragetable(folder_t* folder, int folder_quantity, init_t settings)
{
    if(settings.covercalctype == separate)
    {
        for(u32 l = 0; l < folder_quantity; l++)
        {
            if(strstr(folder[l].name, "GPS Data") != NULL) continue;
            printf("\n%s\n", folder[l].short_name);
            for(int i = 0; i < folder[l].reg_quantity; i ++)
            {
                printf("������� � %d; �������� - %d; %f ��; ������ %f�.�. %f�.�.; ����� %f�.�. %f�.�.\n", i, folder[l].covreg[i].coverfl, folder[l].covreg[i].distance, folder[l].covreg[i].startlat, 
                    folder[l].covreg[i].startlong, folder[l].covreg[i].endlat, folder[l].covreg[i].endlong);
            }
        }
    }
    if(settings.covercalctype == total)
    {
        printf("\n%s\n", "TOTAL COVERAGE");
        for(int i = 0; i < folder[1].reg_quantity; i ++)
        {
            printf("������� � %d; �������� - %d, %f ��; ������ %f�.�. %f�.�.; ����� %f�.�. %f�.�.\n", i, folder[1].covreg[i].coverfl, folder[1].covreg[i].distance, folder[1].covreg[i].startlat, 
                folder[1].covreg[i].startlong, folder[1].covreg[i].endlat, folder[1].covreg[i].endlong);
        }
    }
}

init_t initialization(void)
{
    static const char init_string[300] = "[OPTIONS]\nfilflag=1\nGSMlevel=-85.0\nUMTSlevel=-90.0\nLTElevel=-83.5\ndefaultlvl=-85.0\n"
                               "calctype=TOT\navertype=MED\naverdepth=20\nmaxskipdistance=0.8\n";
    init_t settings;
    char str_1[100];
    char str_2[100];
    system("mkdir Output_CSV_files");
    system("mkdir Output_KML_files");
    system("cls");

    FILE* instr = fopen("���������.txt", "w");
    if(instr == NULL)
    {
        printf("������ �������� ����� settings.ini\n");
            perror("���������.txt");
            exit(1);
    }
    fprintf(instr, instruction);
    fclose(instr);

    FILE* set_file = fopen("settings.ini", "r");
    if(set_file == NULL)
    {
        set_file = fopen("settings.ini", "w");
        if(set_file == NULL) {
            printf("������ �������� ����� settings.ini\n");
            perror("settings.ini");
            exit(1);
        }
        fprintf(set_file, init_string);
        fclose(set_file);
    } 
    set_file = fopen("settings.ini", "r");
    if(set_file == NULL)
    {
        printf("������ �������� ����� settings.ini\n");
            perror("settings.ini");
            exit(1);
    }

    int test = fscanf(set_file,"[OPTIONS]\nfilflag=%d\nGSMlevel=%lf\nUMTSlevel=%lf\nLTElevel=%lf\ndefaultlvl=%lf\ncalctype=%s\navertype=%s\naverdepth=%d\nmaxskipdistance=%lf\n", 
        &settings.fillinflag, &settings.GSMcoveragelvl, &settings.UMTScoveragelvl, &settings.LTEcoveragelvl, &settings.defaultcovlvl, str_1, str_2, &settings.avgdepth, &settings.maxskipdist);
    if(test != 9)
    {
        printf("������ ������ ����� �������� settings.ini.\n��������� ������ � ����� ��� ������� ��� � ������������� ���������!\n");
        getchar();
        exit(1);
    }   
    if((strstr(str_1, "TOT") != NULL) || (strstr(str_1, "TOTAL") != NULL) ) {
        settings.covercalctype = total;
    }
    else {
        if((strstr(str_1, "SEP") != NULL) || (strstr(str_1, "SEPARATE") != NULL)) {
            settings.covercalctype = separate;}
        else {
            printf("������ ������ settings.ini,\n ������ ��� �������� �� ��������� - �����\n");
            settings.covercalctype = total;
        }
    }
    if(strstr(str_2, "SMA") != NULL) {
        settings.avgtype = sma;}
    else {
        if((strstr(str_2, "MED") != NULL) || (strstr(str_2, "MEDIAN") != NULL)) {
            settings.avgtype = median;}
        else {
            printf("������ ������ settings.ini,\n ������ �������� ������ �� ��������� - �������\n");
            settings.avgtype = median;
        }
    }
    fclose(set_file);
    if(settings.avgdepth > 100) {settings.avgdepth = 100;}
    if(settings.avgdepth < 2) {settings.avgdepth = 2;}
    if((settings.fillinflag < 0) || (settings.fillinflag > 1)) {settings.fillinflag = 1;}
    return settings;
}

/*------------------------------------------MAIN-------------------------------------------------*/
int main(void)
{   
    char *locale = setlocale(LC_CTYPE, ""); //
    init_t settings = initialization();
    int choice = 0;
    char file_name[200];
    folder_t folder[20] = {0};
	int folder_quantity = 0;

    
#if 1
    printf("��������� ������ ���������:\n");
    if(settings.fillinflag) printf("  �������������� ������ ����� - ��������\n"); else printf("  �������������� ������ ����� - ���������\n");
    printf("\n  ��������� ������� �������\n  GSM - %.2f dBm\n  UMTS - %.2f dBm\n  LTE - %.2f dBm\n  default - %.2f dBm\n", 
        settings.GSMcoveragelvl,settings.UMTScoveragelvl, settings.LTEcoveragelvl, settings.defaultcovlvl);
    if(settings.covercalctype == total) printf("\n  ���������� ���������� ������� ��������:\n  - ���� ����� ������� ���������� ������� � ������ ����������� ����� ��� ���� ���������� ���� ����������\n");
    else printf("\n  ���������� ���������� ������� ��������:\n  - ��������� ������, ������ �������� ������� �� ��� ����� ��� ������ ��������� ������ ���������\n");
    if(settings.avgtype == median) printf("\n  ������ ���������� �������� ������ ������� - �������\n");
    else printf("\n  ������ ���������� �������� ������ ������� - ���������� �������\n");
    printf("  ���������� ����� ��� �������� �������� ������ - %d\n", settings.avgdepth);
    printf("  �������� ��������������� ��������� ����������� ������� �� ���������� - %.3f ��\n\n\n", settings.maxskipdist);
#endif 

#if 1
    while(1)
    {
        printf("�������� ��������:\n1 - ������� kml-����\n2 - ������� csv-����\n3 - �����\n������� ����� ���������� ��������:\n");
        scanf("%d", &choice);

        if( choice == 1) 
        {
            clean_stdin();
            printf("������� ��� �����:\n");
            scanf("%s", file_name);
            folder_quantity = openkml(file_name, folder);
            savecsv(file_name, folder, folder_quantity, settings.fillinflag);
            foldermemfree(folder, folder_quantity);
            folder_quantity = opencsv(file_name, folder);
            printkmldata(folder, folder_quantity, 0, 0);
            getaverlvl(folder,folder_quantity, settings.avgdepth, settings.avgtype);
            createcovtab(folder, folder_quantity, settings);
            savekml(file_name,folder, folder_quantity);
            savecsvcovchart(file_name, folder, folder_quantity);
            savecsvcovtab(file_name, folder, folder_quantity, settings);
            if(settings.covercalctype == total) savekml_test(file_name, folder, folder_quantity);
            printcoveragetable(folder, folder_quantity, settings);
            break;
        } 

        if( choice == 2) 
        {
            clean_stdin();
            printf("������� ��� �����:\n");
            scanf("%s", file_name);
            folder_quantity = opencsv(file_name, folder);
            printkmldata(folder, folder_quantity, 0, 0);
            savekml(file_name,folder, folder_quantity);
            getaverlvl(folder,folder_quantity, settings.avgdepth, settings.avgtype);
            createcovtab(folder, folder_quantity, settings);
            savecsvcovchart(file_name, folder, folder_quantity);
            savecsvcovtab(file_name, folder, folder_quantity, settings);
            if(settings.covercalctype == total) savekml_test(file_name, folder, folder_quantity);
            printcoveragetable(folder, folder_quantity, settings);
            break;
        } 
        if( choice == 3) 
        {
            break;
        } 
        printf("���-�� ����� �� ���, ���������� �����\n");
    }
#endif    

    clean_stdin();
    printf("\n������� Enter ��� ������.\n");
    getchar();
    return 0;
}