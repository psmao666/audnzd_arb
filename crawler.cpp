#include <iostream>
#include <cstring>
#include <fstream>

int main() {
    std::ifstream reader("report.txt");
    std::ofstream writer("portfolio.txt");

    // Use std::string instead of char array
    std::string data;

    // Read lines until the end of the file
    while (std::getline(reader, data)) {
        
        if (data[0] == 'T' && data[1] == 'i') {
            // Extract the substring starting from index 16 to the end
            std::string tmp = data.substr(6);

            // Write the result to the output file
            writer << tmp << ' ';
        }
        else if (data[0] == 'C' && data.size() >= 14 && data[8] == 'E') {
            // Extract the substring starting from index 16 to the end
            std::string tmp = data.substr(16);

            // Convert the substring to double
            double result = std::stod(tmp);

            // Write the result to the output file
            writer << result << std::endl;
        }
    }

    // Close the files
    reader.close();
    writer.close();
    return 0;
}