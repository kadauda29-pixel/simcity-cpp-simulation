============================================================
Project 1: SimCity
CSCE 2110 – Foundations of Data Structures
============================================================

TEAM MEMBERS:
• [Jeff]
• [Khad]
• [Benjamin]

------------------------------------------------------------
INSTRUCTIONS TO COMPILE:
------------------------------------------------------------
1. Ensure all .cpp and .h files are in the same directory.
2. Open a terminal in this directory.
3. Compile using:
       g++ -std=c++17 -o simcity main.cpp [other .cpp files]
   or, if a Makefile is provided:
       make
4. The executable file will be named 'simcity' (or as defined in the Makefile).

------------------------------------------------------------
INSTRUCTIONS TO RUN:
------------------------------------------------------------
1. Run the program using:
       ./simcity <configuration_filename>
   Example:
       ./simcity config.txt

2. The configuration file (config.txt) should contain:
   • Line 1: name of the region layout CSV file (e.g., region.csv)
   • Line 2: maximum number of simulation time steps (integer)
   • Line 3: refresh rate (integer – how often to display region state)

3. The region layout file must be a valid CSV file consisting of:
   • 'R' for Residential zone
   • 'I' for Industrial zone
   • 'C' for Commercial zone
   • 'T' for Road
   • '#' for Powerline
   • 'P' for Powerplant (if used)

4. The program will simulate the city growth according to the provided rules and display output at each refresh interval.

------------------------------------------------------------
PROGRAM FUNCTIONALITY SUMMARY:
------------------------------------------------------------
Our SimCity simulation implements all major functionality components:
1. Reading configuration and region initialization
2. Residential zone functionality (data storage, growth rules, transformations)
3. Commercial zone functionality
4. Industrial zone functionality
5. Pollution functionality (spread and decay)
6. Regional analysis and final output generation

------------------------------------------------------------
NOTES:
------------------------------------------------------------
• All file names are case-sensitive.
• Do NOT include any example input/output files in your submission.
• Ensure code compiles successfully on CSE CELL machines before submission.
• Code is modular, with each major functionality defined in its own .cpp/.h pair.

------------------------------------------------------------
BONUS:
------------------------------------------------------------
[Specify whether your team attempted the bonus or not.]

------------------------------------------------------------
MAKEFILE NOTE:
------------------------------------------------------------
If using a Makefile:
• Run 'make' to build the project.
• Run './simcity config.txt' to execute.
• No additional compiler flags required unless noted.

============================================================
END OF README
============================================================
