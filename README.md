# X-tree
This data structure was made for the Advanced Data Structures course at
Universidad Católica San Pablo.

# Dataset
The dataset used for this project can be downloaded from [here](https://drive.google.com/drive/folders/1u7fEmyFPxSD19C9_MRRJr9p640Oa3Gbk?usp=sharing).
It is a `.csv` file containing a bit over 170k songs from Spotify, classified by
their different attributes.

Once the dataset is downloaded, it should be placed inside a folder named `data`
at the root of the project.

```
project_root
├── d  cmake
├── f  CMakeLists.googletest.txt.in
├── f  CMakeLists.txt
├── d  data
│  └── f  data.csv        <-------- HERE
├── f  LICENSE
├── f  README.md
├── d  src
└── d  tests
```

# Compiling
The project is made using solely the standard library (STL). Thus, you can compile
this project very easily. Make use of the `CMakeLists.txt` file to create your
build folder. If you are on Windows or use an IDE, you should do your research
to find out how to do it for your environment. If you are on Linux or Mac, you
can run the following commands from the root of the project to build and run
the project:

```bash
cmake -B build/ -S .
cd build/
make
make test
./x_tree
```

**Note:** From what I've tested, compiling the project using `clang++` over `g++`
offers better overall performance (almost double). If you want to use `clang++`,
run the command `export CXX=/path/to/clang++` before running the above commands.
If you are on Linux, the `/path/to/clang++` should be `/usr/bin/clang++`.

# Usage
Once you run the project, it will start loading the data file and proceed to
index all the data points.
![Banner and loading screen](https://cdn.discordapp.com/attachments/790070167488561153/790070222715224064/2020-12-19-23-16-00-scrot-screenshot.png)

Once the data is loaded, you will be prompted to start making kNN queries. You
will be asked for a k, number of Nearest Neighbors to be retrieved, and then
the query point from which start the search. You will need to enter the values
in the following order of attributes: `valence, acousticness, daceability,
   duration_ms, energy, explicit, instrumentalness, key, liveness, loudness,
   mode, popularity, speechiness, tempo`.
![Query prompt and results](https://cdn.discordapp.com/attachments/790070167488561153/790071518062837780/2020-12-19-23-21-59-scrot-screenshot.png)
