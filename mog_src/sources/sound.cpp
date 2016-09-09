#include "SDL.h"
#include "SDL_mixer.h"
//#include "SDL_sound.h"
#include "sound.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "MOGtypes.h"

#define MAXMUSIC 2
#define AUDIO_BUFFER	4096
#define MIN(a,b) (a<b?a:b)

/* Paths: */ 
extern char *default_g_path;
extern char *default_s_path;
extern char **g_paths;
extern int n_g_paths,act_g_path;
extern char *g_path;
extern char **s_paths;
extern int n_s_paths,act_s_path;
extern char *s_path;
extern int music_volume;

bool sound_enabled=false;

int currentmusic = 0;
bool playing_music=false;
bool music_loaded[8]={false,false,false,false,false,false,false,false};
Mix_Music *music_sound[8]={0,0,0,0,0,0,0,0};
char music_files[8][128];		/* File names */ 
char music_realfiles[8][128];	/* File names including music directory */ 


bool Sound_initialization(void)
{
    char SoundcardName[256];
	int audio_rate = 44100;
	int audio_channels = 2;
	int audio_bufsize = AUDIO_BUFFER;
	Uint16 audio_format = AUDIO_S16;
	SDL_version compile_version;

	sound_enabled=true;
//	fprintf (stderr, "Initializing SDL_mixer.\n");
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_bufsize))  {
//	  fprintf (stderr, "Unable to open audio: %s\n", Mix_GetError());
	  sound_enabled=false;
//	  fprintf (stderr, "Running the game without audio.\n");
	  return false;	
	} /* if */ 

	SDL_AudioDriverName (SoundcardName, sizeof (SoundcardName));
	Mix_QuerySpec (&audio_rate, &audio_format, &audio_channels);
//	fprintf (stderr, "    opened %s at %d Hz %d bit %s, %d bytes audio buffer\n",
//			 SoundcardName, audio_rate, audio_format & 0xFF,
//			 audio_channels > 1 ? "stereo" : "mono", audio_bufsize);
	MIX_VERSION (&compile_version);
//	fprintf (stderr, "    compiled with SDL_mixer version: %d.%d.%d\n",
//			 compile_version.major,
//			 compile_version.minor,
//			 compile_version.patch);
//	fprintf (stderr, "    running with SDL_mixer version: %d.%d.%d\n",
//			 Mix_Linked_Version()->major,
//			 Mix_Linked_Version()->minor,
//			 Mix_Linked_Version()->patch);


//	Mix_HookMusic(myMusicPlayer, 0);

	return true;
} /* Sound_init */ 

void Sound_release(void)
{
	if (sound_enabled) {
		Mix_CloseAudio();
	} /* if */ 
	sound_enabled=false;
} /* Sound_release */ 


/* a check to see if file is readable and greater than zero */
int file_check(char *fname)
{
	FILE *fp;

	if ((fp=fopen(fname,"r"))!=NULL) {
		if (fseek(fp,0L, SEEK_END)==0 && ftell(fp)>0) {
  			fclose(fp);
			return true;
		} /* if */ 
		/* either the file could not be read (==-1) or size was zero (==0) */ 
		fprintf(stderr,"ERROR in file_check(): the file %s is corrupted.\n", fname);
		fclose(fp);
		exit(1);
	} /* if */ 
	return false;
} /* file_check */ 



SOUNDT Sound_create_sound(char *file,int flags)
{
	int n_ext=6;
	char *ext[6]={".WAV",".OGG",".MP3",".wav",".ogg",".mp3"};
	char name[256],name2[256];
	int i;

	if (sound_enabled) {
		for(i=0;i<n_ext;i++) {
			strcpy(name,file);
			strcat(name,ext[i]);
			sprintf(name2,"%s%s",s_path,name);
			if (file_check(name2)) return Mix_LoadWAV(name2);
		} /* for */ 
		for(i=0;i<n_ext;i++) {
			strcpy(name,file);
			strcat(name,ext[i]);
			sprintf(name2,"%s%s",default_s_path,name);
			if (file_check(name2)) return Mix_LoadWAV(name2);
		} /* for */ 

		fprintf(stderr,"ERROR in Sound_create_sound(): Could not load sound file: %s%s.(wav|ogg|mp3)\n",s_path, file);
		exit(1);
	} else {
		return 0;
	} /* if */ 
} /* Sound_create_sound */ 


void Delete_sound(SOUNDT s)
{
	if (sound_enabled) Mix_FreeChunk(s);
} /* Delete_sound */ 


void Sound_play(SOUNDT s)
{
	if (sound_enabled) Mix_PlayChannel(-1,s,0);
} /* Sound_play */ 


void Sound_obtain_file_name(char *file,char *fullname)
{
	int n_ext=6;
	char *ext[6]={".WAV",".OGG",".MP3",".wav",".ogg",".mp3"};
	char name[256],name2[256];
	int i;

	for(i=0;i<n_ext;i++) {
		strcpy(name,file);
		strcat(name,ext[i]);
		sprintf(name2,"%s%s",s_path,name);
		if (file_check(name2)) {
			strcpy(fullname,name2);
			return;
		} /* if */ 
		strcpy(name,file);
		strcat(name,ext[i]);
		sprintf(name2,"%s%s",default_s_path,name);
		if (file_check(name2)) {
			strcpy(fullname,name2);
			return;
		} /* if */ 

	} /* for */ 

	fprintf(stderr,"ERROR in Sound_obtain_file_name(): Could not find sound file: %s%s.(wav|ogg|mp3)\n",s_path, file);
 	fullname[0]=0;
	exit(1);
} /* Sound_obtain_file_name */ 


Mix_Music *Sound_create_stream(char *file)
{
	int n_ext=6;
	char *ext[6]={".WAV",".OGG",".MP3",".wav",".ogg",".mp3"};
	char name[256],name2[256];
	int i;

//	Sound_AudioInfo inf;
//	inf.format=AUDIO_S16;
//	inf.channels=2;
//	inf.rate=44100;

	if (sound_enabled) {
		for(i=0;i<n_ext;i++) {
			strcpy(name,file);
			strcat(name,ext[i]);
			sprintf(name2,"%s%s",s_path,name);
			if (file_check(name2)) return Mix_LoadMUS(name2);
//			if (file_check(name2)) return Sound_NewSampleFromFile(name2,&inf,AUDIO_BUFFER);
//			if (file_check(name2)) return Sound_NewSampleFromFile(name2,0,AUDIO_BUFFER);
		} /* for */ 
		for(i=0;i<n_ext;i++) {
			strcpy(name,file);
			strcat(name,ext[i]);
			sprintf(name2,"%s%s",default_s_path,name);
			if (file_check(name2)) return Mix_LoadMUS(name2);
//			if (file_check(name2)) return Sound_NewSampleFromFile(name2,&inf,AUDIO_BUFFER);
//			if (file_check(name2)) return Sound_NewSampleFromFile(name2,0,AUDIO_BUFFER);
		} /* for */ 
		
		fprintf(stderr,"ERROR in Sound_create_stream(): Could not load sound file: %s%s.(wav|ogg|mp3)\n",s_path, file);
		exit(1);
	} else {
		return 0;
	} /* if */ 
} /* Sound_create_stream */ 


void Sound_create_music(char *f1,char *f2,char *f3)
{
	char tmp[128];
	int seq_len=0;

	if (sound_enabled) {
		if (f1!=0) {
			Sound_obtain_file_name(f1,tmp);
			strcpy(music_files[0],f1);
			strcpy(music_realfiles[0],tmp);
			music_loaded[0]=true;
			music_sound[0]=Sound_create_stream(f1);
			seq_len=1;
		} else {
			music_files[0][0]=0;
			music_loaded[0]=false;
		} /* if */ 
		if (f2!=0) {
			Sound_obtain_file_name(f2,tmp);
			strcpy(music_files[1],f2);
			strcpy(music_realfiles[1],tmp);
			music_loaded[1]=true;
			music_sound[1]=Sound_create_stream(f2);
			seq_len=2;
		} else {
			music_files[1][0]=0;
			music_loaded[1]=false;
		} /* if */ 
		if (f3!=0) {
			Sound_obtain_file_name(f3,tmp);
			strcpy(music_files[2],f3);
			strcpy(music_realfiles[2],tmp);
			music_loaded[2]=true;
			music_sound[2]=Sound_create_stream(f3);
			seq_len=3;
		} else {
			music_files[2][0]=0;
			music_loaded[2]=false;
		} /* if */ 
		
		currentmusic = 0;
		Mix_PlayMusic(music_sound[currentmusic],1);
	} /* if */ 
} /* Sound_create_music */ 

void Sound_subst_music(char *f)
{
	char tmp[128];

	Sound_obtain_file_name(f,tmp);
} /* Sound_sust_music */ 


void Sound_subst_music_now(char *f)
{
	Sound_release_music();
	Sound_create_music(f,0,0);
} /* Sound_sust_music_now */ 



void Sound_release_music(void)
{
	int i;

	if (sound_enabled) {
		for(i=0;i<3;i++) {
			if (music_loaded[i]) Mix_FreeMusic(music_sound[i]);
			music_loaded[i]=false;
		} /* if */ 
	} /* if */ 
} /* Sound_release_music */ 


void Sound_temporary_release_music(void)
{
	int i;

	if (sound_enabled) {
		for(i=0;i<3;i++) {
			if (music_loaded[i]) Mix_FreeMusic(music_sound[i]);
		} /* for */ 
	} /* if */ 
} /* Sound_release_music */ 


void Sound_pause_music(void)
{
	Mix_PauseMusic();
} /* Sound_pause_music */ 


void Sound_unpause_music(void)
{
	Mix_ResumeMusic;
} /* Sound_unpause_music */ 


void music_recovery(void)
{	
	char tmp[128];
	int seq_len=0;

	if (sound_enabled) {

		if (music_loaded[0]) { 
			music_loaded[0]=true;
			Sound_obtain_file_name(music_files[0],tmp);
			strcpy(music_realfiles[0],tmp);
			music_sound[0]=Sound_create_stream(music_files[0]);
			seq_len=1;
		} /* if */ 
		if (music_loaded[1]) { 
			music_loaded[1]=true;
			Sound_obtain_file_name(music_files[1],tmp);
			strcpy(music_realfiles[1],tmp);
			music_sound[1]=Sound_create_stream(music_files[1]);
			seq_len=2;
		} /* if */ 
		if (music_loaded[2]) { 
			music_loaded[2]=true;
			Sound_obtain_file_name(music_files[2],tmp);
			strcpy(music_realfiles[2],tmp);
			music_sound[2]=Sound_create_stream(music_files[2]);
			seq_len=3;
		} /* if */ 

		currentmusic = 0;
		Mix_PlayMusic(music_sound[currentmusic],1);
	} /* if */ 

} /* music_recovery */ 

void Sound_playnext_music(void)
{
	if ((sound_enabled) && (Mix_PausedMusic() == 0) && (Mix_PlayingMusic() == 0))
	{
		if (++currentmusic > MAXMUSIC) currentmusic = 0;
		Mix_PlayMusic(music_sound[currentmusic],1);
	}		
}



















