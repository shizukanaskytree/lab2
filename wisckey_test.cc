#include "lab2_common.h"

typedef struct WiscKey {
  string dir;
  DB * leveldb;
  FILE * logfile;
  //TODO: add more as you need
} WK;

// TODO: add more helper functions as you need

  static bool
wisckey_get(WK * wk, string &key, string &value)
{
  //TODO: Your code here
  // 1. go to leveldb get addr of key
  string s_addr;
  leveldb_get(wk->leveldb, key, s_addr);
  int addr = atoi(s_addr.c_str());
  // 2. according to addr, get value from logfile.txt
  int cnt = 0;
  if(wk->logfile != NULL){
    char line[1024];
    while( fgets(line, sizeof(line), wk->logfile) != NULL ){
      // read a line until NO.addr
      if( cnt == addr ){
        // get the value
        std::string val(line);
	value.assign(val);
	return true;
      }else{
        cnt++;
      }
    }
    return false;
  }else{
    printf("wisckey_get:: logfile does not exist! \n");
    return false;
  }
}

  static void
wisckey_set(WK * wk, string &key, string &value)
{
  //TODO: Your code here
  // addr use a number to indicate its address
  // increase it every time when invoking set key
  static int addr = 0;
  // store key and addr
  string s_addr = std::to_string(addr);
  leveldb_set(wk->leveldb, key, s_addr);
  
  // addr += 1;
  // write value into logfile.txt
  int cnt = 0;
  if(wk->logfile != NULL){
    fprintf(wk->logfile, "%s\n", value);
  }else{
    printf("logfile does not exist");
  }

}

  static void
wisckey_del(WK * wk, string &key)
{
  //TODO: Your code here
  // only del <key, addr>, leave logfile unchanged to GC
  leveldb_del(wk->leveldb, key);
}

  static WK *
open_wisckey(const string& dirname)
{
  WK * wk = new WK;
  wk->leveldb = open_leveldb(dirname);
  wk->dir = dirname;
  //TODO: Your code here: logfile
  // create file first
  FILE *file;
  int file_exists;
  const char* filename = "logfile.txt";
  file = fopen(filename, "r");
  if(file==NULL) file_exists = 0;
  else {
    file_exists = 1;
    fclose(file);
  }
  // open file or create it
  if(file_exists==1){
    printf("logfile exists, and it is open\n");
    file = fopen(filename, "a+");
  }else{
    printf("logfile does not exist, but it is created now\n");
    file = fopen(filename, "w+");
  }

  // assign wk logfile attribute
  wk->logfile = file;
  return wk;
}

  static void
close_wisckey(WK * wk)
{
  delete wk->leveldb;
  // TODO: Your code here
  // flush and close logfile
  if(wk->logfile != NULL){
    printf("close logfile, then close wisckey");
    fclose(wk->logfile);
  }
  delete wk;
}

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

  WK * wk = open_wisckey("wisckey_test_dir");
  if (wk == NULL) {
    cerr << "Open WiscKey failed!" << endl;
    exit(1);
  }
  char * vbuf = new char[value_size];
  for (size_t i = 0; i < value_size; i++) {
    vbuf[i] = rand();
  }
  string value = string(vbuf, value_size);

  size_t nfill = 1000000000 / (value_size + 8);
  clock_t t0 = clock();
  size_t p1 = nfill / 40;
  for (size_t j = 0; j < nfill; j++) {
    string key = std::to_string(((size_t)rand())*((size_t)rand()));
    // test value
    // cout<< value  <<endl<<std::flush;
    wisckey_set(wk, key, value);
    if (j >= p1) {
      clock_t dt = clock() - t0;
      cout << "progress: " << j+1 << "/" << nfill << " time elapsed: " << dt * 1.0e-6 << endl << std::flush;
      p1 += (nfill / 40);
    }
  }
  close_wisckey(wk);
  clock_t dt = clock() - t0;
  cout << "time elapsed: " << dt * 1.0e-6 << " seconds" << endl;
  destroy_leveldb("wisckey_test_dir");
  exit(0);
}
