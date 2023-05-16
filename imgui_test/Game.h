#include "Dev.h"

class Game
{
public:
	Game(float scale = 1.0f);
	~Game();

	void LoadFonts(float scale = 1.0f);
	
	Resources res;
	float scale;
	Dev* dev = nullptr;
private:
};