#include <mysql.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glut.h>    // Header File For The GLUT Library
#include <GL/gl.h>      // Header File For The OpenGL32 Library
#include <GL/glu.h>     // Header File For The GLu32 Library
#include <GL/glx.h>     // Header file fot the glx libraries.
#include <GL/glc.h>             // danish ttf support

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <math.h>
#include <ctype.h>
#include <ical.h>


//#include "text3d.h"
#include "utility.h"
#include "myctrl_tvprg.h"
#include "myth_ttffont.h"

#define MAXSIZE 16000+1;

extern int screen_size;
extern int debugmode;
extern int fonttype;

extern GLuint _tvbar1;
extern GLuint _tvbar1_1;
extern GLuint _tvbar2;
extern GLuint _textureIdclose;
extern GLuint tvprginfobig;
extern GLuint _tvprgrecorded;
extern GLuint _tvprgrecordedr;
extern GLuint _tvprgrecorded1;
extern GLuint _tvprgrecorded_mask;
extern GLuint _tvrecordbutton;
GLuint _textureId13;
extern GLuint _tvrecordcancelbutton;
extern GLuint _tvoldrecorded;
//extern GLuint _tvoldrecordedmask;

extern GLuint _tvoldprgrecordedbutton;
extern GLuint _tvnewprgrecordedbutton;
extern GLuint _tvmaskprgrecordedbutton;
extern GLuint _tvoverskrift;
extern GLuint _tv_prgtype;


extern fontctrl aktivfont;

extern int orgwinsizex,orgwinsizey;


//extern earlyrecorded oldrecorded;
//extern earlyrecorded newtcrecordlist;


const char *prgtypee[2*11]={"Ukendt"," Serier",
                          " Nyheder","Børn",
                          "  Diverse","Film",
                          "Action","underhold.",
                          "sifi","komedier",
                          "","",
                          "","",
                          "","",
                          "","",
                          "","",
                          "",""};

const float prgtypeRGB[2*3*10]={0.4f,0.4f,0.4f ,0.0f,0.0f,0.5f,
                               0.0f,0.5f,0.0f ,0.5f,0.1f,0.1f,
                               0.0f,0.0f,0.5f ,0.1f,0.2f,0.1f,
                               0.2f,0.2f,0.2f ,0.2f,0.2f,0.2f,
                               0.2f,0.2f,0.8f ,0.2f,0.5,0.7f,
                               0.2f,0.8f,0.7f ,0.2f,0.3f,0.2f,
                               0.2f,0.3f,0.8f ,0.3f,0.2f,0.5f,
                               0.2f,0.8f,0.2f ,0.2f,0.2f,0.2f,
                               0.2f,0.2f,0.4f ,0.2f,0.2f,0.2f,
                               0.2f,0.9f,0.2f ,0.2f,0.2f,0.2f};



// bruges af show_tvoversigt

void myglprinttv(char *string) {
    int len,i;
    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
       glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
    }
}



unsigned int ELFHash(const char *s) {
    /* ELF hash uses unsigned chars and unsigned arithmetic for portability */
    const unsigned char *name = (const unsigned char *)s;
    unsigned long h = 0, g;
    while (*name) {
        h = (h << 4) + (unsigned long)(*name++);
        if ((g = (h & 0xF0000000UL))!=0)
            h ^= (g >> 24);
        h &= ~g;
    }
    return (int)h;
}




tv_oversigt_prgtype::tv_oversigt_prgtype() {
    strcpy(program_navn,"");
    strcpy(starttime,"");
    strcpy(endtime,"");
    program_length_minuter=0;
    starttime_unix=0;
    prg_type=0;
    aktiv=false;
    brugt=true;
    recorded=0;
}


tv_oversigt_prgtype::~tv_oversigt_prgtype() {
}


int tv_oversigt_prgtype::putprograminfo(char *prgname,char *stime,char *etime,char *prglength,char *sunixtime,char *desc,char *subtitle,int ptype,int prgrecorded) {
    strcpy(program_navn,prgname);
    strcpy(starttime,stime);
    strcpy(endtime,etime);
    strcpy(description,desc);
    strcpy(sub_title,subtitle);
    starttime_unix=atol(sunixtime);
    program_length_minuter=atoi(prglength);
    prg_type=ptype;
    brugt=true;
    recorded=prgrecorded;
    return(1);
}


void tv_oversigt_prgtype::getprograminfo(char *prgname,char *stime,char *etime,int *prglength,unsigned long *sunixtime,char *desc,char *subtitle,int *ptype,bool *bgt,int *prgrecorded) {
    strcpy(prgname,program_navn);
    strcpy(stime,starttime);
    strcpy(etime,endtime);
    strcpy(subtitle,sub_title);
    *sunixtime=starttime_unix;
    *prglength=program_length_minuter;
    *ptype=prg_type;
    *bgt=brugt;
    *prgrecorded=recorded;
    strcpy(desc,description);
}


void tv_oversigt_prgtype::getprogramrecinfo(char *prgname,char *stime,char *etime) {
    strcpy(prgname,program_navn);
    strcpy(stime,starttime);
    strcpy(etime,endtime);
}



// constructor

tv_oversigt_pr_kanal::tv_oversigt_pr_kanal() {
    programantal=0;
    chanid=0;
    strcpy(chanel_name,"");
}


// destructor

tv_oversigt_pr_kanal::~tv_oversigt_pr_kanal() {
}




void tv_oversigt_pr_kanal::putkanalname(char *kname) {
    strcpy(chanel_name,kname);
}


// constructor

tv_oversigt::tv_oversigt() {
    kanal_antal=0;
    strcpy(mysqllhost,"");
    strcpy(mysqlluser,"");
    strcpy(mysqllpass,"");
}


// destructor

tv_oversigt::~tv_oversigt() {
}




//
// Clean tvprogram oversigt
//

int tv_oversigt::cleanchannels() {
    for(int i=0;i<MAXKANAL_ANTAL-1;i++) {
        for(int ii=0;ii<maxprogram_antal-1;ii++) {
            strcpy(tvkanaler[i].tv_prog_guide[ii].program_navn,"");
            strcpy(tvkanaler[i].tv_prog_guide[ii].starttime,"");
            strcpy(tvkanaler[i].tv_prog_guide[ii].endtime,"");
            strcpy(tvkanaler[i].tv_prog_guide[ii].sub_title,"");
            strcpy(tvkanaler[i].tv_prog_guide[ii].description,"");
            tvkanaler[i].tv_prog_guide[ii].program_length_minuter=0;
            tvkanaler[i].tv_prog_guide[ii].starttime_unix=0;
            tvkanaler[i].tv_prog_guide[ii].prg_type=0;
            tvkanaler[i].tv_prog_guide[ii].aktiv=false;
            tvkanaler[i].tv_prog_guide[ii].brugt=true;
            tvkanaler[i].tv_prog_guide[ii].recorded=0;
        }
        strcpy(tvkanaler[i].chanel_name,"");
    }
    return(1);
}





//
// Checker om et program er optaget tidligere og retunere antal
//

int tv_oversigt::tvprgrecordedbefore(char *ftitle,unsigned int fchannelid) {
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    static int recantal=0;
    char *database = (char *) "mythconverg";
    bool fundet=false;
    char sqlselect[200];
    sprintf(sqlselect,"SELECT count(title) FROM recorded where chanid=%d and title=\"%s\"",fchannelid,ftitle);
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, mysqllhost,mysqlluser, mysqllpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (fundet==false)) {
            fundet=true;
            recantal=atoi(row[0]);
        }
    }
    mysql_close(conn);
    if (fundet) return(recantal); else return(0);
}



//
// Checker om et program er optaget retunere type
//

int tv_oversigt::tvprgrecorded(char *fstarttime,char *ftitle,char *fchannelid) {
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    static int rectype=0;
    char *database = (char *) "mythconverg";
    bool fundet=false;
    char sqlselect[400];
    sprintf(sqlselect,"SELECT type FROM record where (chanid=%s and title=\"%s\" and starttime='%s') or (chanid=%s and title=\"%s\" and type=10)",fchannelid,ftitle,fstarttime,fchannelid,ftitle);
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, mysqllhost,mysqlluser, mysqllpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (fundet==false)) {
            fundet=true;
            rectype=atoi(row[0]);
        }
    }
    mysql_close(conn);
    if (fundet) return(rectype); else return(0);
}


//
// Fjerner et program som skal optages
//

int tv_oversigt::removetvprgrecorded(char *fstarttime,char *ftitle,char *fchannelid) {
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    static int rectype=0;
    char *database = (char *) "mythconverg";
    bool fundet=false;
    char sqlselect[200];
    sprintf(sqlselect,"DELETE FROM record where chanid=%s and title=\"%s\" and starttime='%s'",fchannelid,ftitle,fstarttime);
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, mysqllhost,mysqlluser, mysqllpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (fundet==false)) {
            fundet=true;
        }
    }
    mysql_close(conn);
    if (fundet) return(rectype); else return(0);
}


//
// Indsæt into table record to sectule new tv recording
//

int tv_oversigt::tvprgrecord_addrec(int tvvalgtrecordnr,int tvsubvalgtrecordnr) {
    // mysql vars
    MYSQL *conn;
    MYSQL *conn1;
    MYSQL_RES *res;
    MYSQL_RES *res1;
    MYSQL_ROW row;
    char *sqlselect;
    sqlselect=new char[40000];
    // mysql stuf
//    static int rectype=0;
    char *database = (char *) "mythconverg";
    bool doneok=false;

    time_t aktueltid;
    time_t prgtid;
    time(&aktueltid);					// hent hvad klokken er
    struct tm *timeinfo;
    struct tm prgtidinfo;
    timeinfo=localtime(&aktueltid);				// convert to localtime

    if (strptime(tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime,"%Y-%m-%d %H:%M:%S",&prgtidinfo)==NULL) {
        printf("DO INSERT RECORDED PROGRAM DATE FORMAT ERROR can't convert. by strptime\n");
    }
    // lav tv proram starttid om til time_t format
    prgtid=mktime(&prgtidinfo);

    if ((difftime(aktueltid,prgtid)<=0) && (sqlselect)) {
        sprintf(sqlselect,"SELECT channel.name as channelname, TIME(starttime) as starttime,DATE(starttime) as startdate,TIME(endtime) as endtime,DATE(endtime) as enddate,NOW() as datenu, program.chanid, program.category from program left join channel on program.chanid=channel.chanid where program.title='%s' and program.starttime='%s' and program.endtime='%s'",tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn,tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime,tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].endtime);
        conn=mysql_init(NULL);
        conn1=mysql_init(NULL);
        // Connect to databases
        mysql_real_connect(conn, mysqllhost,mysqlluser, mysqllpass, database, 0, NULL, 0);
        mysql_real_connect(conn1, mysqllhost,mysqlluser, mysqllpass, database, 0, NULL, 0);
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);

        mysql_query(conn1,"set NAMES 'utf8'");
        res1 = mysql_store_result(conn1);

        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        if (res) {
            while (((row = mysql_fetch_row(res)) != NULL) && (doneok==false)) {
                //tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn
                sprintf(sqlselect,"INSERT INTO record values (0,1,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",'Default',0,0,1,0,0,0,'Default',6,15,\"%s\",%u,'',0,0,0,0,0,0,0,0,TIME('%s'),%lu,0,0,0,'Default',0,'0000-00-00 00:00:00','','0000-00-00 00:00:00','Default',100)", row[6], row[1], row[2], row[3],row[4], tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn, tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].sub_title, tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].description, row[7],row[0], ELFHash(tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].program_navn) , row[1], ((tvkanaler[tvvalgtrecordnr].tv_prog_guide[tvsubvalgtrecordnr].starttime_unix)/60/60/24)+719528);
                mysql_query(conn1,sqlselect);
                res1 = mysql_store_result(conn1);
                doneok=true;
            }
        }
        mysql_close(conn);
        mysql_close(conn1);
        delete sqlselect;
    }
    return(doneok);
}



//
// Find tv guide kanal med samme eller senere tidspunkt som tidspunkt og retunre hviken element i array som har den start tid
// hvis ingen findes gå til start dvs array element 0
// bruges ved pil up/down i tv kanal listen i vis_tv_oversigt
//

int tv_oversigt::findguidetvtidspunkt(int kanalnr,time_t tidspunkt) {
    int prgnr=0;
    bool fundet=false;
    while((prgnr<tvkanaler[kanalnr].program_antal()) && (!(fundet))) {
        if ((time_t) tvkanaler[kanalnr].tv_prog_guide[prgnr].starttime_unix<tidspunkt) prgnr++; else fundet=true;
    }
    if (fundet) return(prgnr); else return(0);
}



// retunere tvprg starttid in unix start time

time_t tv_oversigt::hentprgstartklint(int kanalnr,int prgnr) {
    return(tvkanaler[kanalnr].tv_prog_guide[prgnr].starttime_unix);
}



//
// henter aktiv tv overigt fra mythtv databasen
//

void tv_oversigt::opdatere_tv_oversigt(char *mysqlhost,char *mysqluser,char *mysqlpass,time_t nystarttid) {
    int i,ii;
    int prgtype;
    bool recorded;
    char sqlselect[512];
    char dagsdato[128];
    char enddate[128];
    time_t rawtime;
    time_t rawtime2;
    struct tm *timeinfo;
    struct tm *timeinfo2;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    // mysql stuf
    char *database = (char *) "mythconverg";
    char tmptxt[100];


    strcpy(this->mysqllhost,mysqlhost);
    strcpy(this->mysqlluser,mysqluser);
    strcpy(this->mysqllpass,mysqlpass);

    if (nystarttid==0) {
        // get time now in a string format (yyyy-mm-dd hh:mm:ss)
        rawtime=time( NULL );							// hent nu tid
        rawtime2=time( NULL );
    } else {
        // hent ny starttid
        rawtime=starttid;
        rawtime2=sluttid;
    }

    timeinfo = localtime ( &rawtime );					// lav om til local time
    strftime(dagsdato, 128, "%Y-%m-%d 00:00:00", timeinfo );		// lav nu tids sting strftime(dagsdato, 128, "%Y-%m-%d %H:%M:%S", timeinfo );
//    rawtime2+=((60*60)*24);
    timeinfo2= localtime ( &rawtime2 );					//
    strftime(enddate, 128, "%Y-%m-%d 23:59:59", timeinfo2 );		// lav nu tids sting

    this->starttid=rawtime;						// gem tider i class
    this->sluttid=rawtime2;						//

//    gotoxy(10,9);
    printf("\nHent tv program guide.\n");
//    gotoxy(10,10);
    printf("Start dato : %20s End date : %20s \n",dagsdato,enddate);

    // clear last tv guide array
    cleanchannels();


    strcpy(sqlselect,"SELECT channel.callsign,program.starttime,program.endtime,title,subtitle,TIMESTAMPDIFF(MINUTE,starttime,endtime),UNIX_TIMESTAMP(program.starttime),category,category_type,description,program.chanid FROM program left join channel on program.chanid=channel.chanid where channel.visible=1 and endtime<='");
    strcat(sqlselect,enddate);
    strcat(sqlselect,"' and endtime>='");
    strcat(sqlselect,dagsdato);
    strcat(sqlselect,"' order by abs(channel.channum),starttime");
    conn=mysql_init(NULL);
    // Connect to database
    if (mysql_real_connect(conn, mysqlhost,mysqluser, mysqlpass, database, 0, NULL, 0)) {
        mysql_query(conn,"set NAMES 'utf8'");
        res = mysql_store_result(conn);
        mysql_query(conn,sqlselect);
        res = mysql_store_result(conn);
        i=0;
        ii=0;
        if (res) {
            while (((row = mysql_fetch_row(res)) != NULL) && (ii<=maxprogram_antal) && (i<MAXKANAL_ANTAL)) {
                if (ii==0) {
                    tvkanaler[i].putkanalname(row[0]);

                    tvkanaler[i].chanid=atoi(row[10]);

                    strcpy(tmptxt,row[0]);
                }

                if (strcmp("series",row[7])==0) prgtype=1;						// serie
                else if (strcmp("dansk underholdning.",row[7])==0) prgtype=1;
                else if (strcmp("sport",row[7])==0) prgtype=2;
                else if (strncmp("børn",row[7],4)==0) prgtype=3;
                else if (strncmp("dukkefilm",row[7],9)==0) prgtype=3;
                else if (strstr(row[7],"tegnefilm")!=0) prgtype=3;
                else if (strstr(row[7],"animationsfilm")!=0) prgtype=3;
                else if (strcmp("news",row[7])==0) prgtype=4;
                else if (strcmp("movie",row[7])==0) prgtype=5;
                else if (strstr(row[7],"dokumentarserie")!=0) prgtype=6;
                else if (strcmp("engelsk madprogram",row[7])==0) prgtype=11;
                else if (strcmp("dansk reportageserie.",row[7])==0) prgtype=7;
                else if (strcmp("amerikansk krimi.",row[7])==0) prgtype=5;
                else if (strcmp("debatprogram.",row[7])==0) prgtype=7;
                else if (strcmp("music",row[7])==0) prgtype=8;
                else if (strcmp("dyr",row[7])==0) prgtype=9;
                else prgtype=0;
                if (prgtype==0) {
                    if (strcmp("series",row[8])==0) prgtype=1;
                    else if (strcmp("movie",row[8])==0) prgtype=5;
                    else prgtype=0;
                }
                recorded=tvprgrecorded(row[1],row[3],row[10]);			// get recorded status from backend
                tvkanaler[i].tv_prog_guide[ii].putprograminfo(row[3],row[1],row[2],row[5],row[6],row[9],row[4],prgtype,recorded);
                //            gotoxy(10,11);
                //            printf("gemmer tv kanal : %30s %d \n",row[0],i);

                ii++;

                if ((strcmp(tmptxt,row[0])!=0) || (ii>=maxprogram_antal)) {
                    tvkanaler[i].set_program_antal(ii-1);
                    ii=0;
                    i++;								// next tv channel
                }
            }
            kanal_antal=i;
        }
        mysql_close(conn);
    }
}



// build block display no text

void tv_oversigt::build_tv_oversigt(int xxofset,int kanalstartofset)
{
    int prgrecorded;
    char prgname[80];
    char subtitle[128];
    char stime[20];
    char etime[20];
    char desc[16000];
    int prglength;
    unsigned long sunixtime;
    int ptype;

    char tmptxt[100];
    int kanalnr=0;
    int tidspunkt=0;
    int kanalantal;
    int starttimet,starttimem;
    int starttimeofset;
    int time_divide_by=1;
    int i;
    bool bgt;
    int xofset=0;


    glColor3f(1.0f, 1.0f, 1.0f);      				// color

    switch(screen_size) {
        case 1: kanalantal=12;
                break;
        case 2: kanalantal=12;
                break;
        case 3: kanalantal=12;
                break;
        case 4: kanalantal=12;
                break;
        default:kanalantal=12;
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _tvbar1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    while (kanalnr<kanalantal) {				// kanalantal
        glLoadIdentity();
        switch(screen_size) {
            case 1: glTranslatef(-46.0f-xxofset, 24.0-(kanalnr*4.6f), -110.0f);
                    break;
            case 2: glTranslatef(-54.0f-xxofset, 24.0-(kanalnr*4.6f), -110.0f);
                    break;
            case 3: glTranslatef(-71.0f-xxofset, 24.0-(kanalnr*4.6f), -110.0f);
                    break;
            case 4: glTranslatef(-68.0f-xxofset, 24.0-(kanalnr*4.6f), -110.0f);
                    break;
        }
        tidspunkt=0;
        while(tidspunkt<7) {
            tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt+xofset].getprograminfo(prgname,stime,etime,&prglength,&sunixtime,desc,subtitle,&ptype,&bgt,&prgrecorded);
//            prglength=tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt+xofset].program_length_minuter/time_divide_by;	// length i minuter
            if (bgt) {
                strncpy(tmptxt,stime+12,2);
                tmptxt[3]='\0';
                starttimet=atof(tmptxt);

//            strncpy(tmptxt,tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt+xofset].starttime+14,2);
                strncpy(tmptxt,stime+14,2);
                tmptxt[3]='\0';
                starttimem=atof(tmptxt);


//            printf("kanal %2d  prg navn%20s starttid %20s end tid %20s \n",kanalnr,tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt].program_navn,tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt].starttime,tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt].endtime);


                starttimeofset=((starttimet*60)/time_divide_by)+(starttimem/time_divide_by);
                tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt+xofset].starttimeinmin=starttimeofset;		// gem startx tid i minuter

                switch (ptype) {
                    case 0:
                        glColor3f(prgtypeRGB[0], prgtypeRGB[1], prgtypeRGB[2]);		// film
                        break;
                    case 1:
                        glColor3f(prgtypeRGB[3], prgtypeRGB[4], prgtypeRGB[5]);      	// serier
                        break;
                    case 2:
                        glColor3f(prgtypeRGB[6], prgtypeRGB[7], prgtypeRGB[8]);      	// div
                        break;
                    case 3:
                        glColor3f(prgtypeRGB[9], prgtypeRGB[10], prgtypeRGB[11]);      	// action
                        break;
                    case 4:
                        glColor3f(prgtypeRGB[12], prgtypeRGB[13], prgtypeRGB[14]);      	// nyheder
                        break;
                    case 5:
                        glColor3f(prgtypeRGB[15], prgtypeRGB[16], prgtypeRGB[17]);		// komedier
                        break;
                    case 6:
                        glColor3f(prgtypeRGB[18], prgtypeRGB[19], prgtypeRGB[20]);      	// underholdning
                        break;
                    case 7:
                        glColor3f(prgtypeRGB[21], prgtypeRGB[22], prgtypeRGB[23]);      	// music
                        break;
                    case 8:
                        glColor3f(prgtypeRGB[24], prgtypeRGB[25], prgtypeRGB[26]);      	// andet
                        break;
                    case 9:
                        glColor3f(prgtypeRGB[27], prgtypeRGB[28], prgtypeRGB[29]);		// sifi
                        break;
                    case 10:
                        glColor3f(prgtypeRGB[30], prgtypeRGB[31], prgtypeRGB[32]);		// ukdentd
                        break;
                    case 11:
                        glColor3f(prgtypeRGB[33], prgtypeRGB[34], prgtypeRGB[35]);		// rejser
                        break;
                    case 12:
                        glColor3f(prgtypeRGB[36], prgtypeRGB[37], prgtypeRGB[38]);		//
                        break;
                    case 13:
                        glColor3f(prgtypeRGB[39], prgtypeRGB[40], prgtypeRGB[41]);		// ukendt
                        break;
                    case 14:
                        glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		// rejser
                        break;
                    default:
                        glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		// rejser
                        break;
                }


//            if (aktiv) glColor3f(1.0f,1.0f,1.0f);

//            if (starttimeofset<200) {
                glLoadName((100*(kanalnr+1))+tidspunkt);

                glBegin(GL_QUADS);									// Begin quadrilateral coordinates
                glTexCoord2f(0.0, 0.0); glVertex3f(starttimeofset, -2.2	, 0.0);
                glTexCoord2f(1.0, 0.0); glVertex3f(starttimeofset+prglength-0.2, -2.2, 0.0);
                glTexCoord2f(1.0, 1.0); glVertex3f(starttimeofset+prglength-0.2, 2.2, 0.0);
                glTexCoord2f(0.0, 1.0); glVertex3f(starttimeofset, 2.2, 0.0);
                glEnd(); 										// End quadrilateral coordinates
//            }
                tidspunkt++;
            }
        }
        kanalnr++;
    }
    glBindTexture(GL_TEXTURE_2D, _tvbar2);
    glColor3f(1.0f,1.0f,1.0f);

    i=0;
    while (i<kanalantal) {
        glLoadIdentity();
        switch (screen_size) {
            case 1: glTranslatef(-54, 24.0-(i*(4.6)), -110.0f); 	//
                    break;
            case 2: glTranslatef(-52, 24.0-(i*(4.6)), -110.0f); 	//
                    break;
            case 3: glTranslatef(-78, 24.0-(i*(4.6)), -110.0f); 	//
                    break;
            case 4: glTranslatef(-74, 24.0-(i*(4.6)), -110.0f); 	//
                    break;
        }
        glRotatef(45.0f,0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-10, -3.0, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(  7, -3.0, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(  7,  3.0, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-10,  3.0, 0.0);
        glEnd();
        i++;
    }
}




/*
void tv_oversigt::build_tv_oversigt(float xofset,int kanalstartofset)
{
    char tmptxt[50];
    float startyofset=-7.0f;
    float zofset=-110.0f;
    int i,ii;
    float yofset=4.2;
    xofset=xofset-10.0;

    float time_divide_by=1;
    int kanalnr=0;
    int tidspunkt=0;
    float length;
    float starttimet;
    float starttimem;
    float starttimeofset;
    float lengthofset=0.0f;

    time_t rawtime;
    struct tm *timeinfo;

    rawtime=time( NULL );						// hent kl nu
    timeinfo = localtime ( &rawtime );					// lav om til local time


    int kanalantal;

    xofset-=15;


    switch(screen_size) {
        case 1: kanalantal=14;
                break;
        case 2: kanalantal=14;
                break;
        case 3: kanalantal=14;
                break;
        case 4: kanalantal=14;
                break;
        default:kanalantal=14;
    }

    // beregn start ofset tid
    //xofset=-30.0f*(timeinfo->tm_hour);
    //xofset-=(timeinfo->tm_min/2);
    //xofset+=15;

    glDisable(GL_TEXTURE_2D);

    tvkanaler[2].tv_prog_guide[1].aktiv=true;



    while (kanalnr<kanalantal) {				// kanalantal
        tidspunkt=0;


        while ((tidspunkt<48*8) && (strcmp(tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt].program_navn,"")!=0)) {

//            printf("kanal %2d  prg navn%20s starttid %20s end tid %20s \n",kanalnr,tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt].program_navn,tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt].starttime,tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt].endtime);

            glLoadIdentity();
            length=tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt].program_length_minuter/time_divide_by;	// length i minuter


            strncpy(tmptxt,tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt].starttime+12,2);
            tmptxt[3]='\0';
            starttimet=atof(tmptxt);

            strncpy(tmptxt,tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt].starttime+14,2);
            tmptxt[3]='\0';
            starttimem=atof(tmptxt);

            starttimeofset=((starttimet*60)/time_divide_by)+(starttimem/time_divide_by);

            switch(screen_size) {
                case 1: glTranslatef(-36.0f+xofset+lengthofset, 30.4-(kanalnr*yofset)+startyofset, zofset); 	// -30 til sidst
                        break;
                case 2: glTranslatef(-48.0f+xofset+lengthofset, 30.4-(kanalnr*yofset)+startyofset, zofset); 	// -30 til sidst
                        break;
                case 3: glTranslatef(-74.0f+xofset+starttimeofset, 32.4-(kanalnr*yofset)+startyofset, (zofset-40)); 	// -30 til sidst
                        break;
                case 4: glTranslatef(-48.0f+xofset+lengthofset, 30.4-(kanalnr*yofset)+startyofset, zofset); 	// -30 til sidst
                        break;
            }

            switch (tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt].prg_type) {
                case 0:
                    glColor3f(prgtypeRGB[0], prgtypeRGB[1], prgtypeRGB[2]);		// film
                    break;
                case 1:
                    glColor3f(prgtypeRGB[3], prgtypeRGB[4], prgtypeRGB[5]);      	// serier
                    break;
                case 2:
                    glColor3f(prgtypeRGB[6], prgtypeRGB[7], prgtypeRGB[8]);      	// div
                    break;
                case 3:
                    glColor3f(prgtypeRGB[9], prgtypeRGB[10], prgtypeRGB[11]);      	// action
                    break;
                case 4:
                    glColor3f(prgtypeRGB[12], prgtypeRGB[13], prgtypeRGB[14]);      	// nyheder
                    break;
                case 5:
                    glColor3f(prgtypeRGB[15], prgtypeRGB[16], prgtypeRGB[17]);		// komedier
                    break;
                case 6:
                    glColor3f(prgtypeRGB[18], prgtypeRGB[19], prgtypeRGB[20]);      	// underholdning
                    break;
                case 7:
                    glColor3f(prgtypeRGB[21], prgtypeRGB[22], prgtypeRGB[23]);      	// music
                    break;
                case 8:
                    glColor3f(prgtypeRGB[24], prgtypeRGB[25], prgtypeRGB[26]);      	// andet
                    break;
                case 9:
                    glColor3f(prgtypeRGB[27], prgtypeRGB[28], prgtypeRGB[29]);		// sifi
                    break;
                case 10:
                    glColor3f(prgtypeRGB[30], prgtypeRGB[31], prgtypeRGB[32]);		// ukdentd
                    break;
                case 11:
                    glColor3f(prgtypeRGB[33], prgtypeRGB[34], prgtypeRGB[35]);		// rejser
                    break;
                case 12:
                    glColor3f(prgtypeRGB[36], prgtypeRGB[37], prgtypeRGB[38]);		//
                    break;
                case 13:
                    glColor3f(prgtypeRGB[39], prgtypeRGB[40], prgtypeRGB[41]);		// ukendt
                    break;
                case 14:
                    glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		// rejser
                    break;
            };

            if (tvkanaler[kanalnr+kanalstartofset].tv_prog_guide[tidspunkt].aktiv)
                glColor3f(1.0f,1.0f,1.0f);

//            glEnable(GL_TEXTURE_2D);
//            glBindTexture(GL_TEXTURE_2D, _tvbar);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//            glLoadName((100*(kanalnr+1))+tidspunkt);

            glBegin(GL_QUADS);									// Begin quadrilateral coordinates
            glTexCoord2f(0.0, 0.0); glVertex3f(0, -2.0	, 0.0);
            glTexCoord2f(1.0, 0.0); glVertex3f(length-0.2, -2.0, 0.0);
            glTexCoord2f(1.0, 1.0); glVertex3f(length-0.2, 2.0, 0.0);
            glTexCoord2f(0.0, 1.0); glVertex3f(0, 2.0, 0.0);
            glEnd(); 										// End quadrilateral coordinates


            glcRenderString("TEST");

            tidspunkt++;
            lengthofset+=length;
        }
        kanalnr++;
        lengthofset=0.0f;
    }
    glColor3f(1.0f, 1.0f, 1.0f);      	// color
}

*/



void tv_oversigt::show_canal_names() {
    int i;
    int kanalantal;
    float yofset=4.2f;
//    float zofset=-110.0f;
    // make boxes bihint canal name
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, _tvbar2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    kanalantal=16;
    i=0;
    while (i<kanalantal) {
        glLoadIdentity();
        glTranslatef(-98, 27.8-(i*(yofset)), -99.5f-40); 	//
        glRotatef(45.0f,0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-10, -0.6, -10.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(7.0, -0.6 , -10.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(7.0, -6.4,-10.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-10, -6.4,-10.0);
        glEnd();
        i++;
    }
}



extern GLuint tvoversigt;


void tv_oversigt::show_tv_oversigt1(int startxofset)
{
    char tmptxt[250];
//    char tmptxt1[100];
    int kanalnr=0;
    int tidspunkt=0;
    int kanalantal;
    int prglength;
//    int starttimet,starttimem;
    int starttimeofset;
    int time_divide_by=1;
    int lengthofset=0;
    int starttidofset=0;    // starttidofse=startxofset;

//printf("startofset %d \n",starttidofset);

    glColor3f(1.0f, 1.0f, 1.0f);      				// color

    glCallList(tvoversigt);

    switch(screen_size) {
        case 1: kanalantal=12;
                break;
        case 2: kanalantal=12;
                break;
        case 3: kanalantal=12;
                break;
        case 4: kanalantal=12;
                break;
        default:kanalantal=12;
    }
    glColor3f(1.0f,1.0f,1.0f);
    glDisable(GL_TEXTURE_2D);
    while (kanalnr<12) {				// kanalantal
        tidspunkt=0;
        lengthofset=0;
        glLoadIdentity();
        switch (screen_size) {
            case 1: glTranslatef(-62.8+3, (23.6)-(kanalnr*(4.6)), -110.0f);	// 47.8 = center/55.8 left align
                    break;
            case 2: glTranslatef(-58.8+3, (23.6)-(kanalnr*(4.6)), -110.0f);
                    break;
            case 3: glTranslatef(-83.8+3, (23.6)-(kanalnr*(4.6)), -110.0f);
                    break;
            case 4: glTranslatef(-82.8+3, (23.6)-(kanalnr*(4.6)), -110.0f);
                    break;
        }
        glScalef(1.6, 1.6,1.6);
        glcRenderString(tvkanaler[kanalnr].chanel_name);
        while (tidspunkt<5) {
//  	      printf("Start timeofset %d \n",tvkanaler[kanalnr].tv_prog_guide[tidspunkt].starttimeinmin);
            prglength=tvkanaler[kanalnr].tv_prog_guide[tidspunkt+starttidofset].program_length_minuter/time_divide_by;	// length i minuter
            glLoadIdentity();
            starttimeofset=tvkanaler[kanalnr].tv_prog_guide[tidspunkt+starttidofset].starttimeinmin;		// hent prg start ofset i minuter
            switch (screen_size) {
                case 1: glTranslatef(-46.8+2+starttimeofset-startxofset, (23.6)-(kanalnr*(4.6)), -110.0f);	// 47.8 = center/55.8 left align
                        break;
                case 2: glTranslatef(-54.8+2+starttimeofset-startxofset, (23.6)-(kanalnr*(4.6)), -110.0f);
                        break;
                case 3: glTranslatef(-72.8+2+starttimeofset-startxofset, (23.6)-(kanalnr*(4.6)), -110.0f);
                        break;
                case 4: glTranslatef(-68.8+2+starttimeofset-startxofset, (23.6)-(kanalnr*(4.6)), -110.0f);
                        break;
            }
            glScalef(1.6, 1.6,1.6);
            sprintf(tmptxt,"%d",starttimeofset);
            strcat(tmptxt," ");
            strcat(tmptxt,tvkanaler[kanalnr].tv_prog_guide[tidspunkt+starttidofset].program_navn);
            glcRenderString(tmptxt);
            tidspunkt++;
            lengthofset+=prglength;
        }
        kanalnr++;
    }


    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(-48.8+3, (27.6), -110.0f);	// 47.8 = center/55.8 left align
                break;
        case 2: glTranslatef(-58.8+3, (27.6), -110.0f);
                break;
        case 3: glTranslatef(-72.8+3, (27.6), -110.0f);
                break;
        case 4: glTranslatef(-70.8+3, (27.6), -110.0f);
                break;
    }
    sprintf(tmptxt,"%02d:00                                                                                                  %02d:00",(startxofset/60),(startxofset/60)+1);
    glScalef(1.6, 1.6,1.6);
    glcRenderString(tmptxt);
}








/*


// Viser tv overigt i 3d

void tv_oversigt::show_tv_oversigt(float _anglex,float  _anglez,int kanalstartofset) {

//    int _anglex=0;
//    int _anglez=0;
    float programtxtofsety=0.0;
    float zofset=-110-_anglex;
    int i=0;
    int ii=0;
    int iii=0;
    char temptxt[200];
    float yofset=4.2f;				// 4.2f
    float length;
    int templength;
    float lengthofset=0.0f;
    time_t showstarttid;
    float xofset=0.0f+_anglez;

    float startyofset=-7.0f;			// start y ofset

    int time_divide_by=2;
    time_t rawtime2;
    struct tm *timeinfo;
    struct tm *timeinfo2;
    char dagsdato[128];
    char dagsdato1[128];
    char timeofset[10];
    char timeofset1[10];

    char tmptxt[100];
    int cutlength;
// reset x y pos

    glPushMatrix();
    glLoadIdentity();



    glDisable(GL_TEXTURE_2D);

    glLoadIdentity();
    glTranslatef(0.0, 0.0, -30.1f); 	// -30 til sidst
    glRotatef(0, 0.0f, 0.0f, 0.0f);
    glColor3f(0.5f, 0.5f, 0.5f);
    showstarttid=this->starttid;					// start tidspunkt
//    showstarttid-=(_anglez*245);
    timeinfo = localtime ( &showstarttid );				// lav om til local time
    strftime(dagsdato, 128, "%Y-%m-%d %H:%M:%S", timeinfo );		// lav nu tids sting

    strftime(timeofset,10,"%H",timeinfo);		// hent antal timer
    strftime(timeofset1,10,"%m",timeinfo);		// hent antal minuter

//    xofset+=((-11)*atoi(timeofset)-atoi(timeofset1));


    rawtime2=time( NULL );
    timeinfo2 = localtime ( &rawtime2 );					// lav om til local time

    // bund tid
    strftime(dagsdato1, 128, "%H:%M:%S", timeinfo2 );		// lav nu tids sting
    glLoadIdentity();
    glTranslatef(11.5f, -12.0f, -30.0f); 	// -30 til sidst

    glRasterPos2f(0.0f, 2.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    myglprinttv(dagsdato1);
    strftime(dagsdato1, 128, "%d-%m-%Y", timeinfo2 );		// lav nu tids sting
    glLoadIdentity();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(11.0f, -12.0f, -30.0f);
    glRasterPos2f(0.0f, 1.0f);
    myglprinttv(dagsdato1);


    // beregn start ofset tid
    xofset=-30.0f*(timeinfo->tm_hour);
    xofset-=(timeinfo->tm_min/2);
    xofset+=15;

    // show time line
    length=0;
    ii=0;
    for(i=0;i<26;i++) {
       glLoadIdentity();
       glColor3f(1.0f, 1.0f, 1.0f);
       glTranslatef(-48.0f+xofset+lengthofset, 20.4-startyofset, zofset);
       glScalef(2.00f, 2.00f, 2.00f);
       sprintf(tmptxt,"%02d:00",ii);
       glcRenderString(tmptxt);
       lengthofset+=29.01f;
       ii++;
       if (ii==24) ii=0;
    }


    // KANAL_ANTAL = antal kanaler som kan vises
    i=0;
    while ((i<14) && (strcmp(tvkanaler[i].chanel_name,"")!=0)) {
        glLoadIdentity();
        glColor3f(1.0f, 1.0f, 1.0f);								// text color
        ii=0;
        while ((lengthofset<60*16) && (strcmp(tvkanaler[i+kanalstartofset].tv_prog_guide[ii].program_navn,"")!=0)) {
            length=tvkanaler[i+kanalstartofset].tv_prog_guide[ii].program_length_minuter/time_divide_by;	// length i minuter

//printf("len %0.0f of %0.0f ",length,lengthofset);

//            if (length>60) length=50;

            glLoadIdentity();
            switch (screen_size) {
                case 0: glTranslatef(-48.0f+xofset+lengthofset, 30.4-(i*yofset)+startyofset, zofset); 	// -30 til sidst
                        break;
                case 1: glTranslatef(-48.0f+xofset+lengthofset, 30.4-(i*yofset)+startyofset, zofset); 	// -30 til sidst
                        break;
                case 2: glTranslatef(-48.0f+xofset+lengthofset, 30.4-(i*yofset)+startyofset, zofset); 	// -30 til sidst
                        break;
                case 3: glTranslatef(-74.0f+xofset+lengthofset, 30.4-(i*yofset)+startyofset, zofset); 	// -30 til sidst
                        break;
                case 4: glTranslatef(-48.0f+xofset+lengthofset, 30.4-(i*yofset)+startyofset, zofset); 	// -30 til sidst
                        break;
            }

            switch (tvkanaler[i+kanalstartofset].tv_prog_guide[ii].prg_type) {
                case 0:
                    glColor3f(prgtypeRGB[0], prgtypeRGB[1], prgtypeRGB[2]);		// film
                    break;
                case 1:
                    glColor3f(prgtypeRGB[3], prgtypeRGB[4], prgtypeRGB[5]);      	// serier
                    break;
                case 2:
                    glColor3f(prgtypeRGB[6], prgtypeRGB[7], prgtypeRGB[8]);      	// div
                    break;
                case 3:
                    glColor3f(prgtypeRGB[9], prgtypeRGB[10], prgtypeRGB[11]);      	// action
                    break;
                case 4:
                    glColor3f(prgtypeRGB[12], prgtypeRGB[13], prgtypeRGB[14]);      	// nyheder
                    break;
                case 5:
                    glColor3f(prgtypeRGB[15], prgtypeRGB[16], prgtypeRGB[17]);		// komedier
                    break;
                case 6:
                    glColor3f(prgtypeRGB[18], prgtypeRGB[19], prgtypeRGB[20]);      	// underholdning
                    break;
                case 7:
                    glColor3f(prgtypeRGB[21], prgtypeRGB[22], prgtypeRGB[23]);      	// music
                    break;
                case 8:
                    glColor3f(prgtypeRGB[24], prgtypeRGB[25], prgtypeRGB[26]);      	// andet
                    break;
                case 9:
                    glColor3f(prgtypeRGB[27], prgtypeRGB[28], prgtypeRGB[29]);		// sifi
                    break;
                case 10:
                    glColor3f(prgtypeRGB[30], prgtypeRGB[31], prgtypeRGB[32]);		// ukdentd
                    break;
                case 11:
                    glColor3f(prgtypeRGB[33], prgtypeRGB[34], prgtypeRGB[35]);		// rejser
                    break;
                case 12:
                    glColor3f(prgtypeRGB[36], prgtypeRGB[37], prgtypeRGB[38]);		//
                    break;
                case 13:
                    glColor3f(prgtypeRGB[39], prgtypeRGB[40], prgtypeRGB[41]);		// ukendt
                    break;
                case 14:
                    glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		// rejser
                    break;
            };

//            glColor3f(0.0, 0.0, 1.0f);
            glLoadName((100*i)+ii);



            glBegin(GL_QUADS);				// Begin quadrilateral coordinates
            glVertex3f(0, -2.0	, 0.0);
            glVertex3f(length-0.2, -2.0, 0.0);
            glVertex3f(length-0.2, 2.0, 0.0);
            glVertex3f(0, 2.0, 0.0);
            glEnd(); 					// End quadrilateral coordinates
            glLoadIdentity();
            switch (screen_size) {
                case 0: glTranslatef(-47.8+lengthofset+xofset, (30.0+0.2)-(i*(yofset))+startyofset, zofset+0.1f);
                        break;
                case 1: glTranslatef(-47.8+lengthofset+xofset, (30.0+0.2)-(i*(yofset))+startyofset, zofset+0.1f);
                        break;
                case 2: glTranslatef(-47.8+lengthofset+xofset, (30.0+0.2)-(i*(yofset))+startyofset, zofset+0.1f);
                        break;
                case 3: glTranslatef(-73.8+lengthofset+xofset, (30.0+0.2)-(i*(yofset))+startyofset, zofset+0.1f);
                        break;
                case 4: glTranslatef(-47.8+lengthofset+xofset, (30.0+0.2)-(i*(yofset))+startyofset, zofset+0.1f);
                        break;
            }
            glColor3f(1.0f, 1.0f, 1.0f);
//            glRasterPos2f(0.0f+(xofset), 0.0);
            strcpy(tmptxt,tvkanaler[i+kanalstartofset].tv_prog_guide[ii].program_navn);
            // check max tekst length
            if (strlen(tmptxt)>length) {
                cutlength=length;
                if (cutlength-4>=0) tmptxt[cutlength-4]=0;
            }

//                glScalef(1.50f, 1.50f, 1.00f);
            glScalef(2.00f, 2.00f, 1.00f);
            if (strlen(tmptxt)>length) tmptxt[2]=0;
            glcRenderString(tmptxt);
//            myglprint(tmptxt);

            if ((ii<59) && ( ((length*60)+tvkanaler[i+kanalstartofset].tv_prog_guide[ii].starttime_unix)<tvkanaler[i+kanalstartofset].tv_prog_guide[ii+1].starttime_unix)) {
                templength=(tvkanaler[i+kanalstartofset].tv_prog_guide[ii+1].starttime_unix-tvkanaler[i+kanalstartofset].tv_prog_guide[ii].starttime_unix)/60;		// beregn diff i unix tid (sec) lav om til min
                // der manger at blive tegnet til næste program start


//                glLoadIdentity();
//                glTranslatef(-10.0f+xofset+lengthofset, 5.4-(i*yofset), zofset); 	// -30 til sidst
//                glColor3f(1.0, 0.0, 0.0f);
 //               glBegin(GL_QUADS);				// Begin quadrilateral coordinates
//                glVertex3f(0, -0.6, 0.0);
//                glVertex3f(.1+(templength), -0.6, 0.0);
//                glVertex3f(.1+(templength), 0.6, 0.0);
//                glVertex3f(0, 0.6, 0.0);
 //               glEnd(); 					// End quadrilateral coordinates




//        if (i==0) printf("_anglez= %f  \n ",_anglex);

//                lengthofset+=templength/time_divide_by;
                    length=templength/time_divide_by;				// test
            }


            lengthofset+=length;
            ii++;
        }
        lengthofset=0;
        i++;
    }


    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_TEXTURE_2D);
    i=0;
    glLoadIdentity();
    glColor3f(0.3f, 0.3f, 0.3f);      	// color

    switch (screen_size) {
        case 0: glTranslatef(-54-(_anglex/2), 34.1-(i*(yofset))+startyofset, -99.5f+(zofset+110)); 	//
                break;
        case 1: glTranslatef(-54-(_anglex/2), 34.1-(i*(yofset))+startyofset, -99.5f+(zofset+110)); 	//
                break;
        case 2: glTranslatef(-54-(_anglex/2), 34.1-(i*(yofset))+startyofset, -99.5f+(zofset+110)); 	//
                break;
        case 3: glTranslatef(-80-(_anglex/2), 34.1-(i*(yofset))+startyofset, -99.5f+(zofset+110)); 	//
                break;
        case 4: glTranslatef(-54-(_anglex/2), 34.1-(i*(yofset))+startyofset, -99.5f+(zofset+110)); 	//
                break;
    }
    glRotatef(0.0f,0.0f, 0.0f, 0.0f);

    // make boxes bihint canal name
    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    i=0;
    programtxtofsety=0.0f;
    while (i<14) {
        glVertex3f(-10, -1.8-programtxtofsety, -10.0);
        glVertex3f(6.0, -1.8-programtxtofsety , -10.0);
        glVertex3f(6.0, -5.9-programtxtofsety,-10.0);
        glVertex3f(-10, -5.9-programtxtofsety,-10.0);
        programtxtofsety+=4.175f;
        i++;
    }


    glEnd(); //End quadrilateral coordinates

    // show channel names

    glLoadIdentity();
    switch (screen_size) {
        case 0: glTranslatef(-10.0f+xofset, (30.0f+4.0f)+startyofset, zofset); 						// -30 til sidst
                break;
        case 1: glTranslatef(-48.0f+xofset, (30.0f+4.0f)+startyofset, zofset); 						// -30 til sidst
                break;
        case 2: glTranslatef(-10.0f+xofset, (30.0f+4.0f)+startyofset, zofset); 						// -30 til sidst
                break;
        case 3: glTranslatef(-10.0f+xofset, (30.0f+4.0f)+startyofset, zofset); 						// -30 til sidst
                break;
        case 4: glTranslatef(-48.0f+xofset, (30.0f+4.0f)+startyofset, zofset); 						// -30 til sidst
                break;
    }
    // KANAL_ANTAL = antal kanaler som kan vises
    i=0;
//    glDisable(GL_BLEND);
//    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_TEXTURE_2D);
    //glBlendFunc(GL_DST_COLOR, GL_ZERO);

    while ((i<14) && (strcmp(tvkanaler[i].chanel_name,"")!=0)) {
        glLoadIdentity();

        switch (screen_size) {
            case 0: glTranslatef(-86.0f+30-(_anglex/2), 30.4-(i*(yofset))+startyofset, zofset+0.5f);			//
                    break;
            case 1: glTranslatef(-86.0f+26-(_anglex/2), 30.4-(i*(yofset))+startyofset, zofset+0.5f);			//
                    break;
            case 2: glTranslatef(-86.0f+30-(_anglex/2), 30.4-(i*(yofset))+startyofset, zofset+0.5f);			//
                    break;
            case 3: glTranslatef(-86.0f+2-(_anglex/2), 30.4-(i*(yofset))+startyofset, zofset+0.5f);			//
                    break;
            case 4: glTranslatef(-86.0f+2-(_anglex/2), 30.4-(i*(yofset))+startyofset, zofset+0.5f);			//
                    break;
        }
//        glRotatef(45, 0.1f, 0.0f, 0.0f);
//        glRasterPos2f(0.0f, 2.6-(i*(yofset/2)));
        glColor3f(1.0f, 1.0f, 1.0f);								// text color
        glScalef(2.00f, 2.00f, 0.00f);
        strcpy(tmptxt,tvkanaler[i+kanalstartofset].chanel_name);
        tmptxt[9]=0;
        glcRenderString(tmptxt);

//        myglprint(tvkanaler[i].chanel_name);     						// OLD VER no zoom
        i++;
    }
    i=0;
    ii=0;
    iii=0;
    xofset=0.0f;
    glDisable(GL_BLEND);
//        glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_TEXTURE_2D);
    while(i<7) {
        glLoadIdentity();
        glTranslatef(-48.0f+xofset, -32.0f, -100.0f);
        glDisable(GL_TEXTURE);
        glColor3f(prgtypeRGB[iii],prgtypeRGB[iii+1],prgtypeRGB[iii+2]);
        glBegin(GL_QUADS); //Begin quadrilateral coordinates
        glVertex3f(-6.0, -3.1, 0.0);
        glVertex3f(-6.0, 1.0, 0.0);
        glVertex3f(6.0, 1.0, 0.0);
        glVertex3f(6.0, -3.1, 0.0);
        glEnd(); //End quadrilateral coordinates
        strcpy(temptxt,prgtypee[ii]);
        glTranslatef(2.0f-(strlen(temptxt)), -1.4f, 0.0f);
        glScalef(2.2, 2.2,1.0);
        glColor3f(1.0f, 1.0f, 1.0f);
        glcRenderString(temptxt);

        glLoadIdentity();
        glTranslatef(-48.0f+xofset, -36.6f, -100.0f);
        glDisable(GL_TEXTURE);
        glColor3f(prgtypeRGB[iii+3],prgtypeRGB[iii+4],prgtypeRGB[iii+5]);
        glBegin(GL_QUADS); //Begin quadrilateral coordinates
        glVertex3f(-6.0, -3.1, 0.0);
        glVertex3f(-6.0, 1.0, 0.0);
        glVertex3f(6.0, 1.0, 0.0);
        glVertex3f(6.0, -3.1, 0.0);
        glEnd(); //End quadrilateral coordinates
        strcpy(temptxt,prgtypee[ii+1]);
        glTranslatef(2.0f-(strlen(temptxt)), -1.4f, 0.0f);
        glScalef(2.2, 2.2,1.0);
        glColor3f(1.0f, 1.0f, 1.0f);
        glcRenderString(temptxt);

        i++;
        ii+=2;			// next in table
        iii+=6;
        xofset+=12.2;
    }
    glPopMatrix();

    glLoadIdentity();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.0f, -0.0f, -4.0f);

    glCallList(tvoversigt);

}



*/

// Outputs a string wrapped to N columns
void WordWrap( char *str, int N ) {
    int i,j,k,pos;
    i = pos = 0;
    char word[16000];

    while( 1 ) {
        j = 0;
        while( !isspace( str[i]) ) // Find next word
        {
            if( str[i] == '\0' )
                break;
            word[j] = str[i];
            i++;
            j++;
        }

        word[j] = '\0'; // The value of j is the word length

        if( j > N ) // Word length is greater than column length
        { // print char-by-char
            k = 0;
            while( word[k] != '\0' ) {
                if( pos >= N-1 ) {
                    if( k != 0 ) putchar('-'); // print '-' if last-but-one column
                    pos = 0;
                    putchar('\n');
                }
                putchar( word[k] );
                pos++;
                k++;
            }
        } else {
            if( pos + j > N ) // Word doesn't fit in line
            {
                putchar('\n');
                pos = 0;
            }

            printf("%s", word );
            pos += j;
        }
        if( pos < N-1 ) // print space if not last column
        {
            putchar(' ');
            pos++;
        }
        if( str[i] == '\0' ) break;

        i++; // Skip space
    } // end of while(1)
}






void tv_oversigt::build_fasttv_oversigt(int selectchanel,int selectprg)
{
    char temptxt[200];
    int kanalnr=0;
    int kanalantal;
    int kanallength;
    int i=0;
    int ii,iii;
    float xofset=0.0f;
    int pstartofset;
    int cstartofset;
    time_t aktueltid;
    time(&aktueltid);					// hent hvad klokken er
    struct tm *timeinfo;
//    struct tm *prgtidinfo;
    timeinfo=localtime(&aktueltid);				// convert to localtime

    // find start ofset på prg
    if (selectprg>11) pstartofset=selectprg-11;
    else pstartofset=0;

    if (selectchanel>11) cstartofset=selectchanel-11;
    else cstartofset=0;

    glColor3f(1.0f, 1.0f, 1.0f);      				// color

    switch(screen_size) {
        case 1: kanalantal=12;
                kanallength=60;
                break;
        case 2: kanalantal=12;
                kanallength=60;
                break;
        case 3: kanalantal=12;
                kanallength=100;
                break;
        case 4: kanalantal=12;
                kanallength=102;
                break;
        default:kanalantal=12;
                kanallength=100;
                break;
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _tvbar1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    while (kanalnr<kanalantal) {				// kanalantal
        glLoadIdentity();
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, _tvbar1);
        switch(screen_size) {
            case 1: glTranslatef(-46.0f, 24.0-(kanalnr*4.6f), -110.0f);
                    break;
            case 2: glTranslatef(-46.0f, 24.0-(kanalnr*4.6f), -110.0f);
                    break;
            case 3: glTranslatef(-70.6f, 24.0-(kanalnr*4.6f), -110.0f);
                    break;
            case 4: glTranslatef(-68.4f, 24.0-(kanalnr*4.6f), -110.0f);
                    break;
        }
        switch (tvkanaler[selectchanel+cstartofset].tv_prog_guide[i+pstartofset].prg_type) {
            case 0:
                glColor3f(prgtypeRGB[0], prgtypeRGB[1], prgtypeRGB[2]);		// film
                break;
             case 1:
                glColor3f(prgtypeRGB[3], prgtypeRGB[4], prgtypeRGB[5]);      	// serier
                break;
             case 2:
                glColor3f(prgtypeRGB[6], prgtypeRGB[7], prgtypeRGB[8]);      	// div
                break;
             case 3:
                glColor3f(prgtypeRGB[9], prgtypeRGB[10], prgtypeRGB[11]);      	// action
                break;
            case 4:
                glColor3f(prgtypeRGB[12], prgtypeRGB[13], prgtypeRGB[14]);      	// nyheder
                break;
            case 5:
                glColor3f(prgtypeRGB[15], prgtypeRGB[16], prgtypeRGB[17]);		// komedier
                break;
            case 6:
                glColor3f(prgtypeRGB[18], prgtypeRGB[19], prgtypeRGB[20]);      	// underholdning
                break;
            case 7:
                glColor3f(prgtypeRGB[21], prgtypeRGB[22], prgtypeRGB[23]);      	// music
                break;
            case 8:
                glColor3f(prgtypeRGB[24], prgtypeRGB[25], prgtypeRGB[26]);      	// andet
                break;
            case 9:
                glColor3f(prgtypeRGB[27], prgtypeRGB[28], prgtypeRGB[29]);		// sifi
                break;
            case 10:
                glColor3f(prgtypeRGB[30], prgtypeRGB[31], prgtypeRGB[32]);		// ukdentd
                break;
            case 11:
                glColor3f(prgtypeRGB[33], prgtypeRGB[34], prgtypeRGB[35]);		// rejser
                break;
            case 12:
                glColor3f(prgtypeRGB[36], prgtypeRGB[37], prgtypeRGB[38]);		//
                break;
            case 13:
                glColor3f(prgtypeRGB[39], prgtypeRGB[40], prgtypeRGB[41]);		// ukendt
                break;
            case 14:
                glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		// rejser
                break;
            default:
                glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		// rejser
                 break;
        }
//        glColor3f(prgtypeRGB[3], prgtypeRGB[4], prgtypeRGB[5]);
        if (kanalnr==selectprg-pstartofset) {
            glBindTexture(GL_TEXTURE_2D, _tvbar1_1);
            glColor3f(1.0f, 1.0f, 1.0f);
        }

        glLoadName(100+(i+pstartofset));

        glBegin(GL_QUADS);									// Begin quadrilateral coordinates
        glTexCoord2f(0.0, 0.0); glVertex3f(0,   -2.2, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(kanallength+0.2, -2.2, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(kanallength+0.2,  2.2, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(0,    2.2, 0.0);
        glEnd();

 //       printf("Show recorded tv kanal %d prgnr %d record %d \n",selectchanel,i,tvkanaler[selectchanel].tv_prog_guide[i].recorded);

        // create RECORDED stamp
        if (tvkanaler[selectchanel+cstartofset].tv_prog_guide[i+pstartofset].recorded) {
            // draw mask
            glPushMatrix();
            glLoadIdentity();
            glBindTexture(GL_TEXTURE_2D,_tvprgrecorded_mask);
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            //glBlendFunc(GL_ONE, GL_ZERO);

            switch(screen_size) {
                case 1: glTranslatef(12.0f, 24.0-(kanalnr*4.6f), -110.0f);
                        break;
                case 2: glTranslatef(12.0f, 24.0-(kanalnr*4.6f), -110.0f);
                        break;
                case 3: glTranslatef(26.0f, 24.0-(kanalnr*4.6f), -110.0f);
                        break;
                case 4: glTranslatef(31.0f, 24.0-(kanalnr*4.6f), -110.0f);
                        break;
            }
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex3f(-2.0f, -2.0f, 0.0f);
            glTexCoord2f(1.0, 0.0); glVertex3f( 2.0f, -2.0f, 0.0f);
            glTexCoord2f(1.0, 1.0); glVertex3f( 2.0f,  2.0f, 0.0f);
            glTexCoord2f(0.0, 1.0); glVertex3f(-2.0f,  2.0f, 0.0f);
            glEnd();
            // draw icon
            glLoadIdentity();
            switch (tvkanaler[selectchanel+cstartofset].tv_prog_guide[i+pstartofset].recorded) {
                case 1:	glBindTexture(GL_TEXTURE_2D,_tvprgrecorded);
                        break;
                case 2:	glBindTexture(GL_TEXTURE_2D,_tvprgrecorded);
                        break;
                case 3:	glBindTexture(GL_TEXTURE_2D,_tvprgrecorded);
                        break;
                case 10:glBindTexture(GL_TEXTURE_2D,_tvprgrecorded1);
                        break;
                default:glBindTexture(GL_TEXTURE_2D,_tvprgrecorded1);
            }

            //prgtidinfo=localtime((time_t *) &tvkanaler[selectchanel+cstartofset].tv_prog_guide[i+pstartofset].starttime_unix);
            //if (timeinfo->tm_hour==prgtidinfo->tm_hour) glBindTexture(GL_TEXTURE_2D,_tvprgrecordedr);

            glBlendFunc(GL_ONE, GL_ONE);
            switch(screen_size) {
                case 1: glTranslatef(12.0f, 24.0-(kanalnr*4.6f), -110.0f);
                        break;
                case 2: glTranslatef(12.0f, 24.0-(kanalnr*4.6f), -110.0f);
                        break;
                case 3: glTranslatef(26.0f, 24.0-(kanalnr*4.6f), -110.0f);
                        break;
                case 4: glTranslatef(31.0f, 24.0-(kanalnr*4.6f), -110.0f);
                        break;
            }
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0); glVertex3f(-2.0f, -2.0f, 0.0f);
            glTexCoord2f(1.0, 0.0); glVertex3f( 2.0f, -2.0f, 0.0f);
            glTexCoord2f(1.0, 1.0); glVertex3f( 2.0f,  2.0f, 0.0f);
            glTexCoord2f(0.0, 1.0); glVertex3f(-2.0f,  2.0f, 0.0f);
            glEnd();
            glPopMatrix();
        }
        kanalnr++;
        i++;
    }

    // show big background for tvprograminfo
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, tvprginfobig);
    glColor3f(0.5f, 0.5f, 0.5f);
    glRotatef(45.0f,0.0f, 0.0f, 0.0f);
    switch(screen_size) {
        case 1:	glTranslatef(37.0f, 0.0f, -110.0f);
                break;
        case 2:	glTranslatef(37.0f, 0.0f, -110.0f);
                break;
        case 3:	glTranslatef(52.0f, 0.0f, -110.0f);
                break;
        case 4:	glTranslatef(56.0f, 0.0f, -110.0f);
                break;
    }

    glBegin(GL_QUADS);									// Begin quadrilateral coordinates
    glTexCoord2f(0.0, 0.0); glVertex3f(-22, -29.0, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f( 22, -29.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f( 22,  26.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-22,  26.0, 0.0);
    glEnd();


    // channel names background
    glBindTexture(GL_TEXTURE_2D, _tvbar2);
    glColor3f(1.0f,1.0f,1.0f);
    glDisable(GL_BLEND);
    i=0;
    while (i<kanalantal) {
        glLoadIdentity();
        switch (screen_size) {
            case 1: glTranslatef(-52, 24.0-(i*(4.6)), -110.0f); 	//
                    break;
            case 2: glTranslatef(-52, 24.0-(i*(4.6)), -110.0f); 	//
                    break;
            case 3: glTranslatef(-76, 24.0-(i*(4.6)), -110.0f); 	//
                    break;
            case 4: glTranslatef(-74, 24.0-(i*(4.6)), -110.0f); 	//
                    break;
        }
        glRotatef(45.0f,0.0f, 0.0f, 0.0f);

//        glLoadName(200+i);

        glBegin(GL_QUADS);									// Begin quadrilateral coordinates
        glTexCoord2f(0.0, 0.0); glVertex3f(-10, -2.9, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(  7, -2.9, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(  7,  2.9, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-10,  2.9, 0.0);
        glEnd();
        i++;
    }
    //
    // show art color map
    //
    i=0;
    ii=0;
    iii=0;
//    char temptxt[200];

    glDisable(GL_BLEND);
    while(i<7) {

        glLoadIdentity();
        switch(screen_size) {
            case 1: glTranslatef(-48.0f+xofset, -32.0f, -100.0f);
                    break;
            case 2: glTranslatef(-48.0f+xofset, -32.0f, -100.0f);
                    break;
            case 3: glTranslatef(-48.0f+xofset, -32.0f, -100.0f);
                    break;
            case 4: glTranslatef(-48.0f+xofset, -32.0f, -100.0f);
                    break;
        }
        glDisable(GL_TEXTURE);
        glColor3f(prgtypeRGB[iii],prgtypeRGB[iii+1],prgtypeRGB[iii+2]);
        glBegin(GL_QUADS); //Begin quadrilateral coordinates
        glVertex3f(-6.0, -3.1, 0.0);
        glVertex3f(-6.0, 1.0, 0.0);
        glVertex3f(6.0, 1.0, 0.0);
        glVertex3f(6.0, -3.1, 0.0);
        glEnd(); //End quadrilateral coordinates
        strcpy(temptxt,prgtypee[ii]);
        glTranslatef(2.0f-(strlen(temptxt)), -1.4f, 0.0f);
        glScalef(2.2, 2.2,1.0);
        glColor3f(1.0f, 1.0f, 1.0f);
        glcRenderString(temptxt);

        glLoadIdentity();
        switch(screen_size) {
            case 1: glTranslatef(-48.0f+xofset, -36.6f, -100.0f);
                    break;
            case 2: glTranslatef(-48.0f+xofset, -36.6f, -100.0f);
                    break;
            case 3: glTranslatef(-48.0f+xofset, -36.6f, -100.0f);
                    break;
            case 4: glTranslatef(-48.0f+xofset, -36.6f, -100.0f);
                    break;
        }

        glDisable(GL_TEXTURE);
        glColor3f(prgtypeRGB[iii+3],prgtypeRGB[iii+4],prgtypeRGB[iii+5]);
        glBegin(GL_QUADS); //Begin quadrilateral coordinates
        glVertex3f(-6.0, -3.1, 0.0);
        glVertex3f(-6.0, 1.0, 0.0);
        glVertex3f(6.0, 1.0, 0.0);
        glVertex3f(6.0, -3.1, 0.0);
        glEnd(); //End quadrilateral coordinates
        strcpy(temptxt,prgtypee[ii+1]);
        glTranslatef(2.0f-(strlen(temptxt)), -1.4f, 0.0f);
        glScalef(2.2, 2.2,1.0);
        glColor3f(1.0f, 1.0f, 1.0f);
        glcRenderString(temptxt);

        i++;
        ii+=2;			// next in table
        iii+=6;
        xofset+=12.2;
    }
}






// den som bruges

void tv_oversigt::show_fasttv_oversigt1(int selectchanel,int selectprg) {

    struct tm * timeinfo;
    int i,j,k,pos;
    int iii;
    int kanalantal=12;
    char tmptxt[250];
    int xpos,ypos;
    int xsiz,ysiz;

    int pstartofset=0;
    int kanalnr=0;
    int ptype=1;

    int cstartofset=0;


    glPushMatrix();
    aktivfont.selectfont((char *) "Norasi");

    glTranslatef(400,orgwinsizey-100, 0.0f);
    glDisable(GL_TEXTURE_2D);

    glScalef(70.0, 70.0, 1.0);
    glColor3f(1.0f, 1.0f, 1.0f);

    starttid=time( NULL );
    timeinfo=localtime(&starttid);
    sprintf(tmptxt,"TV Guiden %s den %d-%d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
    glcRenderString(tmptxt);
    glPopMatrix();

    //size 1870*150

    xpos=10;
    ypos=orgwinsizey-200;
    xsiz=(orgwinsizex-50);
    ysiz=150;

    glPushMatrix();
    glTranslatef(10,50, 0.0f);

    // top
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,_tvoverskrift);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glLoadName(27);     // 40

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
    glEnd(); //End quadrilateral coordinates



    glPopMatrix();

    iii=0;
    while (iii<14) {
        xpos=10;
        ypos=orgwinsizey-300-(iii*50);
        xsiz=160;
        ysiz=50;

        glPushMatrix();
        glTranslatef(10,50, 0.0f);

        // box2
        //    glLoadIdentity();
        //    glTranslatef(-2.0f, 4.0f,-15.0f+screen_zofset);

        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_ONE, GL_ONE);

        glBindTexture(GL_TEXTURE_2D,_tvbar1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBegin(GL_QUADS); //Begin quadrilateral coordinates
        glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
        glEnd(); //End quadrilateral coordinates

        glPopMatrix();

        xpos=170+2;
        ypos=orgwinsizey-300-(iii*50);
        xsiz=1000;
        ysiz=50;

        glPushMatrix();
        glTranslatef(10,50, 0.0f);
        glEnable(GL_TEXTURE_2D);
        //glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glBindTexture(GL_TEXTURE_2D,_tvbar1_1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBegin(GL_QUADS); //Begin quadrilateral coordinates
        glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
        glEnd(); //End quadrilateral coordinates


        glPopMatrix();
        // kanal navn
        strcpy(tvkanaler[kanalnr+pstartofset].chanel_name,"kanal1");
        glPushMatrix();
        glTranslatef(2,orgwinsizey-230-(iii*50), 0.0f);
        glScalef(20.0, 20.0,1);
        sprintf(tmptxt,"%16s",tvkanaler[kanalnr+pstartofset].chanel_name);
        glcRenderString(tmptxt);
        glPopMatrix();

        // tv prg info text
        glPushMatrix();
        glTranslatef(120,orgwinsizey-230-(iii*50), 0.0f);
        glScalef(20.0, 20.0,1);
        sprintf(tmptxt,"%16s",tvkanaler[kanalnr+pstartofset].chanel_name);
        glcRenderString(tmptxt);
        glPopMatrix();


        // tv prg big info text box
        glPushMatrix();
        glTranslatef(1250,orgwinsizey-280, 0.0f);
        glScalef(20.0, 20.0,1);
        sprintf(tmptxt,"%16s",tvkanaler[kanalnr+pstartofset].chanel_name);
        glcRenderString(tmptxt);
        glPopMatrix();


        kanalnr++;
        iii++;
    }


    // tv program info window
    xpos=1300;
    ypos=orgwinsizey-900;

    xsiz=500;
    ysiz=600;
    glPushMatrix();
    glTranslatef(10,50, 0.0f);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,tvprginfobig);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates
    glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
    glEnd(); //End quadrilateral coordinates


    // size 1000*120
    glPushMatrix();
    xsiz=1000;
    ysiz=120;
    xpos=460;
    ypos=0;

    glTranslatef(0,0, 0.0f);
    glEnable(GL_TEXTURE_2D);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D,_tvbar2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBegin(GL_QUADS); //Begin quadrilateral coordinates120
    glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
    glEnd(); //End quadrilateral coordinates
    glPopMatrix();

    iii=0;
    i=0;
    while(iii<10) {
      // size
      xsiz=100;
      ysiz=60;
      xpos=470+(iii*100);
      ypos=110;
      glPushMatrix();
      glTranslatef(0,0, 0.0f);
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      switch (ptype) {
        case 0:
          glColor3f(prgtypeRGB[0], prgtypeRGB[1], prgtypeRGB[2]);		// film
          break;
        case 1:
          glColor3f(prgtypeRGB[3], prgtypeRGB[4], prgtypeRGB[5]);      	// serier
          break;
        case 2:
          glColor3f(prgtypeRGB[6], prgtypeRGB[7], prgtypeRGB[8]);      	// div
          break;
        case 3:
          glColor3f(prgtypeRGB[9], prgtypeRGB[10], prgtypeRGB[11]);      	// action
          break;
        case 4:
          glColor3f(prgtypeRGB[12], prgtypeRGB[13], prgtypeRGB[14]);      	// nyheder
          break;
        case 5:
          glColor3f(prgtypeRGB[15], prgtypeRGB[16], prgtypeRGB[17]);		// komedier
          break;
        case 6:
          glColor3f(prgtypeRGB[18], prgtypeRGB[19], prgtypeRGB[20]);      	// underholdning
          break;
        case 7:
          glColor3f(prgtypeRGB[21], prgtypeRGB[22], prgtypeRGB[23]);      	// music
          break;
        case 8:
          glColor3f(prgtypeRGB[24], prgtypeRGB[25], prgtypeRGB[26]);      	// andet
          break;
        case 9:
          glColor3f(prgtypeRGB[27], prgtypeRGB[28], prgtypeRGB[29]);		// sifi
          break;
        case 10:
          glColor3f(prgtypeRGB[30], prgtypeRGB[31], prgtypeRGB[32]);		// ukdentd
          break;
        case 11:
          glColor3f(prgtypeRGB[33], prgtypeRGB[34], prgtypeRGB[35]);		// rejser
          break;
        case 12:
          glColor3f(prgtypeRGB[36], prgtypeRGB[37], prgtypeRGB[38]);		//
          break;
        case 13:
          glColor3f(prgtypeRGB[39], prgtypeRGB[40], prgtypeRGB[41]);		// ukendt
          break;
        case 14:
          glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		// rejser
          break;
        default:
          glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		// rejser
          break;
      }

      glBindTexture(GL_TEXTURE_2D,_tv_prgtype);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS); //Begin quadrilateral coordinates120
      glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
      glEnd(); //End quadrilateral coordinates
      glPopMatrix();

      // text
      glPushMatrix();
      glColor4f(1.0f,1.0f,1.0f,1.0f);
      glTranslatef(480+(iii*100),130,0.0f);
      glScalef(20.0, 20.0,1);
      sprintf(tmptxt,"%s",prgtypee[i]);
      glcRenderString(tmptxt);
      glPopMatrix();

      ptype++;

      xsiz=100;
      ysiz=60;
      xpos=470+(iii*100);
      ypos=50;
      glPushMatrix();
      glTranslatef(0,0, 0.0f);
      glEnable(GL_TEXTURE_2D);
      glBlendFunc(GL_ONE, GL_ONE);
      switch (ptype) {
          case 0:
              glColor3f(prgtypeRGB[0], prgtypeRGB[1], prgtypeRGB[2]);		// film
              break;
          case 1:
              glColor3f(prgtypeRGB[3], prgtypeRGB[4], prgtypeRGB[5]);      	// serier
              break;
          case 2:
              glColor3f(prgtypeRGB[6], prgtypeRGB[7], prgtypeRGB[8]);      	// div
              break;
          case 3:
              glColor3f(prgtypeRGB[9], prgtypeRGB[10], prgtypeRGB[11]);      	// action
              break;
          case 4:
              glColor3f(prgtypeRGB[12], prgtypeRGB[13], prgtypeRGB[14]);      	// nyheder
              break;
          case 5:
              glColor3f(prgtypeRGB[15], prgtypeRGB[16], prgtypeRGB[17]);		// komedier
              break;
          case 6:
              glColor3f(prgtypeRGB[18], prgtypeRGB[19], prgtypeRGB[20]);      	// underholdning
              break;
          case 7:
              glColor3f(prgtypeRGB[21], prgtypeRGB[22], prgtypeRGB[23]);      	// music
              break;
          case 8:
              glColor3f(prgtypeRGB[24], prgtypeRGB[25], prgtypeRGB[26]);      	// andet
              break;
          case 9:
              glColor3f(prgtypeRGB[27], prgtypeRGB[28], prgtypeRGB[29]);		// sifi
              break;
          case 10:
              glColor3f(prgtypeRGB[30], prgtypeRGB[31], prgtypeRGB[32]);		// ukdentd
              break;
          case 11:
              glColor3f(prgtypeRGB[33], prgtypeRGB[34], prgtypeRGB[35]);		// rejser
              break;
          case 12:
              glColor3f(prgtypeRGB[36], prgtypeRGB[37], prgtypeRGB[38]);		//
              break;
          case 13:
              glColor3f(prgtypeRGB[39], prgtypeRGB[40], prgtypeRGB[41]);		// ukendt
              break;
          case 14:
              glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		// rejser
              break;
          default:
              glColor3f(prgtypeRGB[42], prgtypeRGB[43], prgtypeRGB[44]);		// rejser
              break;
      }
      glBindTexture(GL_TEXTURE_2D,_tv_prgtype);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBegin(GL_QUADS); //Begin quadrilateral coordinates120
      glTexCoord2f(0.0, 0.0); glVertex3f(xpos, ypos, 0.0);
      glTexCoord2f(0.0, 1.0); glVertex3f(xpos, ypos+ysiz, 0.0);
      glTexCoord2f(1.0, 1.0); glVertex3f(xpos+xsiz, ypos+ysiz, 0.0);
      glTexCoord2f(1.0, 0.0); glVertex3f(xpos+xsiz, ypos, 0.0);
      glEnd(); //End quadrilateral coordinates
      glPopMatrix();


      // text
      glPushMatrix();
      glColor4f(1.0f,1.0f,1.0f,1.0f);
      glTranslatef(480+(iii*100),70,0.0f);
      glScalef(20.0, 20.0,1);
      sprintf(tmptxt,"%s",prgtypee[i+1]);
      glcRenderString(tmptxt);
      glPopMatrix();


      i+=2;
      iii+=1;
    }

}









// viser teksten i fast tvguide

void tv_oversigt::show_fasttv_oversigt(int selectchanel,int selectprg)
{
    char tmptxt[250];
    char tmptxt1[200];
    char tmptxt2[200];
    int kanalnr=0;
//    int tidspunkt=0;
    int kanalantal;
    int prglength;
    int pstartofset;
    int upstartofset;
    struct tm * timeinfo;
    int i,j,k,pos;
    int iii;
    const int MAXLINES=40;
//    const int MAXLINESLENGTH=80;
    i = pos = 0;
    int N=36;
    char word[1600];
    char str[16000];
    char desclines[40][80];
    for(int ii=0;ii<40;ii++) desclines[ii][0]='\0';
    if (selectchanel>11) pstartofset=selectchanel-11;
    else pstartofset=0;

    if (selectprg>11) upstartofset=selectprg-11;
    else upstartofset=0;

//printf("startofset %d \n",starttidofset);
    glColor3f(1.0f, 1.0f, 1.0f);      				// color
    switch(screen_size) {
        case 1: kanalantal=12;
                break;
        case 2: kanalantal=12;
                break;
        case 3: kanalantal=12;
                break;
        case 4: kanalantal=12;
                break;
        default:kanalantal=12;
    }

    //aktivfont.selectfont("Umpush");
    aktivfont.selectfont((char *) "Norasi");


    // show time/date in the guide
    glLoadIdentity();
    glTranslatef(-34.0f, (23.6)+2.6, -80.0f);
    glScalef(3.6, 3.6, 3.6);
    starttid=time( NULL );
    timeinfo=localtime(&starttid);
    sprintf(tmptxt,"TV Guiden %s den %d-%d-%d ",ugedage[timeinfo->tm_wday],timeinfo->tm_mday,(timeinfo->tm_mon)+1,(timeinfo->tm_year)+1900);
    glcRenderString(tmptxt);

    glColor3f(1.0f,1.0f,1.0f);
    glDisable(GL_TEXTURE_2D);
    while (kanalnr<12) {				// kanalantal
        glLoadIdentity();
        switch (screen_size) {
            case 1: glTranslatef(-62.8+3, (23.6)-(kanalnr*(4.6)), -110.0f);	// 47.8 = center/55.8 left align
                    break;
            case 2: glTranslatef(-62.8+3, (23.6)-(kanalnr*(4.6)), -110.0f);
                    break;
            case 3: glTranslatef(-88.8+3, (23.6)-(kanalnr*(4.6)), -110.0f);
                    break;
            case 4: glTranslatef(-85.8+3, (23.6)-(kanalnr*(4.6)), -110.0f);
                    break;
        }
        if (kanalnr==selectchanel-pstartofset)
            glColor3f(1.0f,1.0f,1.0f);
        else
            glColor3f(0.5f,0.5f,0.5f);
        glScalef(1.6, 1.6,1.6);
        sprintf(tmptxt,"%16s",tvkanaler[kanalnr+pstartofset].chanel_name);
        glcRenderString(tmptxt);
        kanalnr++;
    }

    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(-48.8+3, (27.6), -110.0f);	// 47.8 = center/55.8 left align
                break;
        case 2: glTranslatef(-58.8+3, (27.6), -110.0f);
                break;
        case 3: glTranslatef(-72.8+3, (27.6), -110.0f);
                break;
        case 4: glTranslatef(-70.8+3, (27.6), -110.0f);
                break;
    }

    i=0;
    while(i<12) {
        prglength=tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].program_length_minuter;
        glLoadIdentity();
        switch (screen_size) {
            case 1: glTranslatef(-46.8+2, (23.6)-(i*(4.6)), -110.0f);	// 47.8 = center/55.8 left align
                      break;
            case 2: glTranslatef(-46.8+2, (23.6)-(i*(4.6)), -110.0f);
                    break;
            case 3: glTranslatef(-72.8+4, (23.6)-(i*(4.6)), -110.0f);
                    break;
            case 4: glTranslatef(-68.8+2, (23.6)-(i*(4.6)), -110.0f);
                    break;
        }
        if (i==selectprg-upstartofset) glColor3f(1,1,1);
        else glColor3f(0.5f,0.5f,0.5f);
        glScalef(1.8, 1.8,1.8);
        if (strlen(tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].program_navn)>0) {
            sprintf(tmptxt1,"%s",tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].starttime+11);
            tmptxt1[5]='\0';
            sprintf(tmptxt2,"%s",tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].endtime+11);
            tmptxt2[5]='\0';
            sprintf(tmptxt,"%s-%s %s ",tmptxt1,tmptxt2,tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].program_navn);
            if (strlen(tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].sub_title)>0) {
//                strcat(tmptxt,"- ");
                strcat(tmptxt,tvkanaler[selectchanel+pstartofset].tv_prog_guide[i+upstartofset].sub_title);
            }
            glcRenderString(tmptxt);
        } else {
            glcRenderString("Ingen program information.");
        }
        i++;
    }



    strcpy(str,tvkanaler[selectchanel].tv_prog_guide[selectprg].description);
    iii=0;			// aktiv linie i array
    i=0;
    while( 1 ) {
        j = 0;
        while( !isspace( str[i]) ) // Find next word
        {
            if( str[i] == '\0' )
                break;
            word[j] = str[i];
            i++;
            j++;
        }

        word[j] = '\0'; // The value of j is the word length

        if( j > N ) // Word length is greater than column length
        { // print char-by-char
            k = 0;
            while( word[k] != '\0' ) {
                if( pos >= N-1 ) {
                    if( k != 0 ) {
                        desclines[iii][strlen(desclines[iii])]='-';
                        desclines[iii][strlen(desclines[iii])+1]='\0';
                        //putchar('-'); // print '-' if last-but-one column
                    }
                    pos = 0;
                    //putchar('\n');
                    if (iii<MAXLINES) iii++;
                }
                desclines[iii][strlen(desclines[iii])]=word[k];
                //putchar( word[k] );
                pos++;
                k++;
            }
        } else {
            if( pos + j > N ) // Word doesn't fit in line
            {
                //putchar('\n');
                pos = 0;
                if (iii<MAXLINES) iii++;
            }
            strcat(desclines[iii],word);				// add word
            //printf("%s", word );
            pos += j;
        }
        if( pos < N-1 ) // print space if not last column
        {
            strcat(desclines[iii]," ");
            //putchar(' ');
            pos++;
        }
        if( str[i] == '\0' ) break;
        i++; // Skip space
    }

    // show tv prgoram subtitle
    glLoadIdentity();
    glColor3f(1,1,1);
//    glScalef(3.8, 3.8, 3.8);
    glRotatef(45.0f,0.0f, 0.0f, 0.0f);
    switch(screen_size) {
        case 1:	glTranslatef(39.0f-30.0f, 0.0f+12.5f, -60.0f);
                break;
        case 2:	glTranslatef(39.0f-30.0f, 0.0f+12.5f, -60.0f);
                break;
        case 3:	glTranslatef(48.0f-30.0f, 0.0f+12.5f, -60.0f);
                break;
        case 4:	glTranslatef(50.0f-30.0f, 0.0f+12.5f, -60.0f);
                break;
    }
    glcRenderString(tvkanaler[selectchanel].tv_prog_guide[selectprg].sub_title);

    //
    // draw tv program big info
    //
    i=0;
    while(i<22) {
        if (strcmp(desclines[i],"")!=0) {
            glLoadIdentity();
            glcScale(3.0f,3.0f);
            glColor3f(1.0f, 1.0f, 1.0f);
            switch(screen_size) {
                case 1:	glTranslatef(39.0f-30.0f, 0.0f+10.5f-(i*1.2f), -60.0f);
                        break;
                case 2:	glTranslatef(39.0f-30.0f, 0.0f+10.5f-(i*1.2f), -60.0f);
                        break;
                case 3:	glTranslatef(48.0f-30.0f, 0.0f+10.5f-(i*1.2f), -60.0f);
                        break;
                case 4:	glTranslatef(50.0f-30.0f, 0.0f+10.5f-(i*1.2f), -60.0f);
                        break;
            }
            glcRenderString(desclines[i]);
        }
        i++;
    }
    // show clock
    glLoadIdentity();
    sprintf(tmptxt,"%02d:%02d",timeinfo->tm_hour,timeinfo->tm_min);
    glcScale(3.0f,3.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    switch(screen_size) {
        case 1: glTranslatef(8.0f, -8.0f, -22.0f);
                break;
        case 2: glTranslatef(8.0f, -8.0f, -22.0f);
                break;
        case 3: glTranslatef(8.0f, -8.0f, -22.0f);
                break;
        case 4: glTranslatef(8.0f, -8.0f, -22.0f);
                break;
    }
//    glcRenderStyle(GLC_BITMAP);
    glcRenderString(tmptxt);    				// render clock
//    glcRenderStyle(GLC_TRIANGLE );


    glEnable(GL_TEXTURE_2D);
    // old recordeings button mask
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
        case 2: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
        case 3: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
        case 4: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glBindTexture(GL_TEXTURE_2D, _tvmaskprgrecordedbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-10, 5, 0.0);
    glEnd();

    // old recordings button gfx
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
        case 2: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
        case 3: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
        case 4: glTranslatef(0.0f, -39.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
                //            glColor4f(1.0f,1.0f,1.0f,1.0f);
                //            glBlendFunc(GL_ONE, GL_ONE);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, _tvoldprgrecordedbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(44);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-10, 5, 0.0);
    glEnd();

    // End button 1 (old recordings)

    // new recordeings button mask
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
        case 2: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
        case 3: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
        case 4: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glBindTexture(GL_TEXTURE_2D, _tvmaskprgrecordedbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-10, 5, 0.0);
    glEnd();

    // new recordings button gfx
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
        case 2: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
        case 3: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
        case 4: glTranslatef(25.0f, -39.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
                //            glColor4f(1.0f,1.0f,1.0f,1.0f);
                //            glBlendFunc(GL_ONE, GL_ONE);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, _tvnewprgrecordedbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(45);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-10, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(10, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(10, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-10, 5, 0.0);
    glEnd();

    // End button 2 (new recordings)

}


//
// viser et prgrams record info.
//

void tv_oversigt::showandsetprginfo(int kanalnr,int tvprgnr) {
    char tmptxt[200];
    time_t aktueltid;
    time_t prgtid;
    time(&aktueltid);					// hent hvad klokken er
    struct tm *timeinfo;
    struct tm prgtidinfo;
    static int antalrec=-1;
    timeinfo=localtime(&aktueltid);				// convert to localtime


    glLoadIdentity();
    glScalef(3.6f, 3.6f, 3.6f);
    glDisable(GL_BLEND);
//    glDisable(GL_TEXTURE);
    switch (screen_size) {
        case 1: glTranslatef(-20.0f, 8.0f, -60.0f);
                break;
        case 2: glTranslatef(-20.0f, 8.0f, -60.0f);
                break;
        case 3: glTranslatef(-20.0f, 8.0f, -60.0f);
                break;
        case 4: glTranslatef(-20.0f, 8.0f, -60.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D,0);
    glcRenderString("Kanal");
    glLoadIdentity();
    glTranslatef(-14.0f, 8.0f, -60.0f);
    sprintf(tmptxt,"%s",tvkanaler[kanalnr].chanel_name);
    glcRenderString(tmptxt);

    glLoadIdentity();
    glTranslatef(-20.0f, 8.0f-1.2f, -60.0f);
    glcRenderString("Navn");
    glLoadIdentity();
    glTranslatef(-14.0f, 8.0f-1.2f, -60.0f);
    sprintf(tmptxt,"%s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn);
    glcRenderString(tmptxt);


    glLoadIdentity();
    glTranslatef(-20.0f, 8.0f-2.4f, -60.0f);
    glcRenderString("Start");
    glLoadIdentity();
    glTranslatef(-14.0f, 8.0f-2.4f, -60.0f);
    sprintf(tmptxt,"%s",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime);
    glcRenderString(tmptxt);

    glLoadIdentity();
    glTranslatef(-20.0f, 8.0f-3.6f, -60.0f);
    glcRenderString("Længde ");
    glLoadIdentity();
    glTranslatef(-14.0f, 8.0f-3.6f, -60.0f);
    sprintf(tmptxt,"%d min.",tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_length_minuter);
    glcRenderString(tmptxt);



    glLoadIdentity();
    glTranslatef(-20.0f, 8.0f-4.8f, -60.0f);
    glcRenderString("Type ");
    glLoadIdentity();
    glTranslatef(-14.0f, 8.0f-4.8f, -60.0f);
    if (tvkanaler[kanalnr].tv_prog_guide[tvprgnr].prg_type<=7)
        sprintf(tmptxt,"%s",prgtypee[tvkanaler[kanalnr].tv_prog_guide[tvprgnr].prg_type]);
    else strcpy(tmptxt,"");
    glcRenderString(tmptxt);

    if (antalrec==-1) antalrec=tvprgrecordedbefore(tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn,tvkanaler[kanalnr].chanid);

    glLoadIdentity();
    glTranslatef(-20.0f, 8.0f-6.0f, -60.0f);
    glcRenderString("Recorded ");
    glLoadIdentity();
    glTranslatef(-14.0f, 8.0f-6.0f, -60.0f);
    sprintf(tmptxt,"%d times before.",antalrec);
    glcRenderString(tmptxt);


    if (strptime(tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime,"%Y-%m-%d %H:%M:%S",&prgtidinfo)==NULL) {
        printf("RECORDED PROGRAM DATE FORMAT ERROR can't convert. by strptime\n");
    }
    prgtid=mktime(&prgtidinfo);

    //printf("kl er %d prg  starttid er %d diff is %f \n",aktueltid,prgtid,difftime(aktueltid,prgtid));

    sprintf(tmptxt,"%d",tvkanaler[kanalnr].chanid);

    if ((difftime(aktueltid,prgtid)<=0) && (tvprgrecorded(tvkanaler[kanalnr].tv_prog_guide[tvprgnr].starttime,tvkanaler[kanalnr].tv_prog_guide[tvprgnr].program_navn,tmptxt)==0)) {
        // record button mask
        glLoadIdentity();
        switch (screen_size) {
            case 1: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
            case 2: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
            case 3: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
            case 4: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
        }
        glColor3f(1.0f, 1.0f, 1.0f);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_DST_COLOR, GL_ZERO);

        // error
        glBindTexture(GL_TEXTURE_2D, _textureId13);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-5, -5, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(5, -5, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(5, 5, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-5, 5, 0.0);
        glEnd();

        // record button gfx
        glLoadIdentity();
        switch (screen_size) {
            case 1: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
            case 2: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
            case 3: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
            case 4: glTranslatef(5.0f, 5.0f, -110.0f);
                    break;
        }
        glColor3f(1.0f, 1.0f, 1.0f);
                    //            glColor4f(1.0f,1.0f,1.0f,1.0f);
                    //            glBlendFunc(GL_ONE, GL_ONE);
        glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
        glBlendFunc(GL_ONE, GL_ONE);
        glBindTexture(GL_TEXTURE_2D, _tvrecordbutton);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glLoadName(41);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(-5, -5, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(5, -5, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(5, 5, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-5, 5, 0.0);
        glEnd();
        // End button 1
    }

    // cancel button mask
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
        case 2: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
        case 3: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
        case 4: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_DST_COLOR, GL_ZERO);
    glBindTexture(GL_TEXTURE_2D, _textureId13);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-5, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(5, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(5, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-5, 5, 0.0);
    glEnd();

    // cancel button gfx
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
        case 2: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
        case 3: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
        case 4: glTranslatef(5.0f, -10.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
                //            glColor4f(1.0f,1.0f,1.0f,1.0f);
                //            glBlendFunc(GL_ONE, GL_ONE);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, _tvrecordcancelbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(40);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-5, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(5, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(5, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-5, 5, 0.0);
    glEnd();

    // End button 1
}

//
//*******************************************************************************************************************************//
//

//
// construktor
//


earlyrecorded::earlyrecorded() {
    for(int i=0;i<199;i++) {
        strcpy(this->programinfo[i].name,"");
        strcpy(this->programinfo[i].dato,"");
    }
}


//
// load recorded history not programs
//


void earlyrecorded::earlyrecordedload(char *mysqlhost,char *mysqluser,char *mysqlpass) {
    int i;
    // mysql vars
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *database = (char *) "mythconverg";
    // mysql stuf
    char sqlselect[200];
    strcpy(sqlselect,"SELECT title,starttime FROM recorded order by starttime limit 60");
    conn=mysql_init(NULL);
    // Connect to database
    mysql_real_connect(conn, mysqlhost,mysqluser, mysqlpass, database, 0, NULL, 0);
    mysql_query(conn,"set NAMES 'utf8'");
    res = mysql_store_result(conn);
    mysql_query(conn,sqlselect);
    res = mysql_store_result(conn);
    i=0;
    if (res) {
        while (((row = mysql_fetch_row(res)) != NULL) && (i<=200)) {
            strcpy(this->programinfo[i].dato,row[1]);
            strcpy(this->programinfo[i].name,row[0]);
            i++;
        }
    }
    this->antal=i--;
}


//
// viser liste over record(historie) som har været optaget en gang.
// dvs det er ike optagelser som ligger
//

void earlyrecorded::showearlyrecorded() {
    int i;
    glColor3f(1.0f, 1.0f, 1.0f);
    // img bg
    glLoadIdentity();
    glEnable(GL_TEXTURE);
    glTranslatef(-10.0f, 0.0f, -110.0f);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, _tvoldrecorded);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-50.0, -33.0, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f( 70.0, -33.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f( 70.0, 30.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-50.0, 30.0, 0.0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLoadIdentity();
    glTranslatef(-4.5f, 9.7f, -40.0f);
    glcRenderString(oldrecordning);

    i=0;
    glColor3f(0.8f, 0.8f, 0.8f);
    while((i<this->antal) && (i<40)) {
        if (strcmp(this->programinfo[i].name,"")!=0) {
            glLoadIdentity();
            if (i<20) glTranslatef(-30.0f, 12.5f-(i*1.2f), -60.0f); else glTranslatef(5.0f, 12.5f-((i-20)*1.2f), -60.0f);
            glcRenderCountedString(16,this->programinfo[i].dato);
            glLoadIdentity();
            if (i<20) glTranslatef(-22.0f, 12.5f-(i*1.2f), -60.0f); else glTranslatef(13.0f, 12.5f-((i-20)*1.2f), -60.0f);
            glcRenderString(this->programinfo[i].name);
        }
        i++;
    }

    // Close button
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
        case 2: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
        case 3: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
        case 4: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, _tvrecordcancelbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(40);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-7.5f, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(7.5f, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(7.5f, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-7.5f, 5, 0.0);
    glEnd();

    // End button 1

}


// ***********************************************************************************************************************************


char *read_stream(char *stream,size_t size, void *d) {
    char *c=fgets(stream,size,(FILE *) d);
    return c;
}

//
// load recorded history not programs
//


void earlyrecorded::getrecordprogram(char *mysqlhost,char *mysqluser,char *mysqlpass) {
    int i=-1;
    bool startcoll=false;
//    icalvalue *v;
    char* line;
    char endtid[200];
    FILE* stream;
//    icalcomponent *c;
    /* Create a new parser object */
    icalparser *parser = icalparser_new();
    stream=fopen("rectv.rss","r");
    /* Tell the parser what input routie it should use. */
    if (stream) {
        icalparser_set_gen_data(parser,stream);
        do {
            strcpy(endtid,"");
            line = icalparser_get_line(parser,read_stream);
            if (line) {
//            printf("LINE IS = %s i is = %d \n",line,i);
//      	  c = icalparser_add_line(parser,line);
                if (strncmp(line,"BEGIN:VEVENT",12)==0) {
                    i++;
                    startcoll=true;
                    strcpy(endtid,"");					// reset
                }
                if ((i>=0) && (startcoll)) {
                    if (strncmp(line,"DTSTART:",8)==0) {
                        strncpy(this->programinfo[i].dato,line+8,4);		// hente dato
                        strcat(this->programinfo[i].dato,"-");			//
                        strncat(this->programinfo[i].dato,line+12,2);
                        strcat(this->programinfo[i].dato,"-");
                        strncat(this->programinfo[i].dato,line+14,2);
                        strcat(this->programinfo[i].dato," ");
                        strncat(this->programinfo[i].dato,line+17,2);             	// hente tidspunkt
                        strcat(this->programinfo[i].dato,":");
                        strncat(this->programinfo[i].dato,line+19,2);             	// hente tidspunkt
                    }
                    if (strncmp(line,"DTEND:",6)==0) {
                        strncat(this->programinfo[i].endtime,line+15,2);              // hente tid
                        strcat(this->programinfo[i].endtime,":");
                        strncat(this->programinfo[i].endtime,line+17,2);
                    }
                    if (strncmp(line,"SUMMARY:",8)==0) {
                        strcpy(this->programinfo[i].name,line+8);			// hente dato
                    }
                    if (strncmp(line,"LOCATION:",9)==0) {

                    }
                }
                if (strncmp(line,"END:VEVENT",10)==0) {
                    startcoll=false;
                }
            }
        } while ((line!=0) && (i<21));
        this->antal=i;
        icalparser_free(parser);
    }
    if (stream) fclose(stream);
}



//
// viser liste over tv programmer som skal optages.
//

void earlyrecorded::showtvreclist() {
//    char tmptxt[200];
    int i;
//    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);

    glColor4f(1.0f, 1.0f, 1.0f,1.0f);
        // mask
    glLoadIdentity();
    glTranslatef(-10.0f, 0.0f, -110.0f);
    glEnable(GL_TEXTURE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    // img
    glLoadIdentity();
    glEnable(GL_TEXTURE);
    glTranslatef(-10.0f, 0.0f, -110.0f);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, _tvoldrecorded);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-50.0, -33.0, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f( 70.0, -33.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f( 70.0, 30.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-50.0, 30.0, 0.0);
    glEnd();
    // overskrift
    glLoadIdentity();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE);
    glBindTexture(GL_TEXTURE_2D,0);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(-6.0f, 9.7f, -40.0f);
    glcRenderString(upcommingrec);
    i=0;
    glColor4f(1.0f, 1.0f, 1.0f,1.0f);
    while((i<this->antal) && (i<50)) {
        if (strcmp(this->programinfo[i].name,"")!=0) {
            glLoadIdentity();
            if (i<25) glTranslatef(-30.0f, 12.5f-(i*1.2f), -60.0f); else glTranslatef(5.0f-3.4f, 12.5f-((i-25)*1.2f), -60.0f);
            glcRenderString(this->programinfo[i].dato);
            glcRenderString("-");
            glcRenderString(this->programinfo[i].endtime);
            glLoadIdentity();
            if (i<25) glTranslatef(-18.5f, 12.5f-(i*1.2f), -60.0f); else glTranslatef(12.0f-3.4f, 12.5f-((i-25)*1.2f), -60.0f);
            glcRenderString(this->programinfo[i].name);
        }
        i++;
    }



    // Close button
    glLoadIdentity();
    switch (screen_size) {
        case 1: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
        case 2: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
        case 3: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
        case 4: glTranslatef(0.0f, -25.0f, -110.0f);
                break;
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, _tvrecordcancelbutton);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glLoadName(40);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-7.5f, -5, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(7.5f, -5, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(7.5f, 5, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-7.5f, 5, 0.0);
    glEnd();

    // End button 1


}
