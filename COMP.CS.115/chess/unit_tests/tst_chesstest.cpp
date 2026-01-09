#include <QtTest>
#include <memory>
#include <set>
#include <iostream>

#include "../bishop.hh"
#include "../chess.hh"
#include "../chessboard.hh"
#include "../chesspiece.hh"
#include "../king.hh"
#include "../knight.hh"
#include "../pawn.hh"
#include "../queen.hh"
#include "../rook.hh"

using namespace std;

Coord TEST_POSITION = {3,3};

bool contains(const set<Coord>& moves, Coord move)
{
    return moves.find(move) != moves.end();
}

void print_board(const ChessBoard& board) {
    for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                auto piece = board.get_piece_at({row, col});
                if (piece == nullptr) {
                    cout << ".";
                } else {
                    char symbol = '?';
                    switch (piece->get_type()) {
                        case KING:   symbol = 'K'; break;
                        case QUEEN:  symbol = 'Q'; break;
                        case ROOK:   symbol = 'R'; break;
                        case BISHOP: symbol = 'B'; break;
                        case KNIGHT: symbol = 'N'; break;
                        case PAWN:   symbol = 'P'; break;
                    }
                    // lowercase letters for black
                    if (piece->get_color() == BLACK) {
                        symbol = tolower(symbol);
                    }
                    cout << symbol;
                }
            }
            cout << endl;
        }
        cout << endl;
}

class ChessTest : public QObject
{
    Q_OBJECT

private:
    Chess chess;
    ChessBoard board = chess.get_board();

private slots:
    void init();
    void test_rook();
    void test_bishop();
    void test_queen();
    void test_king();
    void test_multiple_pieces();

};


void ChessTest::init()
{

}

void ChessTest::test_rook()
{

    shared_ptr<Rook> rook = make_shared<Rook>(ChessColor::WHITE);
    rook->set_position(TEST_POSITION);

    cout << rook->get_name() << " current position: ";
    cout << rook->get_position().row<<',' << rook->get_position().col << endl<<endl;

    cout << "allowed moves" << endl;
    auto moves = rook->get_allowed_moves(board);
    for (auto move :moves){
        cout << move.row << ',' << move.col << " | ";
    }
    cout << endl << endl;;

    // Test valid moves (vertical and horizontal)
    QVERIFY(contains(moves,{3, 0})); // Left
    QVERIFY(contains(moves,{3, 7})); // right
    QVERIFY(contains(moves,{0, 3})); //Up
    QVERIFY(contains(moves,{7, 3})); //Down

    // Test invalid moves (diagonal and out of the board)
    QVERIFY(!contains(moves,{4,4}));
    QVERIFY(!contains(moves,{-2, 0}));

}

void ChessTest::test_bishop()
{
    shared_ptr<Bishop> bishop = make_shared<Bishop>(ChessColor::WHITE);
    bishop->set_position(TEST_POSITION);
    cout << bishop->get_name() << " position: ";
    cout << bishop->get_position().row<<',' << bishop->get_position().col << endl <<endl;;

    cout << "allowed moves" << endl;
    auto moves = bishop->get_allowed_moves(board);
    for (auto move :moves){
        cout << move.row << ',' << move.col << " | ";
    }
    cout << endl <<endl;

    // Test valid moves (diagonal)
    QVERIFY(contains(moves,{0, 0}));
    QVERIFY(contains(moves,{7, 7}));
    QVERIFY(contains(moves,{0, 6}));
    QVERIFY(contains(moves,{6, 0}));

    // Test invalid moves (vertical, horizontal, out of board)
    QVERIFY(!contains(moves,{3, 0})); // Left
    QVERIFY(!contains(moves,{3, 7})); // right
    QVERIFY(!contains(moves,{0, 3})); //Up
    QVERIFY(!contains(moves,{7, 3})); //Down
    QVERIFY(!contains(moves,{-2, 0}));

}

void ChessTest::test_queen()
{
    shared_ptr<Queen> queen = make_shared<Queen>(ChessColor::WHITE);
    queen->set_position(TEST_POSITION);
    cout << queen->get_name() << " position: ";
    cout << queen->get_position().row<<',' << queen->get_position().col << endl <<endl;;

    cout << "allowed moves" << endl;
    auto moves = queen->get_allowed_moves(board);
    for (auto move :moves){
        cout << move.row << ',' << move.col << " | ";
    }
    cout << endl <<endl;

    // Test valid moves (diagonal)
    QVERIFY(contains(moves,{0, 0}));
    QVERIFY(contains(moves,{7, 7}));
    QVERIFY(contains(moves,{0, 6}));
    QVERIFY(contains(moves,{6, 0}));

    // Test valid moves (vertical and horizontal)
    QVERIFY(contains(moves,{3, 0})); // Left
    QVERIFY(contains(moves,{3, 7})); // right
    QVERIFY(contains(moves,{0, 3})); //Up
    QVERIFY(contains(moves,{7, 3})); //Down

    //invalid moves
    QVERIFY(!contains(moves,{0, 7}));
    QVERIFY(!contains(moves,{-2, 0}));
}

void ChessTest::test_king()
{
    shared_ptr<King> king = make_shared<King>(ChessColor::WHITE);
    king->set_position(TEST_POSITION);
    cout << king->get_name() << " position: ";
    cout << king->get_position().row<<',' << king->get_position().col << endl <<endl;;

    cout << "allowed moves" << endl;
    auto moves = king->get_allowed_moves(board);
    for (auto move :moves){
        cout << move.row << ',' << move.col << " | ";
    }
    cout << endl <<endl;

    // Test valid moves (diagonal)
    QVERIFY(contains(moves,{2, 2}));
    QVERIFY(contains(moves,{2, 4}));
    QVERIFY(contains(moves,{4, 2}));
    QVERIFY(contains(moves,{4, 4}));

    // Test valid moves (vertical and horizontal)
    QVERIFY(contains(moves,{3, 2})); // Left
    QVERIFY(contains(moves,{3, 4})); // right
    QVERIFY(contains(moves,{2, 3})); //Up
    QVERIFY(contains(moves,{4, 3})); //Down

    //test illegal
    QVERIFY(!contains(moves,{3, 0}));
    QVERIFY(!contains(moves,{3, 7}));
}

void ChessTest::test_multiple_pieces()
{
    chess.start_game(WHITE);
    print_board(chess.get_board());

    cout << "illegal move:" << endl;

    //test illegal move -> rook over pawn
    auto rook = chess.get_board().get_piece_at(0,0);
    QVERIFY(!chess.make_move(rook,{4,0}));
    print_board(chess.get_board());

    cout << "legal move:" << endl;

    //test legal move
    auto knight = chess.get_board().get_piece_at(0,1);
    QVERIFY(chess.make_move(knight,{2,0}));
    print_board(chess.get_board());
}

QTEST_APPLESS_MAIN(ChessTest)

#include "tst_chesstest.moc"
