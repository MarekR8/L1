#pragma once
#include <vector>
#include <windows.h>

class Board {
public:
    Board(int size); // size – liczba pól w rzêdzie/kolumnie
    int getSize() const;
    int getCellWidth() const;   // ca³kowity rozmiar komórki (100px)
    int getCellHeight() const;
    int getFieldWidth() const;  // wymiar pola (90px)
    int getFieldHeight() const;
    int getMargin() const;      // margines (5px)
    // Zwraca listê prostok¹tów (RECT) okreœlaj¹cych pozycje pól (w uk³adzie klienta)
    const std::vector<RECT>& getCells() const;
private:
    int size;
    int margin;
    int fieldSize;
    std::vector<RECT> cells;
    void computeCells();
};
