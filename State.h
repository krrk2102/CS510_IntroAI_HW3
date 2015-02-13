//
//  State.h
//  AI_hw3
//
//  Created by Shangqi Wu on 15/2/22.
//  Copyright (c) 2015 Shangqi Wu. All rights reserved.
//

#ifndef __AI_hw3__State__
#define __AI_hw3__State__

#include <stdio.h>
#include <vector>

using namespace std;

// This struct represents position of a smallest element in the state
typedef struct _position {
    int x;
    int y;
}position;

// This struct represents a brick, containing its index, positions of members, and its available moves.
typedef struct _brick {
    int index;
    int initial_index;
    vector<position> members;
    vector<string> available_move;
}brick;

class State{
private:
    int height;
    int width;
    // Using a 2-D vector to represents state.
    vector<vector<int> > board;
    // Using a vector of bricks to store information about pieces
    vector<brick> bricks;
    vector<vector<string> > move_record;
    // These positions store where are -1s located
    vector<position> exit;
    int depth;
    double heuristic;
    // Following are private methods.
    vector<string> move_of_a_brick(int _birck_no);
    void swap_Idx(int _index1, int _index2);
    void add_move(int _brick_no, string _move);
public:
    // Constructors
    State();
    State(const vector<vector<int> > &_board);
    
    // Methods to modify data
    void updateState(vector<vector<int> > _board);
    void heuristic_standard();
    void heuristic_EC();
    
    // Methods to retrieve data
    int getHeight();
    int getWidth();
    int getDepth();
    vector<vector<int> > getBoard();
    vector<brick> getBricks();
    vector<vector<string> > getMove();
    vector<position> getExit();
    
    // Methods required by assignments
    void state_display();
    void state_display_initial();
    State clone_state();
    bool complete_check();
    vector<vector<string> > all_moves_onboard();
    void applyMove(int _piece_no, string _move_cmd);
    State applyMoveCloning(int _piece_no, string _move_cmd);
    bool cmp(State _input);
    void state_norm();
    
    const State & operator=(const State &_original);
    bool operator==(const State &_new);
    const bool operator<(const State &_new);
};

#endif /* defined(__AI_hw3__State__) */
