#include "assetloader.h"

AssetMap::~AssetMap()
{
    for (auto it = assets.begin(); it!=assets.end(); it++)
    {
        PRINT_DEBUG("Destroying asset: %s", it->first.c_str());
        destroyAsset(it->second);
    }
    assets.clear();
}

void AssetMap::destroyAsset(struct Asset* asset)
{
    switch (asset->type)
    {
        case BMP: al_destroy_bitmap(asset->bitmap); break;
        case FONT: al_destroy_font(asset->font); break;
        case SHADER: al_destroy_shader(asset->shader); break;
        case USER_DATA: delete asset->userData; break;
        case SETTINGS: delete asset->settings; break;
    }
    delete asset;
}

void AssetMap::insertAsset(const char* key, struct Asset* asset)
{
    std::lock_guard<std::mutex> lock{mtx};
    assets[key] = asset;
}

void AssetMap::addBitmap(const char* key, ALLEGRO_BITMAP* bitmap)
{
    Asset* asset = new Asset();
    asset->type = BMP;
    asset->bitmap = bitmap;
    insertAsset(key, asset);
    PRINT_DEBUG("Loaded bitmap: %s", key);
}

void AssetMap::addFont(const char* key, ALLEGRO_FONT* font)
{
    Asset* asset = new Asset();
    asset->type = FONT;
    asset->font = font;
    insertAsset(key, asset);
    PRINT_DEBUG("Loaded font: %s", key);
}

void AssetMap::addShader(const char* key, ALLEGRO_SHADER* shader)
{
    Asset* asset = new Asset();
    asset->type = SHADER;
    asset->shader = shader;
    insertAsset(key, asset);
    PRINT_DEBUG("Loaded shader: %s", key);
}

void AssetMap::addUserData(const char* key, UserData* userData)
{
    Asset* asset = new Asset();
    asset->type = USER_DATA;
    asset->userData = userData;
    insertAsset(key, asset);
    PRINT_DEBUG("Loaded user data: %s", key);
}

void AssetMap::addSettings(const char* key, Settings* settings)
{
    Asset* asset = new Asset();
    asset->type = SETTINGS;
    asset->settings = settings;
    insertAsset(key, asset);
    PRINT_DEBUG("Loaded settings: %s", key);
}

ALLEGRO_BITMAP* AssetMap::getBitmap(const char* key)
{
    auto it = assets.find(key);
    if (it==assets.end()||it->second->type!=BMP)
        return nullptr;
    return it->second->bitmap;
}

ALLEGRO_FONT* AssetMap::getFont(const char* key)
{
    auto it = assets.find(key);
    if (it==assets.end()||it->second->type!=FONT)
        return nullptr;
    return it->second->font;
}

ALLEGRO_SHADER* AssetMap::getShader(const char* key)
{
    auto it = assets.find(key);
    if (it==assets.end()||it->second->type!=SHADER)
        return nullptr;
    return it->second->shader;
}

UserData* AssetMap::getUserData(const char* key)
{
    auto it = assets.find(key);
    if (it==assets.end()||it->second->type!=USER_DATA)
        return nullptr;
    return it->second->userData;
}

Settings* AssetMap::getSettings(const char* key)
{
    auto it = assets.find(key);
    if (it==assets.end()||it->second->type!=SETTINGS)
        return nullptr;
    return it->second->settings;
}

bool AssetMap::deleteAsset(const char* key)
{
    auto it = assets.find(key);
    if (it==assets.end())
        return false;
    destroyAsset(it->second);
    assets.erase(it);
    return true;
}

size_t AssetMap::size() { return assets.size(); }

void AssetLoader::doLoad(std::function<int(void)> fn)
{
    requested++;
    futures.push_back(std::async(std::launch::async,
    [this, fn]
    {
        int res = fn();
        if (res==ASSET_LOAD_FAIL)
            failed++;
        else if (res==ASSET_LOAD_SUCCESS)
            loaded++;
    }));
}

void AssetLoader::loadBitmap(const char* key, const char* path)
{
    doLoad([this, key, path]
    {
        al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR);
        ALLEGRO_BITMAP* bitmap = al_load_bitmap(path);
        if (!bitmap)
        {
            PRINT_DEBUG(
                ANSI_COLOR_RED ANSI_COLOR_BOLD "Failed to load bitmap '%s': %s" ANSI_COLOR_RESET,
                key, path);
            return ASSET_LOAD_FAIL;
        }
        assetMap->addBitmap(key, bitmap);
        return ASSET_LOAD_SUCCESS;
    });
}

void AssetLoader::loadFont(const char* key, const char* path, int size, int flags)
{
    doLoad([this, key, path, size, flags]
    {
        ALLEGRO_FONT* font = al_load_font(path, size, flags);
        if (!font)
        {
            PRINT_DEBUG(
                ANSI_COLOR_RED ANSI_COLOR_BOLD "Failed to load font '%s': %s" ANSI_COLOR_RESET,
                key, path);
            return ASSET_LOAD_FAIL;
        }
        assetMap->addFont(key, font);
        return ASSET_LOAD_SUCCESS;
    });
}

void AssetLoader::loadShader(const char* key, const char* path)
{
    doLoad([this, key, path]
    {
        bool readerr;
        std::string shaderString = fileReadAllAsString(path, readerr);
        if (readerr)
        {
            PRINT_DEBUG(ANSI_COLOR_RED "File open failed '%s': %s" ANSI_COLOR_RESET,
                key, path);
            return ASSET_LOAD_FAIL;
        }
        evloop->runOnMain([this, key, path, shaderString]
        {
            ALLEGRO_SHADER* shader = al_create_shader(ALLEGRO_SHADER_GLSL);
            if (!shader)
                goto shaderError;
            if (!al_attach_shader_source(shader, ALLEGRO_VERTEX_SHADER,
                al_get_default_shader_source(ALLEGRO_SHADER_GLSL, ALLEGRO_VERTEX_SHADER)))
                goto shaderError;
            if (!al_attach_shader_source(shader, ALLEGRO_PIXEL_SHADER, shaderString.c_str()))
                goto shaderError;
            if (!al_build_shader(shader))
                goto shaderError;
            assetMap->addShader(key, shader);
            loaded++;
            return;
shaderError:
            PRINT_DEBUG(
                ANSI_COLOR_RED ANSI_COLOR_BOLD "Failed to load shader '%s': %s -> %s" ANSI_COLOR_RESET,
                key, path, al_get_shader_log(shader));
            failed++;
        });
        return ASSET_LOAD_DEFER;
    });
}

void AssetLoader::loadUserData(const char* key, const char* path)
{
    doLoad([this, key, path]
    {
        UserData* userData = new UserData();
        readUserData(userData, path); // no check - ok if doesn't exist
        assetMap->addUserData(key, userData);
        return ASSET_LOAD_SUCCESS;
    });
}

void AssetLoader::loadSettings(const char* key, const char* path)
{
    doLoad([this, key, path]
    {
        Settings* settings = new Settings();
        readSettings(settings, path); // no check - ok if doesn't exist
        assetMap->addSettings(key, settings);
        return ASSET_LOAD_SUCCESS;
    });
}
