#ifndef __TYPES_N_STRUCTS__
#define __TYPES_N_STRUCTS__

typedef unsigned char  u8;   
typedef unsigned short u16;    
typedef unsigned int   u32;  



typedef struct {
    int* string_length;
    u32 strings;
    char** farray;
} filedata_t;

typedef struct {
    char        name[400];
    int         measnumber;
    int         timestamp;
    double      signallevel;
    double      latitude;
    double      longitude;
    double      altitude;
    char        color[10];
    double      averagelevel;
    int         coverfl;
    double      distfromstart;
} placemark_t;

typedef struct {
    int        coverfl;
    double     startlat;
    double     startlong;
    double     endlat;
    double     endlong;
    double     distance;
    u32        startnumber;
    u32        endnumber;
} covreg_t;

typedef struct {
    int           number;
    u32           startstr;
    u32           endstr;
    char          name[400];
    char          short_name[400];
    u32           placemark_quantity;
    u32           firstplacemark_startstr;
    u32*          placemark_startstr;
    u32*          placemark_endstr;
    placemark_t*  placemark_arr;
    covreg_t*     covreg;
    double        threshold;
    u32           reg_quantity;
    double        totdist;
    double        uncovtotdist;
    double        covtotdist;
} folder_t;

typedef struct {
    u32        string;
    char*      pos_ptr;
} posptr_t;

typedef enum {
	separate,
	total,
}COVER_TYPE;

typedef enum {
	sma,
	median,
}AVG_TYPE;

typedef struct{
int          fillinflag;
double       GSMcoveragelvl;
double       UMTScoveragelvl;
double       LTEcoveragelvl;
double       defaultcovlvl;
COVER_TYPE   covercalctype;
AVG_TYPE     avgtype;
int          avgdepth;
double       maxskipdist;
} init_t;

#endif