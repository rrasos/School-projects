#include "mainwindow.hh"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // top row buttons
    QWidget *topBar = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setAlignment(Qt::AlignCenter);

    // Restart Button
    restartButton_ = new QPushButton("Restart", this);
    connect(restartButton_, &QPushButton::clicked, this, &MainWindow::handle_restart);
    topLayout->addWidget(restartButton_);

    // Close Button
    closeButton_ = new QPushButton("Close", this);
    connect(closeButton_, &QPushButton::clicked, this, &MainWindow::close);
    topLayout->addWidget(closeButton_);

    mainLayout->addWidget(topBar);

    // Chessboard and info label
    QWidget *boardContainer = new QWidget(this);
    QVBoxLayout *boardLayout = new QVBoxLayout(boardContainer);
    boardLayout->setAlignment(Qt::AlignTop);
    boardLayout->setContentsMargins(20, 20, 20, 20); 

    // info label
    infoLabel_ = new QLabel("", this);
    infoLabel_->setAlignment(Qt::AlignCenter);
    infoLabel_->setContentsMargins(0, 10, 0, 10);
    QFont font = infoLabel_->font();
    font.setPointSize(20);
    infoLabel_->setFont(font);
    boardLayout->addWidget(infoLabel_);

    // board
    QWidget *boardWidget = new QWidget(this);
    boardWidget->setFixedSize(8 * 66, 8 * 66);
    gridLayout_ = new QGridLayout(boardWidget);
    gridLayout_->setSpacing(0);
    gridLayout_->setContentsMargins(0, 0, 0, 0);
    boardLayout->addWidget(boardWidget, 0, Qt::AlignCenter);

    mainLayout->addWidget(boardContainer);

    // start game and initialise board
    chess_.start_game();
    setupBoard();
    updateBoard();
}

void MainWindow::handle_piece_click(int row, int col)
{
    clearIndicators();

    //if game is ended do nothing
    if (chess_.get_game_state()!=IN_PROGRESS){
        return;
    }

    //if there is no selected pawn
    if (!selected_){
        selectPiece(row,col);
        return;
    }

    //if piece is selected but clicked square is not in avaible moves
    if (moves_.find({row,col})==moves_.end()){

        //select new piece (handles scenarios where empty square is clicked)
        selectPiece(row,col);
        return;
    }

    //if avaible square is clicked move the piece
    movePiece(row,col);
}

void MainWindow::handle_restart()
{
    chess_.start_game();
    updateBoard();

    resetAtributes();
}

void MainWindow::setupBoard()
{
    for (int row = 0; row < 8; ++row) {
         for (int col = 0; col < 8; ++col) {

             //avaible moves indicators
             indicators_[row][col] = new QLabel(this);
             indicators_[row][col]->setFixedSize(66, 66);

             //set indicators same color as squares color
             QString clr = ((row + col) % 2 == 0) ? CLOSED_B : CLOSED_W;
             indicators_[row][col]->setStyleSheet(clr);
             gridLayout_->addWidget(indicators_[row][col], row, col,Qt::AlignCenter);

             //actual squares (whitout pictures)
             squares_[row][col] = new QPushButton(this);
             squares_[row][col]->setFixedSize(62, 62);
             squares_[row][col]->setIconSize(QSize(62, 62));
             gridLayout_->addWidget(squares_[row][col], row, col,Qt::AlignCenter);

             connect(squares_[row][col], &QPushButton::clicked, this, [=]() {
                 handle_piece_click(row, col);
             });
         }
     }

    setInfoText();
}

void MainWindow::updateBoard()
{
    ChessBoard board = chess_.get_board();

    //set pictures to the buttons
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
          auto piece = board.get_piece_at(row, col);
          QString imagePath = getPieceImagePath(piece, row, col);

          if (!imagePath.isEmpty()) {
             squares_[row][col]->setIcon(QIcon(imagePath));
            }

          else {
            squares_[row][col]->setIcon(QIcon());
            }
        }
    }
    setInfoText();
}

QString MainWindow::getPieceImagePath(const std::shared_ptr<ChessPiece> &piece, int row, int col) const
{
    const std::string PREFIX(":pieces/");
    const std::string SUFFIX(".png");
    std::string filename;

    std::string bg_color = ((row + col) % 2 == 0) ? "bl" : "wt";

    // in case of just square without piece
    if (!piece) {
        filename = PREFIX + "empty-" + bg_color + SUFFIX;
        return QString::fromStdString(filename);
    }

    std::string color = (piece->get_color() == WHITE) ? "wt-" : "bl-";
    std::string type;

    switch (piece->get_type()) {
    case PAWN:
        type = "pawn";
        break;
    case ROOK:
        type = "rook";
        break;
    case KNIGHT:
        type = "knight";
        break;
    case BISHOP:
        type = "bishop";
        break;
    case QUEEN:
        type = "queen";
        break;
    case KING:
        type = "king";
        break;
    }

    filename = PREFIX + color + type + "-on-" + bg_color + SUFFIX;
    return QString::fromStdString(filename);
}

void MainWindow::clearIndicators()
{
    //reset info label to black
    infoLabel_->setStyleSheet("color : black");

    //reset indicators to the black or white
    for (int r = 0; r < 8; ++r){
        for (int c = 0; c < 8; ++c){
          QString clr = ((r + c) % 2 == 0) ? CLOSED_B : CLOSED_W;
          indicators_[r][c]->setStyleSheet(clr);
        }
    }
}

void MainWindow::setInfoText()
{
    std::string infoText;

    if (chess_.get_game_state() == IN_PROGRESS){
        std::string color = (chess_.get_current_turn() == WHITE) ? "White" : "Black";
        infoText = color + "'s turn";
    }

    if (chess_.get_game_state() == BLACK_WIN){
        infoText = "Black won!";
    }

    if (chess_.get_game_state() == WHITE_WIN){
        infoText = "White won!";
    }

    infoLabel_->setText(QString::fromStdString(infoText));
}

void MainWindow::selectPiece(int row, int col)
{
    ChessBoard board = chess_.get_board();
    piece_ = board.get_piece_at(row,col);

    //if not piece
    if (!piece_) {
        selected_ = false;
        return;
    }
    //wrong color
    if (piece_->get_color() != chess_.get_current_turn()){
        infoLabel_->setStyleSheet("color : red");
        selected_ = false;
        return;
    }

    //piece is selected correctly -> get possible move
    moves_ = piece_->get_allowed_moves(board);

    //indicate selected piece whit green
    indicators_[row][col]->setStyleSheet(SELECTED);

    //indicate squares where is possible to move whith red
    for (const auto& move : moves_){
        indicators_[move.row][move.col]->setStyleSheet(AVAIBLE);
    }
    selected_ = true;

}

void MainWindow::movePiece(int row, int col)
{
    chess_.make_move(piece_,{row,col});
    updateBoard();

    resetAtributes();
}

void MainWindow::resetAtributes()
{
    clearIndicators();
    selected_ = false;
    moves_.clear();
    piece_ = nullptr;
}
