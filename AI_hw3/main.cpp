//
//  main.cpp
//  AI_hw3
//
//  Created by Shangqi Wu on 15/2/2.
//  Copyright (c) 2015 Shangqi Wu. All rights reserved.
//

#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <limits>
#include <sys/time.h>
#include "State.h"

using namespace std;

/*
 * This program is homework 3 of course CS 510.
 * It can perform A* search for solving a puzzle. Also contains an extra credit A*.
 * All interaction with users will be displayed as program runs, there is no need to have input arguments in command line other than program name itself.
 * There are 11 default input file choices, by simply entering 1 to 11. Hints will output to screen.
 * You can choose different search algorithm by pressing 1 and 2.
 * Result will be displayed on screen once the program comes to an end. Result includes move list, number of steps, final state, nodes calculated and search time.
 */

// Pre-define function for A* search
State A_Star(State _Parent, long &_counter);
// Pre-define function for Extra Credit
State A_Star_EC(State _Parent, long &_counter);

// Pre-define tool functions
State readfile(const string &input_name);
bool string_split(const string &text_content, vector<int> &row_of_state, const char identifier, int &rows);
void get_filename(string &file_name);

// MAIN FUNCTION STARTS HERE ********************************************************************************
int main(void) {
    string filename;
    // Get filename from user input
    get_filename(filename);
    // Load state from text file
    State InitialState = readfile(filename);
    cout<<"Press 1 for standard A* search algorithm\nPress 2 for improved A* search algorithm(Extra Credit)\nPlease enter your choice:"<<endl;
    int choice = 0;
    while (choice != 1 && choice != 2) {
        //cin>>choice;
        choice = 1;
        if (choice == 1) {
            cout<<"You have chosen standard A* algorithm."<<endl;
            // Perform breadth-first search
            long node_counter = 0;
            struct timeval start,end;
            gettimeofday(&start, NULL);
            State final = A_Star(InitialState, node_counter);
            // Get start and end time
            gettimeofday(&end, NULL);
            // If the state is not empty, it is a solution, then generate an ouput.
            if (final.getBoard().empty() == false) {
                cout<<"Solution of the puzzle is:"<<endl;
                vector<vector<string> > moved = final.getMove();
                for (int i = 0; i < (int)moved.size(); i++) {
                    cout<<"Step "<<i+1<<": move brick "<<moved[i][1]<<" "<<moved[i][0]<<endl;
                }
                cout<<"The final state is:"<<endl;
                final.state_display_initial();
                // Display nodes numbers
                cout<<"This program has explored "<<node_counter<<" nodes."<<endl;
                // Calculate run time
                long double ms = 1000*(end.tv_sec - start.tv_sec) + (long double)(end.tv_usec - start.tv_usec)/1000;
                cout<<"Total run time for this puzzle is: "<<ms<<" ms."<<endl;
                break;
            } else { // If it is not solved, give a notice.
                cout<<"This program cannot find solution for this puzzle. The final state is:"<<endl;
                final.state_display();
                break;
            }
        } else if (choice == 2) {
            cout<<"You have chosen improved A* search algorithm(Extra Credit)."<<endl;
            // Perform depth-first search
            long node_counter = 0;
            struct timeval start,end;
            gettimeofday(&start, NULL);
            State final = A_Star_EC(InitialState, node_counter);
            gettimeofday(&end, NULL);
            if (final.getBoard().empty() == false) {
                cout<<"Solution of the puzzle is:"<<endl;
                vector<vector<string> > moved = final.getMove();
                for (int i = 0; i < (int)moved.size(); i++) {
                    cout<<"Step "<<i+1<<": move brick "<<moved[i][1]<<" "<<moved[i][0]<<endl;
                }
                cout<<"The final state is:"<<endl;
                final.state_display_initial();
                cout<<"This program has explored "<<node_counter<<" nodes."<<endl;
                long double ms = 1000*(end.tv_sec - start.tv_sec) + (long double)(end.tv_usec - start.tv_usec)/1000;
                cout<<"Total run time for this puzzle is: "<<ms<<" ms."<<endl;
                break;
            } else {
                cout<<"This program cannot find solution for this puzzle. The final state is:"<<endl;
                final.state_display();
                break;
            }
        } else {
            cout<<"Please just enter 1 or 2, or control+c to terminate this program."<<endl;
        }
    }
    return 0;
}
// MAIN FUNCTION ENDS HERE **********************************************************************************



// Function for breadth-first search ************************************************************************
State A_Star(State _Parent, long &_counter) {
    // Store all expanede nodes.
    vector<State> closed;
    // A queue for nodes waiting for expasion.
    list<State> open;
    _Parent.state_norm();
    open.push_back(_Parent);
    State solution;
    while (open.size() > 0) {
        // Pull a node from front of a queue and push it to expanded stack
        State current_node = open.front();
        open.pop_front();
        closed.push_back(current_node);
        vector<vector<string> > moves_of_node = current_node.all_moves_onboard();
        for (int i = 0; i < (int)moves_of_node.size(); i++) {
            int brick_no = atoi(moves_of_node[i].back().c_str());
            for (int j = 0; j < (int)moves_of_node[i].size()-1; j++) {
                State new_child = current_node.applyMoveCloning(brick_no, moves_of_node[i][j]);
                new_child.heuristic_standard();
                if (new_child.complete_check() == true) {
                    closed.push_back(new_child);
                    solution = new_child;
					_counter = (int)open.size() + (int)closed.size() + 1;
                    return solution;
                }
                new_child.state_norm();
                // To check if this node is eligible for further process.
                bool ineligible = false;
                for (int k = 0; k < (int)closed.size(); k++) {
                    if (new_child == closed[k]) {
                        ineligible = true;
                        break;
                    }
                }
                if (ineligible == false) {
                    for (list<State>::iterator it_tmp = open.begin(); it_tmp != open.end(); ++it_tmp) {
                        if (new_child == *it_tmp) {
							if (it_tmp->getDepth() > new_child.getDepth()) {
								*it_tmp = new_child;
							}
							ineligible = true;
							break;
                        }
                    }
                }
                if (ineligible == false) {
                    if (open.size() == 0) {
                        open.push_back(new_child);
                    } else {
                        bool added = false;
                        for (list<State>::iterator it_tmp = open.begin(); it_tmp != open.end(); ++it_tmp) {
                            if (new_child < *it_tmp) {
                                open.insert(it_tmp, new_child);
                                added = true;
                                break;
                            }
                        }
                        if (added == false) {
                            open.push_back(new_child);
                        }
                    }
                }
            }
        }
    }
    return solution;
}

State A_Star_EC(State _Parent, long &_counter) {
	// Store all expanede nodes.
	vector<State> closed;
	// A queue for nodes waiting for expasion.
	list<State> open;
	_Parent.state_norm();
	open.push_back(_Parent);
	State solution;
	while (open.size() > 0) {
		// Pull a node from front of a queue and push it to expanded stack
		State current_node = open.front();
		open.pop_front();
		closed.push_back(current_node);
		vector<vector<string> > moves_of_node = current_node.all_moves_onboard();
		for (int i = 0; i < (int)moves_of_node.size(); i++) {
			int brick_no = atoi(moves_of_node[i].back().c_str());
			for (int j = 0; j < (int)moves_of_node[i].size()-1; j++) {
				State new_child = current_node.applyMoveCloning(brick_no, moves_of_node[i][j]);
				new_child.heuristic_EC();
				if (new_child.complete_check() == true) {
					closed.push_back(new_child);
					solution = new_child;
					_counter = (int)open.size() + (int)closed.size() + 1;
					return solution;
				}
				new_child.state_norm();
				// To check if this node is eligible for further process.
				bool ineligible = false;
				for (int k = 0; k < (int)closed.size(); k++) {
					if (new_child == closed[k]) {
						ineligible = true;
						break;
					}
				}
				if (ineligible == false) {
					for (list<State>::iterator it_tmp = open.begin(); it_tmp != open.end(); ++it_tmp) {
						if (new_child == *it_tmp) {
							if (it_tmp->getDepth() > new_child.getDepth()) {
								*it_tmp = new_child;
							}
							ineligible = true;
							break;
						}
					}
				}
				if (ineligible == false) {
					if (open.size() == 0) {
						open.push_back(new_child);
					} else {
						bool added = false;
						for (list<State>::iterator it_tmp = open.begin(); it_tmp != open.end(); ++it_tmp) {
							if (new_child < *it_tmp) {
								open.insert(it_tmp, new_child);
								added = true;
								break;
							}
						}
						if (added == false) {
							open.push_back(new_child);
						}
					}
				}
			}
		}
	}
	return solution;
}

// FUNCTIONS FOR SEARCH ALGORITHM END HERE ******************************************************************

// Function for reading input text file and transfer it into a state of matrix ******************************
State readfile(const string &input_name) {
    bool file = true;
    int height = 0;
    int width = 0;
    // This is to save state and for initialization of state.
    vector<vector<int> > initial_board;
    ifstream input(input_name.c_str());
    if (!input) {
        cout<<"The file doesn't exist. Please check your input file path."<<endl;
        abort();
    }
    int row = -1;
    bool status = true;
    while (input) {
        string buff;
        input>>buff;
        // The first row is height and width, don't push them into initial_board.
        if (row == -1) {
            vector<int> board_size;
            status = string_split(buff, board_size, ',', row);
            if (status == false) {
                file = false;
                break;
            }
            width = board_size[0];
            height = board_size[1];
            initial_board.resize(height);
        } else {
            if (row < height) {
                // Extract numbers in state row by row.
                status = string_split(buff, initial_board[row], ',', row);
                if (status == false && row < height) {
                    file = false;
                    break;
                }
            }
        }
        row++;
    }
    input.close();
    // Perform some check to see if there is something wrong with input puzzle.
    if (file == false) {
        cout<<"The input file doesn't fit the format."<<endl;
        abort();
    }
    if (initial_board.size() != height) {
        cout<<"The input file doesn't contain a valid puzzle."<<endl;
        abort();
    }
    for (int i = 0; i < initial_board.size(); i++) {
        if (initial_board[i].size() != width) {
            cout<<"The input file doesn't contain a valid puzzle."<<endl;
            abort();
        }
        if (i == 0 || i == (int)initial_board.size()-1) {
            for (int j = 0; j < (int)initial_board[i].size(); j++) {
                if (abs(initial_board[i][j]) != 1) {
                    cout<<"The input file doesn't contain a valid puzzle."<<endl;
                    abort();
                }
            }
        } else {
            if (abs(initial_board[i].front())!=1 || abs(initial_board[i].back())!=1) {
                cout<<"The input file doesn't contain a valid puzzle."<<endl;
                abort();
            }
        }
    }
    State initial(initial_board);
    return initial;
}

// Split input file foor processing *************************************************************************
bool string_split(const string &text_content, vector<int> &row_of_state, const char identifier, int &rows) {
    if (text_content.empty()) {
        return false;
    }
    string str_temp;
    // Extract numbers between commas and push them into state.
    int last_comma = -1;
    int strlen = (int)text_content.length();
    for (int i = 0; i < strlen; i++) {
        if (text_content[i] == identifier) {
            if (i > last_comma + 1) {
                str_temp = text_content.substr(last_comma+1,i-last_comma-1);
                row_of_state.push_back(atoi(str_temp.c_str()));
                last_comma = i;
                str_temp.clear();
            }
        }
    }
    if (!str_temp.empty()) {
        row_of_state.push_back(atoi(str_temp.c_str()));
        str_temp.clear();
    }
    return true;
}

// Process input filename *************************************************************************************
void get_filename(string &file_name) {
    cout<<"Please choose your option:\nPress 1 for SBP-level0.txt\nPress 2 for SBP-level1.txt\nPress 3 for SBP-level2.txt\nPress 4 for SBP-level3.txt\nPress 5 for SBP-bricks-level1.txt\nPress 6 for SBP-bricks-level2.txt\nPress 7 for SBP-bricks-level3.txt\nPress 8 for SBP-bricks-level4.txt\nPress 9 for SBP-bricks-level5.txt\nPress 10 for SBP-bricks-level6.txt\nPress 11 for SBP-bricks-level7.txt\nOr enter your input file path:\nThen press enter to continue."<<endl;
    cin>>file_name;
    if (file_name.empty()) {
        file_name = "./SBP-level0.txt";
        cout<<"No input file path, using default path: ./SBP-level-0.txt"<<endl;
    } else if (file_name == "1") {
        file_name = "./SBP-level0.txt";
    } else if (file_name == "2") {
        file_name = "./SBP-level1.txt";
    } else if (file_name == "3") {
        file_name = "./SBP-level2.txt";
    } else if (file_name == "4") {
        file_name = "./SBP-level3.txt";
    } else if (file_name == "5") {
        file_name = "./SBP-bricks-level1.txt";
    } else if (file_name == "6") {
        file_name = "./SBP-bricks-level2.txt";
    } else if (file_name == "7") {
        file_name = "./SBP-bricks-level3.txt";
    } else if (file_name == "8") {
        file_name = "./SBP-bricks-level4.txt";
    } else if (file_name == "9") {
        file_name = "./SBP-bricks-level5.txt";
    } else if (file_name == "10") {
        file_name = "./SBP-bricks-level6.txt";
    } else if (file_name == "11") {
        file_name = "./SBP-bricks-level7.txt";
    }
}
