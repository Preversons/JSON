#include <iostream>
#include <fstream>
#include "json.hpp"
#include <Windows.h> // For Windows console color support
using namespace std;

int main(int argc, char* argv[]) {

    // Enable ANSI escape codes for colored output on Windows
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hConsole, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, mode);

    string jsonString = "";

    if (argc < 2) {
        cout << "Please provide a JSON file path as an argument." << endl;
        return 1;
    }

    ifstream jsonFile(argv[1]);
    if (!jsonFile.is_open()) {
        cout << "Failed to open the file: " << argv[1] << endl;
        return 1;
    }

    // read into the jsonString variable
    {
        string line;
        while (getline(jsonFile, line)) { jsonString += line; }
    }

    cout << "Input JSON String = "<< jsonString << endl;

    vector<string> result = json::Parser::tokenize(jsonString);

    cout << "Tokenized Result = " << endl;

    for (int i = 0;i < result.size(); i++) {
        cout << result[i] << endl;
    }

    auto parser = json::Parser(jsonString);
    auto parseResult = parser.parse();

    return 0;
}