#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
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
        SDL_Rect rect;
        std::string name;
        std::string readableSize;
        int type;
        SDL_Texture *phrase;
        SDL_Texture *phraseSize;
        std::string permissions;
        SDL_Texture *phrasePermission;
        std::string path;
        int fileSize;
        std::vector<FileEntry*> list;
};
  
// Sub class inheriting from Base Class(Parent)
class Directory : public FileEntry
{
    public:
        
};
 
// Sub class inheriting from Base Class(Parent)
class Exec : public FileEntry
{
    public:
        
        
        
};
// Sub class inheriting from Base Class(Parent)
class Image : public FileEntry
{
    public:
        
};
// Sub class inheriting from Base Class(Parent)
class Video : public FileEntry
{
    public:
};
// Sub class inheriting from Base Class(Parent)
class Code : public FileEntry
{
    public:
};
// Sub class inheriting from Base Class(Parent)
class Misc : public FileEntry
{
    public:
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
    SDL_Texture *buttonR;
    bool scroll_selected;
    SDL_Rect scroll_rect;
    bool button_selected;
    SDL_Rect button_rect;
    SDL_Point offset;
    int offsetY;
    std::vector<FileEntry> list;
}AppData;

void initialize(SDL_Renderer *renderer, AppData *data, std::vector<FileEntry*> list);
void render(SDL_Renderer *renderer, AppData *data, std::vector<FileEntry*> list);
std::string humanReadable(int input);
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

std::vector<FileEntry*> fillFiles(std::string path)
{
    std::string dot (".");
    std::size_t max = -1;
    std::vector<FileEntry*> list;
    std::vector<std::string> files;
    for (const auto & entry : fs::directory_iterator(path)){
        std::string filename(entry.path());
        if(fs::is_directory(entry) && filename.find(dot) != max){continue;}
        else if(fs::is_directory(entry))
        {
            Directory* dict = new Directory();
            dict->name = removeSchmutz(filename.substr(path.length()));
            //std::cout << "dict.name: " << dict.name<<"\n";
            dict->list = fillFiles(filename );
            dict->type = 0;
            dict->path = filename;
            list.push_back(dict);
        }
        else
        {
            Image* im;
            Video* vid;
            Code* code;
            Exec* exec;
            Misc* misc;
            
            switch (typeIder(filename))
            {
            case 0://image
                im = new Image();
                im->name = removeSchmutz(filename.substr(path.length()));
                
                im->permissions = getPerms(fs::status(filename).permissions());
                im->type = 2;
                im->path = filename;
                im->fileSize = entry.file_size();
                im->readableSize = humanReadable(im->fileSize);
                
                list.push_back(im);
                break;
            
            case 1://video
                vid = new Video();
                vid->name = removeSchmutz(filename.substr(path.length()));
                vid->permissions = getPerms(fs::status(filename).permissions());
                vid->type = 3;
                vid->fileSize = entry.file_size();
                vid->path = filename;
                vid->readableSize = humanReadable(vid->fileSize);
                
                list.push_back(vid);
                
                break;
            case 2://code
                code = new Code();
                code->name = removeSchmutz(filename.substr(path.length()));
                code->permissions = getPerms(fs::status(filename).permissions());
                code->type = 4;
                code->fileSize = entry.file_size();
                code->path = filename;
                code->readableSize = humanReadable(code->fileSize);
                
                list.push_back(code);
                break;
            
            case 3://exec
                exec = new Exec();
                exec->name = removeSchmutz(filename.substr(path.length()));
                exec->permissions = getPerms(fs::status(filename).permissions());
                exec->type = 1;
                exec->fileSize = entry.file_size();
                exec->path = filename;
                exec->readableSize = humanReadable(exec->fileSize);
                
                list.push_back(code);
                
                break;

            case 4://misc
                misc = new Misc();
                misc->name = removeSchmutz(filename.substr(path.length()));
                misc->permissions = getPerms(fs::status(filename).permissions());
                misc->type = 5;
                misc->fileSize = entry.file_size();
                misc->path = filename;
                misc->readableSize = humanReadable(misc->fileSize);
                
                list.push_back(misc);
                break;

            default:
                break;
            }
            
        }
    }
    return list;
}

bool compare(FileEntry* x, FileEntry* y) 
{
    std::string xlower = x->name;
    std::string ylower = y->name;
    transform(xlower.begin(), xlower.end(), xlower.begin(), tolower);
    transform(ylower.begin(), ylower.end(), ylower.begin(), tolower);
    return xlower < ylower;
    //std::cout << "xlower: " <<  xlower << "\n";
    //std::cout << "ylower: " <<  ylower << "\n";

} 

/*std::vector<FileEntry*> addParentDictR(std::vector<FileEntry*> parentlist)
{
    for (int i = 0; i < parentlist.size(); i++)
    {
        if(parentlist.at(i)->type == 0 && parentlist.at(i)->name != "..")
        {
            addParentDict(parentlist, parentlist.at(i)->list);
        }
    }
    return parentlist;
}*/


std::vector<FileEntry*> addParentDict(std::vector<FileEntry*> parentlist, std::vector<FileEntry*> currlist)
{
    //std::cout << "adding parent to list1:" << '\n';
    if(currlist.size() == 0)
    {
        //std::cout << "list name:" << currlist.at(i)->name << '\n';
        Directory* dict = new Directory();
        dict->name = "..";
        dict->list = parentlist;
        dict->type = 0;
        //std::cout << "adding parent to list of zero:" << '\n';
        currlist.push_back(dict);
    }
    else if(currlist.at(0)->name != "..")
    {
        //std::cout << "adding parent to list:2" << '\n';
        Directory* dict = new Directory();
        dict->name = "..";
        dict->list = parentlist;
        dict->type = 0;
        //std::cout << "adding parent to list3:" << '\n';
        currlist.push_back(dict);
        //std::cout << "adding parent to list4:" << '\n';
        //std::cout << "currlist.at(currlist.size()-1)->name: "<< currlist.at(currlist.size()-1)->name << '\n';
    }
    return currlist;
}

void calcY(std::vector<FileEntry*> list)
{
    for(int i = 0; i < list.size();i++)
    {
        list.at(i)->x = 10;
        list.at(i)->y = i*60 + i*10 +31;
        list.at(i)->h = 60;
        list.at(i)->w = 60;
    }
}

std::string humanReadable(int input)
{
    int temp = 0;
    std::string result;
            if(input > 1073741824)
            {
                temp = input/1073741824;
                result = std::to_string(temp);
                result.append("GiB");
            }
            else if(input > 10480576)
            {
                temp = input/10480576;
                result = std::to_string(temp);
                result.append("MiB");
            }
            else if(input > 1024)
            {
                temp = input/1024;
                result = std::to_string(temp);
                result.append("KiB");
            }
            else{
                result = std::to_string(input);
                result.append("B");
            }
            return result;
            //std::cout << "list.at(i)->readableSize: " << list.at(i)->readableSize << "\n";        
    
}

int main(int argc, char **argv)
{
    char *home = getenv("HOME");
    printf("HOME: %s\n", home);

    //grab all files
    std::string path(home);
    std::vector<FileEntry*> list = fillFiles(path);
    std::vector<FileEntry*> parentList;
    std::vector<FileEntry*> currList = list;
    std::sort(list.begin(),list.end(), compare);
    //currList = addParentDict(currList);
    //std::cout << list.at(0).name << '\n';
    /*for (int i=0; i<list.size(); i++)
    {
        std::cout << list.at(i)->name << '\n';
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
    initialize(renderer, &data,list);
    render(renderer, &data, list);
    SDL_Event event;
    SDL_WaitEvent(&event);
    while (event.type != SDL_QUIT)
    {
        
        SDL_WaitEvent(&event);
        switch (event.type)
        {
            case SDL_MOUSEMOTION:
                if (data.scroll_selected)
                {
                    data.scroll_rect.y = event.motion.y - data.offset.y;
                    
                }
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT &&
                    event.button.x >= data.scroll_rect.x &&
                    event.button.x <= data.scroll_rect.x + data.scroll_rect.w &&
                    event.button.y >= data.scroll_rect.y &&
                    event.button.y <= data.scroll_rect.y + data.scroll_rect.h)
                {
                    data.scroll_selected = true;
                    data.offset.x = event.button.x - data.scroll_rect.x;
                    data.offset.y = event.button.y - data.scroll_rect.y;
                }

                if (event.button.button == SDL_BUTTON_LEFT &&
                    event.button.x >= data.button_rect.x &&
                    event.button.x <= data.button_rect.x + data.button_rect.w &&
                    event.button.y >= data.button_rect.y &&
                    event.button.y <= data.button_rect.y + data.button_rect.h && !data.scroll_selected)
                {
                    if(data.button_selected){data.button_selected = false;}
                    else{data.button_selected = true;}
                }

                for(int i = 0; i < currList.size();i++)
                {
                    //std::cout << "list.at(i)->name: " << currList.at(i)->name<<"\n";
                   if (event.button.button == SDL_BUTTON_LEFT &&
                    event.button.x >= currList.at(i)->rect.x &&
                    event.button.x <= currList.at(i)->rect.x + currList.at(i)->rect.w &&
                    event.button.y >= currList.at(i)->rect.y &&
                    event.button.y <= currList.at(i)->rect.y + currList.at(i)->rect.h && !data.scroll_selected)
                    {
                        if(currList.at(i)->type == 0)//dirct
                        {
                            parentList = currList;
                            if(currList.at(i)->name.compare("..") != 0)
                            {
                                currList = addParentDict(parentList,currList.at(i)->list);
                            }  
                            else{
                                currList = currList.at(i)->list;
                            }
                            
                            if(currList.size()>1)
                            {
                                std::sort(currList.begin(),currList.end(), compare);
                            }
                            calcY(currList);
                            initialize(renderer, &data,currList);
                            data.offsetY = 0;
                        }
                        else
                        {
                            int pid = fork();
                            if(pid == 0)//child
                            {
                                char oparg[9];
                                char filePath[128];
                                strcpy(oparg, "xdg-open");
                                strcpy(filePath, currList.at(i)->path.c_str());
                                char *const args[3] = {oparg, filePath, NULL};
                                execvp("xdg-open", args);
                            }
                        }
                    } 
                }
                break;
            case SDL_MOUSEBUTTONUP:
                data.scroll_selected = false;
                break;
        }
        render(renderer, &data, currList);
    }

    // clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    for (int i=0; i<list.size(); i++)
    {
        SDL_DestroyTexture(list.at(i)->phrase);
    }
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

void initialize(SDL_Renderer *renderer, AppData *data, std::vector<FileEntry*> list)
{
    // set color of background when erasing frame
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);

    data->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 16);
    SDL_Color phrase_color = {0,0,0};
    for(int i = 0; i < list.size();i++)
    {
        //name
        const char *nameChar = list.at(i)->name.c_str();
        SDL_Surface *text_surf = TTF_RenderText_Solid(data->font, nameChar, phrase_color);
        list.at(i)->phrase = SDL_CreateTextureFromSurface(renderer,text_surf);
        list.at(i)->rect.x = list.at(i)->x-1;
        list.at(i)->rect.w = 350;
        list.at(i)->rect.h = 62;
        list.at(i)->rect.y = list.at(i)->y-1;
        if(list.at(i)->type !=0)
        {
            //permissions size
            const char *permissionChar = list.at(i)->permissions.c_str();
            SDL_Surface *text_surfPerm = TTF_RenderText_Solid(data->font, permissionChar, phrase_color);
            list.at(i)->phrasePermission = SDL_CreateTextureFromSurface(renderer,text_surfPerm);

            //file size
            const char *sizeChar = list.at(i)->readableSize.c_str();
            SDL_Surface *text_surfSize = TTF_RenderText_Solid(data->font, sizeChar, phrase_color);
            list.at(i)->phraseSize = SDL_CreateTextureFromSurface(renderer,text_surfSize);

            SDL_FreeSurface(text_surfSize);
            SDL_FreeSurface(text_surfPerm);
        }
        SDL_FreeSurface(text_surf);
        
    }

    //scrollbar
    data->offsetY =0;
    data->button_rect.x = 320;
    data->button_rect.w = 20;
    data->button_rect.h = 20;
    data->button_rect.y = 5;
    data->button_selected = false;
    data->scroll_rect.x = 780;
    data->scroll_rect.w = 20;
    data->scroll_rect.h = 60;
    data->scroll_rect.y = 0;
    data->scroll_selected = false;
    phrase_color = {255,255,255};
    SDL_Surface *text_surfButton = TTF_RenderText_Solid(data->font, "Recursive Mode", phrase_color);
    data->buttonR = SDL_CreateTextureFromSurface(renderer,text_surfButton);
    SDL_FreeSurface(text_surfButton);
    
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

void render(SDL_Renderer *renderer, AppData *data, std::vector<FileEntry*> list)
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 0, 235, 235, 255);
    SDL_RenderClear(renderer);

    //render all file icons
    
    double bottomLimit = list.at(list.size()-1)->y+160;
    double temp = data->scroll_rect.y;
    double percent = (temp/600.0)*bottomLimit;
    data->offsetY = int(percent);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &data->scroll_rect);
    for(int i = 0; i < list.size(); i++)
    {
        list.at(i)->rect.y = list.at(i)->y - data->offsetY;
        if(list.at(i)->y-data->offsetY < 600 && list.at(i)->y-data->offsetY > -29)
        {
            SDL_Rect rectPerm;
            rectPerm.x = 420;
            rectPerm.w = 60;
            rectPerm.h = 60;
            rectPerm.y = list.at(i)->y-data->offsetY;
            
            SDL_Rect rect;
            rect.x = 10;
            rect.w = 60;
            rect.h = 60;
            rect.y = list.at(i)->y-data->offsetY;
            
            SDL_Rect rectStr;
            rectStr.x = 110;
            rectStr.w = 300;
            rectStr.h = 60;
            rectStr.y = list.at(i)->y-data->offsetY;

            SDL_Rect rectFSize;
            rectFSize.x = 630;
            rectFSize.w = 300;
            rectFSize.h = 60;
            rectFSize.y = list.at(i)->y-data->offsetY;
            
            if(list.at(i)->type == 0)
            {
                SDL_RenderCopy(renderer, data->direct, NULL, &rect);
                SDL_QueryTexture(list.at(i)->phrase, NULL,NULL, &(rectStr.w), &(rectStr.h));
                SDL_RenderCopy(renderer, list.at(i)->phrase, NULL, &rectStr);
            }
            else if(list.at(i)->type == 1)
            {
                SDL_RenderCopy(renderer, data->exec, NULL, &rect);
                SDL_QueryTexture(list.at(i)->phrase, NULL,NULL, &(rectStr.w), &(rectStr.h));
                SDL_RenderCopy(renderer, list.at(i)->phrase, NULL, &rectStr);
                SDL_QueryTexture(list.at(i)->phrasePermission, NULL,NULL, &(rectPerm.w), &(rectPerm.h));
                SDL_RenderCopy(renderer, list.at(i)->phrasePermission, NULL, &rectPerm);
                SDL_QueryTexture(list.at(i)->phraseSize, NULL,NULL, &(rectFSize.w), &(rectFSize.h));
                SDL_RenderCopy(renderer, list.at(i)->phraseSize, NULL, &rectFSize);
            }
            else if(list.at(i)->type == 2)
            {
                SDL_RenderCopy(renderer, data->image, NULL, &rect);
                SDL_QueryTexture(list.at(i)->phrase, NULL,NULL, &(rectStr.w), &(rectStr.h));
                SDL_RenderCopy(renderer, list.at(i)->phrase, NULL, &rectStr);
                SDL_QueryTexture(list.at(i)->phrasePermission, NULL,NULL, &(rectPerm.w), &(rectPerm.h));
                SDL_RenderCopy(renderer, list.at(i)->phrasePermission, NULL, &rectPerm);
                SDL_QueryTexture(list.at(i)->phraseSize, NULL,NULL, &(rectFSize.w), &(rectFSize.h));
                SDL_RenderCopy(renderer, list.at(i)->phraseSize, NULL, &rectFSize);
            }
            else if(list.at(i)->type == 3)
            {
                SDL_RenderCopy(renderer, data->video, NULL, &rect);
                SDL_QueryTexture(list.at(i)->phrase, NULL,NULL, &(rectStr.w), &(rectStr.h));
                SDL_RenderCopy(renderer, list.at(i)->phrase, NULL, &rectStr);
                SDL_QueryTexture(list.at(i)->phrasePermission, NULL,NULL, &(rectPerm.w), &(rectPerm.h));
                SDL_RenderCopy(renderer, list.at(i)->phrasePermission, NULL, &rectPerm);
                SDL_QueryTexture(list.at(i)->phraseSize, NULL,NULL, &(rectFSize.w), &(rectFSize.h));
                SDL_RenderCopy(renderer, list.at(i)->phraseSize, NULL, &rectFSize);
            }
            else if(list.at(i)->type == 4)
            {
                SDL_RenderCopy(renderer, data->code, NULL, &rect);
                SDL_QueryTexture(list.at(i)->phrase, NULL,NULL, &(rectStr.w), &(rectStr.h));
                SDL_RenderCopy(renderer, list.at(i)->phrase, NULL, &rectStr);
                SDL_QueryTexture(list.at(i)->phrasePermission, NULL,NULL, &(rectPerm.w), &(rectPerm.h));
                SDL_RenderCopy(renderer, list.at(i)->phrasePermission, NULL, &rectPerm);
                SDL_QueryTexture(list.at(i)->phraseSize, NULL,NULL, &(rectFSize.w), &(rectFSize.h));
                SDL_RenderCopy(renderer, list.at(i)->phraseSize, NULL, &rectFSize);
            }
            else if(list.at(i)->type == 5)
            {
                SDL_RenderCopy(renderer, data->other, NULL, &rect);
                SDL_QueryTexture(list.at(i)->phrase, NULL,NULL, &(rectStr.w), &(rectStr.h));
                SDL_RenderCopy(renderer, list.at(i)->phrase, NULL, &rectStr);
                SDL_QueryTexture(list.at(i)->phrasePermission, NULL,NULL, &(rectPerm.w), &(rectPerm.h));
                SDL_RenderCopy(renderer, list.at(i)->phrasePermission, NULL, &rectPerm);
                SDL_QueryTexture(list.at(i)->phraseSize, NULL,NULL, &(rectFSize.w), &(rectFSize.h));
                SDL_RenderCopy(renderer, list.at(i)->phraseSize, NULL, &rectFSize);
            }
        }
    }

    SDL_Rect bar;
    bar.x = 0;
    bar.y = 0;
    bar.w = 780;
    bar.h = 30;
    SDL_SetRenderDrawColor(renderer, 102, 102, 102, 255);
    SDL_RenderFillRect(renderer, &bar);
    if(data->button_selected)
    {
        SDL_SetRenderDrawColor(renderer, 12, 210, 12, 255);
    }
    else{
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }
    SDL_RenderFillRect(renderer, &data->button_rect);
    SDL_Rect rect;
            rect.x = 350;
            rect.w = 60;
            rect.h = 60;
            rect.y = 2;
    SDL_QueryTexture(data->buttonR, NULL,NULL, &(rect.w), &(rect.h));
    SDL_RenderCopy(renderer, data->buttonR, NULL, &rect);
    // show rendered frame
    SDL_RenderPresent(renderer);
}

