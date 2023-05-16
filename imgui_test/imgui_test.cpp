#include <stdio.h>
#include "Game.h"

namespace fs = std::filesystem;

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

#if defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, window_flags);
    int w, h, left = 0, top = 0;
    float scale;
    SDL_GetWindowSize(window, &w, &h);
    scale = (float)w / h;
    if (scale > 1.77f && scale < 1.78f)
    {
        scale = (float)w / 1920;
        left = top = 0;
    }
    else if (scale >= 1.78)
    {
        scale = (float)h / 1080;
        left = (1920 - w) / 2;
    }
    else if (scale <= 1.7)
    {
        scale = (float)w / 1920;
        top = (1080 - h) / 2;
    }
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    ImVec4 clear_color = ImVec4(0.2f, 0.2f, 0.3f, 1.0f);
    int window_id = Window_id::START_MENU;
    Texture_Data bg_tex;

    assert(LoadTextureFromFile("resources/images/background/background.png", &bg_tex.id, &bg_tex.w, &bg_tex.h));

    // Main loop
    Game game(scale);
    bool quit = false;
    bool dev_mode = false;
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = NULL;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!quit)
#endif
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                quit = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                quit = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (window_id == Window_id::START_MENU)
        {
            ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));

            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("StartMenu", nullptr, flags); 
            
            ImGui::SetCursorPos(ImVec2(0, 0));
            ImGui::Image((void*)bg_tex.id, ImVec2(io.DisplaySize.x, io.DisplaySize.y));

            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(80, 255, 128, 255));
            ImGui::PushFont(io.Fonts->Fonts[1]);
            ImGui::SetCursorPos(ImVec2(5, 0));
            ImGui::Text("Authors: Broder\nBuild 0.0.0");
            ImGui::PopFont();
            ImGui::PopStyleColor();

            ImVec2 btn_size(io.DisplaySize.x * 0.18f, io.DisplaySize.y * 0.08f);

            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(128, 0, 255, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(128, 0, 200, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(128, 0, 150, 255));
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 255, 0, 255));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - btn_size.x) * 0.8f);
            ImGui::SetCursorPosY((ImGui::GetWindowHeight() - 40 * 4) * 0.4f);

            ImGui::BeginGroup();
            if (ImGui::Button("New Game", btn_size))
                window_id = Window_id::NEW_GAME;
            if (ImGui::Button("Load Game", btn_size))
                window_id = Window_id::LOAD_GAME;
            if (ImGui::Button("Settings", btn_size))
                window_id = Window_id::SETTINGS;
            if (dev_mode)
                if (ImGui::Button("Dev Tools", btn_size))
                {
                    window_id = Window_id::DEV_TOOLS;
                    game.dev = new Dev(scale);
                }
            if (ImGui::Button("Quit", btn_size))
                quit = true;
            ImGui::EndGroup();

            ImGui::PopStyleVar();
            ImGui::PopStyleColor(4);

            ImGui::End();
            ImGui::PopStyleVar(2);
        }
        else if (window_id == Window_id::SETTINGS)
        {
            ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));

            ImGui::Begin("Settings", nullptr, flags);

            ImVec2 btn_size(io.DisplaySize.x * 0.1f, io.DisplaySize.y * 0.05f);

            ImGui::SetCursorPos(ImVec2(io.DisplaySize.x * 0.4f, io.DisplaySize.y * 0.3f));
            ImGui::PushFont(io.Fonts->Fonts[1]);
            ImGui::BeginGroup();
            ImGui::Checkbox("Dev mode", &dev_mode);
            ImGui::EndGroup();
            ImGui::PopFont();

            //ImGui::BeginListBox("")

            ImGui::SetCursorPos(ImVec2((io.DisplaySize.x - btn_size.x) * 0.5f, (io.DisplaySize.y - btn_size.y) * 0.8f));
            if (ImGui::Button("OK", btn_size))
                window_id = Window_id::START_MENU;

            ImGui::End();
        }
        else if (window_id == Window_id::DEV_TOOLS)
            game.dev->show(io, game.res, window_id);

        // Rendering
        ImGui::Render();
        //glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}