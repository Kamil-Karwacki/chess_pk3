#include <iostream>
#include "input.h"
#include "render.h"
#include "board.h"
#include "debug.h"

void ProcessInput(int xTile, int yTile)
{
    std::cout << "Processing input : " << xTile << " " << yTile << "\n";

    if(checkmate || resultDraw)
        return;

    // tile that was clicked in this event
    uint64_t clickedTile = (static_cast<uint64_t>(1) << (7 - xTile + (7 - yTile) * 8));

    // piece that was clicked in this event
    int clickedPiece = 12; 

    // Smallest number representing a piece of current colors piece //
    int min = board.whiteToMove ? 6 : 0;
    // Biggest number representing a piece of current colors piece //
    int max = board.whiteToMove ? 11 : 5;

    int enemyMin = board.whiteToMove ? min - 6 : min + 6;
    int enemyMax = board.whiteToMove ? max - 6 : max + 6;

    // TODO: refactor this
    std::cout << promotionPos.x << "x\n";
    std::cout << promotionPos.y << "y\n";
    if(displayPromotion)
    {
        int offset = whitePromoting ? 1 : -1;
        uint64_t newX = static_cast<uint64_t>(promotionPos.x);
        uint64_t newY = static_cast<uint64_t>(promotionPos.y);
        if(xTile == promotionPos.x && yTile == promotionPos.y + 1 * offset) // bishop
        {
            board.bitboards[enemyMax - 1] |= (static_cast<uint64_t>(1) << (7 - newX + (7 - newY) * 8));
        }
        else if(xTile == promotionPos.x && yTile == promotionPos.y + 2 * offset) // knight
        {
            board.bitboards[enemyMax - 2] |= (static_cast<uint64_t>(1) << (7 - newX + (7 - newY) * 8));
        }
        else if(xTile == promotionPos.x && yTile == promotionPos.y + 3 * offset) // rook
        {
            board.bitboards[enemyMax - 3] |= (static_cast<uint64_t>(1) << (7 - newX + (7 - newY) * 8));
        }
        else if(xTile == promotionPos.x && yTile == promotionPos.y + 4 * offset) // queen
        {
            board.bitboards[enemyMax - 5] |= (static_cast<uint64_t>(1) << (7 - newX + (7 - newY) * 8));
        }
        else
        {
            return;
        }
        board.bitboards[enemyMax] &= ~(static_cast<uint64_t>(1) << (7 - newX + (7 - newY) * 8));
        displayPromotion = false;
    }

    if(board.selectedTile) // if some piece/tile is already selected
    {
        if(clickedTile == board.selectedTile)
        {
            std::cout << "Unselecting current tile\n";
            board.selectedTile = 0;
            board.generatedMoves = 0;
        }
        else // selected other tile/piece
        {
            board.generatedMoves = 0;
            for(int i = 0; i < board.bitboards.size(); i++)
            {
                if((clickedTile & board.bitboards[i])) // if user pressed on a piece
                {
                    clickedPiece = i;
                    break;
                }
            }
        }
    }
    else // if nothing is selected
    {
        // 12 is null
        board.selectedPiece = 12;
        for(int i = 0; i < board.bitboards.size(); i++)
        {
            if((clickedTile & board.bitboards[i])) // if user pressed on a piece
            {
                board.selectedPiece = i;
                highlitedTile = (xTile + yTile * 8);
                board.selectedTile |= (static_cast<uint64_t>(1) << (7 - xTile + (7 - yTile) * 8));
                board.selectedTileX = xTile;
                board.selectedTileY = yTile;
                break;
            }
        }
        
        std::cout << "Piece clicked: " << board.selectedPiece << "\n";
        std::cout << "Selected tile: \n";
        PrintBoard(board.selectedTile);

        if(board.selectedPiece > max || board.selectedPiece < min)
        {
            board.selectedPiece = 12;
            board.selectedTile = 0;
        }
    }


    // check if piece belongs to white or black or is an empty tile
    if(board.selectedPiece >= min && board.selectedPiece <= max && board.selectedTile)
    {
        // generate move for this piece
        std::cout << "Selected piece " << board.selectedPiece << "\n";
        if(clickedPiece >= min && clickedPiece <= max)
        {
            std::cout << "Changing piece to " << clickedPiece << "\n";
            board.selectedPiece = clickedPiece;
            board.selectedTile = (static_cast<uint64_t>(1) << (7 - xTile + (7 - yTile) * 8));
            board.selectedTileX = xTile;
            board.selectedTileY = yTile;
            highlitedTile = (xTile + yTile * 8);
            board.generatedMoves = board.generateMoves(board.selectedTile, board.selectedPiece, board.selectedTileX, board.selectedTileY);
            board.legalMoves = board.getLegalMoves(board.whiteToMove, board.selectedTile, board.selectedPiece);
            board.generatedMoves &= board.legalMoves;
            return;
        }
        else
        {
            board.generatedMoves = board.generateMoves(board.selectedTile, board.selectedPiece, board.selectedTileX, board.selectedTileY);
            board.legalMoves = board.getLegalMoves(board.whiteToMove, board.selectedTile, board.selectedPiece);
            board.generatedMoves &= board.legalMoves;

            std::cout << "Legal moves\n";
            PrintBoard(board.legalMoves);
            if(clickedTile & board.generatedMoves)
            {
                std::cout << "Moving piece\n";
                uint64_t oldBitmap = (~board.selectedTile) & board.bitboards[board.selectedPiece];
                board.halfmove++;

                // en passant //
                if(clickedTile & board.enPassantTarget)
                {
                    if(board.selectedPiece == 5)
                    {
                        board.bitboards[11] &= ~(clickedTile << 8);
                        board.halfmove = 0;
                    }
                    else if(board.selectedPiece == 11)
                    {
                        board.bitboards[5] &= ~(clickedTile >> 8);
                        board.halfmove = 0;
                    }
                }

                board.enPassantTarget = 0;
                if(board.selectedTile & board.RANK_2 && clickedTile & board.RANK_4)
                {
                    std::cout << "White pawn has moved two tiles\n";
                    board.enPassantTarget = clickedTile >> 8;
                }
                else if(board.selectedTile & board.RANK_7 && clickedTile & board.RANK_5)
                {
                    std::cout << "Black pawn has moved two tiles\n";
                    board.enPassantTarget = clickedTile << 8;
                }


                for(int i = enemyMin; i <= enemyMax; i++)
                {
                    if(clickedTile & board.bitboards[i]) // if user is taking enemy piece
                    {
                        board.bitboards[i] &= (~clickedTile);
                        board.halfmove = 0;
                    }
                }

                // castling //
                if(board.selectedPiece == 1)
                {
                    board.castleKingB = board.castleQueenB = false;
                }
                else if(board.selectedPiece == 7)
                {
                    board.castleKingW = board.castleQueenW = false;
                }

                if(board.castleKingB)
                    board.castleKingB = (board.bitboards[2] & board.RIGHT_UP_CORNER) ? true : false;

                if(board.castleQueenB)
                    board.castleQueenB = (board.bitboards[2] & board.LEFT_UP_CORNER) ? true : false;
                    
                if(board.castleKingW)
                    board.castleKingW = (board.bitboards[8] & board.RIGHT_DOWN_CORNER) ? true : false;

                if(board.castleQueenW)
                    board.castleQueenW = (board.bitboards[8] & board.LEFT_DOWN_CORNER) ? true : false;


                if(board.selectedPiece == (max - 4) && clickedTile & (board.selectedTile >> 2)) // castling king side
                {
                    board.bitboards[max - 3] |= board.selectedTile >> 1;
                    board.bitboards[max - 3] &= ~(board.selectedTile >> 3);
                }

                if(board.selectedPiece == (max - 4) && clickedTile & (board.selectedTile << 2)) // castling king side
                {
                    board.bitboards[max - 3] |= board.selectedTile << 1;
                    board.bitboards[max - 3] &= ~(board.selectedTile << 4);
                }

                // Pawn promotion //
                if(board.selectedPiece == max && ((clickedTile & board.RANK_1 && !board.whiteToMove) | (clickedTile & board.RANK_8 && board.whiteToMove)))
                {
                    std::cout << "Pawn " << board.whiteToMove << " is promoting\n";
                    displayPromotion = true;
                    promotionPos.x = xTile;
                    promotionPos.y = yTile;
                    whitePromoting = board.whiteToMove;
                }


                
                board.bitboards[board.selectedPiece] = clickedTile | oldBitmap;
                board.selectedPiece = 12;
                board.selectedTile = 0;
                board.generatedMoves = 0;

                if(!board.whiteToMove)
                    board.fullmove++;
                board.whiteToMove = !board.whiteToMove;

                std::string color = board.whiteToMove ? "white" : "black";
                std::cout << color << " to play\n";
                std::cout << "Fullmoves: " << board.fullmove << "\n";
                std::cout << "Halfmoves: " << board.halfmove << "\nCastling:\n";
                std::cout << board.castleKingW << " " << board.castleQueenW << "\n";
                std::cout << board.castleKingB << " " << board.castleQueenB << "\n";

                // check for mate //
                uint64_t allLegalMoves = board.getAllLegalMoves(board.whiteToMove);
                std::cout << "All legal moves\n";
                PrintBoard(allLegalMoves);

                if(board.halfmove >= 50)
                {
                    resultDraw = true;
                }

                if(!allLegalMoves)
                {
                    winner = board.whiteToMove ? "black" : "white";
                    checkmate = true;
                    std::cout << "#########\n";
                    std::cout << "CHECKMATE\n";
                    std::cout << winner << " WON!\n";
                    std::cout << "#########\n";
                }

            }
        }
    }
}
