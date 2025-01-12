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

    // Creating board //
    std::array<uint64_t, 12> bitboards = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    void loadFEN(std::string FEN);
    bool whiteToMove = true;
    uint64_t getAllPieces();
    uint64_t getColorPieces(bool white);
    uint64_t enPassantTarget = 0;
    bool castleQueenB = false;
    bool castleKingB = false;
    bool castleQueenW = false;
    bool castleKingW = false;
    uint64_t halfmove;
    std::string halfmoveText;
    std::string fullmoveText;
    uint64_t fullmove;

    // input //
    // tile that was selected in previous event
    uint64_t selectedTile;
    uint8_t selectedTileX, selectedTileY;

    // piece that was selected in previous event
    int selectedPiece = 12;

    // Move generation //
    uint64_t KING_MOVES[64];
    uint64_t KNIGHT_MOVES[64];
    void GenerateKingMoves();
    void GenerateKnightMoves();

    uint64_t GeneratePawnMoves(bool white, uint64_t selectedTile);
    uint64_t GenerateBishopMoves(bool white, uint64_t selectedTile);
    uint64_t GenerateRookMoves(bool white, uint64_t selectedTile);

    uint64_t generatedMoves;
    uint64_t generateMoves(uint64_t selectedTile, uint64_t selectedPiece, uint64_t selectedTileX, uint64_t selectedTileY);
    
    uint64_t getCheckedTiles(bool white);
    uint64_t legalMoves;
    uint64_t getLegalMoves(bool white, uint64_t selectedTile, int selectedPiece);
    uint64_t getAllLegalMoves(bool white);

    const uint64_t FILE_A = 0x8080808080808080ULL;
    const uint64_t FILE_H = 0x0101010101010101ULL;
    const uint64_t FILE_B = 0x4040404040404040ULL;
    const uint64_t FILE_G = 0x0202020202020202ULL;

    const uint64_t RANK_2 = 0x000000000000FF00ULL;
    const uint64_t RANK_4 = 0x00000000FF000000ULL;
    const uint64_t RANK_7 = 0x00FF000000000000ULL;
    const uint64_t RANK_5 = 0x000000FF00000000ULL;

    const uint64_t RANK_1 = 0x00000000000000FFULL;
    const uint64_t RANK_8 = 0xFF00000000000000ULL;

    const uint64_t LEFT_UP_CORNER = 0x8000000000000000ULL;
    const uint64_t RIGHT_UP_CORNER = 0x0100000000000000ULL;
    const uint64_t RIGHT_DOWN_CORNER = 0x0000000000000001ULL;
    const uint64_t LEFT_DOWN_CORNER = 0x0000000000000080ULL;
};

inline Board board;
