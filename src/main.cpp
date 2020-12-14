#include <iostream>
#include <fstream>
#include <sstream>
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

  std::array<float, 14> attributes;
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
  is >> song.attributes[0]  >> song.year           >> song.attributes[1]
     >> song.attributes[2]  >> song.attributes[3]
     >> song.attributes[4]  >> song.attributes[5]  >> song.id
     >> song.attributes[6]  >> song.attributes[7]  >> song.attributes[8]
     >> song.attributes[9]  >> song.attributes[10]
     >> song.attributes[11] >> song.release_date   >> song.attributes[12]
     >> song.attributes[13];

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

int main() {
  std::vector<Song> data;
  readFromFile(FILENAME, data);

  XTree<14, SongHeader, 10> xtree;

  Hyperrectangle<14> point;

  for (size_t i = 0; i < data.size(); ++i) {
    for (size_t j = 0; j < 14; ++j)
      point[j] = Interval(data[i].attributes[j], data[i].attributes[j]);

    xtree.insert(point, data[i].getHeader());
    std::cout << "Inserted " << i << " regs.\n";
  }
}
