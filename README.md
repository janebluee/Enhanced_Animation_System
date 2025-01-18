# Enhanced Animation System for SA-MP/open.mp

A flexible animation system for SA-MP/open.mp that provides smooth transitions, queuing, and custom animation management.

## Features

- **Custom Animation Creation**: Create and manage custom animations with full control over movement and timing
- **Animation Queue System**: Queue multiple animations to play in sequence
- **Smooth Transitions**: Blend between animations for seamless movement
- **Priority System**: Control animation priority and interruption
- **Position Delta**: Support for position changes during animations
- **State Management**: Robust state tracking for animation status

## Installation

1. Copy the `src/enhanced_animations.inc` to your project's include directory
2. Include the file in your script:
```pawn
#include <enhanced_animations>
```

## Usage

### Creating a Custom Animation

```pawn
new danceAnim = CreateCustomAnimation(
    "DANCING",    // Animation library
    "dance_move", // Animation name
    0.0, 0.0, 1.0, // Delta X, Y, Z (movement)
    4.1,         // Speed
    true,        // Loop
    true,        // Lock X
    true,        // Lock Y
    false,       // Freeze
    1            // Duration
);
```

### Playing Animations

```pawn
// Play animation directly
PlayCustomAnimation(playerid, danceAnim, 2, true);

// Queue animations
QueueAnimation(playerid, "FIGHT", "spin", 1, true);
QueueAnimation(playerid, "LAND", "land", 1, true);
```

### Handling Player Events

```pawn
public OnPlayerConnect(playerid) {
    InitPlayerAnimations(playerid);
    return 1;
}

public OnPlayerDisconnect(playerid, reason) {
    CleanupPlayerAnimations(playerid);
    return 1;
}
```

## Functions

- `CreateCustomAnimation`: Create a new custom animation
- `PlayCustomAnimation`: Play a custom animation directly
- `QueueAnimation`: Add an animation to the player's queue
- `ClearAnimationQueue`: Clear all queued animations
- `InitPlayerAnimations`: Initialize animation system for a player
- `CleanupPlayerAnimations`: Clean up animation resources

## States

The system uses the following states to track animation status:
- `ANIM_STATE_IDLE`: No animation playing
- `ANIM_STATE_PLAYING`: Currently playing an animation
- `ANIM_STATE_TRANSITIONING`: Transitioning between animations
- `ANIM_STATE_BLENDING`: Blending animations
- `ANIM_STATE_QUEUED`: Waiting in queue

## Requirements

- SA-MP 0.3.7 or later / open.mp
- Latest open.mp/sa-mp includes

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
