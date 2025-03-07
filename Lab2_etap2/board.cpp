#include "Board.h"

Board::Board(int size) : size(size), margin(5), fieldSize(90) {
    computeCells();
}

int Board::getSize() const { return size; }
int Board::getCellWidth() const { return margin + fieldSize + margin; }
int Board::getCellHeight() const { return margin + fieldSize + margin; }
int Board::getFieldWidth() const { return fieldSize; }
int Board::getFieldHeight() const { return fieldSize; }
int Board::getMargin() const { return margin; }

void Board::computeCells() {
    cells.clear();
    int cellWidth = getCellWidth();
    int cellHeight = getCellHeight();
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            RECT rect;
            rect.left = col * cellWidth + margin;
            rect.top = row * cellHeight + margin;
            rect.right = rect.left + fieldSize;
            rect.bottom = rect.top + fieldSize;
            cells.push_back(rect);
        }
    }
}

const std::vector<RECT>& Board::getCells() const {
    return cells;
}
