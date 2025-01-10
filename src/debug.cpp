
#include "debug.h"
#include <iostream>
#include <bitset>

void PrintBoard(uint64_t board)
{
    std::string textBoard =  std::bitset<64>(board).to_string();
    for(int i = 0; i < 64; i++)
    {
        std::cout << textBoard[i];
        if((i+1) % 8 == 0)
        {
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}
