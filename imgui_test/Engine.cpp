#include "Engine.h"

using namespace Broder;

Renderer::Renderer(int side) : side(side)
{

}

Renderer::~Renderer()
{

}

void Renderer::show(float scale)
{
    GLuint id = 0;
    int w = 0, h = 0;
    float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
    int type = GetGraphic()->GetType();
    if (type == Graphic::IMAGE)
    {
        Image_Renderer& image_renderer = dynamic_cast<Image_Renderer&>(*this);
        Image* image = image_renderer.GetGraphic();
        Texture_Data texture_data = image->GetFileData(image_renderer.getSelectedSide());
        texture_data.Deconstruct(id, w, h);
        x1 = y1 = 0.0f;
        x2 = y2 = 1.0f;
        w *= scale;
        h *= scale;
    }
    else if (type == Graphic::ANIMATION)
    {
        Animation_Renderer& animation_renderer = dynamic_cast<Animation_Renderer&>(*this);
        animation_renderer.UpdateFrame();
        Animation* animation = animation_renderer.GetGraphic();
        Texture_Data texture_data = animation->GetFileData(animation_renderer.getSelectedSide());
        id = texture_data.id;
        w = animation->GetFrameWidth() * scale;
        h = animation->GetFrameHeight() * scale;
        x1 = (float)animation_renderer.GetFrameX() / texture_data.w;
        y1 = (float)animation_renderer.GetFrameY() / texture_data.h;
        x2 = x1 + (float)animation->GetFrameWidth() / texture_data.w;
        y2 = y1 + (float)animation->GetFrameHeight() / texture_data.h;
    }
    ImGui::Image((void*)id, { (float)w, (float)h }, { x1, y1 }, { x2, y2 });
}

Image_Renderer::Image_Renderer(Graphic* image, int side): Renderer(side)
{
    this->image = dynamic_cast<Image*>(image);
}

Animation_Renderer::Animation_Renderer(Graphic* animation, int frame_id, int side, int x, int y) : Renderer(side), frame_id(frame_id),
    x(x), y(y)
{
    assert(side < animation->GetFilesData().size());
    this->animation = dynamic_cast<Animation*>(animation);
    SetFrameDefault();
};

void Animation_Renderer::NextFrame()
{
    frame_id++;
    if (frame_id == animation->GetFrameCount())
        frame_id = 0;
    
    int frames_per_line = 1 + (animation->GetFileData(side).w - animation->GetFrameWidth()) / (animation->GetFrameWidth() + 1);
    int frames_per_column = 1 + (animation->GetFileData(side).h - animation->GetFrameHeight()) / (animation->GetFrameHeight() + 1);
    int frame_column = frame_id % frames_per_line;
    int frame_line = ceil(frame_id / frames_per_line);

    x = frame_column * (animation->GetFrameWidth() + 1);
    y = frame_line * (animation->GetFrameHeight() + 1);
}

void Animation_Renderer::UpdateFrame()
{
    while (SDL_GetTicks64() >= update_time)
    {
        update_time += animation->GetFrameTimeout();
        NextFrame();
    }
}

void Animation_Renderer::SetFrameDefault()
{
    // next time to update = current time + animation timeout
    update_time = SDL_GetTicks64() + dynamic_cast<Animation&>(*animation).GetFrameTimeout();

    frame_id = 0;
    x = y = 0;
}

Canvas::Canvas(float scale, float grid_step, int steps_x, int steps_y): scale(scale), grid_step(grid_step), steps_x(steps_x), steps_y(steps_y)
{
    this->grid_step *= scale;
    this->steps_x *= scale;
    this->steps_y *= scale;
    size = { this->steps_x * grid_step, this->steps_y * grid_step };
}

Canvas::~Canvas()
{

}

void Canvas::clear()
{
    floor.clear();
    buildings.clear();
    entities.clear();
}

void Canvas::reset(int steps_x, int steps_y)
{
    this->steps_x = steps_x;
    this->steps_y = steps_y;
    size.x = steps_x * grid_step;
    size.y = steps_y * grid_step;
    SetDefaultScrolling();
}

void Canvas::show()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetCursorPos({ io.DisplaySize.x * 0.16f, io.DisplaySize.y * 0.05f });

    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size{ io.DisplaySize.x * 0.8f, io.DisplaySize.y * 0.8f };
    ImVec2 canvas_p1 = canvas_p0 + canvas_size;

    ImVec2 map_p0 = canvas_p0 + io.DisplaySize * 0.02f;
    ImVec2 map_size = canvas_size - (map_p0 - canvas_p0);
    ImVec2 map_p1 = map_p0 + map_size;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(map_p0, map_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(map_p0, map_p1, IM_COL32(255, 255, 255, 255));

    ImGui::InvisibleButton("canvas", map_size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {
        // scrolling
        ImVec2 temp_scrolling(scrolling.x - io.MouseDelta.x, scrolling.y - io.MouseDelta.y);
        if (temp_scrolling.x <= size.x - map_size.x && temp_scrolling.x >= 0)
            scrolling.x = temp_scrolling.x;
        if (temp_scrolling.y <= size.y - map_size.y && temp_scrolling.y >= 0)
            scrolling.y = temp_scrolling.y;
    }

    draw_list->PushClipRect(canvas_p0, canvas_p1);

    // draw indexes
    for (float x = grid_step - fmodf(scrolling.x, grid_step); x < size.x; x += grid_step)
    {
        std::string index = std::to_string((int)round(((scrolling.x + x) / grid_step)));
        ImVec2 center = ImVec2(ImGui::CalcTextSize(index.c_str()).x, 0.0f) / 2.0f;
        draw_list->AddText(ImVec2(canvas_p0.x + x, canvas_p0.y) + center, IM_COL32(255, 255, 255, 255),
            index.c_str());
    }
    for (float y = grid_step - fmodf(scrolling.y, grid_step); y < size.y; y += grid_step)
    {
        std::string index = std::to_string((int)round(((scrolling.y + y) / grid_step))).c_str();
        ImVec2 center = ImVec2(0.0f, ImGui::CalcTextSize(index.c_str()).y) / 2.0f;
        draw_list->AddText(ImVec2(canvas_p0.x, canvas_p0.y + y) - center, IM_COL32(255, 255, 255, 255),
            std::to_string((int)round(((scrolling.y + y) / grid_step))).c_str());
    }

    draw_list->PushClipRect(map_p0, map_p1);

    // draw grid
    if (enabled_grid)
    {
        for (float x = grid_step - fmodf(scrolling.x, grid_step); x < size.x; x += grid_step)
        {
            draw_list->AddLine(ImVec2(map_p0.x + x, map_p0.y), ImVec2(map_p0.x + x, map_p1.y), IM_COL32(200, 200, 200, 40));
        }
        for (float y = grid_step - fmodf(scrolling.y, grid_step); y < size.y; y += grid_step)
        {
            draw_list->AddLine(ImVec2(map_p0.x, map_p0.y + y), ImVec2(map_p1.x, map_p0.y + y), IM_COL32(200, 200, 200, 40));
        }
    }
    draw_list->PopClipRect();
    draw_list->PopClipRect();
}

std::string Broder::FloatToStr(float num, int sym)
{
    std::string str = std::to_string(num);
    auto erase_to = std::find(str.begin(), str.end(), '.');
    for (int i = 0; i < sym + 1 && erase_to != str.end(); i++)
        erase_to++;
    str.erase(erase_to, str.end());
    return str;
}

ImVec2 Broder::operator * (ImVec2 vec, float f)
{
    vec.x *= f;
    vec.y *= f;
    return vec;
}

ImVec2 Broder::operator / (ImVec2 vec, float f)
{
    vec.x /= f;
    vec.y /= f;
    return vec;
}

ImVec2 Broder::operator - (ImVec2 vec1, ImVec2 vec2)
{
    ImVec2 vec;
    vec.x = vec1.x - vec2.x;
    vec.y = vec1.y - vec2.y;
    return vec;
}

ImVec2 Broder::operator + (ImVec2 vec1, ImVec2 vec2)
{
    ImVec2 vec;
    vec.x = vec1.x + vec2.x;
    vec.y = vec1.y + vec2.y;
    return vec;
}