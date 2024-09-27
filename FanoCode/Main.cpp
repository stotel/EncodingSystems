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
    return (a.second.length() < b.second.length());
}

void calculateCodes(vector<pair<string, int>> v, map<char, string> &m, double num)
{  
    if (v.size() == 1) 
    {
        return;
    }
    cout << num <<endl;
    for (const auto& p : v) {
        std::cout << "{" << p.first << ", " << p.second << "}" << std::endl;
    }
    double prevsum = 0;
    double cursum = 0;
    int i;
    for (i = 0; i < v.size(); i++)
    {
        int n = v[i].second;
        prevsum = cursum;
        cursum += n;
        if (cursum >= num / 2) 
        {
            break;
        }
    }
    double a = cursum - num / 2;
    double b = prevsum - num / 2;
    if (a + b >= 0) 
    {
        i--;
        cursum = prevsum;
    }
    for (int j = 0; j < i+1; j++) 
    {
        m[v[j].first[0]] += '0';
    }
    for (int j = i+1; j < v.size(); j++)
    {
        m[v[j].first[0]] += '1';
    }
    cout << i <<endl;
    cout << v[i].first[0] << endl;
    cout << "------------------------" << endl;
    vector<pair<string, int>> v1(v.begin(), v.begin() + i+1);
    calculateCodes(v1, m, cursum);
    vector<pair<string, int>> v2(v.begin() + i+1, v.begin() + v.size());
    calculateCodes(v2, m, num-cursum);
    cout << "b";
}

void encode() {
    map<string, unsigned int> frequency;
    pair<char*, int> data = getCharacters("D:/Projects/Cpp/EncodingSystems/FanoCode/source.txt");
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
    cout << "___________________________" << endl;
    sort(frequencyVec.begin(), frequencyVec.end(), sortBySecStringInt);
    calculateCodes(frequencyVec,alphabet,data.second);
    //#####serializing alphabet
    string textAlphabet="";
    vector<pair<char, string> > A;
    for (auto& it : alphabet) {
        //reverse(it.second.begin(), it.second.end());
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
    ofstream dict("D:/Projects/Cpp/EncodingSystems/FanoCode/dict.txt");
    dict << raw.length() << endl;
    dict << textAlphabet;
    dict.close();
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
    ofstream encodedf("D:/Projects/Cpp/EncodingSystems/FanoCode/encoded.bin", std::ios::binary);
    encodedf.write(encoded.c_str(), encoded.size());
    encodedf.close();
    //#####serializing encoded
}

void decode() {
    ifstream encodedf("D:/Projects/Cpp/EncodingSystems/FanoCode/encoded.bin", std::ios::binary);
    string encoded((std::istreambuf_iterator<char>(encodedf)),std::istreambuf_iterator<char>());
    encodedf.close();

    string line = "";
    map<char, string> alphabet;
    int length;
    ifstream alphabetf("D:/Projects/Cpp/EncodingSystems/FanoCode/dict.txt");
    if (getline(alphabetf, line)) {
        length = stoi(line); // Convert first line to int
    }
    while (getline(alphabetf, line)) 
    {
        alphabet.insert({line[0],line.substr(2)});
    }
    alphabetf.close();

    string raw = "";
    for (char c : encoded)
    {
        raw += std::bitset<8>(c).to_string();
    }

    //#####serializing encoded
    ofstream decodedf("D:/Projects/Cpp/EncodingSystems/FanoCode/decoded.txt", std::ios::binary);
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