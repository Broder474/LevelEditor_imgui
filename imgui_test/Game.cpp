#include "Game.h"

Game::Game(float scale): scale(scale)
{
    LoadFonts(scale);
}

Game::~Game()
{

}

void Game::LoadFonts(float scale)
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF("c:/Windows/Fonts/segoeui.ttf", 48.0f * scale);
    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf", 32.0f * scale);
    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf", 24.0f * scale);
}