//
// vlc loader/player controller
//
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <vlc/vlc.h>
#include <pthread.h>
#include <unistd.h>
#include <curl/curl.h>
#include <iostream>
#include "myth_vlcplayer.h"


// vlc player class stuf
// constructor


void vlc_controller::createTexture() {
    glGenTextures(1,&videoTexture);
    glBindTexture(GL_TEXTURE_2D,videoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
}


// ****************************************************************************************
//
// Constructor
//
// ****************************************************************************************


vlc_controller::vlc_controller() {
    const char *args[] = {
      "--no-video-title-show",
      "--no-xlib",
      "--sub-autodetect-file",
      "--sub-autodetect-path",
      "--sub-file",
      "--sub-language=da"
    };

    vlc_inst = libvlc_new(sizeof(args)/sizeof(args[0]), args);
    pixels.resize(width*height*4);
    is_playing=false;
    videoTexture = 0;
    is_pause=false;
}



// ****************************************************************************************
//
// Destructor
//
// ****************************************************************************************


vlc_controller::~vlc_controller() {
    stopmedia();
    if(videoTexture) glDeleteTextures(1,&videoTexture);
    if(vlc_inst) libvlc_release(vlc_inst);
}

// init opengl for video texture

void vlc_controller::initOpenGL() {
    if(videoTexture != 0)
        return;
    glGenTextures(1,&videoTexture);
    glBindTexture(GL_TEXTURE_2D,videoTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_BGRA,
        GL_UNSIGNED_BYTE,
        nullptr);
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR);
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
}

void *vlc_controller::lock(void *opaque,void **planes) {
  vlc_controller *v = (vlc_controller*)opaque;
  v->mutex.lock();
  *planes = v->pixels.data();
  return nullptr;
}



void vlc_controller::unlock( void *opaque,void *, void *const *) {
  vlc_controller *v = (vlc_controller*)opaque;
  v->newFrame=true;
  v->mutex.unlock();
}



void vlc_controller::display(void *opaque, void *) {
    // VLC kalder denne efter frame
}




void vlc_controller::frem10sec() {
    if(!vlc_mp) return;
    libvlc_media_player_set_time(vlc_mp,libvlc_media_player_get_time(vlc_mp) + 10000);
}

void vlc_controller::tilbage10sec() {
    if(!vlc_mp) return;
    libvlc_media_player_set_time(vlc_mp,libvlc_media_player_get_time(vlc_mp) - 10000);    
}


void vlc_controller::frem60sec() {
    if(!vlc_mp) return;
    libvlc_media_player_set_time(vlc_mp,libvlc_media_player_get_time(vlc_mp) + 60000);
}

void vlc_controller::tilbage60sec() {
    if(!vlc_mp) return;
    libvlc_media_player_set_time(vlc_mp,libvlc_media_player_get_time(vlc_mp) - 60000);    
}

// ****************************************************************************************
//
// retunere true if playing
//
// ****************************************************************************************


bool vlc_controller::vlc_in_playing() {
    if(!vlc_mp) return false;
    return libvlc_media_player_is_playing(vlc_mp);
}


// ****************************************************************************************
//
// get subtitle tracks from movie file (if any).
//
// ****************************************************************************************


void vlc_controller::GetSubtitleTracks() {
  subtitle_def subtitle;
  if (!vlc_mp)
    return;
  if (subtitleTracksLoaded==false) {
    libvlc_track_description_t* tracks = libvlc_video_get_spu_description(vlc_mp);
    for (libvlc_track_description_t* track = tracks; track != nullptr; track = track->p_next) {
      subtitleTracksLoaded = true;
      std::printf("Subtitle ID: %d, navn: %s\n",track->i_id, track->psz_name ? track->psz_name : "Ukendt");
      // add subtitle to vector
      subtitle.subtitle_id=0;
      subtitle.subtitle_name="";
      subtitle_tracks.push_back(subtitle);
    }
    if (tracks==nullptr) printf("No subtitle tracks.\n");
    libvlc_track_description_list_release(tracks);
  }
}



// ****************************************************************************************
//
// Select subtitle tracks from movie file to show in movie
//
// ****************************************************************************************

bool vlc_controller::SelectSubtitle(const std::string& search) {
  if (!vlc_mp)
      return false;
  libvlc_track_description_t* tracks = libvlc_video_get_spu_description(vlc_mp);
  if (!tracks)
      return false;
  bool selected = false;
  for (libvlc_track_description_t* track = tracks; track != nullptr; track = track->p_next) {
    std::string name = track->psz_name ? track->psz_name : "";
    if (name.find(search) != std::string::npos) {
      selected = libvlc_video_set_spu(vlc_mp, track->i_id) == 0;
      break;
    }
  }
  libvlc_track_description_list_release(tracks);
  return selected;
}


// ****************************************************************************************
//
// get audio tracks list
//
// ****************************************************************************************


int vlc_controller::GetAudioTracks() {
  libvlc_track_description_t *tracks=nullptr;
  libvlc_track_description_t *track=nullptr;
  audiotracks_def audiotrack;
  if (vlc_mp == nullptr)
    return(0);
  if (libvlc_media_player_is_playing(vlc_mp) && !audioTracksLoaded) {
    int count = libvlc_audio_get_track_count(vlc_mp);
    if (count > 0) {
      std::printf("Antal lydspor: %d\n", count);
      audioTracksLoaded = true;
      tracks = libvlc_audio_get_track_description(vlc_mp);
      track = tracks;
      while (track != nullptr) {
        printf("ID=%d, navn=%s\n",track->i_id,track->psz_name != nullptr ? track->psz_name : "Ukendt");
        audiotrack.track_id=track->i_id;
        audiotrack.track_name=track->psz_name;
        audio_tracks.push_back(audiotrack);
        track = track->p_next;
      }
    }
    if (tracks != nullptr) {
      libvlc_track_description_list_release(tracks);
    }
  }
  return(1);
}



// ****************************************************************************************
//
// Return active audio track name
//
// ****************************************************************************************


std::string vlc_controller::active_audiotrack_name() {
  // audiotrack_active
  if (audio_tracks.size()>0) return(audio_tracks.at(1).track_name); 
    else return("");
}


// ****************************************************************************************
//
// Return active audio track id
//
// ****************************************************************************************

int vlc_controller::active_audiotrack_id() {
  // audiotrack_active
  if (audio_tracks.size()>0) return(audio_tracks.at(1).track_id);
    else return(0);
}


// ****************************************************************************************
//
// do play function by use (libvlc)
//
// ****************************************************************************************


int vlc_controller::playmedia(char *path) {
  if (!vlc_inst)
    return 0;
  // Stop gammel film først vis den køre
  if (vlc_mp) {
    libvlc_media_player_stop(vlc_mp);
    libvlc_media_player_release(vlc_mp);
    vlc_mp = nullptr;
  }
  libvlc_media_t *media = libvlc_media_new_path( vlc_inst, path);
  if(!media) 
    return 0;
  vlc_mp = libvlc_media_player_new_from_media(media);
  libvlc_media_release(media);                                                      // release file handler
  if(!vlc_mp)
    return 0;
  libvlc_video_set_callbacks(vlc_mp,lock,unlock,display,this);                      // set display out to texture
  libvlc_video_set_format(vlc_mp,"RV32",width,height,width*4);                      // set size
  libvlc_audio_set_mute(vlc_mp, false);
  if(libvlc_media_player_play(vlc_mp)!=0) {                                         // start play
    return 0;
  }
  is_playing=true;
  return 1;
}


// ****************************************************************************************
//
// do play function by use (libvlc)
//
// ****************************************************************************************


int vlc_controller::playmedia(const char* path,const char* subtitlePath) {
  if (!vlc_inst || !path)
      return 0;
  // Stop og frigiv gammel afspiller
  if (vlc_mp) {
    libvlc_media_player_stop(vlc_mp);
    libvlc_media_player_release(vlc_mp);
    vlc_mp = nullptr;
  }
  // Opret videomedie
  libvlc_media_t* media = libvlc_media_new_path(vlc_inst, path);
  if (!media)
    return 0;
  vlc_mp = libvlc_media_player_new_from_media(media);
  libvlc_media_release(media);
  if (!vlc_mp)
    return 0;
  // Send video til din OpenGL-texture
  libvlc_video_set_callbacks(vlc_mp,lock,unlock,display,this);
  libvlc_video_set_format( vlc_mp,"RV32", width,height,width * 4);
  libvlc_audio_set_mute(vlc_mp, false);
  // Indlæs ekstern undertekst
  if (subtitlePath && subtitlePath[0] != '\0') {
    libvlc_media_t* subtitleMedia = libvlc_media_new_path(vlc_inst, subtitlePath);
    if (subtitleMedia) {
      char* subtitleUri = libvlc_media_get_mrl(subtitleMedia);
      if (subtitleUri) {
          // vælg underteksten automatisk
        int result = libvlc_media_player_add_slave(vlc_mp,libvlc_media_slave_type_subtitle,subtitleUri,true);
        if (result != 0) {
          std::fprintf(stderr,"Kunne ikke indlæse undertekst: %s\n",libvlc_errmsg());
        }
        libvlc_free(subtitleUri);
      }
      libvlc_media_release(subtitleMedia);
    }
  }
  // *****************************************************************************************************
  //
  // start player
  //
  // *****************************************************************************************************
  // Start afspilning
  if (libvlc_media_player_play(vlc_mp) != 0) {
      libvlc_media_player_release(vlc_mp);
      vlc_mp = nullptr;
      is_playing = false;
      return 0;
  }
  is_playing = true;
  return 1;
}


// update vlc call back texture


void vlc_controller::updateTexture() {
    if(!newFrame)
        return;
    mutex.lock();
    glBindTexture( GL_TEXTURE_2D, videoTexture);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height,GL_BGRA,GL_UNSIGNED_BYTE,pixels.data());
    glBindTexture(GL_TEXTURE_2D,0);
    newFrame=false;
    mutex.unlock();
}



// ****************************************************************************************
//
// web player
//
// ****************************************************************************************


static size_t discardData(void* ptr,size_t size,size_t nmemb, void* userdata) {
    return size * nmemb;
}

std::string resolvePodcastUrl(const std::string& originalUrl) {
    CURL* curl = curl_easy_init();
    if (!curl) 
      return originalUrl;
    curl_easy_setopt(curl,CURLOPT_URL, originalUrl.c_str());
    curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
    // Podcast-URL'er kan have mange tracking-redirects
    curl_easy_setopt(curl,CURLOPT_MAXREDIRS,30L);
    curl_easy_setopt(curl,CURLOPT_USERAGENT,"Mozilla/5.0");
    curl_easy_setopt(curl,CURLOPT_ACCEPT_ENCODING,"");
    curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,10L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);
    // Bed kun om den første byte
    curl_easy_setopt(curl,CURLOPT_RANGE,"0-0");
    // Vi skal ikke gemme MP3-data
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,discardData);
    CURLcode result = curl_easy_perform(curl);
    std::string finalUrl = originalUrl;
    if (result == CURLE_OK) {
        char* effectiveUrl = nullptr;
        curl_easy_getinfo(
          curl,
          CURLINFO_EFFECTIVE_URL,
          &effectiveUrl
        );
        if (effectiveUrl)
          finalUrl = effectiveUrl;
    } else {
        std::cerr
          << "Kunne ikke resolve URL: "
          << curl_easy_strerror(result)
          << '\n';
    }
    curl_easy_cleanup(curl);
    return finalUrl;
}



int vlc_controller::playwebmedia(char *path) {
  int error=0;
  libvlc_media_t *vlc_m;
  if (!vlc_inst || !path || path[0] == '\0')
    return 0;
  // get url
  std::string finalUrl = resolvePodcastUrl(path);
  std::cout << "Original URL:\n"  << path << "\n\n";
  std::cout << "Endelig URL:\n" << finalUrl << '\n';
  vlc_m=libvlc_media_new_location(vlc_inst,finalUrl.c_str());
  if (vlc_m) {
    libvlc_media_add_option(vlc_m, ":network-caching=2000");
    // Create a media player playing environement
    vlc_mp=libvlc_media_player_new_from_media(vlc_m);
    libvlc_media_add_option(vlc_m,"no-video-title-show");
    if (!(vlc_mp)) error=1;
    libvlc_media_release(vlc_m);
    if (!vlc_mp) {
      std::cerr << "Kunne ikke oprette VLC media player: "  << (libvlc_errmsg() ? libvlc_errmsg() : "ukendt fejl") << '\n';
      return 0;
    }
    // start play
    libvlc_audio_set_mute(vlc_mp, 0);
    int volumeResult = libvlc_audio_set_volume(vlc_mp, 100);
    if (libvlc_media_player_play(vlc_mp)!=0) {
      fprintf(stderr, "VLC play fejlede: %s\n", libvlc_errmsg() ? libvlc_errmsg() : "ukendt fejl");
      libvlc_media_player_release(vlc_mp);
      vlc_mp = nullptr;
      return 0;
    }
    // set playing flag in class
    is_playing=true;
  }
  return(1);
}


// ****************************************************************************************
//
// stop play media
//
// ****************************************************************************************



void vlc_controller::stopmedia() {
  if(vlc_mp) {
    libvlc_audio_set_mute(vlc_mp, true);
    libvlc_media_player_stop(vlc_mp);
    usleep(100000); // 100 ms
    libvlc_media_player_release(vlc_mp);
    vlc_mp=nullptr;
    newFrame = false;
  }
  is_playing=false;
}


/* OLD
void vlc_controller::stopmedia() {
  if (vlc_mp) {
    if (libvlc_media_player_is_playing(vlc_mp)) {
      libvlc_media_player_stop(vlc_mp);
      libvlc_media_player_release(vlc_mp);
      is_playing=false;
    }
  } else {
    printf("Error stop movie player\n");
    exit(1);
  }
}
*/

// ****************************************************************************************
//
// ****************************************************************************************

float vlc_controller::jump_position(float ofset) {
  int totallengths;
  float addval;
  float pos=get_position();
  totallengths=(get_length_in_ms()/1000/60);
  addval=(float) totallengths/100;
  //printf("(addval/100) is %f \n ",(addval/100));
  if (ofset>0) {
    if (pos+addval<1.0f) pos+=pos+(addval/100); else if (pos+(addval/100)*10>0.0f) pos-=pos+((addval/100)*10);
  }
  set_position(pos);
  return(pos);
}


// ****************************************************************************************
//
// ****************************************************************************************


unsigned long vlc_controller::get_length_in_ms() {
  libvlc_time_t length;
  length=libvlc_media_player_get_length(vlc_mp);
  return((unsigned long) length);
}

// ****************************************************************************************
//
// return play pos
//
// ****************************************************************************************

unsigned long vlc_controller::get_position() {
  if (vlc_mp) return(libvlc_media_player_get_time(vlc_mp)); else return(0);
}


// ****************************************************************************************
//
// set position
//
// ****************************************************************************************

float vlc_controller::set_position(float pos) {
  libvlc_media_player_set_position(vlc_mp,pos);
  return(pos);
}


// ****************************************************************************************
//
//
//
// ****************************************************************************************

void vlc_controller::pnext_chapter() {
  libvlc_media_player_next_chapter(vlc_mp);
}


// ****************************************************************************************
//
//
//
// ****************************************************************************************

void vlc_controller::plast_chapter() {
  //libvlc_media_player_last_chapter(vlc_mp);
}


// ****************************************************************************************
//
//
//
// ****************************************************************************************


void vlc_controller::volume_up(int volume) {
  libvlc_audio_set_volume(vlc_mp,volume);
}

// ****************************************************************************************
//
//
//
// ****************************************************************************************


void vlc_controller::volume_down(int volume) {
  libvlc_audio_set_volume(vlc_mp,volume);
}

// ****************************************************************************************
//
//
//
// ****************************************************************************************


void vlc_controller::setvolume(int volume) {
  libvlc_audio_set_volume(vlc_mp,volume);
}

// ****************************************************************************************
//
// set play or pause
//
// ****************************************************************************************



void vlc_controller::pause(int pause) {
  if (vlc_mp) {
    if (is_pause) {
      libvlc_media_player_set_pause(vlc_mp,0);
      is_pause=false;
    } else {
      libvlc_media_player_set_pause(vlc_mp,1);
      is_pause=true;
    }
  }
}
