#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <fstream>

int solveCSV(std::vector<std::vector<std::string>>&table, size_t x1, size_t x2);

std::vector<std::string> readCSVRow(const std::string& row, size_t rowNumber) {
    std::vector<std::string> fields{ "" };
    size_t i = 0;
    size_t j = 0;
    for (char c : row) {
        switch (c) {
        case ',':
            if (j == 0) {
                if (rowNumber == 0 && i != 0) {
                    throw std::invalid_argument("Null column");
                }
                else if (rowNumber != 0 && i == 0) {
                    throw std::invalid_argument("Null row number");
                } 
                else if(rowNumber != 0 && i != 0){
                    fields.push_back("0");
                }
            }
            fields.push_back(""); i++; j = 0;
            break;
        default:  
            fields[i].push_back(c); j++;
            break;
        }
    }
    return fields;
}

std::vector<std::vector<std::string>> readCSV(std::istream& in) {
    std::vector<std::vector<std::string>> table;
    std::string row;
    size_t i = 0;
    while (!in.eof()) {
        std::getline(in, row);
        if (in.bad() || in.fail()) {
            std::cout << "Unknown error";
            break;
        }
        std::vector<std::string> fields = readCSVRow(row, i); i++;
        table.push_back(fields);
    }
    return table;
}

int getIntFromTable(std::vector<std::vector<std::string>> &table, std::string column, std::string row)
{
    size_t i = 1;
    while (table[0][i] != column){
        i++;
        if (i > table[0].size()) {
            //std::cout << "Column " << column << " not found";
            return 0;
        }
    }
    size_t j = 1;
    while (table[j][0] != row) {
        j++;
        if (j > table.size()) {
            //std::cout << "Row " << row << " not found";
            return 0;
        }
    }
    if (table[j][i][0] == '=') {
        return solveCSV(table, j, i);
    }
    else if ((int)table[j][i][0] > 47 && (int)table[j][i][0] < 58) {
        return std::stoi(table[j][i]);
    }
    else {
        //std::cout << "Not format in " << column << " " << row;
        return 0;
    }
    
}

int solveCSV(std::vector<std::vector<std::string>> &table, size_t x1, size_t x2)
{
    std::string column;
    std::string row;
    char op = ' ';
    int first = 0;

    size_t i = 1;
    size_t j = 1;
    int result = 0;
    int second = 0;

    //first cell
    if (table[x1][x2][i] == '/') {
        throw std::invalid_argument("Cyclic dependency");
    }
    while ((int)table[x1][x2][i] > 64) {
        column += table[x1][x2][i];
        i++;
    }
    while ((int)table[x1][x2][i] > 47 && (int)table[x1][x2][i] < 58) {
        row += table[x1][x2][i];
        i++;
    }
    // Covering cell with / to check Cyclic dependency
    while(j < i) {
        table[x1][x2][j] = '/';
        j++;
    }
    if (column == "") {
        first = std::stoi(row);
    } else 
        first = getIntFromTable(table, column, row);

    //operator
    op = table[x1][x2][i];
    i++;
    j = i;
    //second cell
    column = "";
    row = "";
    if (table[x1][x2][i] == '/') {
        throw std::invalid_argument("Cyclic dependency");
    }
    while ((int)table[x1][x2][i] > 64) {
        column += table[x1][x2][i];
        i++;
    }
    while ((int)table[x1][x2][i] > 47 && (int)table[x1][x2][i] < 58) {
        row += table[x1][x2][i];
        i++;
    }
    while (j < i) {
        table[x1][x2][j] = '/';
        j++;
    }
    if (column == "") {
        second = std::stoi(row);
    }
    else
        second = getIntFromTable(table, column, row);
    switch (op)
    {
    case '+':
        result = first + second;
        break;
    case '-':
        result = first - second;
        break;
    case '/':
        second = second;
        if (second == 0) {
            std::cout << "Division by ";
            result = 0;
        }
        else {
            result = first / second;
        }
        break;
    case '*':
        result = first * second;
        break;
    default:
        throw std::invalid_argument("Unknown operator");
        break;
    }
   table[x1][x2] = std::to_string(result);
    return result;
}

void printSolvedCSV(std::vector<std::vector<std::string>> &table)
{
    for (size_t j = 0; j < table[0].size() - 1; j++) {
        std::cout << table[0][j] << ",";
    }
    std::cout << table[0][table[0].size() - 1];
    std::cout << '\n';
    for (size_t i = 1; i < table.size(); i++) {
        for (size_t j = 0; j < table[0].size()-1; j++) {
            if (table[i][j][0] == '=') {
                std::cout << solveCSV(table, i, j) << ",";
            }
            else
                std::cout << table[i][j] << ",";
        }
        if (table[i][table[0].size() - 1][0] == '=') {
            std::cout << solveCSV(table, i, table[0].size() - 1);
        } else 
            std::cout << table[i][table[0].size()-1];
        std::cout << '\n';
    }
}

int main(int argc, char *argv[])
{
    std::string filename;
    std::vector<std::vector<std::string>> table;

    if (argc > 1) {
        filename = argv[1];
    }
    else {
        std::cout << "Please, enter csv file name: ";
        std::cin >> filename;
    }


    std::ifstream file;
    file.open(filename);
    if (file) {
        try {
            table = readCSV(file);
        } 
        catch (const std::invalid_argument& e) {
            std::cout << e.what();
            return -1;
        }
        try {
            printSolvedCSV(table);
        }
        catch (const std::invalid_argument& e) {
            std::cout << e.what();
            return -1;
        }
    }
    else {
        std::cout << "File doesn't exist";
    }
    

    return 0;
}
