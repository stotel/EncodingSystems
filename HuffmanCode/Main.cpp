#include<fstream>
#include<vector>
#include<iostream>
#include<string>
#include<map>
#include<algorithm>
#include<bitset>

using namespace std;

pair<char*,int> getCharacters(string name)
{
    ifstream file(name, ios::binary | ios::in);
    std::streampos fsize;
    file.seekg(0, std::ios::end);
    fsize = file.tellg();
    file.seekg(0, std::ios::beg);
    char* buffer = new char[fsize];
    file.read(buffer, fsize);
    file.close();
    return { buffer,fsize };
}

bool sortBySecStringInt(const pair<string, int>& a, const pair<string, int>& b)
{
    return (a.second > b.second);
}

bool sortBySecCharString(pair<char, string>& a, pair<char, string>& b)
{
    //stoi(str, nullptr, 2)
    return (a.second.length() < b.second.length());
    //return (stoi(a.second, nullptr, 2) < stoi(b.second, nullptr, 2));
}

void encode() {
    map<string, unsigned int> frequency;
    pair<char*, int> data = getCharacters("D:/Projects/Cpp/EncodingSystems/HuffmanCode/source.txt");
    char* source = data.first;

    map<char, string> alphabet;

    for (int i = 0; i < data.second; i++) 
    {
        char c = source[i];
        if (c == '\n')
        {
            c = 235;
        }
        if (frequency.find(string{ c }) == frequency.end())
        {
            frequency.insert({ string{ c }, 1 });
            alphabet.insert({ c, ""});
        }
        else
        {
            frequency[string{ c }]++;
        }
    }

    vector<pair<string, int> > frequencyVec;
    copy(frequency.begin(), frequency.end(), back_inserter(frequencyVec));

    //for (map<string, unsigned int>::const_iterator it = frequency.begin(); it != frequency.end(); ++it) {
    //    cout << it->first << ' ' << it->second << endl;
    //}
    //cout << "___________________________" << endl;
    sort(frequencyVec.begin(), frequencyVec.end(), sortBySecStringInt);
    //for (int i = 0; i < frequencyVec.size(); i++)
    //{
    //    cout << frequencyVec[i].first << " " << frequencyVec[i].second << endl;
    //}
    cout << "___________________________" << endl;
    sort(frequencyVec.begin(), frequencyVec.end());
    while (frequencyVec.size() > 1) 
    {
        sort(frequencyVec.begin(), frequencyVec.end(), sortBySecStringInt);
        pair<string, int> p1 = frequencyVec[frequencyVec.size() - 1];
        for (int i = 0; i < p1.first.length(); i++) {
            alphabet[p1.first[i]] += '0';
        }
        pair<string, int> p2 = frequencyVec[frequencyVec.size() - 2];
        for (int i = 0; i < p2.first.length(); i++) {
            alphabet[p2.first[i]] += '1';
        }
        frequencyVec.pop_back();
        frequencyVec.pop_back();
        frequencyVec.push_back({ p1.first + p2.first,p1.second + p2.second });
    }
    //#####serializing alphabet
    string textAlphabet="";
    vector<pair<char, string> > A;
    for (auto& it : alphabet) {
        reverse(it.second.begin(), it.second.end());
        A.push_back(it);
    }
    sort(A.begin(), A.end(), sortBySecCharString);
    for (auto& it : A) {
        textAlphabet += it.first;
        textAlphabet += ' ';
        textAlphabet += it.second;
        textAlphabet += "\n";
    }
    string raw = "";
    for (int i = 0; i < data.second; i++) 
    {
        char c = source[i];
        if (c == '\n') {
            raw += alphabet[235];
        }
        else
        {
            raw += alphabet[c];
        }
    }
    //#####serializing alphabet
    ofstream dict("D:/Projects/Cpp/EncodingSystems/HuffmanCode/dict.txt");
    dict << raw.length() << endl;
    dict << textAlphabet;
    dict.close();
    //#####serializing alphabet
    string encoded = "";
    // Process 8 bits at a time
    for (size_t i = 0; i < raw.size(); i += 8) {
        std::string byte = raw.substr(i, 8);  // Take 8 bits
        if (byte.length() < 8)
        {
            while(byte.length() < 8)
            {
                byte += '0';
            }
            char character = static_cast<char>(std::bitset<8>(byte).to_ulong());
            encoded += character;
            break;  // Handle incomplete byte at the end
        }
        char character = static_cast<char>(std::bitset<8>(byte).to_ulong());
        encoded += character;
    }
    //#####serializing encoded
    ofstream encodedf("D:/Projects/Cpp/EncodingSystems/HuffmanCode/encoded.bin", std::ios::binary);
    //encodedf << encoded;
    encodedf.write(encoded.c_str(), encoded.size());
    encodedf.close();
    //#####serializing encoded
}

void decode() {
    ifstream encodedf("D:/Projects/Cpp/EncodingSystems/HuffmanCode/encoded.bin", std::ios::binary);
    string encoded((std::istreambuf_iterator<char>(encodedf)),std::istreambuf_iterator<char>());
    encodedf.close();
    //cout << encoded;

    string line = "";
    map<char, string> alphabet;
    int length;
    ifstream alphabetf("D:/Projects/Cpp/EncodingSystems/HuffmanCode/dict.txt");
    if (getline(alphabetf, line)) {
        length = stoi(line); // Convert first line to int
    }
    while (getline(alphabetf, line)) 
    {
        alphabet.insert({line[0],line.substr(2)});
    }
    alphabetf.close();

    // Process each character
    string raw = "";
    for (char c : encoded)
    {
        // Convert each character to its binary representation using std::bitset
        raw += std::bitset<8>(c).to_string();
    }

    //#####serializing encoded
    ofstream decodedf("D:/Projects/Cpp/EncodingSystems/HuffmanCode/decoded.txt", std::ios::binary);
    string buffer = "";
    for (int i = 0; i < length; i++) {
        buffer += raw[i];
        map<char, string>::iterator it;
        for (it = alphabet.begin(); it != alphabet.end(); it++)
        {
            if (buffer._Equal(it->second)) {
                buffer = "";
                if (it->first == char(235)) {
                    decodedf << endl;
                }
                else
                {
                    decodedf << it->first;
                }
            }
            //std::cout << it->first << ' ' << it->second << '\n';
        }
    }
    decodedf.close();
    //#####serializing encoded
}

int main() {
    encode();
    decode();
    return 0;
}