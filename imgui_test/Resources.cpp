#include "Resources.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Texture_Data::Deconstruct(GLuint& id, int& w, int& h)
{
    id = this->id;
    w = this->w;
    h = this->h;
}

Texture_Data Graphic::GetFileData(int id)
{
    return files_data.at(id);
}

Graphic::Graphic(std::vector<std::string>filepaths, int type) : type(type)
{
    files_data.reserve(filepaths.size());
    for (auto& filepath : filepaths)
    {
        Texture_Data data;
        LoadTextureFromFile(filepath.c_str(), &data.id, &data.w, &data.h);
        files_data.push_back(data);
    }
}

Graphic::~Graphic()
{

}

Image::Image(std::vector<std::string>filepaths) : Graphic(filepaths, IMAGE)
{

}

Image::~Image()
{

}

Animation::Animation(std::vector<std::string>filepaths, int frame_count, int frame_timeout, int frame_width, int frame_height) : Graphic(filepaths, ANIMATION),
frame_count(frame_count), frame_timeout(frame_timeout), frame_width(frame_width), frame_height(frame_height)
{
    for (auto& data : files_data)
        assert(data.w > frame_width && data.h > frame_height);
}

Animation::~Animation()
{

}

Resources::Resources()
{
    {
        rapidxml::file<> xmlFile("resources/data/floor.xml");
        xml_document<> doc;
        doc.parse<0>(xmlFile.data());
        xml_node<>* root = doc.first_node("floor");
        std::string name, file = "resources/", type;
        for (xml_node<>* tile = root->first_node(); tile; tile = tile->next_sibling())
        {
            name = tile->first_node("name")->value();
            file = tile->first_node("file")->value();
            type = tile->first_node("type")->value();
            if (type == "image")
            {
                file = "resources/images/floor/" + file;
                floor[name] = new Image({ file });
            }
            else if (type == "animation")
            {
                file = "resources/animations/floor/" + file;
                int frames = std::stoi(tile->first_node("frames")->value());
                int timeout = std::stoi(tile->first_node("timeout")->value());
                floor[name] = new Animation({ file }, frames, timeout, 48, 48);
            }
        }
    }

    {
        rapidxml::file<> xmlFile("resources/data/buildings.xml");
        xml_document<> doc;
        doc.parse<0>(xmlFile.data());
        xml_node<>* root = doc.first_node("building");
        std::string name, file = "resources/", type, sides;
        for (xml_node<>* sample = root->first_node(); sample; sample = sample->next_sibling())
        {
            name = sample->first_node("name")->value();
            type = sample->first_node("type")->value();
            sides = sample->first_node("sides")->value();
            if (type == "image")
            {
                if (sides == "1")
                {

                }
                else if (sides == "4")
                {

                }
            }
            else if (type == "animation")
            {
                if (sides == "1")
                {

                }
                else if (sides == "4")
                {

                }
            }
        }
    }
}

Resources::~Resources()
{

}

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}