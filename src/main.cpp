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

#define FILENAME "../data/data.csv"

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
  // duration_ms, energy, explicit;
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

  // Normalize values
  song.attributes[2].begin() = song.attributes[2].end() =
                                 song.attributes[2].begin() / 6.f;
  song.attributes[3].begin() = song.attributes[3].end() =
                                 song.attributes[3].begin() / 3.49977e+06;
  song.attributes[5].begin() = song.attributes[5].end() =
                                 song.attributes[5].begin() / 106.609f;
  song.attributes[6].begin() = song.attributes[6].end() =
                                 song.attributes[6].begin() / 1942.f;
  song.attributes[7].begin() = song.attributes[7].end() =
                                 song.attributes[7].begin() / 11.f;
  song.attributes[9].begin() = song.attributes[9].end() =
                                 song.attributes[9].begin() / 184442.f;
  song.attributes[11].begin() = song.attributes[11].end() =
                                  song.attributes[11].begin() / 76.f;
  song.attributes[13].begin() = song.attributes[13].end() =
                                  song.attributes[13].begin() / 221.741f;
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

XTree<14, SongHeader, 5> G_DS;
std::vector<Song> data;

int build_data_structure() {
  for (size_t i = 0; i < data.size(); ++i)
    G_DS.insert(data[i].attributes, data[i].getHeader());

  return 0;
}

std::vector<std::pair<Hyperrectangle<14>, const SongHeader*>>& query_knn(
      std::vector<float> query, int k) {
  Hyperrectangle<14> point;

  for (int i = 0; i < 14; ++i)
    point[i].begin() = query[i];

  return G_DS.kNN(point, k);
}

void printBanner() {
  std::cout << "\n";
  std::cout << "\t██╗  ██╗  ████████╗██████╗ ███████╗███████╗\n";
  std::cout << "\t╚██╗██╔╝  ╚══██╔══╝██╔══██╗██╔════╝██╔════╝\t Copyright 2020\n";
  std::cout << "\t ╚███╔╝█████╗██║   ██████╔╝█████╗  █████╗  \t By Mateo Gonzales Navarrete\n";
  std::cout << "\t ██╔██╗╚════╝██║   ██╔══██╗██╔══╝  ██╔══╝  \t Github: mgonnav\n";
  std::cout << "\t██╔╝ ██╗     ██║   ██║  ██║███████╗███████╗\t Website: www.mgonnav.com\n";
  std::cout << "\t╚═╝  ╚═╝     ╚═╝   ╚═╝  ╚═╝╚══════╝╚══════╝\n" << std::endl;
}

template <size_t N>
float euclidean_dist(const std::vector<float>& p1,
                     const Hyperrectangle<N>& p2) {
  float dist = 0;
  float tmp_dist;

  for (size_t i = 0; i < N; ++i) {
    tmp_dist = p1[i] - p2[i].begin();
    dist += tmp_dist*tmp_dist;
  }

  return dist;
}

void normalize_query(std::vector<float>& query) {
  query[2] /= 6.f;
  query[3] /= 3.49977e+06;
  query[5] /= 106.609f;
  query[6] /= 1942.f;
  query[7] /= 11.f;
  query[9] /= 184442.f;
  query[11] /= 76.f;
  query[13] /= 221.741f;
}

void denormalize_result(
  std::vector<std::pair<Hyperrectangle<14>, const SongHeader*>>& result,
  size_t idx) {
  (result[idx].first)[2].begin()  *= 6.f;
  (result[idx].first)[3].begin()  *= 3.49977e+06;
  (result[idx].first)[5].begin()  *= 106.609f;
  (result[idx].first)[6].begin()  *= 1942.f;
  (result[idx].first)[7].begin()  *= 11.f;
  (result[idx].first)[9].begin()  *= 184442.f;
  (result[idx].first)[11].begin() *= 76.f;
  (result[idx].first)[13].begin() *= 221.741f;
}

int main() {
  printBanner();

  readFromFile(FILENAME, data);

  std::cout << "[[ Indexing data... ]]\n";
  Timer<int()> timed_built(build_data_structure, "Index");
  timed_built();
  std::cout << "\n";

  Timer<std::vector<std::pair<Hyperrectangle<14>, const SongHeader*>>&(std::vector<float>, int)>
      timed_query(query_knn, "Query kNN");

  std::vector<float> query(14);
  int k;
  std::vector<std::pair<Hyperrectangle<14>, const SongHeader*>> result;
  std::string delim;

  while (true) {
    std::cout << "[[ Query k Nearest Neighbors (kNN) ]]\n";
    std::cout << ">> Number of Nearest Neighbors: ";
    std::cin >> k;
    std::cout << ">> Insert query point values: ";

    for (int i = 0; i < 14; ++i)
      std::cin >> query[i];

    normalize_query(query);
    result = timed_query(query, k);
    std::cout <<
              "--------------------------------------------------------------------------------\n";

    for (int i = k - 1; i >= 0; --i) {
      std::cout << "| #" << k - i << " " << result[i].second->name << "\n";
      std::cout << "|\tDistance: " << euclidean_dist<14>(query,
                result[i].first) << "\n";
      denormalize_result(result, i);
      delim = "";
      std::cout << "|\tValues: {";

      for (int j = 0; j < 14; ++j) {
        std::cout << delim << (result[i].first)[j].begin();
        delim = ", ";
      }

      std::cout << "}\n";
      std::cout <<
                "--------------------------------------------------------------------------------\n";
    }

    std::cout << std::endl;
  }
}
