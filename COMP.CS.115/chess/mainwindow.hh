#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <memory>
#include "chess.hh"


//const values for indicator colors
const QString AVAIBLE = "background-color: red;";
const QString CLOSED_W = "background-color: white;";
const QString CLOSED_B = "background-color: black;";
const QString SELECTED = "background-color: green;";


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void handle_piece_click(int row, int col);
    void handle_restart();

private:
    QGridLayout *gridLayout_;

    //board
    QLabel *indicators_[8][8]; //8x8
    QPushButton *squares_[8][8]; //8x8

    //buttons
    QPushButton *closeButton_;
    QPushButton *restartButton_;

    QLabel *infoLabel_;

    Chess chess_;
    bool selected_ = false;
    set<Coord> moves_;
    shared_ptr<ChessPiece> piece_;

    //setups 8x8 pushbuttons for chessboard
    void setupBoard();

    //updates images to the pushbuttons (chessboard)
    void updateBoard();

    //gets image address
    //param: ptr to the piece, current row and col (needed for empty squares)
    //returns addres ":pieces/clr-type-on-bg.png"
    QString getPieceImagePath(const std::shared_ptr<ChessPiece> &piece,int row, int col) const;

    //clears colors in indicators and sets infolabel to balck
    void clearIndicators();

    void setInfoText();

    //checks if clicked square have a piece, if so handles indicators
    //if wrong color piece is clicked turns info label red
    void selectPiece(int row, int col);

    void movePiece(int row, int col);

    //resets selected_, moves_, piece_ and uses clearIndicators
    void resetAtributes();


};

#endif // MAINWINDOW_HH
