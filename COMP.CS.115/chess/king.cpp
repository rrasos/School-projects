#include "king.hh"

King::King(ChessColor col) : ChessPiece(col, KING, "king")
{
}

set<Coord> King::get_allowed_moves(ChessBoard const &board) const
{
    set<Coord> moves;
    Coord directions[] = {{1,1},{1,0},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
    Coord current_pos = get_position();
    for (const auto& dir : directions) {
        Coord dest = {current_pos.row+dir.row,current_pos.col+dir.col};
        check_move(board,dest, moves);
    }

    return moves;
}

