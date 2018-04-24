#include "lab2_common.h"

  int
main(int argc, char ** argv)
{
  if (argc < 2) {
    cout << "Usage: " << argv[0] << " <value-size>" << endl;
    exit(0);
  }
  // value size is provided in bytes
  const size_t value_size = std::stoull(argv[1], NULL, 10);
  if (value_size < 1 || value_size > 100000) {
    cout << "  <value-size> must be positive and less then 100000" << endl;
    exit(0);
  }

  DB * db = open_leveldb("leveldb_test_dir");
  if (db == NULL) {
    cerr << "Open LevelDB failed!" << endl;
    exit(1);
  }
  char * vbuf = new char[value_size];
  for (size_t i = 0; i < value_size; i++) {
    vbuf[i] = rand();
  }
  string value = string(vbuf, value_size);
  // add by wxf: store keys for part2 test
  vector<string> keys;

  size_t nfill = 1000000000 / (value_size + 8);
  clock_t t0 = clock();
  size_t p1 = nfill / 40;
  for (size_t j = 0; j < nfill; j++) {
    string key = std::to_string(((size_t)rand())*((size_t)rand()));
    // added by wxf:
    keys.push_back(key);
    leveldb_set(db, key, value);
    if (j >= p1) {
      clock_t dt = clock() - t0;
      cout << "progress: " << j+1 << "/" << nfill << " time elapsed: " << dt * 1.0e-6 << endl << std::flush;
      p1 += (nfill / 40);
    }
  }
  // delete db;
  clock_t dt = clock() - t0;
  cout << "time elapsed: " << dt * 1.0e-6 << " seconds" << endl;

  // added by wxf: test part2: get sequential 
  /*
  p1 = nfill/40;
  string val;
  clock_t t2 = clock();
  for(size_t j = 0; j < nfill; j++){
    string key = keys.at(j);
    leveldb_get(db, key, val);
    if (j >= p1) {
      dt = clock() - t2;
      cout << "progress: " << j+1 << "/" << nfill << " time elapsed: " << dt * 1.0e-6 << endl << std::flush;
      p1 += (nfill / 40);
    }
  }
  dt= clock() - t2;
  cout << "time elapsed(get sequencial): " << dt * 1.0e-6 << " seconds" << endl;
  */

  // test part3: random get
  p1 = nfill/40;
  string val;
  clock_t t3 = clock();
  for(size_t j = 0; j < nfill; j++){
    int rand_j = rand() % nfill;
    string key = keys.at(rand_j);
    leveldb_get(db, key, val);
    if (j >= p1) {
      dt = clock() - t3;
      cout << "progress: " << j+1 << "/" << nfill << " time elapsed: " << dt * 1.0e-6 << endl << std::flush;
      p1 += (nfill / 40);
    }
  }
  dt= clock() - t3;
  cout << "time elapsed(get randomly): " << dt * 1.0e-6 << " seconds" << endl;

  // test part4: del seq
  //
  delete db;
  destroy_leveldb("leveldb_test_dir");
  exit(0);
}
