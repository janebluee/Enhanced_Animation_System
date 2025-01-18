#include <open.mp>
#include "src/enhanced_animations"

/*Command declaration
COMMAND:testanims(playerid, params[]) {
    new jumpAnim = CreateCustomAnimation("JUMP", "jump", 0.0, 0.0, 2.0);
    new spinAnim = CreateCustomAnimation("FIGHT", "spin", 1.0, 1.0, 0.0);
    new landAnim = CreateCustomAnimation("LAND", "land", 0.0, 0.0, -2.0);

    PlayCustomAnimation(playerid, jumpAnim);
    QueueAnimation(playerid, "FIGHT", "spin", 1, true);
    QueueAnimation(playerid, "LAND", "land", 1, true);
    
    return 1;
}*/

main() {}

public OnPlayerConnect(playerid) {
    InitPlayerAnimations(playerid);
    return 1;
}

public OnPlayerDisconnect(playerid, reason) {
    CleanupPlayerAnimations(playerid);
    return 1;
}

public OnPlayerSpawn(playerid) {
    new customAnim = CreateCustomAnimation("DANCING", "dance_move", 0.0, 0.0, 1.0, 4.1, true, true, true, false, 1);
    PlayCustomAnimation(playerid, customAnim, 2, true);
    return 1;
}
