#include <memory>
#include "common.h"
#include "game.h"

int main(int argc, char** argv)
{
    Game game;
    ASSERT(game.init(), "INIT FAILED");
    game.gameloop();
    return 0;
}
