#pragma once
#include <bits/stdc++.h>
#include "../libraries/randomNumberGenerator.hpp"

#include "E:/programming_tools/SFML-2.5.1/include/SFML/Graphics.hpp"

class Board
{
    // REPRESENTATIONS:
    // BODY PARTS: m_headPos -> snakeLength, tail -> 1, always greater than 0
    // FOOD: -1
    // m_emptyCellCount SPACE: 0
    enum CellType : int
    {
        FOOD = -1,
        EMPTY = 0,
        TAIL = 1
    };
    enum MoveCode : unsigned
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

    struct Vector2
    {
        int x, y; // need int , not unsigned
        Vector2(const int _x, const int _y) : x(_x), y(_y) {}
        Vector2(void) : x(0), y(0) {}
        inline Vector2 operator+(const Vector2 &_other) const
        {
            return Vector2(this->x + _other.x, this->y + _other.y);
        }
    };
    using Coordinates = Vector2;

    const unsigned m_width, m_height;

    Coordinates m_headPos;
    Coordinates m_foodPos;

    struct Grid
    {
    private:
        std::vector<std::vector<int>> m_grid; // indexed by coordinates (y, x)
    public:
        Grid(void) {}
        Grid(const unsigned _width, const unsigned _height)
        {
            for (int i = 0; i < _height; i++)
            {
                std::vector<int> row(_width, CellType::EMPTY);
                m_grid.push_back(row);
            }
        }

    public:
        inline int at(const Coordinates &_coords) const // const variant of at() used for reading
        {
            return m_grid.at(_coords.y).at(_coords.x);
        }
        inline int &at(const Coordinates &_coords) // non const variant of at used for writing
        {
            return m_grid.at(_coords.y).at(_coords.x);
        }
        void clear(void)
        {
            for (auto &row : m_grid)
            {
                for (auto &cell : row)
                {
                    cell = CellType::EMPTY;
                }
            }
        }
    };

    Grid m_board;

    unsigned m_emptyCellCount;

    using MovementDirection = Vector2;
    const std::array<MovementDirection, 4> m_possibleMovementDirections = {Vector2(0, -1), Vector2(1, 0), Vector2(0, 1), Vector2(-1, 0)};

    const std::array<Vector2, 8> m_visionDirections = {Vector2(0, -1), Vector2(1, -1), Vector2(1, 0), Vector2(1, 1), Vector2(0, 1), Vector2(-1, 1), Vector2(-1, 0), Vector2(-1, -1)};

private:
    inline Coordinates wrap_index(const int _flattenedIndex) const
    {
        return Coordinates(_flattenedIndex % m_width, _flattenedIndex / m_width);
    }
    inline int flatten_index(const Coordinates &_wrappedIndex) const
    {
        return _wrappedIndex.x + _wrappedIndex.y * m_width;
    }

private:
    CellStatus check_cell_status(const Coordinates &_coordinates) const
    {
        if (_coordinates.x >= m_width or _coordinates.x < 0 or _coordinates.y >= m_height or _coordinates.y < 0)
            return CellStatus::WALL;
        else if (m_board.at(_coordinates) > 0)
            return CellStatus::BODY;
        else if (m_board.at(_coordinates) == CellType::FOOD)
            return CellStatus::FOOD;
        else
            return CellStatus::EMPTY;
    }
    void generate_food(void)
    {
        int exIndex = random_32.generate(0, m_emptyCellCount - 1);
        int counter = 0;

        while (true)
        {
            const auto coordinates = wrap_index(counter);

            if (check_cell_status(coordinates) == CellStatus::BODY)
            {
                counter++;
                continue;
            }
            exIndex--;
            if (exIndex < 0)
                break;
            counter++;
        }
        m_foodPos = wrap_index(counter);
        m_board.at(m_foodPos) = CellType::FOOD;
        // cout << "Food position: (" << m_foodPos.x << ", " << m_foodPos.y << ")" << endl;
    }
    void generate_initials(void)
    {
        // cout << "Here in generate Initials before random" << endl;
        const auto flattenedIndex = random_32.generate(0, m_emptyCellCount - 1);
        m_headPos = wrap_index(flattenedIndex);
        // cout << "Random Number between " << 0 << " and " << (m_emptyCellCount - 1) << ": " << _flattenedIndex << ", wrapped _coordinates: (" << m_headPos.x << ", " << m_headPos.y << ")" << endl;
        snakeLength++;
        m_emptyCellCount--;
        // cout << "Here in generate Initials after random" << endl;
        m_board.at(m_headPos) = snakeLength;

        // More work if extra parts required

        // Work left above
        generate_food();
    }

public:
    bool status; // gameover or not
    int snakeLength;

    void reset_stats(void)
    {
        // cout << "Here in reset stats" << endl;
        m_board.clear();

        m_emptyCellCount = m_width * m_height;
        snakeLength = 0;

        generate_initials();

        status = true;
    }
    Board(const int _width, const int _height) : m_width(_width), m_height(_height)
    {
        // cout << "Here in constructor" << endl;
        m_board = Grid(_width, _height);
        reset_stats();
    }

    bool play_one_move(const MoveCode &_move)
    {
        const auto headPosNext = m_headPos + m_possibleMovementDirections[_move];
        const auto nextCellStatus = check_cell_status(headPosNext);

        if (nextCellStatus == CellStatus::WALL or nextCellStatus == CellStatus::BODY) // we will hit a wall or body
        {
            status = false;
            return false;
        }
        else if (nextCellStatus == CellStatus::FOOD)
        {
            snakeLength++;
            m_emptyCellCount--;

            if (m_emptyCellCount not_eq 0)
                generate_food();
        }
        else
        {
            auto currentBodyPartCoords = m_headPos;
            while (true)
            {
                // if we are done until tail then break out of the loop
                if (--m_board.at(currentBodyPartCoords) == CellType::EMPTY)
                    break;

                // otherwise we need to check every direction for NEXT bodypart and point currentBodyPartCoords there if we find one
                const auto thisCellValue = m_board.at(currentBodyPartCoords);

                for (const auto &direction : m_possibleMovementDirections)
                {
                    const auto checkingCoords = currentBodyPartCoords + direction;
                    const auto status = check_cell_status(checkingCoords);
                    const auto checkingCoordsValue = m_board.at(checkingCoords);

                    if (status == CellStatus::BODY and checkingCoordsValue == thisCellValue - 1)
                    {
                        currentBodyPartCoords = checkingCoords;
                        break;
                    }
                }
            }
        }

        m_board.at(headPosNext) = snakeLength;
        m_headPos = headPosNext;

        return true;
    }
    std::array<float, 24> get_input_for_NN(void) const
    {
        // materials order:- WALL, BODY, FOOD
        // this order is the same as CellStatus values
        // which is helpful later when indexing into array
        std::array<float, 24> returnArray = {3.4e38f}; // init it with max value of float

        unsigned arrayIndex = 0;
        for (const auto &direction : m_visionDirections)
        {
            float distance = 1;

            Coordinates coordinates = m_headPos + direction;
            while (true)
            {
                const auto cellStatus = check_cell_status(coordinates);

                if (cellStatus not_eq CellStatus::EMPTY)
                {
                    returnArray[arrayIndex + cellStatus] = distance;
                    arrayIndex += 3;
                    break;
                }

                distance = distance + 1;

                coordinates = coordinates + direction;
            }
        }
        return returnArray;
    }

public: // GRAPHICS
    void g_draw(void)
    {
    }
};