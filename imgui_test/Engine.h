#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <filesystem>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "Resources.h"

namespace fs = std::filesystem;

namespace Broder
{

	class Renderer
	{
	public:
		Renderer(int side);
		virtual ~Renderer();
		virtual Graphic* GetGraphic() = 0;
		int getSelectedSide() { return side; }
		void show(float scale);
	protected:
		int side; // selected side of graphic object
	};

	class Image_Renderer : public Renderer
	{
	public:
		Image_Renderer(Graphic* image, int side = 0);
		Image* GetGraphic() override { return image; }
	protected:
		Image* image = nullptr;
	};

	class Animation_Renderer : public Renderer
	{
	public:
		Animation_Renderer(Graphic* animation, int frame_id = 0, int side = 0, int x = 0, int y = 0);
		void NextFrame();
		void UpdateFrame();

		// set default update time and default frame
		void SetFrameDefault();

		Animation* GetGraphic() override { return animation; }
		int GetFrameId() { return frame_id; }
		int GetFrameX() { return x; }
		int GetFrameY() { return y; }
		Uint64 GetUpdateTime() { return update_time; }
		void SetUpdateTime(Uint64 time) { update_time = time; }
		void AddUpdateTime(Uint64 time) { update_time += time; }
	protected:
		Animation* animation = nullptr;
		Uint64 update_time;
		int frame_id, x, y;
	};
	
	class Canvas
	{
	public:
		Canvas(float scale, float grid_step, int steps_x = 0, int steps_y = 0);
		~Canvas();
		void clear();
		void show();
		void reset(int steps_x, int steps_y);
		float GetGridStep() { return grid_step; }
		void SetDefaultScrolling() { scrolling = { 0, 0 }; }
	protected:
		std::vector<Renderer*>floor, buildings, entities;
		float grid_step;
		bool enabled_grid = true;
		ImVec2 scrolling{ 0, 0 };
		ImVec2 size{ 0, 0 };
		int steps_x, steps_y;
		float scale = 1.0f;
	};
	std::string FloatToStr(float num, int sym);
	ImVec2 operator * (ImVec2 vec, float f);
	ImVec2 operator / (ImVec2 vec, float f);
	ImVec2 operator - (ImVec2 vec1, ImVec2 vec2);
	ImVec2 operator + (ImVec2 vec1, ImVec2 vec2);
}