# Summer of LabVIEW 2024 Word Search

This is my **ineligible** solution to the first challenge of the 2024 Summer of LabVIEW. I created it to see how quick I could get via the unholy (and unsporting) alliance of LabVIEW and C++.

Please try it out and open an issue if you have any questions.

## Challenge
> Given a 2D array of letters and a list of words to find, return a map of words with the positions of each letter. 
> Try to code as efficient an algorithm as you can as submissions will be ranked by how fast they complete the word search correctly.
> Challenge ends at 11:59 UTC on June 22 and submissions beyond that time will not be scored.

## Dependencies
LabVIEW 2020 Community Edition or later

## Building the Binaries
If you plan on building the C++ code into binaries you will need a compiler that supports the C++ 20 standard (specifically `std::span`) such as
* MSVC on Windows (2019 onwards)
* GCC on Linux (10 onwards)

You will also require CMake 3.5 onwards

A copy of the binaries is included in this repository so you can just `clone` and go but if want to make changes to the C++ code, binary builds are managed via CMake - check for CMake integration in your IDE for ease of use.

An example `.vscode` configuration for building and connecting the C++ debugger is provided for both platforms for _VS Code_ users. 

Use CMake to `configure`, `build` and `install` the *Release* version for maximum performance

## Licence
The licence specified covers the code created by the author and any contributors
* `word-search-solver.cpp`
* `CMakeLists.txt`
* `LV-Solver.vi`
* `Cpp-Solver.vi`
* `.vscode` example files and misc `.git` configuration files

The other code supplied in this repository is either from
[Summer of LabVIEW - WordSearch by Illuminated G](https://www.vipm.io/package/illuminatedg_lib_sol_wordsearch/) distributed under the MIT licence or from [Darren Nattinger](https://forums.ni.com/t5/World-s-Fastest-LabVIEW/Darren-Nattinger-World-s-Fastest-LabVIEW-Programmer/ta-p/3518113) and [Chrisian Altenbach](https://forums.ni.com/t5/LabVIEW-Champions-Directory/LabVIEW-Champion-Christian-Altenbach/ta-p/3495223) who I hope are happy with me distributing it in this way 😅🤞

## Bonus
A bonus branch exisits which demonstrates LabVIEW Managed Memory allocation from the C++ code.
