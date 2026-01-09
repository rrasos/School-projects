/*
STUDENT: DO NOT MODIFY THIS FILE!
*/

#include "chesspiece.hh"

ChessPiece::ChessPiece(ChessColor color, PieceType type, string name): color_(color), type_(type), name_(name) {}

ChessColor ChessPiece::get_color() const
{
    return color_;
}

PieceType ChessPiece::get_type() const
{
    return type_;
}

Coord ChessPiece::get_position() const
{
    return position_;
}

void ChessPiece::set_position(Coord pos)
{
    position_ = pos;
}

bool ChessPiece::check_move(const ChessBoard &board, const Coord &dest, set<Coord> &moves) const
{
    // Add the move, if the square is in bounds,
    // it's empty or there is an enemy piece
    if (board.coord_in_bounds(dest))
    {
        shared_ptr<ChessPiece> piece = board.get_piece_at(dest);

        //empty square (avaible)
        if (piece == nullptr)
        {
            moves.insert(dest);
            return true;
        }

        //empty square whith enemy (avaible)
        if (piece->get_color() != get_color()){
            moves.insert(dest);
            return false;
        }
    }

    //there is no available move
    return false;
}

string ChessPiece::get_name() const
{
    return name_;
}
