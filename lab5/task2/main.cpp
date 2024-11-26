#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include <filesystem>


class encoder {
private:

    std::vector<std::byte> key;

    void rc4Init(std::vector<std::byte> &s) {
        for (size_t i = 0; i < 256; ++i) {
            s[i] = static_cast<std::byte>(i);
        }

        size_t j = 0;
        for (size_t i = 0; i < 256; ++i) {
            j = (j + static_cast<size_t>(s[i]) + static_cast<size_t>(key[i % key.size()])) % 256;
            std::swap(s[i], s[j]);
        }
    }

    std::vector<std::byte> rc4Process(const std::vector<std::byte> &data) {
        std::vector<std::byte> s(256);
        rc4Init(s);

        size_t i = 0, j = 0;
        std::vector<std::byte> result(data.size());

        for (size_t k = 0; k < data.size(); ++k) {
            i = (i + 1) % 256;
            j = (j + static_cast<size_t>(s[i])) % 256;
            std::swap(s[i], s[j]);

            auto t = static_cast<size_t>(s[(static_cast<size_t>(s[i]) + static_cast<size_t>(s[j])) % 256]);
            result[k] = data[k] ^ static_cast<std::byte>(t);
        }

        return result;
    }

public:

    encoder(const std::vector<std::byte> &encryptionKey) : key(encryptionKey) {
        if (key.empty() || key.size() > 256) {
            throw std::invalid_argument("Invalid key length");
        }
    }


    void setKey(const std::vector<std::byte> &newKey) {
        if (newKey.empty() || newKey.size() > 256) {
            throw std::invalid_argument("Invalid key length");
        }
        key = newKey;
    }

    void encode(const std::filesystem::path &inputFilePath, const std::filesystem::path &outputFilePath) {
        std::filesystem::path absoluteInput = std::filesystem::absolute(inputFilePath);
        std::filesystem::path absoluteOutput = std::filesystem::absolute(outputFilePath);
        if (absoluteInput == absoluteOutput) {
            throw std::runtime_error("Files has same names");
        }
        std::ifstream inputFile(inputFilePath, std::ios::binary);
        if (!inputFile.is_open()) {
            throw std::runtime_error("Failed to open input file");
        }

        std::ofstream outputFile(outputFilePath, std::ios::binary);
        if (!outputFile.is_open()) {
            throw std::runtime_error("Failed to open output file");
        }

        std::vector<std::byte> data;
        char ch;
        while (inputFile.get(ch)) {
            data.push_back(static_cast<std::byte>(ch));
        }


        auto result = rc4Process(data);

        outputFile.write(reinterpret_cast<const char *>(result.data()), result.size());
    }
};

int main() {
    try {

        std::vector<std::byte> key = {std::byte(1), std::byte(2), std::byte(3), std::byte(4)};

        encoder enc(key);

        enc.encode("input.txt", "input.bin");

        enc.encode("input.bin", "decrypted.txt");

        std::cout << "Encrypt and decrypt complete" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
