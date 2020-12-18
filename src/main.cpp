// Copyright 2020 Roger Peralta Aranibar Advanced Data Estructures
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <chrono>

#include "Hyperrectangle.hpp"
#include "XTree.hpp"

#define FILENAME "../data/sample.csv"

struct SongHeader {
  std::string id;
  int year;
  std::string name;
  std::string release_date;

  SongHeader() {}
  SongHeader(std::string id, int year, std::string name,
             std::string release_date) : id(id), year(year), name(name),
    release_date(release_date) {}
};

struct Song {
  std::string id;
  int year;
  std::string name;
  std::string release_date;

  Hyperrectangle<14> attributes;
  // valence, acousticness, danceability;
  // duration_ms, energy, xplicit;
  // instrumentalness, key, liveness;
  // loudness, mode, popularity;
  // speechiness, tempo;

  SongHeader getHeader() {
    SongHeader song_header(id, year, name, release_date);
    return song_header;
  }
};

std::istream& operator>>(std::istream& is, Song& song) {
  is >> song.attributes[0].begin()  >> song.year           >> song.attributes[1].begin()
     >> song.attributes[2].begin()  >> song.attributes[3].begin()
     >> song.attributes[4].begin()  >> song.attributes[5].begin()  >> song.id
     >> song.attributes[6].begin()  >> song.attributes[7].begin()  >> song.attributes[8].begin()
     >> song.attributes[9].begin()  >> song.attributes[10].begin()
     >> song.attributes[11].begin() >> song.release_date   >> song.attributes[12].begin()
     >> song.attributes[13].begin();

  for (int i = 0; i < 14; ++i)
    song.attributes[i].end() = song.attributes[i].begin();

  song.attributes[3].begin() = song.attributes[3].end() = song.attributes[3].begin() / 5.4035e+06;
  song.attributes[7].begin() = song.attributes[7].end() = song.attributes[7].begin() / 11.f;
  song.attributes[9].begin() = song.attributes[9].end() = song.attributes[9].begin() / 3.855f;
  song.attributes[11].begin() = song.attributes[11].end() = song.attributes[11].begin() / 100.f;
  song.attributes[13].begin() = song.attributes[13].end() = song.attributes[13].begin() / 243.507f;

  return is;
}

void readFromFile(std::string filename, std::vector<Song>& data) {
  std::ifstream inFile (filename);

  if (!inFile.is_open()) {
    std::cerr << "Failed to open file " << filename << "\n";
    exit(1);
  }

  std::string value, values, song_name;
  std::stringstream ss;
  getline(inFile, value); // list of columns

  while (!inFile.eof()) {
    values.clear();

    for (int i = 0; i < 17; ++i) {
      getline(inFile, value, ',');
      values += (value + " ");
    }

    getline(inFile, value);
    song_name = value;

    Song song;
    ss.str(values);
    ss >> song;
    song.name = song_name;
    data.push_back(song);
  }
}

template <typename>
class Timer;

template <typename R, typename... T>
class Timer<R(T...)> {
 public:
  typedef R (*function_type)(T...);
  function_type function;

  explicit Timer(function_type function, std::string process_name = "")
    : function_(function), process_name_(process_name) {}

  R operator()(T... args) {
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();

    R result = function_(std::forward<T>(args)...);

    end = std::chrono::high_resolution_clock::now();
    int64_t duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
      .count();

    std::cout << std::setw(10) << process_name_ << std::setw(30)
              << "Duration: " + std::to_string(duration) + " ns\n";
    return result;
  }

 private:
  function_type function_;
  std::string process_name_;
};

XTree<14, SongHeader, 10> G_DS;
std::vector<Song> data;

int build_data_structure() {
  for (size_t i = 0; i < data.size(); ++i) {
    G_DS.insert(data[i].attributes, data[i].getHeader());
  }

  return 0;
}

std::vector<std::pair<const Hyperrectangle<14>*, const SongHeader*>> query_knn(std::vector<float> query,
int k) {
  Hyperrectangle<14> point;

  for (int i = 0; i < 14; ++i)
    point[i].begin() = point[i].end() = query[i];

  return G_DS.kNN(point, k);
}

int main() {
  readFromFile(FILENAME, data);

  Timer<int()> timed_built(build_data_structure, "Index");
  timed_built();

  Timer<std::vector<std::pair<const Hyperrectangle<14>*, const SongHeader*>>(std::vector<float>, int)>
      timed_query(
        query_knn, "Query KNN");
  std::vector<float> query = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  int k = 2;
  std::vector<std::pair<const Hyperrectangle<14>*, const SongHeader*>> result = timed_query(query, k);

  for (int i = 0; i < k; ++i) {
    std::cout << result[i].second->name << "\n";
    for (int j = 0; j < 14; ++j)
      std::cout << (*result[i].first)[j].begin() << " ";
    std::cout << "\n";
  }
}
