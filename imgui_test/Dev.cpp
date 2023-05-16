#include "Dev.h"

namespace Broder
{
    void InputIntInRange(const char* label, int* v, int step, int step_fast, int bottom_border, int top_border, ImGuiInputTextFlags flags = 0)
    {
        if (ImGui::InputInt(label, v, step, step_fast, flags))
            if (*v < bottom_border)
                *v = bottom_border;
            else if (*v > top_border)
                *v = top_border;
    }
}

Item::Item(std::string name, int type, Renderer* renderer): name(name), type(type), renderer(renderer)
{

}

Mode::Mode(float scale): scale(scale) {};

Mode::~Mode() {};

Level_editor::Level_editor(Resources* res, const char* levels_dir, float scale): Mode(scale), canvas(scale, 48.0f)
{
    for (auto& entry : fs::directory_iterator(levels_dir))
    {
        // ignore folders and files with extension other than .lvl
        if (!entry.is_regular_file() || entry.path().extension() != ".lvl")
            continue;
        level_paths.push_back(entry.path().filename().stem().string());
    }
    for (auto& floor : res->floor)
    {
        int type = -1;
        Renderer* renderer = nullptr;
        if (floor.second->GetType() == Graphic::IMAGE)
        {
            type = Graphic::IMAGE;
            renderer = new Image_Renderer(floor.second);
        }
        else if (floor.second->GetType() == Graphic::ANIMATION)
        {
            type = Graphic::ANIMATION;
            renderer = new Animation_Renderer(floor.second);
        }
        items.push_back(Item(floor.first, Item::FLOOR, renderer));
    }
}

Level_editor::~Level_editor()
{

}

void Level_editor::show()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetCursorPos(ImVec2(io.DisplaySize.x * 0.003f, io.DisplaySize.y * 0.03f));

    ImVec2 btn_size1(io.DisplaySize.x * 0.1f, io.DisplaySize.y * 0.05f),
        btn_size2(io.DisplaySize.x * 0.07f, io.DisplaySize.y * 0.05f);

    ImGui::BeginGroup();
    ImGui::Text("Levels");
    ImGui::PushFont(io.Fonts->Fonts[1]);
    ImGui::BeginListBox("##levels_list", ImVec2(io.DisplaySize.x * 0.15f, io.DisplaySize.y * 0.4f));
    int i = 0;
    for (auto& level : level_paths)
    {
        if (ImGui::Selectable(level.c_str(), current_level == i))
        {
            current_level = i;

            // level loading
            std::string filepath("resources/levels/" + level_paths.at(i) + ".lvl");
            file<> file(filepath.c_str());
            xml_document<>doc;
            doc.parse<0>(file.data());
            xml_node<>* root_node = doc.first_node("root");

            canvas.SetDefaultScrolling();
            canvas.reset(std::stof(root_node->first_attribute("width")->value()), std::stof(root_node->first_attribute("height")->value()));
        }
        i++;
    }
    ImGui::EndListBox();
    if (ImGui::Button("Add level", btn_size2))
    {
        add_level = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete level", btn_size2))
    {
        if (current_level > -1 && current_level < level_paths.size())
        {
            delete_level = true;
        }
    }
    ImGui::PopFont();
    ImGui::EndGroup();

    ImGui::Text("Items");
    ImGui::PushFont(io.Fonts->Fonts[1]);
    ImGui::BeginListBox("##items_list", ImVec2(io.DisplaySize.x * 0.15f, io.DisplaySize.y * 0.3f));
    if (ImGui::TreeNodeEx("Floor")) // floor tree
    {
        if (ImGui::BeginListBox("##floor_list", ImVec2(io.DisplaySize.x * 0.15f, io.DisplaySize.y * 0.3f)))
        {
            i = 0;
            for (auto& item : items)
            {
                if (item.GetType() == Item::FLOOR)
                    if (ImGui::Selectable(item.GetName().c_str(), current_item == i))
                    {
                        current_item = i;
                        Renderer* renderer = items.at(current_item).GetRenderer();
                        if (renderer->GetGraphic()->GetType() == Graphic::ANIMATION)
                            dynamic_cast<Animation_Renderer&>(*renderer).SetFrameDefault();
                    }
                i++;
            }
            ImGui::EndListBox();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Buildings")) // buildings tree
    {
        if (ImGui::BeginListBox("##building_list", ImVec2(io.DisplaySize.x * 0.15f, io.DisplaySize.y * 0.3f)))
        {
            i = 0;
            for (auto& item : items)
            {
                if (item.GetType() == Item::BUILDING)
                    if (ImGui::Selectable(item.GetName().c_str(), current_item == i))
                    {
                        current_item = i;
                        Renderer* renderer = items.at(current_item).GetRenderer();
                        if (renderer->GetGraphic()->GetType() == Graphic::ANIMATION)
                            dynamic_cast<Animation_Renderer&>(*renderer).SetFrameDefault();
                    }
                i++;
            }
            ImGui::EndListBox();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Entities")) // entities tree
    {
        if (ImGui::BeginListBox("##entity_list", ImVec2(io.DisplaySize.x * 0.15f, io.DisplaySize.y * 0.3f)))
        {
            i = 0;
            for (auto& item : items)
            {
                if (item.GetType() == Item::ENTITY)
                    if (ImGui::Selectable(item.GetName().c_str(), current_item == i))
                    {
                        current_item = i;
                        Renderer* renderer = items.at(current_item).GetRenderer();
                        if (renderer->GetGraphic()->GetType() == Graphic::ANIMATION)
                            dynamic_cast<Animation_Renderer&>(*renderer).SetFrameDefault();
                    }
                i++;
            }
            ImGui::EndListBox();
        }
        ImGui::TreePop();
    }
    ImGui::EndListBox();
    ImGui::PopFont();

    // show selected item
    if (current_item != -1)
    {
        items.at(current_item).GetRenderer()->show(scale);
    }

    if (current_level != -1)
    {
        ImGui::PushFont(io.Fonts->Fonts[2]);
        canvas.show();
        ImGui::PopFont();
    }

    // add level window
    if (add_level == true)
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.25f, io.DisplaySize.y * 0.25f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f));
        ImGui::Begin("Add level", nullptr, flags);
        ImGui::Text("Name");
        ImGui::InputText("##NameInput", new_level_name, 256);

        ImGui::Text("Width");
        Broder::InputIntInRange("##WidthInput", &new_level_width, 1, 10, 50, 500);
        ImGui::Text("Height");
        Broder::InputIntInRange("##HeightInput", &new_level_height, 1, 10, 30, 300);
        if (ImGui::Button("OK", btn_size1))
        {
            if (strcmp(new_level_name, "") != 0 && std::find(level_paths.begin(), level_paths.end(), new_level_name) == level_paths.end()) 
                // if new level name is not empty and not equal to existing
            {
                add_level = false;
                xml_document<>level;
                xml_node<>* decl = level.allocate_node(node_declaration);
                decl->append_attribute(level.allocate_attribute("version", "1.0"));
                decl->append_attribute(level.allocate_attribute("encoding", "utf-8"));
                level.append_node(decl);

                xml_node<>* root = level.allocate_node(node_element, "root");
                std::string width = std::to_string(new_level_width);
                std::string height = std::to_string(new_level_height);
                root->append_attribute(level.allocate_attribute("width", width.c_str()));
                root->append_attribute(level.allocate_attribute("height", height.c_str()));
                level.append_node(root);

                std::string level_as_string;
                rapidxml::print(std::back_inserter(level_as_string), level);
                std::string filepath = "resources/levels/";
                filepath += new_level_name;
                filepath += ".lvl";
                std::ofstream out_level(filepath);
                out_level << level_as_string;
                out_level.close();
                level.clear();

                level_paths.push_back(new_level_name);
                new_level_width = 50;
                new_level_height = 30;
            }
            strcpy_s(new_level_name, "");
        }
        ImGui::SameLine();
        if (ImGui::Button("Back", btn_size1))
        {
            add_level = false;
        }
        ImGui::End();
    }
    // delete level window
    if (delete_level == true)
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.35f, io.DisplaySize.y * 0.445f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.3f, io.DisplaySize.y * 0.11f));
        ImGui::Begin("Are you really want to delete level?", nullptr, flags);
        if (ImGui::Button("Yes", btn_size1))
        {
            // delete selected level
            fs::remove("resources/levels/" + level_paths[current_level] + ".lvl");
            level_paths.erase(std::next(level_paths.begin(), current_level));
            delete_level = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("No", btn_size1))
            delete_level = false;
        ImGui::End();
    }
}

General_editor::General_editor(float scale): Mode(scale) {};

General_editor::~General_editor() {};

Dev::Dev(float scale): scale(scale)
{
	mode = new General_editor(scale);
	modes_items.insert(modes_items.end(), { { "General", Modes::GENERAL }, {"Level editor", Modes::LEVEL_EDITOR} });
}

Dev::~Dev()
{

}

void Dev::show(ImGuiIO& io, Resources& res, int& window_id)
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, -io.DisplaySize.x * 0.002f));
    ImGui::Begin("Dev tools", nullptr, flags);
    ImGui::PopStyleVar();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f;

    ImGui::PushFont(io.Fonts->Fonts[2]);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Back"))
        {
            window_id = Window_id::START_MENU;
            this->~Dev();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Mode"))
        {
            for (auto& item : modes_items)
                if (ImGui::MenuItem(item.first.c_str(), nullptr, nullptr))
                {
                    if (current_mode == item.second)
                        continue;
                    int mode = item.second;
                    current_mode = mode;
                    this->mode->~Mode();
                    if (mode == Dev::Modes::GENERAL)
                        this->mode = new General_editor(scale);
                    else if (mode == Dev::Modes::LEVEL_EDITOR)
                        this->mode = new Level_editor(&res, "resources/levels", scale);
                }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::PopFont();

    if (current_mode == Dev::Modes::GENERAL)
    {

    }
    else if (current_mode == Dev::Modes::LEVEL_EDITOR)
    {
        Level_editor& level_editor = dynamic_cast<Level_editor&>(*mode);
        level_editor.show();
    }
    ImGui::End();
}