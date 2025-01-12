#include "board.h"
#include <iostream>
#include <map>
#include <bitset>
#include <bit>
#include <vector>
#include "debug.h"


Board::Board()
{
    loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    //loadFEN("4kb1r/p4ppp/4q3/8/8/1B6/PPP2PPP/2KR4");
    GenerateKingMoves();
    GenerateKnightMoves();
}

Board::~Board()
{
}

void Board::loadFEN(std::string FEN)
{
    // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1  <- pozycja początkowa

    unsigned int position = 63;

    std::map<char, uint8_t> mapFEN = 
    { 
        {'q', 0}, {'k', 1}, {'r', 2}, {'n', 3}, {'b', 4}, {'p', 5},  
        {'Q', 6}, {'K', 7}, {'R', 8}, {'N', 9}, {'B', 10}, {'P', 11}
    };

    unsigned int numOfSpaces = 0;
    for(int i = 0; i < FEN.size(); i++)
    {
        if(FEN[i] == ' ')
        {
            numOfSpaces++;
        }

        if(isdigit(FEN[i]) && numOfSpaces == 0)
        {
            std::cout << FEN[i] << " Decreasing position from " << position << "to ";
            position -= static_cast<unsigned int>(FEN[i] - '0');
            std::cout << position << "\n";
        }   
        else if(!isdigit(FEN[i]) && numOfSpaces == 0)
        {
            if(FEN[i] == '/')
            {
                continue;
            }
            bitboards[mapFEN[FEN[i]]] |= (static_cast<uint64_t>(1) << position);
            std::cout << "Setting " << FEN[i] << " at " << position << " to 1 \n";
            position--;
        }

        if(numOfSpaces == 1 && FEN[i] == 'b')
        {
            whiteToMove = false;
        }   


        if(numOfSpaces == 2)
        {
            switch (FEN[i])
            {
            case 'K':
                castleKingW = true;
                break;
            case 'Q':
                castleQueenW = true;
                break;
            
            case 'k':
                castleKingB = true;
                break;
            
            case 'q':
                castleQueenB = true;
                break;
            
            default:
                break;
            }
        }   

        int column;
        int row;

        if(numOfSpaces == 3 && FEN[i] == '-')
        {
            enPassantTarget = 0;
        }
        else if(numOfSpaces == 3 && isalpha(FEN[i]))
        {
            column = FEN[i] - 'a';
        }
        else if(numOfSpaces == 3 && isdigit(FEN[i]))
        {
            row = FEN[i] - '0';
            enPassantTarget |= (static_cast<uint64_t>(1) << (row * 8 + column - 8));
        }

        if(numOfSpaces == 3)
        {
            std::cout << row << " row, column " << column << "\n";
        }

        if(numOfSpaces == 4 && isdigit(FEN[i]))
        {
            halfmoveText += FEN[i];
        }

        if(numOfSpaces == 5 && isdigit(FEN[i]))
        {
            fullmoveText += FEN[i];
        }
    }

    halfmove = std::stoi(halfmoveText);
    fullmove = std::stoi(fullmoveText);
    std::cout << "halfmove: " << halfmove << "\n";
    std::cout << "fullmove: " << fullmove << "\n";
    std::cout << castleKingW << " " << castleQueenW << "\n";
    std::cout << castleKingB << " " << castleQueenB << "\n";
    std::cout << "En passant\n";
    PrintBoard(enPassantTarget);
}

void Board::GenerateKingMoves()
{
    uint64_t selectedTile;
    for(int i = 0; i < 64; i++)
    {
        selectedTile = (static_cast<uint64_t>(1) << i);
        uint64_t front = selectedTile << 8;
        uint64_t back = selectedTile >> 8;

        uint64_t left = (selectedTile << 1) & ~FILE_H;
        uint64_t right = (selectedTile >> 1) & ~FILE_A;

        uint64_t frontLeft = (selectedTile << 9) & ~FILE_H;
        uint64_t backLeft = (selectedTile >> 7) & ~FILE_H;

        uint64_t frontRight = (selectedTile << 7) & ~FILE_A;
        uint64_t backRight = (selectedTile >> 9) & ~FILE_A;

        KING_MOVES[i] = front | back | left | right | frontLeft | backLeft | frontRight | backRight;
    }
}

void Board::GenerateKnightMoves()
{
    uint64_t selectedTile;
    for(int i = 0; i < 64; i++)
    {
        selectedTile = (static_cast<uint64_t>(1) << i);
        uint64_t frontLeft = (selectedTile << 17) & ~FILE_H;
        uint64_t frontRight = (selectedTile << 15) & ~FILE_A;

        uint64_t rightUp = (selectedTile << 6) & ~FILE_A & ~FILE_B;
        uint64_t rightDown = (selectedTile >> 10) & ~FILE_A & ~FILE_B;

        uint64_t downRight = (selectedTile >> 17) & ~FILE_A;
        uint64_t downLeft = (selectedTile >> 15) & ~FILE_H;

        uint64_t leftDown = (selectedTile >> 6) & ~FILE_H & ~FILE_G;
        uint64_t leftUp = (selectedTile << 10) & ~FILE_H & ~FILE_G;

        KNIGHT_MOVES[i] = frontLeft | frontRight | rightUp | rightDown | downRight | downLeft | leftDown | leftUp;
    }
}

uint64_t Board::GeneratePawnMoves(bool white, uint64_t selectedTile)
{
    uint64_t generatedMoves = 0;
    uint64_t nextTile;
    uint64_t leftUp;
    uint64_t rightUp;

    if(white)
    {
        nextTile = selectedTile << 8;
        leftUp = selectedTile << 9;
        rightUp = selectedTile << 7;
    }
    else
    {
        nextTile = selectedTile >> 8;
        leftUp = selectedTile >> 9;
        rightUp = selectedTile >> 7;
    }

    if(selectedTile & RANK_2 && white && (selectedTile << 8) & ~getAllPieces())
    {
        nextTile |= nextTile << 8;
    }
    else if(selectedTile & RANK_7 && !white && (selectedTile >> 8) & ~getAllPieces())
    {
        nextTile |= nextTile >> 8;
    }

    //generatedMoves |= (leftUp | rightUp) & enPassantTarget;

    if(white)
    {
        generatedMoves |= (leftUp & (getColorPieces(!white) | enPassantTarget) & ~FILE_H) | (rightUp & (getColorPieces(!white) | enPassantTarget) & ~FILE_A);
    }
    else
    {
        generatedMoves |= (leftUp & (getColorPieces(!white) | enPassantTarget) & ~FILE_A) | (rightUp & (getColorPieces(!white) | enPassantTarget) & ~FILE_H);
    }

    generatedMoves |= nextTile & (~getAllPieces());

    return generatedMoves;
}

uint64_t Board::GenerateRookMoves(bool white, uint64_t selectedTile)
{
    uint64_t generated = 0;
    int dirs[] = {8, -8, 1, -1};
    for(int i = 0; i < 4; i++)
    {
        uint64_t currentTile = selectedTile;
        uint64_t nextTile = currentTile;
        while(true)
        {
            if(dirs[i] > 0)
                nextTile = nextTile << dirs[i];
            else
                nextTile = nextTile >> abs(dirs[i]);

            if(dirs[i] == 8 && (RANK_8 & currentTile))
                break;
            else if(dirs[i] == -8 && (RANK_1 & currentTile))
                break;
            else if(dirs[i] == 1 && (FILE_A & currentTile))
                break;
            else if(dirs[i] == -1 && (FILE_H & currentTile))
                break;

            if(nextTile & getColorPieces(white))
                break;

            if(nextTile & getColorPieces(!white))
            {
                generated |= nextTile;
                break;
            }

            generated |= nextTile;
            currentTile = nextTile;
        }
    }
    return generated;
}

uint64_t Board::GenerateBishopMoves(bool white, uint64_t selectedTile)
{
    uint64_t generated = 0;
    int dirs[] = {9, -9, -7, 7};
    for(int i = 0; i < 4; i++)
    {
        uint64_t currentTile = selectedTile;
        uint64_t nextTile = currentTile;
        while(true)
        {
            if(dirs[i] > 0)
                nextTile = nextTile << dirs[i];
            else
                nextTile = nextTile >> abs(dirs[i]);

            
            if(dirs[i] == 9 && ((FILE_A | RANK_8) & currentTile))
                break;
            else if(dirs[i] == -9 && ((FILE_H | RANK_1) & currentTile))
                break;
            else if(dirs[i] == 7 && ((FILE_H | RANK_8) & currentTile))
                break;
            else if(dirs[i] == -7 && ((FILE_A | RANK_1) & currentTile))
                break;

            if(nextTile & getColorPieces(white))
                break;

            if(nextTile & getColorPieces(!white))
            {
                generated |= nextTile;
                break;
            }

            generated |= nextTile;
            currentTile = nextTile;
        }
    }
    return generated;
}

uint64_t Board::generateMoves(uint64_t selectedTile, uint64_t selectedPiece, uint64_t selectedTileX, uint64_t selectedTileY)
{
    bool white = selectedPiece < 6 ? false : true;
    //std::cout << "Start of move generation " << white << "\n";
    generatedMoves = 0;
    //PrintBoard(selectedTile);

    switch (selectedPiece)
    {
    case 11:
    case 5:
    {
        generatedMoves = GeneratePawnMoves(white, selectedTile);
        break;
    }
    case 7:
    case 1:
    {
        generatedMoves = KING_MOVES[(7 - selectedTileX + (7 - selectedTileY) * 8)] & ~getColorPieces(white);
        // tiles attacked by enemy //
        uint64_t attackedTiles = getCheckedTiles(!white);
        uint64_t allPieces = getAllPieces();
        uint64_t illegalTiles = attackedTiles | allPieces;

        uint64_t kingSideTiles[] = {(selectedTile >> 1), (selectedTile >> 2)};
        bool castleKing = white ? castleKingW : castleKingB;
        if(kingSideTiles[0] & ~illegalTiles && kingSideTiles[1] & ~illegalTiles && castleKing)
        {
            std::cout << "King can castle king side" << "\n";
            generatedMoves |= kingSideTiles[1];
        }

        uint64_t queenSideTiles[] = {(selectedTile << 1), (selectedTile << 2), (selectedTile << 3)};
        bool castleQueen = white ? castleQueenW : castleQueenB;

        if(queenSideTiles[0] & ~illegalTiles && queenSideTiles[1] & ~illegalTiles && queenSideTiles[2] & ~illegalTiles && castleQueen)
        {
            std::cout << "King can castle queen side" << "\n";
            generatedMoves |= queenSideTiles[1];
        }
        break;
    }
    case 9:
    case 3:
    {
        generatedMoves = KNIGHT_MOVES[(7 - selectedTileX + (7 - selectedTileY) * 8)] & ~getColorPieces(white);
        break;
    }
    case 8:
    case 2:
    {
        generatedMoves = GenerateRookMoves(white, selectedTile);
        break;
    }
    case 10:
    case 4:
    {
        generatedMoves = GenerateBishopMoves(white, selectedTile);
        break;
    }
    case 6:
    case 0:
    {
        generatedMoves = GenerateRookMoves(white, selectedTile) | GenerateBishopMoves(white, selectedTile);
        break;
    }
    
    default:
        break;
    }

    //std::cout << "Moves were generated for piece: " << selectedPiece << "\n";
    //PrintBoard(generatedMoves);
    //std::cout << "End of move generation\n";
    return generatedMoves;
}

uint64_t Board::getCheckedTiles(bool white)
{
    uint64_t tiles = 0;

    int min = white ? 6 : 0;
    int max = white ? 11 : 5;
    
    // pawn attacks //
    tiles = bitboards[max] << 7 & ~FILE_A;
    tiles |= bitboards[max] << 9 & ~FILE_H;

    // knight attacks //
    uint64_t frontLeft = (bitboards[max - 2] << 17) & ~FILE_H;
    uint64_t frontRight = (bitboards[max - 2] << 15) & ~FILE_A;

    uint64_t rightUp = (bitboards[max - 2] << 6) & ~FILE_A & ~FILE_B;
    uint64_t rightDown = (bitboards[max - 2] >> 10) & ~FILE_A & ~FILE_B;

    uint64_t downRight = (bitboards[max - 2] >> 17) & ~FILE_A;
    uint64_t downLeft = (bitboards[max - 2] >> 15) & ~FILE_H;

    uint64_t leftDown = (bitboards[max - 2] >> 6) & ~FILE_H & ~FILE_G;
    uint64_t leftUp = (bitboards[max - 2] << 10) & ~FILE_H & ~FILE_G;

    tiles |= frontLeft | frontRight | rightUp | rightDown | downRight | downLeft | leftDown | leftUp;

    // king attacks //
    uint64_t front = bitboards[max - 4] << 8;
    uint64_t back = bitboards[max - 4] >> 8;

    uint64_t left = (bitboards[max - 4] << 1) & ~FILE_H;
    uint64_t right = (bitboards[max - 4] >> 1) & ~FILE_A;

    frontLeft = (bitboards[max - 4] << 9) & ~FILE_H;
    uint64_t backLeft = (bitboards[max - 4] >> 7) & ~FILE_H;

    frontRight = (bitboards[max - 4] << 7) & ~FILE_A;
    uint64_t backRight = (bitboards[max - 4] >> 9) & ~FILE_A;

    tiles |= front | back | left | right | frontLeft | backLeft | frontRight | backRight;

    // rook attacks //
    uint64_t tempRookBitboard = bitboards[max - 3];
    while(tempRookBitboard)
    {
        uint64_t currentTile = (static_cast<uint64_t>(1) << std::__countr_zero(tempRookBitboard));
        tiles |= GenerateRookMoves(white, currentTile);
        tempRookBitboard &= (tempRookBitboard - 1);
    }

    uint64_t tempBishopBitboard = bitboards[max - 1];
    while(tempBishopBitboard)
    {
        uint64_t currentTile = (static_cast<uint64_t>(1) << std::__countr_zero(tempBishopBitboard));
        tiles |= GenerateBishopMoves(white, currentTile);
        tempBishopBitboard &= (tempBishopBitboard - 1);
    }

    uint64_t tempQueenBitboard = bitboards[max - 5];
    while(tempQueenBitboard)
    {
        uint64_t currentTile = (static_cast<uint64_t>(1) << std::__countr_zero(tempQueenBitboard));
        tiles |= GenerateBishopMoves(white, currentTile) | GenerateRookMoves(white, currentTile);
        tempQueenBitboard &= (tempQueenBitboard - 1);
    }

    tiles &= ~getColorPieces(white);

    return tiles;
}

// TOOD CHECK FOR CHECKMATE
uint64_t Board::getLegalMoves(bool white, uint64_t selectedTile, int selectedPiece)
{
    uint64_t legalMoves = 0;
    int tileX = 7 - (std::__countr_zero(selectedTile) % 8);
    int tileY = 7 - (std::__countr_zero(selectedTile) / 8);
    uint64_t generated = generateMoves(selectedTile, selectedPiece, tileX, tileY);

    int min = white ? 6 : 0;
    int max = white ? 11 : 5;

    int enemyMin = white ? min - 6 : min + 6;
    int enemyMax = white ? max - 6 : max + 6;



    while(generated)
    {
        Board copyBoard = *this;
        uint64_t currentTile = (static_cast<uint64_t>(1) << std::__countr_zero(generated));
        copyBoard.bitboards[selectedPiece] |= currentTile;
        copyBoard.bitboards[selectedPiece] &= ~selectedTile;

        for(int i = enemyMin; i <= enemyMax; i++)
        {
            copyBoard.bitboards[i] &= ~currentTile;
        }

        uint64_t checks = copyBoard.getCheckedTiles(!white);
        /*std::cout << "Current \n";
        PrintBoard(currentTile);
         std::cout << "Selected \n";
        PrintBoard(selectedTile);
        PrintBoard(copyBoard.bitboards[selectedPiece]);
        std::cout << "Checks\n";
        PrintBoard(checks);
        std::cout << "All\n";
        PrintBoard(copyBoard.getAllPieces());*/
        if(checks & copyBoard.bitboards[max - 4])
        {
            //std::cout << "Move is illegal\n";
        }
        else
        {
            legalMoves |= currentTile;
        }

        generated &= (generated - 1);
    }

    return legalMoves;
}

uint64_t Board::getAllLegalMoves(bool white)
{
    uint64_t legalMoves = 0;
    //uint64_t generated = getCheckedTiles(white);
    uint64_t allPieces = getColorPieces(white);

    int min = white ? 6 : 0;
    int max = white ? 11 : 5;

    int enemyMin = white ? min - 6 : min + 6;
    int enemyMax = white ? max - 6 : max + 6;

    while(allPieces)
    {
        uint64_t selectedTile = (static_cast<uint64_t>(1) << std::__countr_zero(allPieces));
        allPieces &= (allPieces - 1);
        int currentPiece = 12;
        for(int i = min; i <= max; i++)
        {
            if(bitboards[i] & selectedTile)
            {
                currentPiece = i;
            }
        }

        if(currentPiece == 12)
            continue;

        Board copyBoard = *this;

        //uint64_t generated = copyBoard.generateMoves(selectedTile, currentPiece, tileX, tileY);
        /*while(generated)
        {
            uint64_t currentTile = (static_cast<uint64_t>(1) << std::__countr_zero(generated));
            copyBoard.bitboards[currentPiece] |= currentTile;
            copyBoard.bitboards[currentPiece] &= ~selectedTile;

            for(int i = enemyMin; i <= enemyMax; i++)
            {
                copyBoard.bitboards[i] &= ~currentTile;
            }

            uint64_t checks = copyBoard.getCheckedTiles(!white);
            if(checks & copyBoard.bitboards[max - 4])
            {
                //std::cout << "Move is illegal\n";
            }
            else
            {
                legalMoves |= currentTile;
            }

            generated &= (generated - 1);
        }*/
       legalMoves |= copyBoard.getLegalMoves(white, selectedTile, currentPiece);
    }

    return legalMoves;
}

uint64_t Board::getAllPieces()
{
    uint64_t finalBoard = 0;
    for(int i = 0; i < bitboards.size(); i++)
    {
        finalBoard |= bitboards[i];
    }
    return finalBoard;
}

uint64_t Board::getColorPieces(bool white)
{
    uint64_t finalBoard = 0;
    uint8_t start = white ? 6 : 0;
    uint8_t end = white ? 12 : 6;
    for(int i = start; i < end; i++)
    {
        finalBoard |= bitboards[i];
    }
    
    return finalBoard;
}