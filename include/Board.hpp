#pragma once
#include "Defines.hpp"
#include "Vector2.hpp"

#if ENABLE_GRAPHICS
#include "E:/programming_tools/SFML-2.6.1/include/SFML/Graphics.hpp"
#endif
#include <iostream>
#include<array>
#include <cassert>

class Board
{
public:
    // up, right, down, left
    using MoveCode = unsigned;
    enum GameResult : int {
        GAMEOVER  = -1,
        NOTHING = 0,
        FOODEATEN  = 1
    };

private:
    // BODY PARTS: m_headPos -> m_snakeLength, tail -> 1, always greater than 0
    // REPRESENTATIONS:
    // FOOD: -1
    // m_emptyCellCount SPACE: 0
    enum CellType : int
    {
        food = -1,
        grass = 0,
        tail = 1
    };

    enum CellStatus : unsigned // has to be unsigned as it indexes return array
    {
        WALL,
        BODY,
        FOOD,
        GRASS
    };

    using Coordinates = myLib::Vector2<int>;

    struct Grid
    {
    private:
        std::vector<std::vector<int>> m_grid; // indexed by coordinates (y, x)

    public:
        Grid(void) {}
        Grid(const int _width, const int _height)
        {
            for (int rowIndex = 0; rowIndex < _height; ++rowIndex)
            {
                std::vector<int> row(_width, CellType::grass);
                m_grid.push_back(row);
            }
        }

    public:
        inline int at(const Coordinates &_coords) const // const variant of at() used for reading
        {
            return m_grid.at(_coords.y).at(_coords.x);
        }
        inline int &at(const Coordinates &_coords) // non const variant of at() used for writing
        {
            return m_grid.at(_coords.y).at(_coords.x);
        }
        void clear(void)
        {
            for (auto &row : m_grid)
            {
                for (auto &cell : row)
                {
                    cell = CellType::grass;
                }
            }
        }
        void print(void)
        {
            for (auto &row : m_grid)
            {
                for (auto &cell : row)
                {
                    std::cout << cell << ' ';
                }
                std::cout << '\n';
            }
        }
    };

private:
    Grid m_board;
    Coordinates m_foodPos;
    Coordinates m_headPos;

    // up , right, down, left
    const std::array<myLib::Vector2<int>, 4> m_possibleMovementDirections = {myLib::Vector2(0, -1), myLib::Vector2(1, 0), myLib::Vector2(0, 1), myLib::Vector2(-1, 0)};

    // N, NE, E, SE, S, SW, W, NW
    const std::array<myLib::Vector2<int>, 16> m_visionDirections = {myLib::Vector2(0, -1), myLib::Vector2(1, -2), myLib::Vector2(1, -1), myLib::Vector2(2, -1), myLib::Vector2(1, 0), myLib::Vector2(2, 1), myLib::Vector2(1, 1), myLib::Vector2(1, 2), myLib::Vector2(0, 1), myLib::Vector2(-1, 2), myLib::Vector2(-1, 1), myLib::Vector2(-2, 1), myLib::Vector2(-1, 0), myLib::Vector2(-2, -1), myLib::Vector2(-1, -1), myLib::Vector2(-1, -2)};

    unsigned m_emptyCellCount;
    bool m_gameOver = false; // gameover or not
    int m_snakeLength;

public:
    inline auto game_over(void) const { return m_gameOver; }
    inline auto snakeLength(void) const { return m_snakeLength; }

private:
    inline Coordinates wrap_index(const unsigned _flattenedIndex) const
    {
        return Coordinates(_flattenedIndex % BOARD_WIDTH, _flattenedIndex / BOARD_WIDTH);
    }
    inline unsigned flatten_index(const Coordinates &_wrappedIndex) const
    {
        return _wrappedIndex.x + _wrappedIndex.y * BOARD_WIDTH;
    }

private:
    inline CellStatus check_cell_status(const Coordinates &_coordinates) const
    {
        if (_coordinates.x >= BOARD_WIDTH or _coordinates.x < 0 or _coordinates.y >= BOARD_HEIGHT or _coordinates.y < 0)
            return CellStatus::WALL;
        else if (m_board.at(_coordinates) > 0)
            return CellStatus::BODY;
        else if (m_board.at(_coordinates) == CellType::food)
            return CellStatus::FOOD;
        else
            return CellStatus::GRASS;
    }
    void generate_food(void)
    {
        int exIndex = myLib::random_32.generate(0, m_emptyCellCount - 1);
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
        m_board.at(m_foodPos) = CellType::food;

        m_emptyCellCount--;
        // cout << "Food position: (" << m_foodPos.x << ", " << m_foodPos.y << ")" << endl;
    }
    void generate_initials(void)
    {
        // cout << "Here in generate Initials before random" << endl;
        const auto flattenedIndex = myLib::random_32.generate(0, m_emptyCellCount - 1);
        m_headPos = wrap_index(flattenedIndex);
        m_snakeLength++;
        m_board.at(m_headPos) = m_snakeLength;

        m_emptyCellCount--;

        generate_food();
    }

public:
    void reset_stats(void)
    {
        m_board.clear();

        m_snakeLength = 0;
        m_emptyCellCount = BOARD_WIDTH * BOARD_HEIGHT;

        generate_initials();

        m_gameOver = false;
    }
    Board(void)
    {
        // std::cout << "Here in constructor" << std::endl;
        m_board = Grid(BOARD_WIDTH, BOARD_HEIGHT);
        reset_stats();
    }

    GameResult play_one_move(const MoveCode &_move)
    {
        assert(_move < 4);

        const auto headPosNext = m_headPos + m_possibleMovementDirections[_move];
        const auto nextCellStatus = check_cell_status(headPosNext);

        if (nextCellStatus == CellStatus::WALL or nextCellStatus == CellStatus::BODY) // we will hit a wall or body
        {
            m_gameOver = true;
            return GameResult::GAMEOVER;
        }
        else if (nextCellStatus == CellStatus::FOOD)
        {
            m_snakeLength++;
            m_board.at(headPosNext) = m_snakeLength;

            m_headPos = headPosNext;
            if (m_emptyCellCount not_eq 0)
                generate_food();

            return GameResult::FOODEATEN;
        }
        else
        {
            auto currentBodyPartCoords = m_headPos;
            while (true)
            {
                // if we are done until tail then break out of the loop
                if (--m_board.at(currentBodyPartCoords) == CellType::grass)
                    break;

                // otherwise we need to check every direction for NEXT bodypart and point currentBodyPartCoords there if we find one
                const auto thisCellValue = m_board.at(currentBodyPartCoords);

                for (const auto &direction : m_possibleMovementDirections)
                {
                    const auto checkingCoords = currentBodyPartCoords + direction;
                    const auto checkingCellStatus = check_cell_status(checkingCoords);

                    // if cell in that direction is a wall no need to further investigate
                    if (checkingCellStatus == CellStatus::WALL)
                        continue;
                    const auto checkingCoordsValue = m_board.at(checkingCoords);

                    if (checkingCellStatus == CellStatus::BODY and checkingCoordsValue == thisCellValue)
                    {
                        currentBodyPartCoords = checkingCoords;
                        break;
                    }
                }
            }
            m_board.at(headPosNext) = m_snakeLength;
            m_headPos = headPosNext;

            return GameResult::NOTHING;
        }
    }

    // bianry vision
    std::array<float, NUMBER_OF_INPUTS> get_input_for_NN(void) const
    {
        // materials order:- WALL, BODY, FOOD
        // this order is the same as CellStatus values
        // which is helpful later when indexing into array
        std::array<float, NUMBER_OF_INPUTS> returnArray;
        for (auto &returnValue : returnArray)
        {
            returnValue = -INFINITY; // init it with min value of float
        }

        unsigned arrayIndex = 0;
        for (const auto &direction : m_visionDirections)
        {
            float distance = 0.f;

            Coordinates coordinates = m_headPos;
            while (true)
            {
                coordinates = coordinates + direction;
                const auto cellStatus = check_cell_status(coordinates);
                distance = distance + (std::abs(direction.x) + std::abs(direction.y));

                if (cellStatus not_eq CellStatus::GRASS)
                {
                    returnArray[arrayIndex + cellStatus] = distance;
                    arrayIndex += 3;
                    break;
                }
            }
        }
        // ---------------------------------------
        // for (const auto &dist : returnArray)
        // {
        //     std::cout << dist << ' ';
        // }
        // std::cout << '\n';
        // ----------------------------------------

        return returnArray;
    }

#if ENABLE_GRAPHICS
public: // GRAPHICS
    void g_draw(sf::RenderWindow &_window)
    {
        const float shapeWidth = WINDOW_DIMENSION / BOARD_WIDTH;
        const float shapeHeight = WINDOW_DIMENSION / BOARD_WIDTH;
        sf::RectangleShape rect({shapeWidth, shapeHeight});

        for (int rowIndex = 0; rowIndex < BOARD_HEIGHT; ++rowIndex)
        {
            for (int columnIndex = 0; columnIndex < BOARD_HEIGHT; ++columnIndex)
            {
                const float xPos = WINDOW_DIMENSION + columnIndex * shapeWidth;
                const float yPos = rowIndex * shapeHeight;

                rect.setPosition(sf::Vector2f(xPos, yPos));
                rect.setOutlineThickness(50 / BOARD_WIDTH);

                const auto coords = Coordinates(columnIndex, rowIndex);
                const auto cellvalue = m_board.at(coords);

                if (cellvalue == CellType::grass)
                    rect.setFillColor(sf::Color(144, 238, 144));
                else if (cellvalue == CellType::food)
                    rect.setFillColor(sf::Color(255, 0, 0));
                else if (cellvalue > 0)
                    rect.setFillColor(sf::Color(0xfdfd96ff));
                if (cellvalue == m_snakeLength)
                    rect.setFillColor(sf::Color(0xffaa1dff));

                _window.draw(rect);
            }
        }
    }
#endif
};