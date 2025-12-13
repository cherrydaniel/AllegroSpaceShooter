#ifndef _GAME_ASSETLOADER_H_
#define _GAME_ASSETLOADER_H_

#include <mutex>
#include <unordered_map>
#include <string>
#include <atomic>
#include <memory>
#include <functional>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "util/common.h"
#include "util/async_scope.h"
#include "userdata.h"
#include "settings.h"
#include "eventloop.h"

#define ASSET_LOAD_SUCCESS  0
#define ASSET_LOAD_FAIL     1
#define ASSET_LOAD_DEFER    2

enum AssetType {
    BMP,
    FONT,
    SHADER,
    USER_DATA,
    SETTINGS
};

struct Asset
{
    AssetType type;
    union
    {
        ALLEGRO_BITMAP* bitmap;
        ALLEGRO_FONT* font;
        ALLEGRO_SHADER* shader;
        UserData* userData;
        Settings* settings;
    };
};

class AssetMap
{
private:
    std::unordered_map<std::string, struct Asset*> assets;
    void destroyAsset(struct Asset* asset);
    void insertAsset(const char* key, struct Asset* asset);
    std::mutex mtx;
public:
    AssetMap(){}
    ~AssetMap();
    void addBitmap(const char* key, ALLEGRO_BITMAP* bmp);
    void addFont(const char* key, ALLEGRO_FONT* font);
    void addShader(const char* key, ALLEGRO_SHADER* shader);
    void addUserData(const char* key, UserData* userData);
    void addSettings(const char* key, Settings* settings);
    ALLEGRO_BITMAP* getBitmap(const char* key);
    ALLEGRO_FONT* getFont(const char* key);
    ALLEGRO_SHADER* getShader(const char* key);
    UserData* getUserData(const char* key);
    Settings* getSettings(const char* key);
    bool deleteAsset(const char* key);
    size_t size();
};

class AssetLoader
{
private:
    EventLoop* evloop;
    AsyncScope asyncScope;
    std::atomic_uint32_t loaded;
    std::atomic_uint32_t requested;
    std::atomic_uint32_t failed;
    void doLoad(std::function<int(void)> fn);
public:
    AssetMap* assetMap;
    AssetLoader(EventLoop* evloop, AssetMap* assetMap) :
        evloop{evloop},
        assetMap{assetMap},
        loaded{0},
        requested{0},
        failed{0} {}
    ~AssetLoader(){}
    void loadBitmap(const char* key, const char* path);
    void loadFont(const char* key, const char* path, int size, int flags);
    void loadShader(const char* key, const char* path);
    void loadUserData(const char* key, const char* path);
    void loadSettings(const char* key, const char* path);
    uint32_t numLoaded(){ return loaded.load(); }
    uint32_t numRequested(){ return requested.load(); }
    uint32_t numFailed(){ return failed.load(); }
};

#endif
