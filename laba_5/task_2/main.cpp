#include <iostream>
#include <vector>
#include <cstddef>
#include <string>
#include <fstream>

using namespace std;

class encoder{
private:
    vector<byte> S;
    int x = 0;
    int y = 0;

    // vector<byte> generateKeystream(size_t length) {
    //     vector<byte> S(256);
    //     vector<byte> keystream(length);

    //     for (int i = 0; i < 256; ++i) {
    //         S[i] = static_cast<byte>(i);
    //     }

    //     //key scheduling algorithm
    //     size_t j = 0;
    //     for (size_t i = 0; i < 256; ++i) {
    //         j = (j + static_cast<size_t>(S[i]) + static_cast<size_t>(key[i % key.size()])) % 256;
    //         swap(S[i], S[j]);
    //     }

    //     //PRGA (pseudo random generation algorithm)
    //     size_t i = 0;
    //     j = 0;
    //     for (size_t n = 0; n < length; ++n) {
    //         i = (i + 1) % 256;
    //         j = (j + static_cast<size_t>(S[i])) % 256;
    //         swap(S[i], S[j]);
    //         keystream[n] = S[(static_cast<size_t>(S[i]) + static_cast<size_t>(S[j])) % 256];
    //     }

    //     return keystream;
    // }

    void KSA(vector<byte> key){
        S.resize(256);
        for (int i = 0; i < 256; i++) {
            S[i] = byte(i);
        }
        //key scheduling algorithm
        int j = 0;
        for (int i = 0; i < 256; i++) {
            j = (j + static_cast<int>(S[i]) + static_cast<int>(key[i % key.size()])) % 256;
            swap(S[i], S[j]);
        }
    }

    byte keyItem(){
        x = (x + 1) % 256;
        y = (y + static_cast<int>(S[x])) % 256;

        swap(S[x], S[y]);

        return S[(static_cast<int>(S[x]) + static_cast<int>(S[y])) % 256];
    }
public:
    encoder(vector<byte> key){
        KSA(key);
    }
    
    void encode(const string& filename_in, const string& filename_out, bool flag) {
        ifstream input(filename_in, ios::binary);
        if (!input) {
            throw runtime_error("Failed to open input file.");
        }
        ofstream output(filename_out, ios::binary);
        if (!output) {
            throw runtime_error("Failed to open output file.");
        }

        char ch;
        while (input.get(ch)) {
            ch = static_cast<unsigned char>(ch);
            ch = static_cast<int>(byte(ch) ^ keyItem());           
            output << (static_cast<char>(ch));          
        }

        // char byte; 
        // while (input.get(byte)) {
        //     byte ^= (keyItem());
        //     output.put(byte);                          
        // }

        input.close();
        output.close();
    }

    // void setKey(vector<byte> new_vec){
    //     key = new_vec;
    // }
};



int main(){
    try{
        vector<byte> key = {byte('A'), byte('B'), byte('C')};
        encoder enc(key);

        enc.encode("input.txt", "encrypted.txt", true);
        enc.encode("encrypted.txt", "decrypted.txt", false);
        cout << "Encryption and decryption completed successfully." << endl;
    }
    catch(const exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}