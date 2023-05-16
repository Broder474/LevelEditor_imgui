#include "Engine.h"

enum Window_id { START_MENU, NEW_GAME, LOAD_GAME, SETTINGS, DEV_TOOLS };
using namespace Broder;

class Item
{
public:
	Item(std::string name, int type, Renderer* renderer);
	enum Item_type {FLOOR, BUILDING, ENTITY};
	Renderer* GetRenderer() { return renderer; }
	std::string GetName() { return name; }
	int GetType() { return type; }
private:
	Renderer* renderer;
	std::string name;
	int type;
};

class Mode
{
public:
	Mode(float scale);
	virtual ~Mode();
protected:
	float scale;
};

class Level_editor : public Mode
{
public:
	Level_editor(Resources* resources, const char* levels_dir, float scale);
	~Level_editor();
	void show();
private:
	int current_level = -1;
	std::vector<std::string>level_paths;
	bool add_level = false;
	bool delete_level = false;
	char new_level_name[256] = "";
	int new_level_width = 50, new_level_height = 30;
	std::vector<Item>items;
	int current_item = -1;
	Canvas canvas;
};

class General_editor : public Mode
{
public:
	General_editor(float scale);
	~General_editor();
};

class Dev
{
public:
	enum Modes {GENERAL, LEVEL_EDITOR};
	Dev(float scale);
	~Dev();
	void show(ImGuiIO& io, Resources& res, int& window_id);
private:
	int current_mode = GENERAL;
	float scale;
	std::vector<std::pair<std::string, int>>modes_items;
	Mode* mode = nullptr;
};