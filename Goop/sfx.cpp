#include "sfx.h"
//#include "gconsole.h"

#include <fmod/fmod.h>

Sfx sfx;

Sfx::Sfx()
{
}

Sfx::~Sfx()
{
}

void Sfx::init()
{	
	FSOUND_Init(44100, 32, 0);
}

void Sfx::shutDown()
{
	FSOUND_Close();
}

void Sfx::registerInConsole()
{
}