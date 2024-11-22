#include <iostream>
#include <vector>
#include <cstddef>
#include <string>
#include <fstream>

using namespace std;

class encoder{
private:
    vector<byte> key;

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

        return S;
    }

    byte keyItem(vector<byte> &S, int &x, int &y){
        x = (x + 1) % 256;
        y = (y + to_integer<int>(S[x])) % 256;

        swap(S[x], S[y]);

        return S[(to_integer<int>(S[x]) + to_integer<int>(S[y])) % 256];
    }
public:
    // encoder(vector<byte> key) : key(key) {}

    encoder(vector<byte> key){
        this->key = key;
    }
    
    void encode(const string& filename_in, const string& filename_out) {
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
    
        input.close();
        output.close();
    }

    void setKey(vector<byte> new_key){
        key = new_key;
    }
};



int main(){
    try{
        vector<byte> key = {byte('A'), byte('B'), byte('C')};
        encoder enc(key);

        enc.encode("input.txt", "encrypted1.txt");
        enc.encode("encrypted1.txt", "decrypted1.txt");
        cout << "Encryption and decryption 1 completed successfully." << endl;

        vector<byte> key2 = {byte('G'), byte('O'), byte('O'), byte('O'), byte('O'), byte('O'), byte('L')};
        enc.setKey(key2);
        enc.encode("input.txt", "encrypted2.txt");
        enc.encode("encrypted2.txt", "decrypted2.txt");
        cout << "Encryption and decryption 2 completed successfully." << endl;
    }
    catch(const exception& e){
        std::cerr << e.what() << '\n';
    }

    return 0;
}