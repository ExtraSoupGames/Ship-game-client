#pragma once
#include <string>
using namespace std;
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
enum FlopperStates {
    GROUNDED,
    AIRBORNE,
    SPAWNING
};
enum ClingabingStates {
    IDLE,
    DASHING,
    ATTACHED
};
struct DataPoint {
    int X = 0;
    int Y = 0;
    DataPoint(int x, int y) {
        X = x;
        Y = y;
    }
};
struct EnemyData : public DataPoint {
    EnemyData(int x, int y) : DataPoint(x, y) {
    }
};
struct BobleechData : public EnemyData {
    BobleechData(int x, int y) : EnemyData(x, y){
    }
};
struct FlopperData : public EnemyData {
    FlopperStates state;
    FlopperData(int x, int y, string binaryIn) : EnemyData(x, y) {
        if (binaryIn == "00") {
            state = GROUNDED;
        }
        else if (binaryIn == "01") {
            state = AIRBORNE;
        }
        else if (binaryIn == "10") {
            state = SPAWNING;
        }
        else {
            throw new exception("invalid flopper state provided");
        }
    }
};
struct ClingabingData : public EnemyData {
    ClingabingStates state;
    ClingabingData(int x, int y, string binaryIn) : EnemyData(x, y) {
        if (binaryIn == "00") {
            state = IDLE;
        }
        else if (binaryIn == "01") {
            state = DASHING;
        }
        else if (binaryIn == "10") {
            state = ATTACHED;
        }
        else {
            throw new exception("invalid clingabing state provided");
        }
    }
};
struct PlayerData : public DataPoint {
    PlayerState state;
    PlayerData(int x, int y, PlayerState playerState) : DataPoint(x, y) {
        state = playerState;
    }
};

struct DataStream {
    DataPoint* data;
    double timestamp;
    bool markedForDeletion;
};
struct GameReport {
    int timeSurvived;
    GameReport(int survivalTime) {
        timeSurvived = survivalTime;
    }
};