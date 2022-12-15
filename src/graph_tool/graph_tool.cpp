#include <assert.h>
#include <iostream>
#include <iomanip>

#define pgm_read_byte(a)    *a

#include "../graph.h"

const int FACTOR = 4;
const int WIDTH = 0x20;
const int HEIGHT = 0x20;

constexpr inline uint16_t cc(const int c)
{
    return c * FACTOR;
}

int main()
{
    Window<cc(WIDTH + 1), cc(HEIGHT + 1)> win(colBlack);

    fill_circle(win, cc(WIDTH / 2) + 2, cc(HEIGHT / 2) + 2, cc(WIDTH / 2) + 1, colWhite);

    /*
    for (int x = 0; x < cc(WIDTH + 1); x++)
    {
        if ((x & 3) == 0)
        {
            for (int i = 0; i < cc(WIDTH + 4); ++i)
                std::cout << '.';
            std::cout << std::endl;
        }

        for (int y = 0; y < cc(HEIGHT + 1); y++)
        {
            if ((y & 3) == 0)
                std::cout << '.';

            std::cout << (win.get_color(x, y) == colWhite ? 'x' : ' ');
        }
        std::cout << '.' << std::endl;
    }
    */

    for (int x = 0; x < cc(WIDTH + 1); x += FACTOR)
    {
        std::cout << '{';
        for (int y = 0; y < cc(HEIGHT + 1); y += FACTOR)
        {
            int sum = 0;
            for (int x0 = x; x0 < x + FACTOR; x0++)
            {
                for (int y0 = y; y0 < y + FACTOR; y0++)
                {
                    if (win.get_color(x0, y0) == colWhite)
                        sum++;
                }
            }

            if (sum <= FACTOR * FACTOR)
                std::cout << std::setw(2) << sum << ',';
            else
                assert(false);
        }

        std::cout << "}," << std::endl;
    }
}
