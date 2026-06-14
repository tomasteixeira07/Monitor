//TODO: Permitir ao user adicionar o browser que quer
// melhorar a escolha de browsers, será que caixinhas é possivel?


//#include <csignal>
#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <ncurses.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <unordered_set>
#include <vector>
#include <algorithm>


using namespace std;

struct tot_use_ava {
  double total;
  double used;
  double available;
};

unsigned long int extrair_dados_string_int(const string texto) {
  unsigned long int resultado = 0;
  for (unsigned short i = 0; i < texto.length(); i++) {
    resultado = resultado * 10 + (texto[i] - 48);
  }
  return resultado;
}

string procurar_numeros(const string texto) {
  unsigned short inicio = 0, fim = texto.length();
  for (unsigned long int i = 0; i < texto.length(); i++) {
    if (texto[i] < 58 and texto[i] > 47 and inicio == 0) {
      inicio = i;
    } else if ((texto[i] > 58 or texto[i] < 47) and inicio != 0) {
      fim = i;
      break;
    }
  }
  return texto.substr(inicio, fim - inicio);
}

void global_memory(tot_use_ava &storage) {
  ifstream in("/proc/meminfo");
  string line;
  getline(in, line);
  getline(in, line);
  getline(in, line);
  storage.available =
      extrair_dados_string_int(procurar_numeros(line)) / 1048576.0;
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
  unsigned short contagem = 5;
  unsigned long int total = 0;
  unsigned int work = 0;
  unsigned char contagem_somas = 0;
  while (contagem < line.length()) {
    unsigned short fim = line.length() - 1;
    if (line.find(' ', contagem) != string::npos) {
      fim = line.find(' ', contagem);
    }
    unsigned long int novo =
        extrair_dados_string_int(line.substr(contagem, fim - contagem));
    total += novo;
    if (contagem_somas < 3) {
      work += novo;
      contagem_somas++;
    }
    contagem = fim + 1;
  }
  return {total, work};
}

void browser_search(const vector<string> &nomes, vector<double> &storage,
                    unordered_set<unsigned int> &PIDS_Lixo,
                    vector<unsigned long int> &uptime,
                    vector<unsigned long int> &stime) {
  DIR *dir = opendir("/proc");
  struct dirent *entrada;
  while ((entrada = readdir(dir)) != NULL) {
    if (entrada->d_name[0] >= '0' and entrada->d_name[0] <= '9') {
      string entrada_str = entrada->d_name;
      unsigned long int PID = extrair_dados_string_int(entrada_str);
      if (PIDS_Lixo.find(PID) == PIDS_Lixo.end()) {
        entrada_str = "/proc/" + entrada_str + '/';
        ifstream in(entrada_str + "cgroup");
        string line;
        getline(in, line);
        bool found = 0;
        for (unsigned short i = 0; i < nomes.size(); i++) {
          if (line.find(nomes[i]) != string::npos) {
            ifstream in2(entrada_str + "smaps_rollup");
            getline(in2, line);
            getline(in2, line);
            getline(in2, line);
            storage[i] += extrair_dados_string_int(procurar_numeros(line));
            ifstream in3(entrada_str + "stat");
            getline(in3, line);
            line = line.substr(line.find(')') + 4);

            istringstream iss(line);
            string totem;

            for (unsigned short i2 = 0; i2 < 11; i2++) {
              iss >> totem;
            }
            uptime[i] += extrair_dados_string_int(totem);
            iss >> totem;
            stime[i] += extrair_dados_string_int(totem);
            // while (getline(in2, line)){
            //     if (line.find("Pss:") != string::npos) {
            //         total.ram[i] +=
            //         extrair_dados_string_int(procurar_numeros(line)); break;}
            // }

            found = 1;
            break;
          }
        }
        if (found == 0) {
          PIDS_Lixo.insert(PID);
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
  storage.total = extrair_dados_string_int(procurar_numeros(line)) / 1048576.0;
  in.close();
  line.clear();


  const string nomes[8] = {"firefox", "chrome", "chromium",  "brave", "vivaldi", "edge", "librewolf", "zen"};
  vector<unsigned short> nomes_usados;
  printf("Browsers disponiveis:\n");
  for (unsigned short i = 0; i < 8; i++) {
    printf("%d. %s\n", i, nomes[i].c_str());
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
        if(find(nomes_usados.begin(), nomes_usados.end(), x - 48) != nomes_usados.end()){printf("ERROR! ALREADY ADDED! TRY AGAIN\n");}
        else {nomes_a_procurar.push_back(nomes[x - 48]);nomes_usados.push_back(x-48);}
    }
  }

  
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
  int max_y, max_x;
  box(stdscr, 0, 0);
  cpu last_cpu = cpu_usage();
  getmaxyx(stdscr, max_y, max_x);
  box(stdscr, 0, 0);
  getmaxyx(stdscr, max_y, max_x);
  vector<double> browsers_ram(nomes_a_procurar.size());
  vector<unsigned long int> new_uptime(nomes_a_procurar.size());
  vector<unsigned long int> last_uptime(nomes_a_procurar.size());
  vector<unsigned long int> new_stime(nomes_a_procurar.size());
  vector<unsigned long int> last_stime(nomes_a_procurar.size());
  unordered_set<unsigned int> PIDS_Lixo;
  unsigned short contagem = 0;
  sleep(1);
  while (true) {
    clear();

    global_memory(storage);
    mvprintw(1, 2, "Memory");
    mvprintw(2, 4, "Total     : %8.2f GB", storage.total);
    mvprintw(3, 4, "Available : %8.2f GB              Press 'q' to leave",
             storage.available);
    mvprintw(4, 4, "Used      : %8.2f GB", storage.used);
    mvaddch(5, 4, '[');
    int preenchimento = storage.used / storage.total * 50.0;
    for (int i = 0; i < 50; i++) {
      mvaddch(5, 5 + i, i < preenchimento ? '#' : ' ');
    }
    mvaddch(5, 5 + 50, ']');

    mvprintw(7, 2, "CPU");
    cpu current_cpu = cpu_usage();
    double cpu_percent = (current_cpu.work - last_cpu.work) * 100.0 /
                         (current_cpu.total - last_cpu.total);
    mvprintw(8, 4, "CPU Usage: %6.2f%%", (cpu_percent));
    mvaddch(9, 4, '[');
    preenchimento = cpu_percent * 0.5;
    for (int i = 0; i < 50; i++) {
      mvaddch(9, 5 + i, i < preenchimento ? '#' : ' ');
    }
    mvaddch(9, 5 + 50, ']');

    mvprintw(11, 2, "Browsers");
    browsers_ram.resize(nomes_a_procurar.size());
    browser_search(nomes_a_procurar, browsers_ram, PIDS_Lixo, new_uptime,
                   new_stime);
    for (unsigned short i = 0; i < nomes_a_procurar.size(); i++) {
      // if (browsers_ram[i] > 10000){
      mvprintw(
          12 + i, 4, "%-15s RAM: %7.2f Gb   CPU: %5.1f%%",
          nomes_a_procurar[i].c_str(), browsers_ram[i] / 1048576,
          ((new_uptime[i] - last_uptime[i]) + (new_stime[i] - last_stime[i])) *
              100.0 / (current_cpu.total - last_cpu.total));
      browsers_ram[i] = 0;
      last_uptime[i] = new_uptime[i];
      last_stime[i] = new_stime[i];
      new_uptime[i] = 0;
      new_stime[i] = 0;
      //}
    }
    refresh();
    int ch = getch();
    if (ch == 'q' || ch == 'Q') {
      endwin();
      exit(0);
    }
    sleep(1);
    last_cpu = current_cpu;
    contagem++;
    if (contagem == 30) {
      PIDS_Lixo.clear(), contagem = 0;
    };
  }
}
