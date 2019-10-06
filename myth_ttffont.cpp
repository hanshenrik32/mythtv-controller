#include <GL/gl.h>
#if defined __APPLE__ && defined __MACH__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>                    // Header File For The GLUT Library
#include <GL/glc.h>                     // glc true type font system
#endif
#include <stdio.h>
#include <string.h>
#include "myth_ttffont.h"

extern GLint ctx, myFont;
extern int debugmode;                   // debugmode

// constructor
fontctrl::fontctrl() {
    int n;
    mastercount=0;
    for (n=0;n<FONT_TYPE_MAX;n++) {
        strcpy(typeinfo[n].fontname,"");
    }
}


// * ttf Font control/loader  **************************************************************


int fontctrl::updatefontlist()
{
    static GLint glc_font_id;
    GLint  master;
    GLint face_count;
    int i;
    size_t flen=0;
    GLint count;
    FILE *fil;
    char *sysc="fc-list | awk -F\":\" '{print $2}' | sed 's/ //' | sort -u > fontlist.txt";
    int ret=system(sysc);                                                       // get font name by fc-list command
    glc_font_id = glcGenFontID();
    glcContext(glc_font_id);
    glcAppendCatalog("/usr/share/fonts/truetype/");
    glcAppendCatalog("/usr/share/fonts/type1/");
    /* Choose a master and a face. */
    mastercount=glcGeti(GLC_MASTER_COUNT);		// gem antal af fonte
    if (debugmode) printf("Numbers of fonts found %d \n",mastercount);
    master = 0;
    i=0;
    fil=fopen("fontlist.txt","r");
    if (fil) {
      while((!(feof(fil))) && (i<FONT_TYPE_MAX-1)) {
        fgets(typeinfo[i].fontname,sizeof(typeinfo[i].fontname),fil);
        if (debugmode & 128) printf("Font name found %s",typeinfo[i].fontname);
        i++;
      }
      mastercount=i;
      fclose(fil);
    }
    return(i);
}





int fontctrl::updatefontlist_old()
{
    static GLint glc_font_id;
    GLint  master;
    GLint face_count;
    int i,j;
    GLint count;
    // Get the number of entries in the catalog list
    /* Get a unique font ID. */
    glc_font_id = glcGenFontID();
    //glc_font_id = glcGenContext();

    glcContext(glc_font_id);
    glcAppendCatalog("/usr/share/fonts/truetype/");
    glcAppendCatalog("/usr/share/fonts/type1/");
    /* Choose a master and a face. */
    mastercount=glcGeti(GLC_MASTER_COUNT);		// gem antal af fonte
    if (debugmode) printf("Numbers of fonts found %d \n",mastercount);
    master = 0;
    i=0;
    // Print the path to the catalogs
    count = glcGeti(GLC_FONT_COUNT);                                       // GLC_FONT_COUNT GLC_CATALOG_COUNT
    if (debugmode) printf("\nTrue type fonts is found in this path\n");
    for (i = 0; i<count; i++) {
      printf("%s\n",(char *) glcGetListc(GLC_CATALOG_LIST, i));
    }
    // load font list
    i=0;
    while((i < (int) mastercount) && (i<FONT_TYPE_MAX)) {
        if (glcGetMasterc(i, GLC_FAMILY)) {
          const GLCchar *font = glcGetMasterc(i, GLC_FAMILY );
          strcpy(typeinfo[i].fontname,(char *) glcGetMasterc(i, GLC_FAMILY));
          if (debugmode & 16) printf("Font named %10s found \n",typeinfo[i].fontname);
          face_count = glcGetMasteri(i, GLC_FACE_COUNT);
          for (j = 0; j < face_count; j++) {
              //if (debugmode & 128) printf(" Face types %s \n ",(char *) glcGetMasterListc(i, GLC_FACE_LIST, j));
              //if (j==0) strcpy(typeinfo[0].fonttype,(char *) glcGetMasterListc(i, GLC_FACE_LIST, j));
              if (j==0) sprintf(typeinfo[i].fontname,"%s",(char *) glcGetMasterListc(i, GLC_FACE_LIST, j)); //glcGetMasterListc(i, GLC_FACE_LIST, j)
              master = i;
          }
        }
        i++;
    }
    return(1);
}



// select font

int fontctrl::selectfont(char *fontname)
{
    glcNewFontFromFamily(myFont, fontname);               // Droid Serif,Ubuntu
    glcFont(myFont);
    if (glcFontFace(myFont, "Bold")==GL_TRUE) {
      //glcFontFace(myFont, "Bold"); // Select the face of my font
    } else if (debugmode) printf("Not a face type font (select error) font name : %s.\n",fontname);
    return(1);
}

// *******************************************************************************
