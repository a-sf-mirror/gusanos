#ifndef SOUND_H
#define SOUND_H

#include "resource_list.h"
#include "vec.h"
#include <string>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

//class Vec;
class BaseObject;
struct FSOUND_SAMPLE;

class Sound
{
	public:
		
	Sound();
	~Sound();
	
	bool load(fs::path const& filename);
	void play( float volume = 1,float pitch = 1,float volumeVariation = 1,float pitchVariation = 1);
	void play2D(const Vec& pos, float loudness = 100, float pitch = 1, float pitchVariation = 1);
	void play2D( BaseObject* obj, float loudness = 100, float pitch = 1, float pitchVariation = 1);
	
	private:
	
	FSOUND_SAMPLE * m_sound;
	
};

extern ResourceList<Sound> soundList;

#endif // _SOUND_H_
