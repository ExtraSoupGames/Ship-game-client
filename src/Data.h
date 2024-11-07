#pragma once
struct PlayerState {
    int direction;
    int movementState;
    int attackState;
    PlayerState(int pDirection, int pMovementState, int pAttackState);
    PlayerState() {
        direction = 0;
        movementState = 0;
        attackState = 0;
    }
};
struct DataPoint {
    int X = 0;
    int Y = 0;
};
struct EnemyData : public DataPoint {
};
struct PlayerData : public DataPoint {
    PlayerState state;
    PlayerData(int x, int y, PlayerState playerState) {
        state = playerState;
        X = x;
        Y = y;
    }
};

struct DataStream {
    DataPoint* data;
    double timestamp;
    bool markedForDeletion;
};

