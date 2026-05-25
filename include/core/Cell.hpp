#ifndef CARO_VIET_CORE_CELL_HPP
#define CARO_VIET_CORE_CELL_HPP

namespace caro {

struct Cell {
    int row = -1;
    int col = -1;

    bool operator==(const Cell& other) const {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Cell& other) const {
        return !(*this == other);
    }
};

} // namespace caro

#endif // CARO_VIET_CORE_CELL_HPP
