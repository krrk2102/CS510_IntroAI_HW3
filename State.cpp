//
//  State.cpp
//  AI_hw3
//
//  Created by Shangqi Wu on 15/2/2.
//  Copyright (c) 2015 Shangqi Wu. All rights reserved.
//

#include <iostream>
#include "unistd.h"
#include <cstdlib>
#include "State.h"
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cmath>

using namespace std;

// A simple function determining how to sort bricks by their index.
bool sort_for_bricks (brick _brick1, brick _brick2) {
    return _brick1.index < _brick2.index;
}

// Default constructor, initialize data with 0, clear vectors.
State::State() {
    height = 0;
    width = 0;
    depth = 0;
    heuristic = 0;
}

// Another constructor with input state, automatically recognize bricks and height & width.
State::State(const vector<vector<int> > &_board) {
    if (_board.size() != 0) {
        height = (int)_board.size();
        width = (int)_board[0].size();
        depth = 0;
        heuristic = 0;
        board.resize(height);
        for (int i = 0; i < (int)_board.size(); i++) {
            // Resize state with height and width.
            board[i].resize(width);
            for (int j = 0; j < (int)_board[i].size(); j++) {
                if ((int)_board[i].size() == width) {
                    // Copy state one by one.
                    board[i][j] = _board[i][j];
                    // If a smallest element has a index greater than 1, i.e. it is not wall, exit, nonthing, add them to relative bricks.
                    if (board[i][j] > 1) {
                        // If the index matches one of bricks which has already been added before, just push this position to brick member.
                        bool exist = false;
                        for (int k = 0; k < bricks.size(); k++) {
                            if (_board[i][j] == bricks[k].index) {
                                position tmp;
                                tmp.x = j;
                                tmp.y = i;
                                bricks[k].members.push_back(tmp);
                                exist = true;
                            }
                        }
                        // If this index not recognized before, create a new brick with the index, and position.
                        if (exist == false) {
                            position tmp;
                            tmp.x = j;
                            tmp.y = i;
                            brick newpiece;
                            newpiece.index = _board[i][j];
                            newpiece.members.push_back(tmp);
                            newpiece.initial_index = _board[i][j];
                            bricks.push_back(newpiece);
                        }
                    } else if (board[i][j] == -1) {
                        position tmp;
                        tmp.x = j;
                        tmp.y = i;
                        exit.push_back(tmp);
                    }
                } else { // If there is something wrong with state size, i.e. each row size doesn't equal to each other, clear all information and break.
                    height = 0;
                    width = 0;
                    depth = 0;
                    board.clear();
                    bricks.clear();
                    break;
                }
            }
            sort(bricks.begin(), bricks.end(), sort_for_bricks);
        }
        // After copying and recognizing all information, normalize this state.
        state_norm();
    } else { // If input is an empty board, clear everything.
        height = 0;
        width = 0;
        depth = 0;
        heuristic = 0;
    }
}

// This method detects all available move for specified brick. Input is brick number, which is the sequence in vector of birkcs.
vector<string> State::move_of_a_brick(int _brick_no) {
    vector<string> single_move;
    bool up_able = true;
    bool down_able = true;
    bool left_able = true;
    bool right_able = true;
    // Detecting 4 direction around an element in a brick, if there is 0, the direction is available for move.
    for (int i = 0; i < bricks[_brick_no].members.size(); i++) {
        if (bricks[_brick_no].members[i].y < height && bricks[_brick_no].members[i].x < width) {
            if (board[bricks[_brick_no].members[i].y-1][bricks[_brick_no].members[i].x] != 0 && board[bricks[_brick_no].members[i].y-1][bricks[_brick_no].members[i].x] != bricks[_brick_no].index) {
                up_able = false;
            }
            if (board[bricks[_brick_no].members[i].y][bricks[_brick_no].members[i].x+1] != 0 && board[bricks[_brick_no].members[i].y][bricks[_brick_no].members[i].x+1] != bricks[_brick_no].index) {
                right_able = false;
            }
        }
        if (bricks[_brick_no].members[i].y > 0 && bricks[_brick_no].members[i].x > 0) {
            if (board[bricks[_brick_no].members[i].y+1][bricks[_brick_no].members[i].x] != 0 && board[bricks[_brick_no].members[i].y+1][bricks[_brick_no].members[i].x] != bricks[_brick_no].index) {
                down_able = false;
            }
            if (board[bricks[_brick_no].members[i].y][bricks[_brick_no].members[i].x-1] != 0 && board[bricks[_brick_no].members[i].y][bricks[_brick_no].members[i].x-1] != bricks[_brick_no].index) {
                left_able = false;
            }
        }
    }
    // Pushing detected available move to returning move list, also check if the move forms a loop with last move.
    if (up_able==true) {
        if (move_record.size() == 0) {
            single_move.push_back("up");
            bricks[_brick_no].available_move.push_back("up");
        } else {
            if (move_record.back()[0].compare("down")!=0 || atoi(move_record.back().back().c_str())!=bricks[_brick_no].index) {
                single_move.push_back("up");
                bricks[_brick_no].available_move.push_back("up");
            }
        }
    }
    if (down_able == true) {
        if (move_record.size() == 0) {
            single_move.push_back("down");
            bricks[_brick_no].available_move.push_back("down");
        } else {
            if (move_record.back()[0].compare("up")!=0 || atoi(move_record.back().back().c_str())!=bricks[_brick_no].index) {
                single_move.push_back("down");
                bricks[_brick_no].available_move.push_back("down");
            }
        }
    }
    if (left_able == true) {
        if (move_record.size() == 0) {
            single_move.push_back("left");
            bricks[_brick_no].available_move.push_back("left");
        } else {
            if (move_record.back()[0].compare("right")!=0 || atoi(move_record.back().back().c_str())!=bricks[_brick_no].index) {
                single_move.push_back("left");
                bricks[_brick_no].available_move.push_back("left");
            }
        }
    }
    if (right_able == true) {
        if (move_record.size() == 0) {
            single_move.push_back("right");
            bricks[_brick_no].available_move.push_back("right");
        } else {
            if (move_record.back()[0].compare("left")!=0 || atoi(move_record.back().back().c_str())!=bricks[_brick_no].index) {
                single_move.push_back("right");
                bricks[_brick_no].available_move.push_back("right");
            }
        }
    }
    // For master piece, provide another check for exit, if it is next to the exit, just eliminate all other move choices, leaving only the way to the exit and return the path.
    if (bricks[_brick_no].index == 2) {
        bool final_up = true;
        bool final_down = true;
        bool final_left = true;
        bool final_right = true;
        for (int i = 0; i < bricks[_brick_no].members.size(); i++) {
            if (bricks[_brick_no].members[i].y < height && bricks[_brick_no].members[i].x < width) {
                if (board[bricks[_brick_no].members[i].y-1][bricks[_brick_no].members[i].x] != -1 && board[bricks[_brick_no].members[i].y-1][bricks[_brick_no].members[i].x] != bricks[_brick_no].index) {
                    final_up = false;
                }
                if (board[bricks[_brick_no].members[i].y][bricks[_brick_no].members[i].x+1] != -1 && board[bricks[_brick_no].members[i].y][bricks[_brick_no].members[i].x+1] != bricks[_brick_no].index) {
                    final_right = false;
                }
            }
            if (bricks[_brick_no].members[i].y > 0 && bricks[_brick_no].members[i].x > 0) {
                if (board[bricks[_brick_no].members[i].y+1][bricks[_brick_no].members[i].x] != -1 && board[bricks[_brick_no].members[i].y+1][bricks[_brick_no].members[i].x] != bricks[_brick_no].index) {
                    final_down = false;
                }
                if (board[bricks[_brick_no].members[i].y][bricks[_brick_no].members[i].x-1] != -1 && board[bricks[_brick_no].members[i].y][bricks[_brick_no].members[i].x-1] != bricks[_brick_no].index) {
                    final_left = false;
                }
            }
        }
        if (final_up == true) {
            single_move.clear();
            single_move.push_back("up");
            single_move.push_back("final");
            bricks[_brick_no].available_move.clear();
            bricks[_brick_no].available_move.push_back("up");
        }
        if (final_down == true) {
            single_move.clear();
            single_move.push_back("down");
            single_move.push_back("final");
            bricks[_brick_no].available_move.clear();
            bricks[_brick_no].available_move.push_back("down");
        }
        if (final_left == true) {
            single_move.clear();
            single_move.push_back("left");
            single_move.push_back("final");
            bricks[_brick_no].available_move.clear();
            bricks[_brick_no].available_move.push_back("left");
        }
        if (final_right == true) {
            single_move.clear();
            single_move.push_back("right");
            single_move.push_back("final");
            bricks[_brick_no].available_move.clear();
            bricks[_brick_no].available_move.push_back("right");
        }
    }
    return single_move;
}

// Record all moves performed in a state for check in the future, in case of looping.
void State::add_move(int _brick_no, string _move) {
    // It stores move first, then followed by brick INDEX, NOT brick NUMBER.
    stringstream ss;
    ss<<bricks[_brick_no].initial_index;
    vector<string> tmp;
    tmp.push_back(_move);
    tmp.push_back(ss.str());
    move_record.push_back(tmp);
    ss.clear();
}

// A method incase there is a need for state updating
void State::updateState(vector<vector<int> > _board) {
    if (_board.size() != 0) {
        for (int i = 0; i < _board[i].size(); i++) {
            if (_board[i].size() != _board[0].size()) {
                break;
            }
        }
        height = (int)_board.size();
        width = (int)_board[0].size();
        bricks.clear();
        for (int i = 0; i < _board.size(); i++) {
            for (int j = 0; j < _board[i].size(); j++) {
                board[i][j] = _board[i][j];
                bool exist = false;
                for (int k = 0; k < bricks.size(); k++) {
                    if (_board[i][j] == bricks[k].index) {
                        position tmp;
                        tmp.x = i;
                        tmp.y = j;
                        bricks[k].members.push_back(tmp);
                        exist = true;
                    }
                }
                if (exist == false) {
                    position tmp;
                    tmp.x = i;
                    tmp.y = j;
                    brick newpiece;
                    newpiece.index = _board[i][j];
                    newpiece.members.push_back(tmp);
                    newpiece.initial_index = _board[i][j];
                    bricks.push_back(newpiece);
                }
            }
        }
    } else cout<<"Update failed."<<endl;
}

int State::getDepth() {
    return depth;
}

int State::getHeight() {
    return height;
}

int State::getWidth() {
    return width;
}

vector<vector<int> > State::getBoard() {
    return board;
}

vector<brick> State::getBricks() {
    return bricks;
}

vector<vector<string> > State::getMove() {
    return move_record;
}

State State::clone_state() {
    return *this;
}

vector<position> State::getExit() {
    return exit;
}

// Calculate heuristic value
void State::heuristic_standard() {
    heuristic = 0;
    for (int i = 0; i < (int)bricks[0].members.size(); i++) {
        for (int j = 0; j < (int)exit.size(); j++) {
            heuristic += abs(bricks[0].members[i].y-exit[j].y) + abs(bricks[0].members[i].x-exit[j].x);
        }
    }
    heuristic = heuristic/((double)bricks[0].members.size()*(double)exit.size());
    heuristic += (double)depth;
}

// Calculate heuristic value for extra credit
void State::heuristic_EC() {
    heuristic = 0;
    for (int i = 0; i < (int)bricks[0].members.size(); i++) {
        for (int j = 0; j < (int)exit.size(); j++) {
            heuristic += abs(bricks[0].members[i].y-exit[j].y) + abs(bricks[0].members[i].x-exit[j].x);
        }
    }
    heuristic = heuristic/(double)exit.size();
    heuristic += (double)depth;
}

// Display state board to screen.
void State::state_display() {
    cout<<width<<","<<height<<","<<endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cout<<setw(2)<<board[i][j]<<",";
        }
        cout<<endl;
    }
}

// If there is still -1, return false, otherwise return true.
bool State::complete_check() {
    for (int i = 0; i < height; i++) {
        if (i == 0 || i == height-1) {
            for (int j = 0; j < width; j++) {
                if (board[i][j] == -1) {
                    return false;
                } else if (board[i][j] == 2) {
                    return true;
                }
            }
        } else {
            if (board[i].front() == -1 || board[i].back() == -1) {
                return false;
            } else if (board[i].front() == 2 || board[i].back() == 2) {
                return true;
            }
        }
    }
    return true;
}

// Return a list of move for all bricks on the board.
vector<vector<string> > State::all_moves_onboard() {
    stringstream ss;
    vector<vector<string> > all_move;
    for (int i = 0; i < bricks.size(); i++) {
        vector<string> move_one_brick = move_of_a_brick(i);
        if (move_one_brick.size() > 0) {
            // If the master piece is next to exit, make sure the path to the exit is next move.
            if (move_one_brick.back().compare("final") == 0) {
                all_move.clear();
                move_one_brick.pop_back();
                ss<<i;
                move_one_brick.push_back(ss.str());
                ss.clear();
                all_move.push_back(move_one_brick);
                return all_move;
            }
            ss<<i;
            string _number;
            ss>>_number;
            move_one_brick.push_back(_number);
            all_move.push_back(move_one_brick);
            ss.clear();
        }
    }
    return all_move;
}

// Perform move by input brick number and move command. (NOT brick index)
void State::applyMove(int _piece_no, string _move_cmd) {
    // Double check if input command is available move of this brick.
    if (_move_cmd.compare("up") == 0) {
        bool move = false;
        for (int i = 0; i < bricks[_piece_no].available_move.size(); i++) {
            if (bricks[_piece_no].available_move[i].compare(_move_cmd) == 0) {
                move = true;
            }
        }
        // Performing move by replace brick index by 0 on the board, then calculate new positions of brick, write index to new position.
        if (move == true) {
            for (int i = 0; i < bricks[_piece_no].members.size(); i++) {
                board[bricks[_piece_no].members[i].y][bricks[_piece_no].members[i].x] = 0;
                bricks[_piece_no].members[i].y--;
            }
            for (int i = 0; i < bricks[_piece_no].members.size(); i++) {
                board[bricks[_piece_no].members[i].y][bricks[_piece_no].members[i].x] = bricks[_piece_no].index;
            }
        } else cout<<"Piece no "<<bricks[_piece_no].index<<" cannot move up."<<endl;
    } else if (_move_cmd.compare("down") == 0) {
        bool move = false;
        for (int i = 0; i < bricks[_piece_no].available_move.size(); i++) {
            if (bricks[_piece_no].available_move[i].compare(_move_cmd) == 0) {
                move = true;
            }
        }
        if (move == true) {
            for (int i = 0; i < bricks[_piece_no].members.size(); i++) {
                board[bricks[_piece_no].members[i].y][bricks[_piece_no].members[i].x] = 0;
                bricks[_piece_no].members[i].y++;
            }
            for (int i = 0; i < bricks[_piece_no].members.size(); i++) {
                board[bricks[_piece_no].members[i].y][bricks[_piece_no].members[i].x] = bricks[_piece_no].index;
            }
        } else cout<<"Piece no "<<bricks[_piece_no].index<<" cannot move down."<<endl;
    } else if (_move_cmd.compare("left") == 0) {
        bool move = false;
        for (int i = 0; i < bricks[_piece_no].available_move.size(); i++) {
            if (bricks[_piece_no].available_move[i].compare(_move_cmd) == 0) {
                move = true;
            }
        }
        if (move == true) {
            for (int i = 0; i < bricks[_piece_no].members.size(); i++) {
                board[bricks[_piece_no].members[i].y][bricks[_piece_no].members[i].x] = 0;
                bricks[_piece_no].members[i].x--;
            }
            for (int i = 0; i < bricks[_piece_no].members.size(); i++) {
                board[bricks[_piece_no].members[i].y][bricks[_piece_no].members[i].x] = bricks[_piece_no].index;
            }
        } else cout<<"Piece no "<<bricks[_piece_no].index<<" cannot move left."<<endl;
    } else if (_move_cmd.compare("right") == 0) {
        bool move = false;
        for (int i = 0; i < bricks[_piece_no].available_move.size(); i++) {
            if (bricks[_piece_no].available_move[i].compare(_move_cmd) == 0) {
                move = true;
            }
        }
        if (move == true) {
            for (int i = 0; i < bricks[_piece_no].members.size(); i++) {
                board[bricks[_piece_no].members[i].y][bricks[_piece_no].members[i].x] = 0;
                bricks[_piece_no].members[i].x++;
            }
            for (int i = 0; i < bricks[_piece_no].members.size(); i++) {
                board[bricks[_piece_no].members[i].y][bricks[_piece_no].members[i].x] = bricks[_piece_no].index;
            }
        } else cout<<"Piece no "<<bricks[_piece_no].index<<" cannot move right."<<endl;
    }
    // Add this move to move list. Clear move list for next detection.
    add_move(_piece_no, _move_cmd);
    depth++;
    for (int i = 0; i < bricks.size(); i++) {
        bricks[i].available_move.clear();
    }
}

// Performing moves to a new state identical to this, without changing this.
State State::applyMoveCloning(int _piece_no, string _move_cmd) {
    State for_move = *this;
    for_move.applyMove(_piece_no, _move_cmd);
    return for_move;
}

// Normalization of a state, changing itself.
void State::state_norm() {
    int nextIdx = 3;
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            if (board[i][j] == nextIdx) {
                nextIdx++;
            } else if (board[i][j] > nextIdx) {
                swap_Idx(nextIdx, board[i][j]);
                nextIdx++;
            }
        }
    }
}

// Little function for state normalization
void State::swap_Idx(int _index1, int _index2) {
    if (_index1 > _index2) {
        int tmp = _index2;
        _index2 = _index1;
        _index1 = tmp;
    }
    for (int i = 0; i < bricks.size(); i++) {
        if (bricks[i].index == _index1) {
            bricks[i].index = _index2;
            for (int j = 0; j < bricks[i].members.size(); j++) {
                board[bricks[i].members[j].y][bricks[i].members[j].x] = _index2;
            }
            continue;
        }
        if (bricks[i].index == _index2) {
            bricks[i].index = _index1;
            for (int j = 0; j < bricks[i].members.size(); j++) {
                board[bricks[i].members[j].y][bricks[i].members[j].x] = _index1;
            }
            continue;
        }
    }
}

bool State::cmp(State _input) {
    State for_cmp = *this;
    for_cmp.state_norm();
    _input.state_norm();
    if (for_cmp == _input) {
        return true;
    }
    return false;
}

void State::state_display_initial() {
    for (int i = 0; i < (int)bricks.size(); i++) {
        for (int j = 0; j < (int)bricks[i].members.size(); j++) {
            board[bricks[i].members[j].y][bricks[i].members[j].x] = bricks[i].initial_index;
        }
    }
    state_display();
}

// Overloaded operator, copying everything into this state.
const State & State::operator=(const State &_original) {
    height = _original.height;
    width = _original.width;
    depth = _original.depth;
    heuristic = _original.heuristic;
    exit.clear();
    for (int i = 0; i < (int)_original.exit.size(); i++) {
        exit.push_back(_original.exit[i]);
    }
    board.clear();
    board.resize(height);
    for (int i = 0; i < height; i++) {
        board[i].resize(width);
        for (int j = 0; j < width; j++) {
            board[i][j] = _original.board[i][j];
        }
    }
    bricks.clear();
    for (int i = 0; i < (int)_original.bricks.size(); i++) {
        bricks.push_back(_original.bricks[i]);
    }
    move_record.clear();
    move_record.resize(_original.move_record.size());
    for (int i = 0; i < (int)move_record.size(); i++) {
        move_record[i].resize(2);
        for (int j = 0; j < 2; j++) {
            move_record[i][j] = _original.move_record[i][j];
        }
    }
    return *this;
}

// A simple comparision just examine each position if their index are the same, without normalizing.
bool State::operator==(const State &_new) {
    if (height==_new.height && width==_new.width) {
        for (int i = 1; i < (int)board.size()-1; i++) {
            for (int j = 1; j < (int)board[i].size()-1; j++) {
                if (board[i][j] != _new.board[i][j]) {
                    return false;
                }
            }
        }
    } else return false;
    return true;
}

// Only for comparison of heuristic value.
const bool State::operator<(const State &_new) {
    return heuristic < _new.heuristic;
}