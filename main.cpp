#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

const std::vector<std::string> ep = {
    "class", "namespace", "template", "new", "delete", 
    "virtual", "try", "catch", "throw", "bool", "true", 
    "false", "friend", "public", "protected", "private", "this",
    "export", "import", "typename", "constexpr", "mutable",
    "static_cast", "dynamic_cast", "const_cast", "reinterpret_cast"
};

#define V "1.0"

namespace fs = std::filesystem;

inline void displayHelp() {
    puts("Usage: c2cpp <file.c> [-help] [-h] [-version] [-v]");
    puts("Converts C code to most C++ standards.\n");
    puts("Options:");
    puts("  -help || -h     Display the help message.");
    puts("  -version || -v  Display the c2cpp version.");
}

inline bool shouldReplace(char ch, char next_ch, char prev_ch, bool& isInQuotes, bool& isInSingleQuotes, bool& isInComment, bool& isInMultilineComment) {
    if (ch == '"' && !isInSingleQuotes && !isInComment && prev_ch != '\\') {
        isInQuotes = !isInQuotes;
    }
    if (ch == '\'' && !isInQuotes && !isInComment && prev_ch != '\\') {
        isInSingleQuotes = !isInSingleQuotes;
    }
    if (ch == '/' && next_ch == '*' && !isInQuotes && !isInSingleQuotes && !isInComment) {
        isInMultilineComment = true;
    }
    if (ch == '*' && next_ch == '/' && isInMultilineComment && !isInQuotes && !isInSingleQuotes) {
        isInMultilineComment = false;
    }
    if (ch == '/' && next_ch == '/' && !isInQuotes && !isInSingleQuotes && !isInMultilineComment) {
        isInComment = true;
    }
    if (ch == '\n' && isInComment) {
        isInComment = false;
    }

    return !isInQuotes && !isInSingleQuotes && !isInComment && !isInMultilineComment;
}

inline void replaceInFile(const std::string& srcFilePath, const std::string& destFilePath) {
    std::ifstream inFile(srcFilePath);
    if (!inFile) {
        printf("Error: Could not open file %s\n", srcFilePath.c_str());
        std::exit(1);
    }

    std::string content((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    bool isInQuotes = false;
    bool isInSingleQuotes = false;
    bool isInComment = false;
    bool isInMultilineComment = false;

    for (size_t i = 0; i < content.length(); ++i) {
        if (shouldReplace(content[i], content[i + 1], (i > 0 ? content[i - 1] : '\0'), isInQuotes, isInSingleQuotes, isInComment, isInMultilineComment)) {
            if (content.compare(i, 4, "auto") == 0) {
                content.erase(i, 4);
            }
        }
    }

    for (const auto& element : ep) {
        const std::string replacement = "__" + element;
        size_t pos = 0;
        while ((pos = content.find(element, pos)) != std::string::npos) {
            if (shouldReplace(content[pos], content[pos + element.length()], (pos > 0 ? content[pos - 1] : '\0'), isInQuotes, isInSingleQuotes, isInComment, isInMultilineComment)) {
                content.replace(pos, element.length(), replacement);
                pos += replacement.length(); // Move past the replaced part
            } else {
                pos += element.length();
            }
        }
    }

    std::ofstream outFile(destFilePath);
    if (!outFile) {
        printf("Error: Could not write to file %s\n", destFilePath.c_str());
        std::exit(1);
    }
    outFile << content;
}

inline void renameFile(const std::string& fileName) {
    fs::path filePath(fileName);

    if (!fs::exists(filePath) || filePath.extension() != ".c") {
        puts("Error: File does not exist or is not a .c file.");
        std::exit(1);
    }

    fs::path newFilePath = filePath;
    newFilePath.replace_extension(".cpp");

    replaceInFile(filePath.string(), newFilePath.string());
}

int main(const int argc, const char* argv[]) {
    if (argc < 2) {
        puts("Error: No file specified.");
        displayHelp();
        return 1;
    }

    const std::string arg1 = argv[1];

    if (arg1 == "-help" || arg1 == "-h") {
        displayHelp();
    } else if (arg1 == "-version" || arg1 == "-v") {
        puts("c2cpp version 1.0");
    } else {
        renameFile(arg1);
    }

    return 0;
}