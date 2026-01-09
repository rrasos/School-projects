#include "queen.hh"

Queen::Queen(ChessColor col): ChessPiece(col, QUEEN, "queen")
{
}


set<Coord> Queen::get_allowed_moves(ChessBoard const &board) const
{
    set<Coord> moves;

    // Combine rook and bishop moves
    Rook temp_rook(this->get_color());
    temp_rook.set_position(this->get_position());
    set<Coord> rook_moves = temp_rook.get_allowed_moves(board);

    Bishop temp_bishop(this->get_color());
    temp_bishop.set_position(this->get_position());
    set<Coord> bishop_moves = temp_bishop.get_allowed_moves(board);

    moves.insert(rook_moves.begin(), rook_moves.end());
    moves.insert(bishop_moves.begin(), bishop_moves.end());

    return moves;
}
