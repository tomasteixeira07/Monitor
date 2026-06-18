#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <ncurses.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <algorithm>


using namespace std;


struct tot_use_ava { //struct to storage memory
  double total;
  double used;
  double available;
};


unsigned long int convert_str_int(const string text) {
  unsigned long int final = 0;
  for (unsigned short i = 0; i < text.length(); i++) {
    final = final * 10 + (text[i] - 48);
  }
  return final;
}


string search_int_in_str(const string text) {
  unsigned short start = 0, end = text.length();
  for (unsigned long int i = 0; i < text.length(); i++) {
    if (text[i] < 58 and text[i] > 47 and start == 0) {
      start = i;
    } else if ((text[i] > 58 or text[i] < 47) and start != 0) {
      end = i;
      break;
    }
  }
  return text.substr(start, end - start);
}


void global_memory(tot_use_ava &storage) {
  ifstream in("/proc/meminfo");
  string line;
  getline(in, line);
  getline(in, line);
  getline(in, line);
  storage.available =
      convert_str_int(search_int_in_str(line)) / 1048576.0;
  storage.used = storage.total - storage.available;
}


struct cpu {
  unsigned long int total;
  unsigned long int work;
};


cpu cpu_usage() {
  ifstream in("/proc/stat");
  string line;
  getline(in, line);
  unsigned short count = 5;
  unsigned long int total = 0;
  unsigned int work = 0;
  unsigned char count_som = 0;
  while (count < line.length()) {
    unsigned short end = line.length() - 1;
    if (line.find(' ', count) != string::npos) {
      end = line.find(' ', count);
    }
    unsigned long int novo =
        convert_str_int(line.substr(count, end - count));
    total += novo;
    if (count_som < 3) {
      work += novo;
      count_som++;
    }
    count = end + 1;
  }
  return {total, work};
}


void browser_search(const vector<string> &names, vector<double> &storage,
                    vector<unsigned long int> &uptime,
                    vector<unsigned long int> &stime) {
  DIR *dir = opendir("/proc");
  struct dirent *entrada;
  while ((entrada = readdir(dir)) != NULL) {
    if (entrada->d_name[0] >= '0' and entrada->d_name[0] <= '9') {
        string entrada_str = entrada->d_name;
        entrada_str = "/proc/" + entrada_str + '/';
        ifstream in(entrada_str + "cgroup");
        string line;
        getline(in, line);
        for (unsigned short i = 0; i < names.size(); i++) {
          if (line.find(names[i]) != string::npos) {
            ifstream in2(entrada_str + "smaps_rollup");
            getline(in2, line);
            getline(in2, line);
            getline(in2, line);
            storage[i] += convert_str_int(search_int_in_str(line));
            ifstream in3(entrada_str + "stat");
            getline(in3, line);
            line = line.substr(line.find(')') + 4);

            istringstream iss(line);
            string totem;

            for (unsigned short i2 = 0; i2 < 11; i2++) {
              iss >> totem;
            }
            uptime[i] += convert_str_int(totem);
            iss >> totem;
            stime[i] += convert_str_int(totem);
            // while (getline(in2, line)){
            //     if (line.find("Pss:") != string::npos) {
            //         total.ram[i] +=
            //         convert_str_int(search_int_in_str(line)); break;}
            // }
            break;
          }
      }
    }
  }
  closedir(dir);
}


int main() {
  tot_use_ava storage;
  ifstream in("/proc/meminfo");
  string line;
  getline(in, line);
  storage.total = convert_str_int(search_int_in_str(line)) / 1048576.0;
  in.close();
  line.clear();

  
  //Choose whish browsers you want to analyze
  const string names[] = {"firefox", "chrome", "chromium", "brave"};
  vector<unsigned short> names_to_use;
  printf("Browsers disponiveis:\n");
  for (unsigned short i = 0; i < 4; i++) {
    printf("%d. %s\n", i, names[i].c_str());
  }
  printf("Press the numbers of the browsers you want to add:\n");
  vector<string> nomes_a_procurar;
  while (true) {
    char x;
    printf("Number(press . to exit):");
    cin >> x;
    cin.ignore();
    if (x == '.') {
      break;
    }
    if (x < '0' or x > '7') {
      printf("ERROR! INVALID VALUE\n");
    } else {
        if(find(names_to_use.begin(), names_to_use.end(), x - 48) != names_to_use.end()){printf("ERROR! ALREADY ADDED! TRY AGAIN\n");}
        else {nomes_a_procurar.push_back(names[x - 48]);names_to_use.push_back(x-48);}
    }
  }

  //add browsers whish are not in the list
  printf("Do you want to add a browser that is not in the list? (Y/N)");
  char x;
  cin >>x;
  cin.ignore();
  while (x != 'N' and x != 'n'){
      if (x == 'Y' or x == 'y'){
          string new_browser;
          printf("Name of the new browser:");
          cin >> new_browser;
          cin.ignore();
          nomes_a_procurar.push_back(new_browser);
      }
      else{printf("VALUE ERROR\nTry again");}
      printf("Do you want to add another? (Y/N)");
      cin >>x;
      cin.ignore();
  }

  
  initscr();
  nodelay(stdscr, TRUE);
  box(stdscr, 0, 0);
  vector<double> browsers_ram(nomes_a_procurar.size());
  vector<unsigned long int> new_uptime(nomes_a_procurar.size());
  vector<unsigned long int> last_uptime(nomes_a_procurar.size());
  vector<unsigned long int> new_stime(nomes_a_procurar.size());
  vector<unsigned long int> last_stime(nomes_a_procurar.size());
  cpu last_cpu = cpu_usage();
  while (true) {
    clear();
    
    //print memory
    global_memory(storage);
    mvprintw(1, 2, "Memory");
    mvprintw(2, 4, "Total     : %8.2f GB", storage.total);
    mvprintw(3, 4, "Available : %8.2f GB              Press 'q' to leave", storage.available);
    mvprintw(4, 4, "Used      : %8.2f GB", storage.used);
    mvaddch(5, 4, '[');
    int preenchimento = storage.used / storage.total * 50.0;
    for (int i = 0; i < 50; i++) {mvaddch(5, 5 + i, i < preenchimento ? '#' : ' ');}
    mvaddch(5, 5 + 50, ']');

    //print CPU 
    mvprintw(7, 2, "CPU");
    cpu current_cpu = cpu_usage();
    double cpu_percent = (current_cpu.work - last_cpu.work) * 100.0 /
                         (current_cpu.total - last_cpu.total);
    mvprintw(8, 4, "CPU Usage: %6.2f%%", (cpu_percent));
    mvaddch(9, 4, '[');
    preenchimento = cpu_percent * 0.5;
    for (int i = 0; i < 50; i++) {mvaddch(9, 5 + i, i < preenchimento ? '#' : ' ');}
    mvaddch(9, 5 + 50, ']');

    //print Browsers
    mvprintw(11, 2, "Browsers");
    browsers_ram.resize(nomes_a_procurar.size());
    browser_search(nomes_a_procurar, browsers_ram, new_uptime,new_stime);
    for (unsigned short i = 0; i < nomes_a_procurar.size(); i++) {
        mvprintw(12 + i, 4, "%-15s RAM: %7.2f Gb   CPU: %5.1f%%",
                    nomes_a_procurar[i].c_str(), browsers_ram[i] / 1048576,
                    ((new_uptime[i] - last_uptime[i]) + (new_stime[i] - last_stime[i])) *
                    100.0 / (current_cpu.total - last_cpu.total));
      browsers_ram[i] = 0;
      last_uptime[i] = new_uptime[i];
      last_stime[i] = new_stime[i];
      new_uptime[i] = 0;
      new_stime[i] = 0;
    }

    
    //search if the user wants to leave
    refresh();
    int ch = getch();
    if (ch == 'q' || ch == 'Q') {
      endwin();
      exit(0);
    }


    sleep(1);
    last_cpu = current_cpu;
  }
}
