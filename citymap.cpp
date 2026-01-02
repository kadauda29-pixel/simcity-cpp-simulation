//Benjamin Keller
//CSCE 2110 project 1
// 9/14/2025
#include "citymap.h"
#include <fstream>
#include <cstdio>
#include <algorithm>

CityMap::CityMap() : rows(0), cols(0) {}

bool CityMap::inBounds(int y, int x) const {
    return (y >= 0 && y < rows && x >= 0 && x < cols);
}

bool CityMap::loadRegion(const string &filename) {
    ifstream file(filename.c_str());
    if (!file.is_open()) return false;

    grid.clear();
    string line;

    // CSV: each line like R,R,.,T,C
    while (std::getline(file, line)) {
        vector<Cell> row;
        // simple CSV parse: read characters ignoring commas and CR
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (c == ',' || c == '\r' || c == '\n') continue;
            Cell cell;
            // Letters define zones; digits never appear in input here.
            if (c == 'R' || c == 'I' || c == 'C' || c == 'T' || c == '.') {
                cell.zone = c;
                cell.pop  = 0;
            } else {
                // default to empty on unknown symbol
                cell.zone = '.';
                cell.pop  = 0;
            }
            row.push_back(cell);
        }
        if (!row.empty()) grid.push_back(row);
    }
    file.close();

    rows = (int)grid.size();
    cols = rows ? (int)grid[0].size() : 0;

    // init pollution grid
    pollution.assign(rows, vector<int>(cols, 0));
    computePollution();

    return (rows > 0 && cols > 0);
}

int CityMap::adjacentCountAtLeast(int y, int x, int threshold) const {
    int cnt = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dy == 0 && dx == 0) continue;
            int ny = y + dy, nx = x + dx;
            if (!inBounds(ny, nx)) continue;
            // Only zoned cells have population
            if (grid[ny][nx].zone == 'R' || grid[ny][nx].zone == 'I' || grid[ny][nx].zone == 'C') {
                if (grid[ny][nx].pop >= threshold) cnt++;
            }
        }
    }
    return cnt;
}

int CityMap::adjacentTotalPopulation(int y, int x) const {
    int total = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dy == 0 && dx == 0) continue;
            int ny = y + dy, nx = x + dx;
            if (!inBounds(ny, nx)) continue;
            if (grid[ny][nx].zone == 'R' || grid[ny][nx].zone == 'I' || grid[ny][nx].zone == 'C') {
                total += grid[ny][nx].pop;
            }
        }
    }
    return total;
}

bool CityMap::adjacentToPowerline(int y, int x) const {
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dy == 0 && dx == 0) continue;
            int ny = y + dy, nx = x + dx;
            if (!inBounds(ny, nx)) continue;
            if (grid[ny][nx].zone == 'T') return true;
        }
    }
    return false;
}

void CityMap::computePollution() {
    // Reset pollution then add contributions from industrial cells
    pollution.assign(rows, vector<int>(cols, 0));

    // For each industrial cell with pop p:
    // pollution at (y,x) += p
    // neighbors at distance d get max(p - d, 0). Combine contributions by addition.
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            if (grid[y][x].zone == 'I' && grid[y][x].pop > 0) {
                int p = grid[y][x].pop;
                // simple breadth in Chebyshev distance (8-neighborhood distance)
                int radius = p; // when p-d > 0 => d < p
                for (int dy = -radius; dy <= radius; ++dy) {
                    for (int dx = -radius; dx <= radius; ++dx) {
                        int ny = y + dy, nx = x + dx;
                        if (!inBounds(ny, nx)) continue;
                        int d = std::max(std::abs(dy), std::abs(dx)); // Chebyshev distance
                        int add = p - d;
                        if (add > 0) pollution[ny][nx] += add;
                    }
                }
            }
        }
    }
}

bool CityMap::simulate(int workers_available, int goods_available) {
    // Prepare next-population grid (start as current)
    vector<vector<int>> nextPop(rows, vector<int>(cols, 0));
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            nextPop[y][x] = grid[y][x].pop;

    // Collect candidates:

    // Residential: no resources required, apply all that qualify
    struct Cand {
        int y, x;
        char zone; // 'R','I','C'
        int pop;   // current pop
        int adjTot;// adjacent total population
    };

    vector<Cand> rList, iList, cList;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            char z = grid[y][x].zone;
            int  p = grid[y][x].pop;
            if (z != 'R' && z != 'I' && z != 'C') continue;

            // Check growth rules for each zone using CURRENT state
            if (z == 'R') {
                bool grow = false;
                if (p == 0) {
                    if (adjacentToPowerline(y, x)) grow = true;
                    else if (adjacentCountAtLeast(y, x, 1) >= 1) grow = true;
                } else if (p == 1) {
                    if (adjacentCountAtLeast(y, x, 1) >= 2) grow = true;
                } else if (p == 2) {
                    if (adjacentCountAtLeast(y, x, 2) >= 4) grow = true;
                } else if (p == 3) {
                    if (adjacentCountAtLeast(y, x, 3) >= 6) grow = true;
                } else if (p == 4) {
                    if (adjacentCountAtLeast(y, x, 4) >= 8) grow = true;
                }
                if (grow) {
                    Cand c; c.y = y; c.x = x; c.zone = 'R'; c.pop = p; c.adjTot = adjacentTotalPopulation(y, x);
                    rList.push_back(c);
                }
            } else if (z == 'I') {
                bool grow = false;
                if (p == 0) {
                    if (adjacentToPowerline(y, x) && workers_available >= 2) grow = true;
                    else if (adjacentCountAtLeast(y, x, 1) >= 1 && workers_available >= 2) grow = true;
                } else if (p == 1) {
                    if (adjacentCountAtLeast(y, x, 1) >= 2 && workers_available >= 2) grow = true;
                } else if (p == 2) {
                    if (adjacentCountAtLeast(y, x, 2) >= 4 && workers_available >= 2) grow = true;
                }
                if (grow) {
                    Cand c; c.y = y; c.x = x; c.zone = 'I'; c.pop = p; c.adjTot = adjacentTotalPopulation(y, x);
                    iList.push_back(c);
                }
            } else if (z == 'C') {
                bool grow = false;
                if (p == 0) {
                    if (adjacentToPowerline(y, x) && workers_available >= 1 && goods_available >= 1) grow = true;
                    else if (adjacentCountAtLeast(y, x, 1) >= 1 && workers_available >= 1 && goods_available >= 1) grow = true;
                } else if (p == 1) {
                    if (adjacentCountAtLeast(y, x, 1) >= 2 && workers_available >= 1 && goods_available >= 1) grow = true;
                }
                if (grow) {
                    Cand c; c.y = y; c.x = x; c.zone = 'C'; c.pop = p; c.adjTot = adjacentTotalPopulation(y, x);
                    cList.push_back(c);
                }
            }
        }
    }

    // Residential: apply all (no resource constraints)
    for (size_t k = 0; k < rList.size(); ++k) {
        int y = rList[k].y, x = rList[k].x;
        nextPop[y][x] = grid[y][x].pop + 1;
    }

    // Sort and apply Commercial first (priority)
    auto cmp = [](const Cand &a, const Cand &b){
        // larger population first
        if (a.pop != b.pop) return a.pop > b.pop;
        // larger adjacent total population first
        if (a.adjTot != b.adjTot) return a.adjTot > b.adjTot;
        // smaller Y, then smaller X
        if (a.y != b.y) return a.y < b.y;
        return a.x < b.x;
    };
    std::sort(cList.begin(), cList.end(), cmp);
    int workers = workers_available;
    int goods   = goods_available;

    for (size_t k = 0; k < cList.size(); ++k) {
        if (workers >= 1 && goods >= 1) {
            int y = cList[k].y, x = cList[k].x;
            nextPop[y][x] = grid[y][x].pop + 1;
            workers -= 1;
            goods   -= 1;
        }
    }

    // Then Industrial
    std::sort(iList.begin(), iList.end(), cmp);
    for (size_t k = 0; k < iList.size(); ++k) {
        if (workers >= 2) {
            int y = iList[k].y, x = iList[k].x;
            nextPop[y][x] = grid[y][x].pop + 1;
            workers -= 2;
        }
    }

    // Detect changes and commit next state
    bool changed = false;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            if (grid[y][x].pop != nextPop[y][x]) changed = true;
            grid[y][x].pop = nextPop[y][x];
        }
    }

    // Recompute pollution after population updates
    computePollution();

    return changed;
}

void CityMap::display() const {
    // Display rules:
    // If zoned R/I/C and pop==0 => print the LETTER.
    // If pop>0 => print the population number.
    // Powerline 'T' prints 'T'. Empty '.' prints '.'
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            const Cell &c = grid[y][x];
            if (c.zone == 'R' || c.zone == 'I' || c.zone == 'C') {
                if (c.pop == 0) {
                    printf("%c ", c.zone);
                } else {
                    printf("%d ", c.pop);
                }
            } else {
                // terrain
                printf("%c ", c.zone);
            }
        }
        printf("\n");
    }
}

void CityMap::displayPollution() const {
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            printf("%d ", pollution[y][x]);
        }
        printf("\n");
    }
}

int CityMap::totalPollution() const {
    int s = 0;
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            s += pollution[y][x];
    return s;
}

int CityMap::totalResidentialPopulation() const {
    int s = 0;
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            if (grid[y][x].zone == 'R') s += grid[y][x].pop;
    return s;
}

int CityMap::totalIndustrialPopulation() const {
    int s = 0;
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            if (grid[y][x].zone == 'I') s += grid[y][x].pop;
    return s;
}

int CityMap::totalCommercialPopulation() const {
    int s = 0;
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            if (grid[y][x].zone == 'C') s += grid[y][x].pop;
    return s;
}

void CityMap::areaStats(int x1, int y1, int x2, int y2, int &res, int &ind, int &com, int &pol) const {
    res = ind = com = pol = 0;
    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            const Cell &c = grid[y][x];
            if (c.zone == 'R') res += c.pop;
            else if (c.zone == 'I') ind += c.pop;
            else if (c.zone == 'C') com += c.pop;
            pol += pollution[y][x];
        }
    }
}
