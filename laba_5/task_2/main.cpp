#include <iostream>
#include <vector>
#include <cstddef>
#include <string>
#include <fstream>
#include <linux/limits.h>

bool is_same_file(const char *a, const char *b){
    bool is_same = true;
    char input_path[PATH_MAX];
    char output_path[PATH_MAX];
    realpath(a, input_path);
    realpath(b, output_path);
    int is_not_same = 0;
    
    for (int i = 0; (input_path[i] && output_path[i]); i++) {       
        if (input_path[i] != output_path[i]) is_not_same = 1;
    }   
    if (!is_not_same) is_same = false;

    return is_same;
}

class encoder{
private:
    std::vector<std::byte> key;

    std::vector<std::byte> KSA(std::vector<std::byte> key){
        std::vector<std::byte> S(256);
        for (int i = 0; i < 256; i++) {
            S[i] = std::byte(i);
        }
        //key scheduling algorithm
        int j = 0;
        for (int i = 0; i < 256; i++) {
            j = (j + std::to_integer<int>(S[i]) + std::to_integer<int>(key[i % key.size()])) % 256;
            std::swap(S[i], S[j]);
        }

        return S;
    }

    std::byte keyItem(std::vector<std::byte> &S, int &x, int &y){
        x = (x + 1) % 256;
        y = (y + std::to_integer<int>(S[x])) % 256;

        std::swap(S[x], S[y]);

        return S[(std::to_integer<int>(S[x]) + std::to_integer<int>(S[y])) % 256];
    }
public:
    // encoder(vector<byte> key) : key(key) {}

    encoder(std::vector<std::byte>& key){
        this->key = key;
    }
    
    void encode(const std::string& filename_in, const std::string& filename_out) {
        if(!is_same_file(filename_in.c_str(), filename_out.c_str())) {
            throw std::runtime_error("Same file passed twice to func.");
        }

        std::ifstream input(filename_in, std::ios::binary);
        if (!input) {
            throw std::runtime_error("Failed to open input file.");
        }
        std::ofstream output(filename_out, std::ios::binary);
        if (!output) {
            throw std::runtime_error("Failed to open output file.");
        }

        int x = 0;
        int y = 0;
        std::vector<std::byte> S = KSA(key);

        char ch;
        while (input.get(ch)) {
            ch = std::to_integer<int>(std::byte(ch) ^ keyItem(S, x, y));           
            output.put(ch);
        }
    
        input.close();
        output.close();
    }

    void setKey(std::vector<std::byte> new_key){
        key = new_key;
    }
};



int main(){
    try{
        std::vector<std::byte> key = {std::byte('A'), std::byte('B'), std::byte('C')};
        encoder enc(key);

        enc.encode("input.txt", "encrypted1.txt");
        enc.encode("encrypted1.txt", "../task_2/./decrypted1.txt");
        std::cout << "Encryption and decryption 1 completed successfully." << std::endl;

        std::vector<std::byte> key2 = {std::byte('G'), std::byte('O'), std::byte('O'), std::byte('O'), std::byte('O'), std::byte('O'), std::byte('L')};
        enc.setKey(key2);
        enc.encode("input.txt", "encrypted2.txt");
        enc.encode("encrypted2.txt", "decrypted2.txt");
        std::cout << "Encryption and decryption 2 completed successfully." << std::endl;
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}