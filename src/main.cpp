// Copyright 2020 Roger Peralta Aranibar Advanced Data Structures

#include "XTree.hpp"

struct Record{
    string id;
    Hyperrectangle<DIM> p;
};

vector<Record> parseDataFile(string datafile, int limit) {
    ifstream data(datafile);
    if (!data.is_open()) cout << "Cant open file: " << datafile << endl;

    string line;
    vector<Record> dataArray;

    int count = 0;
    while(getline(data, line)) {
        struct Record r;
        stringstream lineStream(line);
        string cell;

        getline(lineStream, cell, ' ');
        r.id = cell.c_str();

        vector<float> values;
        while(getline(lineStream, cell, ' '))
            values.emplace_back(atof(cell.c_str()));
        if (values.size() != DIM) cerr << "Error with dimensions of data file" << endl;

        for (int i = 0; i < values.size(); i++)
            r.p[i].begin() = r.p[i].end() = values[i];

        dataArray.push_back(r);
        count++;
        if (count == limit) break;
    }
    cout << "Dataset size: " << dataArray.size() << endl;
    return(dataArray);
}

XTree<DIM, string, CAPACITY> xtree;

void buildIndex(vector<Record> data) {
    for (size_t i = 0; i < data.size(); ++i)
        xtree.insert(data[i].p, data[i].id);
}

int main() {
    vector<Record> data =  parseDataFile(FILENAME, LIMIT);
    buildIndex(data);
    xtree.getStats();
}
