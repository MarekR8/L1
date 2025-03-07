#pragma once
#include <vector>
#include <windows.h>

class Board {
public:
    Board(int size); // size � liczba p�l w rz�dzie/kolumnie
    int getSize() const;
    int getCellWidth() const;   // ca�kowity rozmiar kom�rki (100px)
    int getCellHeight() const;
    int getFieldWidth() const;  // wymiar pola (90px)
    int getFieldHeight() const;
    int getMargin() const;      // margines (5px)
    // Zwraca list� prostok�t�w (RECT) okre�laj�cych pozycje p�l (w uk�adzie klienta)
    const std::vector<RECT>& getCells() const;
private:
    int size;
    int margin;
    int fieldSize;
    std::vector<RECT> cells;
    void computeCells();
};
