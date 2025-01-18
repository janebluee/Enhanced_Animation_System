#if defined _enhanced_animations_included
    #endinput
#endif
#define _enhanced_animations_included

#include <open.mp>

#define MAX_CUSTOM_ANIMATIONS 100
#define MAX_ANIMATION_NAME 32
#define MAX_ANIMATION_LIB 32
#define MAX_QUEUED_ANIMATIONS 10

// Forward declarations
forward ProcessAnimationQueue(playerid);
forward UpdateAnimationState(playerid);
forward HandleAnimationTransition(playerid);
forward OnPlayerAnimationStart(playerid, const animname[]);
forward OnPlayerAnimationComplete(playerid);

enum E_ANIMATION_STATE {
    ANIM_STATE_IDLE,
    ANIM_STATE_PLAYING,
    ANIM_STATE_TRANSITIONING,
    ANIM_STATE_BLENDING,
    ANIM_STATE_QUEUED
}

enum E_CUSTOM_ANIMATION {
    c_animLib[MAX_ANIMATION_LIB],
    c_animName[MAX_ANIMATION_NAME],
    Float:c_deltaX,
    Float:c_deltaY,
    Float:c_deltaZ,
    Float:c_speed,
    bool:c_loop,
    bool:c_lockx,
    bool:c_locky,
    bool:c_freeze,
    c_time,
    bool:c_isValid
}

enum E_ANIMATION_DATA {
    Float:animX,
    Float:animY,
    Float:animZ,
    Float:animSpeed,
    bool:animLoop,
    bool:animLockX,
    bool:animLockY,
    bool:animFreezeOnComplete,
    animTime,
    animIndex,
    animPriority,
    bool:animInterruptible,
    Float:animBlendSpeed,
    E_ANIMATION_STATE:animState,
    animSequence[32],
    bool:animInQueue,
    animQueueSize
}

enum E_QUEUE_ITEM {
    q_animLib[MAX_ANIMATION_LIB],
    q_animName[MAX_ANIMATION_NAME],
    q_priority,
    bool:q_interruptible
}

static g_playerAnimData[MAX_PLAYERS][E_ANIMATION_DATA];
static g_playerCurrentAnim[MAX_PLAYERS];
static g_playerAnimationTimer[MAX_PLAYERS];
static g_customAnimations[MAX_CUSTOM_ANIMATIONS][E_CUSTOM_ANIMATION];
static g_queueIndex[MAX_PLAYERS];
static g_animQueue[MAX_PLAYERS][MAX_QUEUED_ANIMATIONS][E_QUEUE_ITEM];

stock CreateCustomAnimation(const animlib[], const animname[], Float:deltaX = 0.0, Float:deltaY = 0.0, Float:deltaZ = 0.0, Float:speed = 4.1, bool:loop = false, bool:lockx = true, bool:locky = true, bool:freeze = false, time = 1) {
    new anim_idx = -1;
    
    for(new i = 0; i < MAX_CUSTOM_ANIMATIONS; i++) {
        if(!g_customAnimations[i][c_isValid]) {
            anim_idx = i;
            break;
        }
    }
    
    if(anim_idx == -1) return -1;
    
    strmid(g_customAnimations[anim_idx][c_animLib], animlib, 0, strlen(animlib));
    strmid(g_customAnimations[anim_idx][c_animName], animname, 0, strlen(animname));
    g_customAnimations[anim_idx][c_deltaX] = deltaX;
    g_customAnimations[anim_idx][c_deltaY] = deltaY;
    g_customAnimations[anim_idx][c_deltaZ] = deltaZ;
    g_customAnimations[anim_idx][c_speed] = speed;
    g_customAnimations[anim_idx][c_loop] = loop;
    g_customAnimations[anim_idx][c_lockx] = lockx;
    g_customAnimations[anim_idx][c_locky] = locky;
    g_customAnimations[anim_idx][c_freeze] = freeze;
    g_customAnimations[anim_idx][c_time] = time;
    g_customAnimations[anim_idx][c_isValid] = true;
    
    return anim_idx;
}

stock DeleteCustomAnimation(anim_idx) {
    if(anim_idx < 0 || anim_idx >= MAX_CUSTOM_ANIMATIONS) return 0;
    if(!g_customAnimations[anim_idx][c_isValid]) return 0;
    
    g_customAnimations[anim_idx][c_isValid] = false;
    return 1;
}

stock ModifyCustomAnimation(anim_idx, Float:deltaX = 0.0, Float:deltaY = 0.0, Float:deltaZ = 0.0, Float:speed = 4.1, bool:loop = false, bool:lockx = true, bool:locky = true, bool:freeze = false, time = 1) {
    if(anim_idx < 0 || anim_idx >= MAX_CUSTOM_ANIMATIONS) return 0;
    if(!g_customAnimations[anim_idx][c_isValid]) return 0;
    
    g_customAnimations[anim_idx][c_deltaX] = deltaX;
    g_customAnimations[anim_idx][c_deltaY] = deltaY;
    g_customAnimations[anim_idx][c_deltaZ] = deltaZ;
    g_customAnimations[anim_idx][c_speed] = speed;
    g_customAnimations[anim_idx][c_loop] = loop;
    g_customAnimations[anim_idx][c_lockx] = lockx;
    g_customAnimations[anim_idx][c_locky] = locky;
    g_customAnimations[anim_idx][c_freeze] = freeze;
    g_customAnimations[anim_idx][c_time] = time;
    
    return 1;
}

stock PlayCustomAnimation(playerid, anim_idx, priority = 1, bool:interruptible = true) {
    if(!IsPlayerConnected(playerid) || anim_idx < 0 || anim_idx >= MAX_CUSTOM_ANIMATIONS) return 0;
    if(!g_customAnimations[anim_idx][c_isValid]) return 0;
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    
    SetPlayerPos(playerid, 
        x + g_customAnimations[anim_idx][c_deltaX],
        y + g_customAnimations[anim_idx][c_deltaY],
        z + g_customAnimations[anim_idx][c_deltaZ]
    );
    
    return SetPlayerSmoothAnim(playerid,
        g_customAnimations[anim_idx][c_animLib],
        g_customAnimations[anim_idx][c_animName],
        g_customAnimations[anim_idx][c_speed],
        bool:g_customAnimations[anim_idx][c_loop],
        bool:g_customAnimations[anim_idx][c_lockx],
        bool:g_customAnimations[anim_idx][c_locky],
        bool:g_customAnimations[anim_idx][c_freeze],
        g_customAnimations[anim_idx][c_time],
        priority,
        interruptible
    );
}

stock QueueAnimation(playerid, const animlib[], const animname[], priority = 1, bool:interruptible = true) {
    if (!IsPlayerConnected(playerid)) return 0;
    if (g_playerAnimData[playerid][animQueueSize] >= MAX_QUEUED_ANIMATIONS) return 0;
    
    new idx = g_queueIndex[playerid];
    
    strmid(g_animQueue[playerid][idx][q_animLib], animlib, 0, strlen(animlib), MAX_ANIMATION_LIB);
    strmid(g_animQueue[playerid][idx][q_animName], animname, 0, strlen(animname), MAX_ANIMATION_NAME);
    g_animQueue[playerid][idx][q_priority] = priority;
    g_animQueue[playerid][idx][q_interruptible] = interruptible;
    
    g_playerAnimData[playerid][animInQueue] = true;
    g_playerAnimData[playerid][animQueueSize]++;
    g_queueIndex[playerid]++;

    if (g_playerAnimData[playerid][animState] == E_ANIMATION_STATE:ANIM_STATE_IDLE) {
        ProcessAnimationQueue(playerid);
    }

    return 1;
}

stock ClearAnimationQueue(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    g_queueIndex[playerid] = 0;
    g_playerAnimData[playerid][animInQueue] = false;
    g_playerAnimData[playerid][animQueueSize] = 0;
    
    return 1;
}

public ProcessAnimationQueue(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    if (!g_playerAnimData[playerid][animInQueue]) return 0;
    if (g_queueIndex[playerid] <= 0) return 0;
    
    new idx = g_queueIndex[playerid] - 1;
    SetPlayerSmoothAnim(playerid, 
        g_animQueue[playerid][idx][q_animLib],
        g_animQueue[playerid][idx][q_animName],
        4.1,
        false,
        true,
        true,
        false,
        1,
        g_animQueue[playerid][idx][q_priority],
        g_animQueue[playerid][idx][q_interruptible]
    );
    
    g_queueIndex[playerid]--;
    g_playerAnimData[playerid][animQueueSize]--;
    
    if (g_queueIndex[playerid] <= 0) {
        g_playerAnimData[playerid][animInQueue] = false;
    }
    
    return 1;
}

stock IsCustomAnimationValid(anim_idx) {
    if(anim_idx < 0 || anim_idx >= MAX_CUSTOM_ANIMATIONS) return 0;
    return g_customAnimations[anim_idx][c_isValid];
}

stock GetCustomAnimationInfo(anim_idx, &Float:deltaX, &Float:deltaY, &Float:deltaZ, &Float:speed) {
    if(anim_idx < 0 || anim_idx >= MAX_CUSTOM_ANIMATIONS) return 0;
    if(!g_customAnimations[anim_idx][c_isValid]) return 0;
    
    deltaX = g_customAnimations[anim_idx][c_deltaX];
    deltaY = g_customAnimations[anim_idx][c_deltaY];
    deltaZ = g_customAnimations[anim_idx][c_deltaZ];
    speed = g_customAnimations[anim_idx][c_speed];
    
    return 1;
}

stock GetAnimationIndex(const animlib[], const animname[]) {
    if (strlen(animlib) == 0 || strlen(animname) == 0) return -1;
    
    new idx = 0;
    for (new i = 0; i < strlen(animlib); i++) {
        idx += (animlib[i] * (i + 1));
    }
    for (new i = 0; i < strlen(animname); i++) {
        idx += (animname[i] * (i + 1));
    }
    
    return idx;
}

stock SetPlayerSmoothAnim(playerid, const animlib[], const animname[], Float:fDelta = 4.1, bool:loop = false, bool:lockx = true, bool:locky = true, bool:freeze = false, time = 1, priority = 1, bool:interruptible = true) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    new animidx = GetAnimationIndex(animlib, animname);
    if (animidx == -1) return 0;

    if (!interruptible && g_playerAnimData[playerid][animState] == E_ANIMATION_STATE:ANIM_STATE_PLAYING) {
        if (g_playerAnimData[playerid][animPriority] > priority) return 0;
    }

    g_playerAnimData[playerid][animSpeed] = fDelta;
    g_playerAnimData[playerid][animLoop] = loop;
    g_playerAnimData[playerid][animLockX] = lockx;
    g_playerAnimData[playerid][animLockY] = locky;
    g_playerAnimData[playerid][animFreezeOnComplete] = freeze;
    g_playerAnimData[playerid][animTime] = time;
    g_playerAnimData[playerid][animIndex] = animidx;
    g_playerAnimData[playerid][animPriority] = priority;
    g_playerAnimData[playerid][animInterruptible] = interruptible;
    g_playerAnimData[playerid][animState] = E_ANIMATION_STATE:ANIM_STATE_PLAYING;

    GetPlayerPos(playerid, g_playerAnimData[playerid][animX], g_playerAnimData[playerid][animY], g_playerAnimData[playerid][animZ]);
    
    ApplyAnimation(playerid, animlib, animname, fDelta, bool:loop, bool:lockx, bool:locky, bool:freeze, time, __FORCE_SYNC);
    
    g_playerCurrentAnim[playerid] = 1;
    g_playerAnimationTimer[playerid] = SetTimerEx("ProcessAnimationQueue", time * 1000, false, "i", playerid);
    
    CallLocalFunction("OnPlayerAnimationStart", "is", playerid, animname);
    return 1;
}

stock SetAnimationSequence(playerid, const sequence[], Float:blendSpeed = 4.1) {
    if (!IsPlayerConnected(playerid)) return 0;

    format(g_playerAnimData[playerid][animSequence], 32, sequence);
    g_playerAnimData[playerid][animBlendSpeed] = blendSpeed;
    
    return 1;
}

stock CreateAnimationChain(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    g_playerAnimData[playerid][animState] = E_ANIMATION_STATE:ANIM_STATE_QUEUED;
    SetTimerEx("HandleAnimationTransition", 50, false, "i", playerid);
    
    return 1;
}

stock Float:GetAnimationProgress(playerid) {
    if (!IsPlayerConnected(playerid)) return 0.0;
    
    new Float:progress = 0.0;
    if (g_playerAnimData[playerid][animTime] > 0) {
        progress = float(GetTickCount() - g_playerAnimationTimer[playerid]) / float(g_playerAnimData[playerid][animTime] * 1000);
    }
    
    return progress;
}

stock SetAnimationPriority(playerid, priority) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    g_playerAnimData[playerid][animPriority] = priority;
    return 1;
}

stock IsAnimationInterruptible(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    return g_playerAnimData[playerid][animInterruptible];
}

stock GetAnimationState(playerid) {
    if (!IsPlayerConnected(playerid)) return -1;
    
    return g_playerAnimData[playerid][animState];
}

stock IsPlayerAnimating(playerid) {
    new animlib[32], animname[32];
    GetAnimationName(
        GetPlayerAnimationIndex(playerid),
        animlib, sizeof(animlib),
        animname, sizeof(animname)
    );
    return (strlen(animlib) > 0 && strlen(animname) > 0);
}

stock SplitAnimationString(const input[], output1[], output1_size, output2[], output2_size) {
    new pos = strfind(input, "|");
    if (pos == -1) return 0;
    
    strmid(output1, input, 0, pos, output1_size);
    strmid(output2, input, pos + 1, strlen(input), output2_size);
    return 1;
}

public UpdateAnimationState(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    new E_ANIMATION_STATE:curState = g_playerAnimData[playerid][animState];
    
    if (curState == E_ANIMATION_STATE:ANIM_STATE_PLAYING) {
        if (!IsPlayerAnimating(playerid)) {
            g_playerAnimData[playerid][animState] = E_ANIMATION_STATE:ANIM_STATE_IDLE;
        }
    }
    
    return 1;
}

stock GetQueuedAnimationsCount(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    return g_playerAnimData[playerid][animQueueSize];
}

stock bool:IsAnimationQueued(playerid) {
    if (!IsPlayerConnected(playerid)) return false;
    
    return g_playerAnimData[playerid][animInQueue];
}

stock SetAnimationBlendSpeed(playerid, Float:speed) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    g_playerAnimData[playerid][animBlendSpeed] = speed;
    return 1;
}

public HandleAnimationTransition(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    if (g_playerAnimData[playerid][animState] == E_ANIMATION_STATE:ANIM_STATE_TRANSITIONING) {
        new nextAnim[32];
        strmid(nextAnim, g_playerAnimData[playerid][animSequence], 0, strlen(g_playerAnimData[playerid][animSequence]));
        
        new animlib[16], animname[16];
        if (SplitAnimationString(nextAnim, animlib, sizeof(animlib), animname, sizeof(animname))) {
            SetPlayerSmoothAnim(playerid, animlib, animname, g_playerAnimData[playerid][animBlendSpeed]);
        }
    }
    
    return 1;
}

stock InitPlayerAnimations(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    g_queueIndex[playerid] = 0;
    g_playerAnimData[playerid][animState] = E_ANIMATION_STATE:ANIM_STATE_IDLE;
    g_playerAnimData[playerid][animInQueue] = false;
    g_playerAnimData[playerid][animQueueSize] = 0;
    g_playerCurrentAnim[playerid] = 0;
    g_playerAnimationTimer[playerid] = -1;
    
    return 1;
}

stock CleanupPlayerAnimations(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    ClearAnimationQueue(playerid);
    if (g_playerAnimationTimer[playerid] != -1) {
        KillTimer(g_playerAnimationTimer[playerid]);
        g_playerAnimationTimer[playerid] = -1;
    }
    
    return 1;
}