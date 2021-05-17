#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <ctype.h>
#define WIDTH 800
#define HEIGHT 600
namespace fs = std::filesystem;
//Base class
class FileEntry
{
    public:
        int x;
        int y;
        int w;
        int h;
        std::string name;
        int type;
};
  
// Sub class inheriting from Base Class(Parent)
class Directory : public FileEntry
{
    public:
        std::vector<FileEntry> list;
};
 
// Sub class inheriting from Base Class(Parent)
class Exec : public FileEntry
{
    public:
        int fileSize;
        std::string permissions;
        
};
// Sub class inheriting from Base Class(Parent)
class Image : public FileEntry
{
    public:
        int fileSize;
        std::string permissions;
};
// Sub class inheriting from Base Class(Parent)
class Video : public FileEntry
{
    public:
        int fileSize;
        std::string permissions;
};
// Sub class inheriting from Base Class(Parent)
class Code : public FileEntry
{
    public:
        int fileSize;
        std::string permissions;
};
// Sub class inheriting from Base Class(Parent)
class Misc : public FileEntry
{
    public:
        int fileSize;
        std::string permissions;
};


typedef struct AppData{
    TTF_Font *font;
    SDL_Texture *other;
    SDL_Texture *video;
    SDL_Texture *code;
    SDL_Texture *image;
    SDL_Texture *exec;
    SDL_Texture *direct;
    SDL_Texture *phrase;
    std::vector<FileEntry> list;
}AppData;

void initialize(SDL_Renderer *renderer, AppData *data);
void render(SDL_Renderer *renderer, AppData *data, std::vector<FileEntry> list);

std::string getPerms(fs::perms p)
{
    std::string result;
    result.append((p & fs::perms::owner_read) != fs::perms::none ? "r" : "-");
    result.append((p & fs::perms::owner_write) != fs::perms::none ? "w" : "-");
    result.append((p & fs::perms::owner_exec) != fs::perms::none ? "x" : "-");
    result.append((p & fs::perms::group_read) != fs::perms::none ? "r" : "-");
    result.append((p & fs::perms::group_write) != fs::perms::none ? "w" : "-");
    result.append((p & fs::perms::group_exec) != fs::perms::none ? "x" : "-");
    result.append((p & fs::perms::others_read) != fs::perms::none ? "r" : "-");
    result.append((p & fs::perms::others_write) != fs::perms::none ? "w" : "-");
    result.append((p & fs::perms::others_exec) != fs::perms::none ? "x" : "-");
    return result;
}

int typeIder(std::string filename)
{
    int type = 4;
    int max = -1;
    
    //image
    if(filename.find(".jpg") != max || filename.find(".jpeg") != max || filename.find(".png") != max || filename.find(".tif") != max || filename.find(".tiff") != max || filename.find(".gif") != max)
    {
        return 0;
    }
    //video
    else if(filename.find(".mp4") != max || filename.find(".mov") != max || filename.find(".mkv") != max || filename.find(".avi") != max || filename.find(".webm") != max)
    {
        return 1;
    }
    //code
    else if(filename.find(".h") != max || filename.find(".c") != max || filename.find(".cpp") != max || filename.find(".py") != max || filename.find(".java")!= max|| filename.find(".js") != max)
    {
        return 2;
    }

    else if((fs::status(filename).permissions() & fs::perms::owner_exec) != fs::perms::none || (fs::status(filename).permissions() & fs::perms::group_exec) != fs::perms::none || (fs::status(filename).permissions() & fs::perms::others_exec) != fs::perms::none )
    {
        return 3;
    }
    return type;
}

std::string removeSchmutz(std::string line)
{
    int max = -1;
    if(line.find("/.") != max)
    {
        return line.substr(2);
    }
    else if(line.find(".") != max)
    {
        return line.substr(1);
    }
    else if(line.find("./") != max)
    {
        return line.substr(2);
    }
    else if(line.find("/") != max)
    {
        return line.substr(1);
    }
    return line;
}

std::vector<FileEntry> fillFiles(std::string path)
{
    std::string dot (".");
    std::size_t max = -1;
    //std::cout << "-------------------------------------\n";
    //std::cout << "path: " << path<<"\n";
    std::vector<FileEntry> list;
    std::vector<std::string> files;
    for (const auto & entry : fs::directory_iterator(path)){
        std::string filename(entry.path());
        if(fs::is_directory(entry) && filename.find(dot) != max){continue;}
        else if(fs::is_directory(entry))
        {
            Directory dict;
            dict.name = removeSchmutz(filename.substr(path.length()));
            //std::cout << "dict.name: " << dict.name<<"\n";
            dict.list = fillFiles(filename);
            dict.type = 0;
            list.push_back(dict);
        }
        else
        {
            Image im;
            Video vid;
            Code code;
            Exec exec;
            Misc misc;
            //std::cout << "FILENAME: " << filename<<"\n";
            //std::cout << "typeIder(filename): " << typeIder(filename)<<"\n";
            switch (typeIder(filename))
            {
            case 0://image
                
                im.name = removeSchmutz(filename.substr(path.length()));
                
                im.permissions = getPerms(fs::status(filename).permissions());
                im.type = 2;
                list.push_back(im);
                break;
            
            case 1://video
                
                vid.name = removeSchmutz(filename.substr(path.length()));
                vid.permissions = getPerms(fs::status(filename).permissions());
                vid.type = 3;
                list.push_back(vid);
                break;
            case 2://code
                
                code.name = removeSchmutz(filename.substr(path.length()));
                code.permissions = getPerms(fs::status(filename).permissions());
                code.type = 4;
                list.push_back(code);
                break;
            
            case 3://exec
                
                exec.name = removeSchmutz(filename.substr(path.length()));
                exec.permissions = getPerms(fs::status(filename).permissions());
                exec.type = 1;
                list.push_back(code);
                
                break;

            case 4://misc
               
                misc.name = removeSchmutz(filename.substr(path.length()));
                misc.permissions = getPerms(fs::status(filename).permissions());
                misc.type = 5;
                list.push_back(misc);
                break;

            default:
                break;
            }
            
        }
    }
    return list;
}

bool compare(FileEntry x, FileEntry y) 
{
    /*//std::cout << "x: " <<  x.name << "\n";
    //std::cout << "y: " <<  y.name << "\n";
    std::string xlower = "";
    std::string ylower = "";
    for (int i=0; i<x.name.length(); ++i)
    {
        xlower.append("" + tolower(x.name.at(i)));
    }   
    std::locale loc2;
    for (int i=0; i<y.name.length(); ++i)
    {
        ylower.append("" + tolower(y.name.at(i)));
    }  
    /*int i=0;
    
    char c;
    for(int i =)
    while (str[i])
    {
        c=str[i];
        putchar (tolower(c));
        i++;
    }
    i=0;
    char c2;
    while (str2[i])
    {
        c2=str2[i];
        ;
        i++;
    }*/
    //std::string xlower(str);
    //std::string ylower(str2);
    //std::cout << "xlower < ylower: " <<  xlower < ylower << "\n";
    //std::cout << "xlower: " <<  xlower << "\n";
    //std::cout << "ylower: " <<  ylower << "\n";
    return x.name < y.name;

} 

void calcY(std::vector<FileEntry> list)
{
    for(int i = 0; i < list.size();i++)
    {
        list.at(i).x = 10;
        list.at(i).y = i*100 + i*10;
        list.at(i).h = 100;
        list.at(i).w = 100;
    }
}

int main(int argc, char **argv)
{
    char *home = getenv("HOME");
    printf("HOME: %s\n", home);

    //grab all files
    std::string path(home);
    std::vector<FileEntry> list = fillFiles(path);
    std::sort(list.begin(),list.end(), compare);
    //std::cout << list.at(0).name << '\n';
    /*for (int i=0; i<list.size(); i++)
    {
        std::cout << list.at(i).name << '\n';
    }*/
    //std::cout << list.at(1).name << '\n';
    // initializing SDL as Video
    calcY(list);

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    // create window and renderer
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

    // initialize and perform rendering loop
    AppData data;
    initialize(renderer, &data);
    render(renderer, &data, list);
    SDL_Event event;
    SDL_WaitEvent(&event);
    while (event.type != SDL_QUIT)
    {
        //render(renderer);
        SDL_WaitEvent(&event);
    }

    // clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(data.other);
    SDL_DestroyTexture(data.exec);
    SDL_DestroyTexture(data.direct);
    SDL_DestroyTexture(data.image);
    SDL_DestroyTexture(data.video);
    SDL_DestroyTexture(data.code);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}

void initialize(SDL_Renderer *renderer, AppData *data)
{
    // set color of background when erasing frame
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);

    data->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 18);
    SDL_Color phrase_color = {0,0,0};
    SDL_Surface *text_surf = TTF_RenderText_Solid(data->font, "video", phrase_color);
    data->phrase = SDL_CreateTextureFromSurface(renderer,text_surf);
    SDL_FreeSurface(text_surf);


    SDL_Surface *surf = IMG_Load("resrc/other.png");
    SDL_Surface *surf1 = IMG_Load("resrc/direct.png");
    SDL_Surface *surf2 = IMG_Load("resrc/exec.png");
    SDL_Surface *surf3 = IMG_Load("resrc/generic.png");
    SDL_Surface *surf4 = IMG_Load("resrc/video.png");
    SDL_Surface *surf5 = IMG_Load("resrc/code.png");
    data->other = SDL_CreateTextureFromSurface(renderer,surf);
    data->direct = SDL_CreateTextureFromSurface(renderer,surf1);
    data->exec = SDL_CreateTextureFromSurface(renderer,surf2);
    data->image = SDL_CreateTextureFromSurface(renderer,surf3);
    data->video = SDL_CreateTextureFromSurface(renderer,surf4);
    data->code = SDL_CreateTextureFromSurface(renderer,surf5);
    SDL_FreeSurface(surf);
    SDL_FreeSurface(surf1);
    SDL_FreeSurface(surf2);
    SDL_FreeSurface(surf3);
    SDL_FreeSurface(surf4);
    SDL_FreeSurface(surf5);
}

void render(SDL_Renderer *renderer, AppData *data, std::vector<FileEntry> list)
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 0, 235, 235, 255);
    SDL_RenderClear(renderer);

    //render all file icons
    SDL_Rect rect;
    rect.x = 10;
    rect.w = 100;
    rect.h = 100;

    for(int i = 0; i < list.size(); i++)
    {
        rect.y = list.at(i).y;
        std::cout << "list.size():: " << list.size();
        switch (list.at(i).type)
        {
        case 0:
            std::cout << "direct";
            SDL_RenderCopy(renderer, data->direct, NULL, &rect);
            break;
        
        case 1:
            SDL_RenderCopy(renderer, data->exec, NULL, &rect);
            break;
        
        case 2:
            SDL_RenderCopy(renderer, data->image, NULL, &rect);
            break;
        
        case 3:
            SDL_RenderCopy(renderer, data->video, NULL, &rect);
            break;
        
        case 4:
            SDL_RenderCopy(renderer, data->code, NULL, &rect);
            break;
        
        case 5:
            SDL_RenderCopy(renderer, data->other, NULL, &rect);
            break;
        
        default:
            break;
            
        }
        
    }

    //SDL_QueryTexture(data->phrase, NULL,NULL, &(rect.w), &(rect.h));
    //SDL_RenderCopy(renderer, data->phrase, NULL, &rect);
    // show rendered frame
    SDL_RenderPresent(renderer);
}

