// Copyright 2020 Roger Peralta Aranibar Advanced Data Structures

#include "XTree.hpp"

using namespace std::chrono;

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

struct Query{
    char type;
    Hyperrectangle<DIM> p;
    float info;
};

vector<Query> parseQueryFile(string fileName) {
    vector<Query> queryArray;
    string line;
    ifstream file(fileName);
    if (file.is_open()) {
        while(getline(file, line)) {
            struct Query q;
            stringstream lineStream(line);
            string cell;

            getline(lineStream, cell, ' ');
            q.type = cell[0];

            if (q.type == 'l') {
                queryArray.emplace_back(q);
                continue;
            }

            vector<float> tmp;
            while(getline(lineStream, cell, ' '))
                tmp.push_back(atof(cell.c_str()));

            q.info = tmp.back();
            tmp.pop_back();

            if (q.type == 'r'){
                for (int i = 0; i < tmp.size() / 2 ; i++){
                    q.p[i].begin() = tmp[i];
                    q.p[i].end() = tmp[tmp.size() / 2 + i];
                }
            }
            else {
                for (int i = 0; i < tmp.size(); i++)
                    q.p[i].begin() = q.p[i].end() = tmp[i];
            }
            queryArray.push_back(q);
        }
        file.close();
    } else cout << "File not found: " << fileName << endl;
    cout << "Number of queries " << queryArray.size() << endl;
    return queryArray;
}

XTree<DIM, string, CAPACITY> xtree;

void buildIndex(vector<Record> data) {
    for (size_t i = 0; i < data.size(); ++i)
        xtree.insert(data[i].p, data[i].id);
}

vector<string> sequentialScanKNN(vector<Record> &dataArray, Hyperrectangle<DIM> q, int k){
    priority_queue<pair<float, string>> result;
    for (auto d: dataArray){
        float dist = objectDist(d.p, q);
        if (result.size() < k)
            result.push(make_pair(dist, d.id));
        else if (dist < result.top().first) {
            result.pop();
            result.push(make_pair(dist, d.id));
        }
    }
    vector<string> res;
    while (!result.empty()) {
        res.push_back(result.top().second);
        result.pop();
    }
    return res;
}

vector<string> sequentialScanRange(vector<Record> &dataArray, Hyperrectangle<DIM> qr){
    vector<string> result;
    for (auto d: dataArray){
        if (contains(d.p , qr))
            result.push_back(d.id);
    }
    return result;
}



int main(int argc, char **argv) {
    if (argc != 4){
        cout << "Usage: ./xtree dataFile limit queryFile" << endl;
        exit(1);
    }

    int limit = atoi(argv[2]);

    vector<Record> dataArray = parseDataFile(argv[1], limit);
    high_resolution_clock::time_point startTime = high_resolution_clock::now();
    buildIndex(dataArray);
    double creationTime = duration_cast<microseconds>(high_resolution_clock::now() - startTime).count();
    cout << "Index creation time: " << creationTime << endl;
    xtree.getStats();

    vector<Query> queryArray = parseQueryFile(argv[3]);

    map<string, double> rangeLog, knnLog, inLog;
    for (auto q: queryArray) {
        if (q.type == 'r') {
            float rs = q.info;
            startTime = high_resolution_clock::now();
            vector<string> results = xtree.rangeQuery(q.p);
            rangeLog["time " + to_string(rs)] += duration_cast<microseconds>(high_resolution_clock::now() - startTime).count();
            rangeLog["count " + to_string(rs)]++;
            //rangeLog["leaf " + to_string(rs)] += xtree.queryLeafCount;
            startTime = high_resolution_clock::now();
            vector<string> resS = sequentialScanRange(dataArray, q.p);
            rangeLog["seq " + to_string(rs)] += duration_cast<microseconds>(high_resolution_clock::now() - startTime).count();
            //cout << "range:";
            cout << results.size() << "-" << resS.size() << endl;
        }
        if (q.type == 'k') {/*
            int k = q.info;
            //high_resolution_clock::time_point startTime = high_resolution_clock::now();
            vector<string> results = xtree.kNN(q.p, k);
            //knnLog["time " + to_string(k)] += duration_cast<microseconds>(high_resolution_clock::now() - startTime).count();
            knnLog["count " + to_string(k)]++;
            //knnLog["leaf " + to_string(k)] += xtree.queryLeafCount;
            //startTime = high_resolution_clock::now();
            vector<string> resS = sequentialScanKNN(dataArray, q.p, k);
            //knnLog["seq " + to_string(k)] += duration_cast<microseconds>(high_resolution_clock::now() - startTime).count();
            sort(results.begin(), results.end());
            sort(resS.begin(), resS.end());
            cout << "knn: " << endl;
            for (int i = 0; i < k; i++) cout << results[i] << " --- " << resS[i] << endl;*/
        } else if (q.type == 'l') {
            cout << "---Range---" << endl;
            for (auto it = rangeLog.begin(); it != rangeLog.end(); ++it) {
                cout << it->first << ": " << it->second << endl;
                it->second = 0;
            }
            cout << "---KNN---" << endl;
            for (auto it = knnLog.begin(); it != knnLog.end(); ++it) {
                cout << it->first << ": " << it->second << endl;
                it->second = 0;
            }
            xtree.getStats();
        }
    }
}
