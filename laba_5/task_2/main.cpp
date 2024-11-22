#include <iostream>
#include <vector>
#include <cstddef>
#include <string>
#include <fstream>

using namespace std;

class encoder{
private:
    vector<byte> key;
    // vector<byte> S;
    // int x = 0;
    // int y = 0;

    vector<byte> KSA(vector<byte> key){
        vector<byte> S(256);
        for (int i = 0; i < 256; i++) {
            S[i] = byte(i);
        }
        //key scheduling algorithm
        int j = 0;
        for (int i = 0; i < 256; i++) {
            j = (j + to_integer<int>(S[i]) + to_integer<int>(key[i % key.size()])) % 256;
            swap(S[i], S[j]);
        }

        for (int i = 0; i < 256; ++i) {
            cout << to_integer<int>(S[i]) << " ";
        }
        cout << endl;

        return S;
    }

    byte keyItem(vector<byte> &S, int &x, int &y){
        x = (x + 1) % 256;
        y = (y + to_integer<int>(S[x])) % 256;

        cout << "(" << x << " " << y << ") ";

        swap(S[x], S[y]);

        return S[(to_integer<int>(S[x]) + to_integer<int>(S[y])) % 256];
    }
public:
    // encoder(vector<byte> key) : key(key) {}

    encoder(vector<byte> key){
        this->key = key;
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

        int x = 0;
        int y = 0;
        vector<byte> S = KSA(key);

        char ch;
        while (input.get(ch)) {
            ch = to_integer<int>(byte(ch) ^ keyItem(S, x, y));           
            output.put(ch);
        }
        cout << "\n";
    
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
        cout << "\n";
        enc.encode("encrypted.txt", "decrypted.txt", false);
        cout << "Encryption and decryption completed successfully." << endl;
    }
    catch(const exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}