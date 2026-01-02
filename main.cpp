//Khadijat Dauda
//CSCE 2110 project 1
// 9/14/2025
#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include "citymap.h"
using namespace std;

int main() {
    char configName[128];
    printf("Type the name of the config file: ");
    if (scanf("%127s", configName) != 1) {
        printf("Error: could not read config file name.\n");
        return 1;
    }

    ifstream cfg(configName);
    if (!cfg.is_open()) {
        printf("Error: could not open %s\n", configName);
        return 1;
    }

    string regionFile;
    int timeLimit = 0;
    int refreshRate = 1; // how often to print the region each step

    // config.txt format:
    // <region filename>
    // <time limit>
    // <refresh rate>
    cfg >> regionFile;
    cfg >> timeLimit;
    cfg >> refreshRate;
    cfg.close();

    CityMap city;
    if (!city.loadRegion(regionFile)) {
        printf("Error: could not load region file.\n");
        return 1;
    }

    // Time step 0 (initial state)
    printf("Initial region state (time step 0):\n");
    city.display();

    int step = 0;
    bool changed = true;

    while (step < timeLimit && changed) {
        step++;

        // compute available workers and goods based on current state
        int workers = city.totalResidentialPopulation();
        int goods   = city.totalIndustrialPopulation();

        // show per-step header (not for step 0)
        printf("\nTime step %d\n", step);
        printf("Available workers: %d\n", workers);
        printf("Available goods: %d\n", goods);

        // run one simulation tick; returns whether any cell changed
        changed = city.simulate(workers, goods);

        // refresh printing per config rate
        if (refreshRate <= 0) refreshRate = 1;
        if (step % refreshRate == 0) {
            printf("\nRegion state at time step %d:\n", step);
            city.display();
        }
    }

    // Final outputs
    printf("\n--- Final Outputs ---\n");
    printf("Final region state (time step %d):\n", step);
    city.display();

    int rTot = city.totalResidentialPopulation();
    int iTot = city.totalIndustrialPopulation();
    int cTot = city.totalCommercialPopulation();
    printf("\nTotal population (Residential): %d\n", rTot);
    printf("Total population (Industrial): %d\n", iTot);
    printf("Total population (Commercial): %d\n", cTot);

    // Pollution (already computed inside simulate), print final pollution map + total
    printf("\nFinal regional pollution state:\n");
    city.displayPollution();
    printf("Total regional pollution: %d\n", city.totalPollution());

    // Area query with bounds checking
    int x1, y1, x2, y2;
    int cols = city.getCols();
    int rows = city.getRows();
    while (true) {
        printf("\nEnter rectangle to analyze as x1 y1 x2 y2 (inclusive, origin at top-left 0,0): ");
        if (scanf("%d %d %d %d", &x1, &y1, &x2, &y2) != 4) {
            printf("Invalid input. Try again.\n");
            // clear stdin line
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            continue;
        }
        // normalize order
        if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
        if (y1 > y2) { int t = y1; y1 = y2; y2 = t; }

        if (x1 < 0 || y1 < 0 || x2 >= cols || y2 >= rows) {
            printf("Coordinates out of bounds. Valid X in [0,%d], Y in [0,%d]. Try again.\n", cols-1, rows-1);
            continue;
        }
        break;
    }

    int rA, iA, cA, polA;
    city.areaStats(x1, y1, x2, y2, rA, iA, cA, polA);
    printf("\n--- Area Summary (%d,%d) to (%d,%d) ---\n", x1, y1, x2, y2);
    printf("Residential population: %d\n", rA);
    printf("Industrial population:  %d\n", iA);
    printf("Commercial population:  %d\n", cA);
    printf("Total pollution:        %d\n", polA);

    return 0;
}
