#include <map>
#include <SDL_opengl.h>
#include <filesystem>
#include <rapidxml_print.hpp>
#include <rapidxml_utils.hpp>

using namespace rapidxml;

struct Texture_Data
{
	GLuint id;
	int w, h;
	void Deconstruct(GLuint& id, int& w, int& h);
};

class Graphic
{
public:
	Graphic(std::vector<std::string>filepaths, int type);
	virtual ~Graphic();
	enum Graphic_type { IMAGE, ANIMATION };
	int GetType() { return type; }
	std::vector<Texture_Data>GetFilesData() { return files_data; }
	Texture_Data GetFileData(int id);
protected:
	int type;
	std::vector<Texture_Data>files_data;
};

class Image : public Graphic
{
public:
	Image(std::vector<std::string>filepaths);
	~Image();
};

class Animation : public Graphic
{
public:
	Animation(std::vector<std::string>filepaths, int frame_count, int frame_timeout, int frame_width, int frame_height);
	~Animation();
	int GetFrameCount() { return frame_count; }
	int GetFrameTimeout() { return frame_timeout; }
	int GetFrameWidth() { return frame_width; }
	int GetFrameHeight() { return frame_height; }
protected:
	int frame_count, frame_timeout, frame_width, frame_height;
};

class Resources
{
public:
	Resources();
	~Resources();

	std::map<std::string, Graphic*>floor;
	std::map<std::string, Graphic*>buildings;
};

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);