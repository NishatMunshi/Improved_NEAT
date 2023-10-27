#pragma once
// #include<random>
// #include<limits.h>
// #include<ctime>
#include "Network.hpp"

using namespace std;

// int randomInRange(int start, int end){
//     // random_device randomDevice;
//     unsigned seed = static_cast<unsigned>(time(nullptr));
//     mt19937 generator(seed);
//     uniform_int_distribution<int> distribution(start, end);
//     return distribution(generator);
// }
class Board
{
    // REPRESENTATIONS:
    // BODY PARTS: head -> length, tail -> 1, always greater than 0
    // FOOD: -1
    // EMPTY SPACE: 0
    enum CellType : int
    {
        FOOD = -1,
        EMPTY = 0,
        TAIL = 1
    };
    // -------------
    // GO-UP: 0
    // GO-RIGHT: 1
    // GO-DOWN: 2
    // GO-LEFT: 3
    // -------------
    enum Move : unsigned
    {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };
    enum CellStatus : unsigned
    {
        WALL,
        BODY,
        FOOD,
        EMPTY
    };
    const int WIDTH, HEIGHT;
    vector<vector<int>> board;

    pair<int, int> head;
    pair<int, int> food;

    int empty;

    pair<int, int> wrapIndex(int flattenedIndex)
    {
        // cout << "Here in wrap Index" << endl;
        return {flattenedIndex / WIDTH, flattenedIndex % WIDTH};
    }
    int flattenIndex(pair<int, int> wrappedIndex)
    {
        return wrappedIndex.first * WIDTH + wrappedIndex.second;
    }
    CellStatus checkCellStatus(pair<int, int> index) const
    {
        if (index.first >= HEIGHT || index.first < 0 || index.second >= WIDTH || index.second < 0)
            return CellStatus::WALL;
        else if (board[index.first][index.second] > 0)
            return CellStatus::BODY;
        else if (board[index.first][index.second] == CellType::FOOD)
            return CellStatus::FOOD;
        else
            return CellStatus::EMPTY;
    }
    void generateFood()
    {
        // cout << "Here in generate food" << endl;
        int exIndex = random_32.generate(0, empty - 1);
        // cout << "Random number between " << 0 << " and " << (empty - 1) << ": " << exIndex << endl;
        int counter = 0;
        while (true)
        {
            pair<int, int> index = wrapIndex(counter);
            if (checkCellStatus(index) == CellStatus::BODY)
            {
                counter++;
                continue;
            }
            exIndex--;
            if (exIndex < 0)
                break;
            counter++;
        }
        food = wrapIndex(counter);
        board[food.first][food.second] = -1;
        // cout << "Food position: (" << food.first << ", " << food.second << ")" << endl;
    }
    void generateInitials()
    {
        // cout << "Here in generate Initials before random" << endl;
        int flattenedIndex = random_32.generate(0, empty - 1);
        head = wrapIndex(flattenedIndex);
        // cout << "Random Number between " << 0 << " and " << (empty - 1) << ": " << flattenedIndex << ", wrapped index: (" << head.first << ", " << head.second << ")" << endl;
        length++;
        empty--;
        // cout << "Here in generate Initials after random" << endl;
        board[head.first][head.second] = length;

        // More work if extra parts required

        // Work left above
        generateFood();
    }

public:
    bool status;
    int length;

    void resetStats()
    {
        cout << "Here in reset stats" << endl;
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[0].size(); j++)
            {
                board[i][j] = 0;
            }
        }
        empty = WIDTH * HEIGHT;
        length = 0;
        generateInitials();

        status = true;
    }
    Board(int width, int height) : WIDTH(width), HEIGHT(height)
    {
        cout << "Here in constructor" << endl;
        for (int i = 0; i < HEIGHT; i++)
        {
            vector<int> row(WIDTH, 0);
            board.push_back(row);
        }
        // empty = WIDTH * HEIGHT;
        // length = 0;
        // generateInitials();

        // status = true;
        resetStats();
    }

    bool play(int move)
    {
        pair<int, int> possibilities[] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
        pair<int, int> headNext = {head.first + possibilities[move].first, head.second + possibilities[move].second};

        if (checkCellStatus(headNext) < 2)
        {
            status = false;
            return false;
        }
        if (board[headNext.first][headNext.second] == -1)
        {
            length++;
            empty--;
            board[headNext.first][headNext.second] = length;
            head = headNext;
            if (empty != 0)
                generateFood();
        }
        else
        {
            pair<int, int> currentIndex = head;
            while (true)
            {
                board[currentIndex.first][currentIndex.second]--;
                if (board[currentIndex.first][currentIndex.second] == 0)
                    break;
                pair<int, int> newIndex;
                for (pair<int, int> &p : possibilities)
                {
                    if (checkCellStatus({currentIndex.first + p.first, currentIndex.second + p.second}) == 1 && board[currentIndex.first + p.first][currentIndex.second + p.second] == board[currentIndex.first][currentIndex.second])
                    {
                        newIndex.first = currentIndex.first + p.first;
                        newIndex.second = currentIndex.second + p.second;
                        break;
                    }
                }
                currentIndex = newIndex;
            }

            board[headNext.first][headNext.second] = length;
            head = headNext;
        }
        return true;
    }
    std::array<float, 24> getNeuralInput() const
    {
        // directions order:- NORTH, NORTH-EAST, EAST, SOUTH-EAST, SOUTH, SOUTH-WEST, WEST, NORTH-WEST
        // materials order:- WALL, BODY, FOOD
        pair<int, int> directions[] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
        vector<vector<int>> toReturn(8, vector<int>(3, INT_MAX));

        int directionIndex = 0;
        for (const auto &direction : directions)
        {
            int distance = 1;
            pair<int, int> coordinate = {head.first + direction.first, head.second + direction.second};
            while (true)
            {
                int cellStatus = checkCellStatus(coordinate);
                if (cellStatus != 3)
                {
                    toReturn[directionIndex][cellStatus] = distance;
                    directionIndex++;
                    break;
                }
                distance++;
                coordinate.first = coordinate.first + direction.first;
                coordinate.second = coordinate.second + direction.second;
            }
        }

        // change into array before return
        std::array<float, 24> returnArray;
        unsigned index = 0;
        for (const auto &direction : toReturn)
        {
            for (const auto &cellStatus : direction)
            {
                returnArray.at(index++) = static_cast<float>(cellStatus);
            }
        }
        return returnArray;
    }
    vector<vector<int>> getField() const
    {
        return board;
    }
};