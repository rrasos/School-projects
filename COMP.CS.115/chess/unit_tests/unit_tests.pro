QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_chesstest.cpp \
            ../bishop.cpp\
            ../chess.cpp \
            ../chessboard.cpp \
            ../chesspiece.cpp \
            ../knight.cpp \
            ../queen.cpp \
            ../pawn.cpp \
            ../rook.cpp \
            ../king.cpp

HEADERS += ../chess.hh \
        ../chessboard.hh \
        ../chesspiece.hh \
        ../knight.hh \
        ../pawn.hh \
        ../rook.hh \
        ../queen.hh \
        ../bishop.hh \
        ../king.cpp
