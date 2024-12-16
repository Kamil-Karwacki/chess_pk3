#pragma once
#include <cstdint>
#include <array>
#include <string>

class Board
{
private:

public:
    Board();
    ~Board();

    static Board& getInstance(){
        static Board theInstance;
        return theInstance;
    }

    // 
    // 
    std::array<uint64_t, 12> bitboards = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    void loadFEN(std::string FEN);
    bool whiteToMove = true;
    uint64_t enPassantTarget = 0;
    uint8_t castlingRights = 0; // white kingside, white queenside, black kingside, black queenside
    uint64_t halfmove;
    uint64_t fullmove;
};


