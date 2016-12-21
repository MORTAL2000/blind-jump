//========================================================================//
// Copyright (C) 2016 Evan Bowman                                         //
// Liscensed under GPL 3, see: <http://www.gnu.org/licenses/>.            //
//========================================================================//

#pragma once
#include "DetailGroup.hpp"
#include "GfxContext.hpp"
#include "LuaProvider.hpp"
#include "RenderType.hpp"
#include "alias.hpp"
#include "backgroundHandler.hpp"
#include "camera.hpp"
#include "colors.hpp"
#include "effectsController.hpp"
#include "enemyController.hpp"
#include "framework/option.hpp"
#include "inputController.hpp"
#include "player.hpp"
#include "resourceHandler.hpp"
#include "soundController.hpp"
#include "tileController.hpp"
#include "userInterface.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <atomic>
#include <cmath>
#include <mutex>
#include "Entity.hpp"

using EntityTable = std::map<std::string, std::vector<std::shared_ptr<Entity>>>;

struct ConfigData {
    sf::Vector2f drawableArea;
    bool enableVsync;
    bool showMouse;
    int framerateLimit;
};

class Game {
public:
    enum class TransitionState {
        None,
        ExitBeamEnter,
        ExitBeamInflate,
        ExitBeamDeflate,
        TransitionOut,
        TransitionIn,
        EntryBeamDrop,
        EntryBeamFade
    };
    Game(const ConfigData &);
    void updateLogic(LuaProvider &);
    void updateGraphics();
    void eventLoop();
    void nextLevel();
    int getLevel();
    DetailGroup & getDetails();
    tileController & getTileController();
    Player & getPlayer();
    EffectGroup & getEffects();
    SoundController & getSounds();
    InputController & getInputController();
    ui::Backend & getUI();
    ui::Frontend & getUIFrontend();
    Camera & getCamera();
    sf::Vector2f viewPort;
    EntityTable & getEntityTable();
    TransitionState transitionState;
    sf::RenderWindow & getWindow();
    const sf::Time & getElapsedTime();
    void setElapsedTime(const sf::Time &);

private:
    void init();
    EntityTable entityTable;
    sf::RenderWindow window;
    sf::Time elapsedTime;
    InputController input;
    SoundController sounds;
    Player player;
    Camera camera;
    ui::Backend UI;
    tileController tiles;
    EffectGroup effectGroup;
    DetailGroup detailGroup;
    ui::Frontend uiFrontend;
    std::mutex overworldMutex, UIMutex, transitionMutex;
    int level;
    bool stashed, preload;
    sf::Sprite vignetteSprite;
    backgroundHandler bkg;
    sf::Sprite vignetteShadowSpr;
    tileController::Tileset set;
    GfxContext gfxContext;
    sf::Sprite beamGlowSpr;
    sf::View worldView, hudView;
    sf::RenderTexture lightingMap;
    sf::RenderTexture target, secondPass, thirdPass, stash;
    sf::RectangleShape transitionShape, beamShape;
    void updateTransitions(const sf::Time &);
    void drawTransitions(sf::RenderWindow &);
    int_fast64_t timer;
};

// The first room is not procedurally generated so the positions of the walls
// need to be hard coded
// There may be a more efficient way to store this, but at O2 optimization clang
// and gcc convert the whole thing to an array of longs
extern const std::array<std::pair<float, float>, 59> levelZeroWalls;

// Foreign function interface for lua requires a pointer
// to an instance of the game to be accessible...
void setgGamePtr(Game *);
Game * getgGamePtr();

inline ConfigData getConfig(lua_State * state) {
    lua_getglobal(state, "getConf");
    if (lua_pcall(state, 0, 1, 0) != 0) {
        throw std::runtime_error(lua_tostring(state, -1));
    }
    lua_pushstring(state, "width");
    lua_gettable(state, -2);
    float width = lua_tonumber(state, -1);
    lua_pop(state, 1);
    lua_pushstring(state, "height");
    lua_gettable(state, -2);
    float height = lua_tonumber(state, -1);
    lua_pop(state, 1);
    lua_pushstring(state, "vsync");
    lua_gettable(state, -2);
    bool vsync = lua_toboolean(state, -1);
    lua_pop(state, 1);
    lua_pushstring(state, "showCursor");
    lua_gettable(state, -2);
    bool showCursor = lua_toboolean(state, -1);
    lua_pop(state, 1);
    lua_pushstring(state, "frameratestateimit");
    lua_gettable(state, -2);
    int frameratestateimit = lua_tointeger(state, -1);
    lua_pop(state, 2);
    return ConfigData{{width, height}, vsync, showCursor, frameratestateimit};
}
