//Khadijat Dauda
//CSCE 2110 project 1
// 9/14/2025
#ifndef CITYMAP_H
#define CITYMAP_H

#include <vector>
#include <string>
using namespace std;

// Cell types (zones / terrain):
// 'R' = Residential zone
// 'I' = Industrial zone
// 'C' = Commercial zone
// 'T' = Powerline
// '.' = empty/land/road (non-zoned, no effect)

struct Cell {
    char zone;     // 'R','I','C','T','.' (zone type/terrain)
    int  pop;      // population for R/I/C (0..9 typical). Non-zoned cells use 0.
};

class CityMap {
private:
    vector<vector<Cell>> grid;
    vector<vector<int>>  pollution; // computed each step
    int rows;
    int cols;

    // helpers
    bool inBounds(int y, int x) const;
    int  adjacentCountAtLeast(int y, int x, int threshold) const;
    int  adjacentTotalPopulation(int y, int x) const;
    bool adjacentToPowerline(int y, int x) const;

    void computePollution(); // recompute pollution map from all industrial cells

public:
    CityMap();

    bool loadRegion(const string &filename);

    // Simulation step:
    // Takes available workers and goods by value (from caller for display),
    // but internally allocates them to growth. Returns true if any cell changed.
    bool simulate(int workers_available, int goods_available);

    // Display region: letters for zero-pop R/I/C; digits for positive pop.
    void display() const;

    // Pollution visualization and totals
    void displayPollution() const;
    int  totalPollution() const;

    // Totals
    int totalResidentialPopulation() const;
    int totalIndustrialPopulation() const;
    int totalCommercialPopulation() const;

    // Area stats within rectangle (inclusive)
    void areaStats(int x1, int y1, int x2, int y2, int &res, int &ind, int &com, int &pol) const;

    // Dimensions
    int getRows() const { return rows; }
    int getCols() const { return cols; }
};

#endif
