#include "board.h"
#include <iostream>
#include <map>
#include <bitset>


Board::Board()
{
    
    loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e3 0 1");
    //loadFEN("4kb1r/p4ppp/4q3/8/8/1B6/PPP2PPP/2KR4");
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
                castlingRights |= (static_cast<uint64_t>(1) << 3);
                break;
            case 'Q':
                castlingRights |= (static_cast<uint64_t>(1) << 2);
                break;
            
            case 'k':
                castlingRights |= (static_cast<uint64_t>(1) << 1);
                break;
            
            case 'q':
                castlingRights |= (static_cast<uint64_t>(1) << 0);
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
            halfmove = FEN[i] - '0';
        }

        if(numOfSpaces == 5 && isdigit(FEN[i]))
        {
            fullmove = FEN[i] - '0';
        }
    }

    std::cout << "halfmove: " << halfmove << "\n";
    std::cout << "fullmove: " << fullmove << "\n";
    std::cout << std::bitset<64>(castlingRights) << "\n";
    std::cout << std::bitset<64>(bitboards[10]) << "\n";
}