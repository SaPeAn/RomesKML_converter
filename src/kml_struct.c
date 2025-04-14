/*
 *               The functions of forming and processing the structure of kml data obtained from a kml file.
 * 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "kml_struct.h"

#define    FOLDER         0
#define    ENDFOLDER      1
#define    PLACEMARK      2
#define    ENDPLACEMARK   3

double getmedian(double *arr, int n)
{
    int i,j;
    double z;
    for (i=0; i<n-1;i++)
        for (j=i+1;j<n;j++)
            if (arr[i] > arr[j])
            {
                z=arr[i];
                arr[i]=arr[j];
                arr[j]=z;
            }
    if ((n % 2) == 0)
        return 0.5*(arr[n/2]+arr[n/2-1]);
    else
        return arr[n/2];
}

double distearth(double lat1d, double lon1d, double lat2d, double lon2d) 
{
    double lat1r, lon1r, lat2r, lon2r, u, v;
    lat1r = (lat1d * 3.14159) / 180;
    lon1r = (lon1d * 3.14159) / 180;
    lat2r = (lat2d * 3.14159) / 180;
    lon2r = (lon2d * 3.14159) / 180;
    u = sin((lat2r - lat1r)/2);
    v = sin((lon2r - lon1r)/2);
    return (2.0 * 6371.032 * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v)));
}

/*-----------------------The function generates an array of folders containing placemarks---------------------*/
int getfolders(folder_t * folder, filedata_t fdata)
{
    int fol_index = 0;
    int prev_tag = FOLDER;
    for(u32 i = 0; i < fdata.strings; i++)
    {
        if(strstr(fdata.farray[i], "<Folder>") != NULL) 
        {
            if(prev_tag == FOLDER)
            {
                folder[fol_index].startstr = i;
                prev_tag = FOLDER;
            }
            if(prev_tag == ENDFOLDER)
            {
                folder[fol_index].startstr = i;
                prev_tag = FOLDER;
            }
        }
        if(strstr(fdata.farray[i], "</Folder>") != NULL)
        {
            if(prev_tag == PLACEMARK)
            {
                folder[fol_index].endstr = i;
                fol_index++;
                prev_tag = ENDFOLDER;
            }
        }

        if(strstr(fdata.farray[i], "<Placemark>") != NULL)
        {
            if(prev_tag == FOLDER)
            {
                folder[fol_index].firstplacemark_startstr = i;
                prev_tag = PLACEMARK;
            }
            if(prev_tag == PLACEMARK)
            {
                folder[fol_index].placemark_quantity++;
                prev_tag = PLACEMARK;
            }
        }
    }
/*---------Getting the start and end line numbers for each placemark in each folder------------------*/
    for(u32 i = 0; i < fol_index; i++)
    {
        folder[i].placemark_startstr = malloc(sizeof(u32)*folder[i].placemark_quantity);
        folder[i].placemark_endstr = malloc(sizeof(u32)*folder[i].placemark_quantity);
        int plm_index = 0;
        for(u32 j = folder[i].startstr; j < folder[i].endstr; j++)
        {
            if(strstr(fdata.farray[j], "<Placemark>"))
            {
                folder[i].placemark_startstr[plm_index] = j;      
            }
            if(strstr(fdata.farray[j], "</Placemark>"))
            {
                folder[i].placemark_endstr[plm_index] = j; 
                plm_index++;     
            }
        }
    }


/*-------------------------------------------Reading folder names------------------------------------------*/
    char* temp_ptr;
    posptr_t startposition;
    posptr_t endposition;
    for(u32 i = 0; i < fol_index; i++)
    {
        for(u32 j = folder[i].startstr; j < folder[i].firstplacemark_startstr; j++)
        {
            if((temp_ptr = strstr(fdata.farray[j], "<name>")) != NULL)
            {
                startposition.string = j;
                startposition.pos_ptr = temp_ptr;
            }
            if((temp_ptr = strstr(fdata.farray[j], "</name>")) != NULL)
            {
                endposition.string = j;
                endposition.pos_ptr = temp_ptr;
            }
        }
        strncpy(folder[i].name, (startposition.pos_ptr + 6), 100);
        if(startposition.string == endposition.string)
        {
            folder[i].name[endposition.pos_ptr-startposition.pos_ptr - 6] = '\0';
        }
    }
    return fol_index;    // returns the number of folders found in the file (containing placemarks)
}



/*--------------------The function fills in the structure of a single placemark from the file data---------------------------*/

placemark_t getplacemarkdata(u32 placemark_startstring, u32 placemark_endstring, filedata_t fdata)
{    
    placemark_t placemark;
    char temp_arr[150];
    char* temp_ptr;
    posptr_t startposition;
    posptr_t endposition;
    int correct_result_fl = 0;

    /*------------------Parsing the name field-------------------------------*/
    for(u32 i = placemark_startstring; i < placemark_endstring; i++)
    {
        if((temp_ptr = strstr(fdata.farray[i], "<name>")) != NULL)
        {
            startposition.string = i;
            startposition.pos_ptr = temp_ptr;
            correct_result_fl = 1;
        }
        if((temp_ptr = strstr(fdata.farray[i], "</name>")) != NULL)
        {
            endposition.string = i;
            endposition.pos_ptr = temp_ptr;
        }
    }
    if(correct_result_fl)
    {
        strncpy(temp_arr, (startposition.pos_ptr + 6), 150);
        if(startposition.string == endposition.string)
        {
            temp_arr[endposition.pos_ptr-startposition.pos_ptr - 6] = '\0';
        }
        temp_ptr = strstr(temp_arr, " Measurement #");       // getting measurement number
        placemark.measnumber = atoi(temp_ptr + 14);
        *(temp_ptr) = '\0';
        strcpy(placemark.name,(temp_arr + 8)); // "R&amp;S " skip this crap ("R&S" in KML)
    }
    correct_result_fl = 0;

    /*----------------------Parsing the snippet field---------------------*/

    for(u32 i = placemark_startstring; i < placemark_endstring; i++)
    {
        if((temp_ptr = strstr(fdata.farray[i], "<snippet>")) != NULL)
        {
            startposition.string = i;
            startposition.pos_ptr = temp_ptr;
            correct_result_fl = 1;
        }
        if((temp_ptr = strstr(fdata.farray[i], "</snippet>")) != NULL)
        {
            endposition.string = i;
            endposition.pos_ptr = temp_ptr;
        }
    }
    int coordfl = 0;    // if "Snippet" with a capital letter
    //The KML specification and associated XML Schema clearly show the element "Snippet" form deprecated and "snippet" as the approved form to use.
    //KML Reference errata: https://kml4earth.appspot.com/kmlErrata.html#snippet)
    //In Romes exportKML "Snippet" form used in the folder "GPS Data"
    for(u32 i = placemark_startstring; i < placemark_endstring; i++)
    {
        if((temp_ptr = strstr(fdata.farray[i], "<Snippet>")) != NULL)
        {
            startposition.string = i;
            startposition.pos_ptr = temp_ptr;
            correct_result_fl = 1;
            coordfl = 1;
        }
        if((temp_ptr = strstr(fdata.farray[i], "</Snippet>")) != NULL)
        {
            endposition.string = i;
            endposition.pos_ptr = temp_ptr;
        }
    }
    if(correct_result_fl && !coordfl)
    {
        strncpy(temp_arr, (startposition.pos_ptr + 9), 150);
        if(startposition.string == endposition.string)
        {
            temp_arr[endposition.pos_ptr-startposition.pos_ptr - 9] = '\0';
        }
        sscanf(temp_arr, "Timestamp: %d &lt;br/&gt; %lf",&placemark.timestamp, &placemark.signallevel);
    }
    if(correct_result_fl && coordfl)
    {
        strncpy(temp_arr, (startposition.pos_ptr + 9), 150);
        if(startposition.string == endposition.string)
        {
            temp_arr[endposition.pos_ptr-startposition.pos_ptr - 9] = '\0';
        }
        sscanf(temp_arr, "<![CDATA[<i>Timestamp:</i>  %d",&placemark.timestamp);
        strcpy(placemark.name,"GPS Data"); // correcting the name field
    }
    correct_result_fl = 0;

    /*-------------------------Parsing the coordinates field----------------*/

    for(u32 i = placemark_startstring; i < placemark_endstring; i++)
    {
        if((temp_ptr = strstr(fdata.farray[i], "<coordinates>")) != NULL)
        {
            startposition.string = i;
            startposition.pos_ptr = temp_ptr;
            correct_result_fl = 1;
        }
        if((temp_ptr = strstr(fdata.farray[i], "</coordinates>")) != NULL)
        {
            endposition.string = i;
            endposition.pos_ptr = temp_ptr;
        }
    }
    if(correct_result_fl)
    {
        strncpy(temp_arr, (startposition.pos_ptr + 13), 150);
        if(startposition.string == endposition.string)
        {
            temp_arr[endposition.pos_ptr-startposition.pos_ptr - 13] = '\0';
        }
        sscanf(temp_arr, "%lf,%lf,%lf",&placemark.longitude, &placemark.latitude, &placemark.altitude);
    }
    correct_result_fl = 0;

    /*--------------------------Parsing the color field----------------------*/

    for(u32 i = placemark_startstring; i < placemark_endstring; i++)
    {
        if((temp_ptr = strstr(fdata.farray[i], "<color>")) != NULL)
        {
            startposition.string = i;
            startposition.pos_ptr = temp_ptr;
            correct_result_fl = 1;
        }
        if((temp_ptr = strstr(fdata.farray[i], "</color>")) != NULL)
        {
            endposition.string = i;
            endposition.pos_ptr = temp_ptr;
        }
    }
    if(correct_result_fl)
    {
        strncpy(temp_arr, (startposition.pos_ptr + 7), 150);
        if(startposition.string == endposition.string)
        {
            temp_arr[endposition.pos_ptr-startposition.pos_ptr - 7] = '\0';
        }
        sscanf(temp_arr, "%s", placemark.color);
    }  
return placemark;
}


/*-----------The function of forming arrays of placemarks combined by folders--------------*/

void getkmldata(folder_t* folder, int folder_quantity, filedata_t fdata)
{
    for(u32 i = 0; i < folder_quantity; i++)
    {
        folder[i].placemark_arr = malloc(folder[i].placemark_quantity * sizeof(placemark_t));
        for(u32 j = 0; j < folder[i].placemark_quantity; j++)
        {
            if(!strcmp(folder[i].name, "GPS Data") && (j % 2)) continue;
            if(!strcmp(folder[i].name, "GPS Data") && !(j % 2)) folder[i].placemark_arr[j/2] = getplacemarkdata(folder[i].placemark_startstr[j], folder[i].placemark_endstr[j], fdata);
            else folder[i].placemark_arr[j] = getplacemarkdata(folder[i].placemark_startstr[j], folder[i].placemark_endstr[j], fdata);
        }
        strncpy(folder[i].name, folder[i].placemark_arr[0].name, 120);
    }
    folder[0].placemark_quantity /= 2;  // we do not use odd placemarks for the GPS Data folder (they are skipped in the loop => the actual number of points is twice as small)
    for(u32 i = 0; i < folder_quantity; i ++)  // clearing the memory allocated for arrays with line numbers of the beginning and end of each placemark
    {
        free(folder[i].placemark_startstr);
        free(folder[i].placemark_endstr);
    }
}

/*---------------------------------------Coverage calculation functions--------------------------------*/

void getaverlvl(folder_t* folder, int folder_quantity, int averageingdepth, AVG_TYPE avgtype) // averaging level for N points (N = averageingdepth)
{
	switch(avgtype)
	{
	case sma:  // simple moving average
		for(u32 i = 0; i < folder_quantity; i++) 
		{
			for(int j = 0; j < folder[i].placemark_quantity; j++)
			{
				folder[i].placemark_arr[j].averagelevel = 0;
				if(j < (averageingdepth / 2))
				{
					for(int k = 0; k < (j + (averageingdepth / 2)); k++) {
						folder[i].placemark_arr[j].averagelevel = folder[i].placemark_arr[j].averagelevel + folder[i].placemark_arr[k].signallevel;
					}
					folder[i].placemark_arr[j].averagelevel /= (j + (averageingdepth / 2));
				}
				if((j >= (averageingdepth / 2)) && (j <= (folder[i].placemark_quantity - (averageingdepth / 2)))) {
					for(int k = (j - (averageingdepth / 2)); k < (j + (averageingdepth / 2)); k++) {
						folder[i].placemark_arr[j].averagelevel = folder[i].placemark_arr[j].averagelevel + folder[i].placemark_arr[k].signallevel;
					}
					folder[i].placemark_arr[j].averagelevel /= (2 * (averageingdepth / 2));
				}
				if(j > (folder[i].placemark_quantity - (averageingdepth / 2))) {
					for(int k = (j - (averageingdepth / 2)); k < folder[i].placemark_quantity; k++) {
						folder[i].placemark_arr[j].averagelevel = folder[i].placemark_arr[j].averagelevel + folder[i].placemark_arr[k].signallevel;
					}
					folder[i].placemark_arr[j].averagelevel /= (folder[i].placemark_quantity - j + (averageingdepth / 2));
				}
			}
		}
	break;
	case median: // median average
		for(u32 i = 0; i < folder_quantity; i++) 
		{
            double tmp[110]; 
			for(int j = 0; j < folder[i].placemark_quantity; j++)
			{
				folder[i].placemark_arr[j].averagelevel = 0;
				if(j < (averageingdepth / 2))
				{
					int l = 0;
					for(int k = 0; k < (j + (averageingdepth / 2)); k++) {
						tmp[l] =  folder[i].placemark_arr[k].signallevel;
						l++;
					}
					folder[i].placemark_arr[j].averagelevel = getmedian(tmp, l);
				}
				if((j >= (averageingdepth / 2)) && (j <= (folder[i].placemark_quantity - (averageingdepth / 2)))) {
					int l = 0;
					for(int k = (j - (averageingdepth / 2)); k < (j + (averageingdepth / 2)); k++) {
						tmp[l] =  folder[i].placemark_arr[k].signallevel;
						l++;
					}
					folder[i].placemark_arr[j].averagelevel = getmedian(tmp, l);
				}
				if(j > (folder[i].placemark_quantity - (averageingdepth / 2))) {
					int l = 0;
					for(int k = (j - (averageingdepth / 2)); k < folder[i].placemark_quantity; k++) {
						tmp[l] =  folder[i].placemark_arr[k].signallevel;
						l++;
					}
					folder[i].placemark_arr[j].averagelevel = getmedian(tmp, l);
				}
			}
		}
	break;
	}
}

/*------------------Function form a table of uncovered sections-----------------------------------*/

void createcovtab(folder_t *folder, int folder_quantity, init_t settings)
{
    double distance_element;
    double threshold; 
    int curr_cover = 0;

    if(settings.covercalctype == separate) 
    { 
        for(u32 i = 0; i < folder_quantity; i++)
        {
            u32 k = 0; //coverage areas array index

            if(strstr(folder[i].name, "GPS Data") != NULL) continue;
            if(strstr(folder[i].name, "GSM") != NULL) threshold = settings.GSMcoveragelvl;
            else if(strstr(folder[i].name, "LTE") != NULL) threshold = settings.LTEcoveragelvl;
            else if((strstr(folder[i].name, "3G") != NULL) || (strstr(folder[i].name, "UMTS") != NULL)) threshold = settings.UMTScoveragelvl;
            else threshold = settings.defaultcovlvl;

            folder[i].totdist = 0;
            folder[i].uncovtotdist = 0;
            folder[i].covtotdist = 0;
            folder[i].threshold = threshold;

            folder[i].covreg = malloc(folder[i].placemark_quantity * sizeof(covreg_t));

            if(folder[i].placemark_arr[0].averagelevel >= threshold) curr_cover = 1;
            else curr_cover = 0;
            folder[i].placemark_arr[0].coverfl = curr_cover;
            folder[i].placemark_arr[0].distfromstart = 0;

            folder[i].covreg[k].coverfl = curr_cover;
            folder[i].covreg[k].startlat = folder[i].placemark_arr[0].latitude;
            folder[i].covreg[k].startlong = folder[i].placemark_arr[0].longitude;
            folder[i].covreg[k].startnumber = folder[i].placemark_arr[0].measnumber;
            folder[i].covreg[k].distance = 0;

            u32 l = (folder[i].placemark_quantity - 1); // for the convenience of calculation last point
            for(u32 j = 1; j < l; j++)
            {
                distance_element = distearth(folder[i].placemark_arr[j].latitude, folder[i].placemark_arr[j].longitude, folder[i].placemark_arr[j-1].latitude, folder[i].placemark_arr[j-1].longitude);
                folder[i].totdist += distance_element;
                folder[i].placemark_arr[j].distfromstart = folder[i].totdist;

                if(folder[i].placemark_arr[j].averagelevel >= threshold) curr_cover = 1;
                else curr_cover = 0;
                folder[i].placemark_arr[j].coverfl = curr_cover;
                
                // CONTINUATION of the current area 
                if(folder[i].covreg[k].coverfl == curr_cover) 
                {
                    folder[i].covreg[k].distance += distance_element;
                };
                // START of the uncovered area, END of the covered area
                // OR
                // END of the covered area, START of the uncovered area
                if(folder[i].covreg[k].coverfl != curr_cover) 
                {
                    folder[i].covreg[k].distance += distance_element;
                    folder[i].covreg[k].endlat = folder[0].placemark_arr[j-1].latitude;
                    folder[i].covreg[k].endlong = folder[0].placemark_arr[j-1].longitude;
                    folder[i].covreg[k].endnumber = folder[0].placemark_arr[j-1].measnumber;
                    k++;
                    folder[i].covreg[k].coverfl = curr_cover;
                    folder[i].covreg[k].startlat = folder[0].placemark_arr[j].latitude;
                    folder[i].covreg[k].startlong = folder[0].placemark_arr[j].longitude;
                    folder[i].covreg[k].startnumber = folder[0].placemark_arr[j].measnumber;
                    folder[i].covreg[k].distance = 0;
                };
                
            }
            distance_element = distearth(folder[i].placemark_arr[l].latitude, folder[i].placemark_arr[l].longitude, folder[i].placemark_arr[l-1].latitude, folder[i].placemark_arr[l-1].longitude);
            folder[i].totdist += distance_element;
            folder[i].placemark_arr[l].distfromstart = folder[i].totdist;
            folder[i].placemark_arr[l].coverfl = curr_cover;
            folder[i].covreg[k].distance += distance_element;
            folder[i].covreg[k].endlat = folder[i].placemark_arr[l].latitude;
            folder[i].covreg[k].endlong = folder[i].placemark_arr[l].longitude;
            folder[i].covreg[k].endnumber = folder[i].placemark_arr[l].measnumber;
            k++;
            folder[i].reg_quantity = k;

            for(u32 m = 0 ; m < folder[i].reg_quantity; m++)
            {
                if(folder[i].covreg[m].coverfl) folder[i].covtotdist += folder[i].covreg[m].distance;
                else folder[i].uncovtotdist += folder[i].covreg[m].distance;
            }
        }

    }

    if(settings.covercalctype == total) 
    {
        folder[0].covreg = malloc((folder[0].placemark_quantity) * sizeof(covreg_t));
        folder[0].totdist = 0;
        folder[0].uncovtotdist = 0;
        folder[0].covtotdist = 0;

        u32 k = 0;
        for(u32 i = 0; i < folder_quantity; i++)
        {
            if(strstr(folder[i].name, "GPS Data") != NULL) continue;
            if(strstr(folder[i].name, "GSM") != NULL) threshold = settings.GSMcoveragelvl;
            else if(strstr(folder[i].name, "LTE") != NULL) threshold = settings.LTEcoveragelvl;
            else if((strstr(folder[i].name, "3G") != NULL) || (strstr(folder[i].name, "UMTS") != NULL)) threshold = settings.UMTScoveragelvl;
            else threshold = settings.defaultcovlvl;

            folder[i].threshold = threshold;

            if(folder[i].placemark_arr[0].averagelevel >= threshold)
            {
                curr_cover = 1;
            }
        }
        folder[0].placemark_arr[0].coverfl = curr_cover;
        folder[0].placemark_arr[0].distfromstart = folder[0].totdist;
        folder[0].covreg[k].startlat = folder[0].placemark_arr[0].latitude;
        folder[0].covreg[k].startlong = folder[0].placemark_arr[0].longitude;
        folder[0].covreg[k].startnumber = folder[0].placemark_arr[0].measnumber;
        folder[0].covreg[k].coverfl = curr_cover;
        folder[0].covreg[k].distance = 0;

        u32 l = folder[0].placemark_quantity - 1;
        for(u32 j = 1; j < l; j++)
        {
            curr_cover = 0;
            distance_element = distearth(folder[0].placemark_arr[j].latitude, folder[0].placemark_arr[j].longitude, folder[0].placemark_arr[j-1].latitude, folder[0].placemark_arr[j-1].longitude);
            folder[0].totdist += distance_element;

            for(u32 i = 0; i < folder_quantity; i++)
            {
                if(strstr(folder[i].name, "GPS Data") != NULL) continue;
                if(strstr(folder[i].name, "GSM") != NULL) threshold = settings.GSMcoveragelvl;
                else if(strstr(folder[i].name, "LTE") != NULL) threshold = settings.LTEcoveragelvl;
                else if((strstr(folder[i].name, "3G") != NULL) || (strstr(folder[i].name, "UMTS") != NULL)) threshold = settings.UMTScoveragelvl;
                else threshold = settings.defaultcovlvl;

                if(folder[i].placemark_arr[j].averagelevel >= threshold)
                {
                    curr_cover = 1;
                    break;
                }
            }
            folder[0].placemark_arr[j].coverfl = curr_cover;
            folder[0].placemark_arr[j].distfromstart = folder[0].totdist;

            // CONTINUATION of the current area 
            if(curr_cover == folder[0].covreg[k].coverfl) 
            {
                folder[0].covreg[k].distance += distance_element;
            }
            // START of the UNCOVERED area, END of the COVERED area 
            // OR
            // END of the UNCOVERED area, START of the COVERED area
            if(curr_cover != folder[0].covreg[k].coverfl) 
            {
                folder[0].covreg[k].distance += distance_element;
                folder[0].covreg[k].endlat = folder[0].placemark_arr[j-1].latitude;
                folder[0].covreg[k].endlong = folder[0].placemark_arr[j-1].longitude;
                folder[0].covreg[k].endnumber = folder[0].placemark_arr[j-1].measnumber;
                k++;
                folder[0].covreg[k].coverfl = curr_cover;
                folder[0].covreg[k].startlat = folder[0].placemark_arr[j].latitude;
                folder[0].covreg[k].startlong = folder[0].placemark_arr[j].longitude;
                folder[0].covreg[k].startnumber = folder[0].placemark_arr[j].measnumber;
                folder[0].covreg[k].distance = 0;
            }
        }

        distance_element = distearth(folder[0].placemark_arr[l].latitude, folder[0].placemark_arr[l].longitude, folder[0].placemark_arr[l-1].latitude, folder[0].placemark_arr[l-1].longitude);
        folder[0].totdist += distance_element;

        folder[0].placemark_arr[l].coverfl = curr_cover;
        folder[0].placemark_arr[l].distfromstart = folder[0].totdist;
        folder[0].covreg[k].endlat = folder[0].placemark_arr[l].latitude;
        folder[0].covreg[k].endlong = folder[0].placemark_arr[l].longitude;
        folder[0].covreg[k].endnumber = folder[0].placemark_arr[l].measnumber;
        folder[0].covreg[k].distance += distance_element;
        k++;
        folder[0].reg_quantity = k;

        for(u32 m = 0 ; m < folder[0].reg_quantity; m++)
        {
            if(folder[0].covreg[m].coverfl) folder[0].covtotdist += folder[0].covreg[m].distance;
            else folder[0].uncovtotdist += folder[0].covreg[m].distance;
        }

/*-----------------------Averaging the coverage by distance----------------------------*/

        folder[1].placemark_arr[0].coverfl = folder[0].placemark_arr[0].coverfl;

        int m;
        int plmk_quant;
        int plmk_covflsum;
        double dist;
        double cov_1;
        double cov_2;
        double coverage;
        for(u32 j = 1; j < folder[0].placemark_quantity; j++)
        {
            plmk_quant = 1;
            plmk_covflsum = folder[0].placemark_arr[j].coverfl;
            m = j;
            dist = 0.0;
            while(dist <= (settings.maxskipdist / 2))
            {
                m--;
                if(m < 0) break;
                plmk_covflsum += folder[0].placemark_arr[m].coverfl;
                dist = folder[0].placemark_arr[j].distfromstart - folder[0].placemark_arr[m].distfromstart;
                plmk_quant++;
            }
            cov_1 = (double) plmk_covflsum / plmk_quant;

            m = j;
            plmk_covflsum = folder[0].placemark_arr[j].coverfl;
            plmk_quant = 1;
            dist = 0.0;
            while(dist <= (settings.maxskipdist / 2))
            {
                m++;
                if(m >= (folder[0].placemark_quantity-1)) break;
                plmk_covflsum += folder[0].placemark_arr[m].coverfl;
                dist = folder[0].placemark_arr[m].distfromstart - folder[0].placemark_arr[j].distfromstart;
                plmk_quant++;
            }
            cov_2 = (double) plmk_covflsum / plmk_quant;

            coverage = (cov_1 + cov_2) / 2;

            if(coverage > 0.52) folder[1].placemark_arr[j].coverfl = 1;
            else if(coverage < 0.48) folder[1].placemark_arr[j].coverfl = 0;
            else folder[1].placemark_arr[j].coverfl = folder[1].placemark_arr[j-1].coverfl;
        }

        folder[1].covreg = malloc((folder[0].placemark_quantity) * sizeof(covreg_t));
        k = 0;
        
        curr_cover = folder[1].placemark_arr[0].coverfl;
        folder[1].covreg[k].startlat = folder[0].placemark_arr[0].latitude;
        folder[1].covreg[k].startlong = folder[0].placemark_arr[0].longitude;
        folder[1].covreg[k].startnumber = folder[0].placemark_arr[0].measnumber;
        folder[1].covreg[k].coverfl = curr_cover;
        folder[1].covreg[k].distance = 0;

        l = folder[1].placemark_quantity - 1;
        for(u32 j = 1; j < folder[1].placemark_quantity; j++)
        {
            distance_element = distearth(folder[0].placemark_arr[j].latitude, folder[0].placemark_arr[j].longitude, folder[0].placemark_arr[j-1].latitude, folder[0].placemark_arr[j-1].longitude);
            folder[1].totdist += distance_element;
            curr_cover = folder[1].placemark_arr[j].coverfl;
            // CONTINUATION of the current area 
            if(curr_cover == folder[1].covreg[k].coverfl) 
            {
                folder[1].covreg[k].distance += distance_element;
            }
            // START of the UNCOVERED area, END of the COVERED area 
            // OR
            // END of the UNCOVERED area, START of the COVERED area
            if(curr_cover != folder[1].covreg[k].coverfl) 
            {
                folder[1].covreg[k].distance += distance_element;
                folder[1].covreg[k].endlat = folder[0].placemark_arr[j-1].latitude;
                folder[1].covreg[k].endlong = folder[0].placemark_arr[j-1].longitude;
                folder[1].covreg[k].endnumber = folder[0].placemark_arr[j-1].measnumber;
                k++;
                folder[1].covreg[k].coverfl = curr_cover;
                folder[1].covreg[k].startlat = folder[0].placemark_arr[j].latitude;
                folder[1].covreg[k].startlong = folder[0].placemark_arr[j].longitude;
                folder[1].covreg[k].startnumber = folder[0].placemark_arr[j].measnumber;
                folder[1].covreg[k].distance = 0;
            }
        }

        distance_element = distearth(folder[0].placemark_arr[l].latitude, folder[0].placemark_arr[l].longitude, folder[0].placemark_arr[l-1].latitude, folder[0].placemark_arr[l-1].longitude);
        folder[1].totdist += distance_element;

        folder[1].placemark_arr[l].coverfl = curr_cover;
        folder[1].placemark_arr[l].distfromstart = folder[0].totdist;
        folder[1].covreg[k].endlat = folder[0].placemark_arr[l].latitude;
        folder[1].covreg[k].endlong = folder[0].placemark_arr[l].longitude;
        folder[1].covreg[k].endnumber = folder[0].placemark_arr[l].measnumber;
        folder[1].covreg[k].distance += distance_element;
        k++;
        folder[1].reg_quantity = k;

        for(u32 m = 0 ; m < folder[1].reg_quantity; m++)
        {
            if(folder[1].covreg[m].coverfl) folder[1].covtotdist += folder[1].covreg[m].distance;
            else folder[1].uncovtotdist += folder[1].covreg[m].distance;
        }
    }
}


void foldermemfree(folder_t* folder, int folder_quantity)
{
    for(u32 i = 0; i < folder_quantity; i++)
    {
        free(folder[i].placemark_arr);
        free(folder[i].covreg);
    }
}