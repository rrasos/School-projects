#include "bishop.hh"

Bishop::Bishop(ChessColor col): ChessPiece(col, BISHOP, "bishop")
{
}

set<Coord> Bishop::get_allowed_moves(ChessBoard const &board) const
{
    set<Coord> moves;
    Coord directions[] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

    for (const auto& dir : directions) {
        Coord current = get_position();
        while (true) {
            current = {current.row + dir.row, current.col + dir.col};
            if (!check_move(board,current,moves)){
                break;
            }
        }
    }
    return moves;
}


