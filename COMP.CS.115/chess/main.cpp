/* Chess
 *
 * Desc:
 * This project implements a chess game with a graphical user interface (GUI) using Qt.
 * The game allows two players to play chess, displaying the board and pieces.
 * The game includes all standard chess pieces (pawn, rook, knight, bishop, queen, king)
 * and their respective movement rules. 
 *  NOTE: better instruction can be found in instructions.txt
 * 
 * Program author's
 * Name: Rasmus Sorila, Niko Vuorela
 * Student number: 150267845, 151313060
 * UserID: rrraso, pqnivu
 * E-Mail: rasmus.sorila@tuni.fi, niko.vuorela@tuni.fi
 * Feedback language (fi/en): fi
 *
 * */


#include "mainwindow.hh"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
