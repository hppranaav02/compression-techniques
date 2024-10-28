#include<iostream>
#include<fstream>
#include<vector>

using namespace std;

// Structure for command line arguments
struct input_params {
    string action;
    string technique;
    string data_type;
    string file_path;
};

// Read Values from CSV
vector<string> readCSV(string filePath) {
    vector<string> data;
    ifstream file(filePath);
    if (!file) {
        cerr << "Error opening file: " << filePath << endl;
        return data;
    }

    string line;
    while (getline(file, line)) {
        data.push_back(line);
    }

    file.close();
    if (!file.good() && !file.eof()) {
        cerr << "Error occurred at reading time!" << endl;
    }

    return data;
}

/**
 * Encoding - Binary(bin)
 */
template<typename T> void encode_bin(vector<T> data, string filePath) {
    ofstream fout(filePath + ".bin", ios::out | ios::binary);
    if (!fout) {
        cerr << "Error opening file for writing: " << filePath << ".bin" << endl;
        return;
    }

    for (const auto& item : data) {
        fout.write(reinterpret_cast<const char*>(&item), sizeof(T));
    }

    fout.close();
    if (!fout.good()) {
        cerr << "Error occurred at writing time!" << endl;
    }
}

/**
 * Decoding - Binary(bin)
 * 
 * Added enable condition to escape compile issues for string data in template conditions (SFINAE).
 */
template<typename T, typename enable_if<is_arithmetic<T>::value,int>::type=0>
void decode_bin(string filePath) {
    ifstream fin(filePath + ".bin", ios::in | ios::binary);
    if (!fin) {
        cerr << "Error opening file for reading: " << filePath << ".bin" << endl;
        return;
    }

    T item;

    // Issues during decoding int8_t values due to collision with character encoding into ASCII.
    if (is_same<T,int8_t>::value) {
        while (fin.read(reinterpret_cast<char*>(&item), sizeof(T))) {
            cout << (int)item << '\n';
        }
    } else {
        while (fin.read(reinterpret_cast<char*>(&item), sizeof(T))) {
            cout << item << '\n';
        }       
    }

    fin.close();

    if (!fin.good() && !fin.eof()) {
        cerr << "Error occurred at reading time!" << endl;
    }
}


template<typename T> void performEncoding(vector<T> data, input_params params) {
    if(params.technique == "bin") {
        encode_bin<T>(data, params.file_path);
    }
}

void performDecoding(input_params params) {
    if(params.technique == "bin" && params.data_type != "string") {
        if(params.data_type == "int8")
            decode_bin<int8_t>(params.file_path);
        else if(params.data_type == "int16")
            decode_bin<int16_t>(params.file_path);
        else if(params.data_type == "int32")
            decode_bin<int32_t>(params.file_path);
        else if(params.data_type == "int64")
            decode_bin<int64_t>(params.file_path);
    }
}


// Get data into desired type
template<typename T> vector<T> parseData(vector<string> data) {
    vector<T> newData;
    for(string value : data) {
        if constexpr (is_same<T, int8_t>::value || is_same<T, int16_t>::value || is_same<T, int32_t>::value || is_same<T, int64_t>::value) {
            newData.push_back(static_cast<T>(stoll(value)));
        } else if constexpr (is_same<T, string>::value) {
            newData.push_back(value);
        } else {
            cerr << "Unsupported data type" << endl;
            exit(1);
        }
    }
    return newData;
}

// Program runner
int main(int argc, char* argv[]) {

    if( argc != 5) {
        for (int i = 1; i <= argc; ++i) {
            cout << argv[i];
        }
        cerr << "Number of arguments are wrong, 4 required\n";
        return 1;
    }

    // Load data into struct
    struct input_params params;
    params.action = argv[1];
    params.technique = argv[2];
    params.data_type = argv[3];
    params.file_path = argv[4];
    vector<string> data;

    // Read from file
    if(params.action == "en") {
        data = readCSV(params.file_path);
        if(params.data_type == "int8") {
            vector<int8_t> data_8 = parseData<int8_t>(data);
            performEncoding<int8_t>(data_8, params);
        } else if(params.data_type == "int16") {
            vector<int16_t> data_16 = parseData<int16_t>(data);
            performEncoding<int16_t>(data_16, params);
        } else if(params.data_type == "int32") {
            vector<int32_t> data_32 = parseData<int32_t>(data);
            performEncoding<int32_t>(data_32, params);
        } else if(params.data_type == "int64") {
            vector<int64_t> data_64 = parseData<int64_t>(data);
            performEncoding<int64_t>(data_64, params);
        } else if(params.data_type == "string") {
            performEncoding<string>(data, params);
        }
    } else if (params.action == "de")
        performDecoding(params);

    return 0;
}

