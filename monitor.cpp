#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <ncurses.h>
#include <vector>
#include <unordered_set>


using namespace std;


struct tot_use_ava{
    double total;
    double used;
    double available;
};


unsigned long int  extrair_dados_string_int(const string texto){
    unsigned long int resultado = 0;
    for (unsigned short i = 0; i < texto.length(); i++){
        resultado = resultado * 10 + (texto[i] - 48 );
    }
    return resultado;
}


string procurar_numeros(const string texto){
    unsigned short inicio = 0, fim = texto.length();
    for (unsigned long int i = 0; i < texto.length(); i++){
        if (texto[i] < 58 and texto[i] > 47 and inicio == 0){inicio = i;}
        else if ((texto[i] > 58 or texto[i] < 47) and inicio != 0){fim = i; break;}
    }
    return texto.substr(inicio, fim - inicio);
}


void global_memory (tot_use_ava &storage){
    ifstream in("/proc/meminfo");
    string line;
    getline(in, line);
    getline(in, line);
    getline(in,line);
    storage.available = extrair_dados_string_int(procurar_numeros(line)) / 1048576.0;
    storage.used = storage.total - storage.available; 
} 


struct cpu{
    unsigned long int total; 
    unsigned long int work;
};


cpu cpu_usage(){
    ifstream in("/proc/stat");
    string line;
    getline(in, line);
    unsigned short contagem = 5;
    unsigned long int total = 0;
    unsigned int work = 0;
    unsigned char contagem_somas = 0;
    while (contagem < line.length()){
        unsigned short fim = line.length() - 1;
        if (line.find(' ', contagem) != string::npos){fim = line.find(' ', contagem);}
        unsigned long int novo = extrair_dados_string_int(line.substr(contagem, fim - contagem));
        total += novo;
        if (contagem_somas < 3){work += novo; contagem_somas++;}
        contagem = fim + 1;
    }
    return {total, work};
}
 

void browser_search(const vector<string>& nomes, vector <double> &storage, unordered_set<int> &PIDS_Lixo, unordered_set<int> &UTEIS){
    
    DIR* dir = opendir("/proc");
    struct dirent* entrada;
    while ((entrada = readdir(dir)) != NULL){
        if (entrada->d_name[0] >= '0' and entrada->d_name[0] <= '9'){
            string entrada_str = entrada->d_name;
            unsigned long int PID = extrair_dados_string_int(entrada_str);
            if (PIDS_Lixo.find(PID) == PIDS_Lixo.end()){
                entrada_str = "/proc/" + entrada_str + '/';
                ifstream in( entrada_str + "cgroup");
                string line;
                getline(in, line);
                bool found = 0;
                for (unsigned short i = 0; i < nomes.size(); i++){
                    if (line.find(nomes[i]) != string::npos){
                        ifstream in2(entrada_str + "smaps_rollup");
                        getline(in2, line);
                        getline(in2, line);
                        getline(in2, line);
                        storage[i] += extrair_dados_string_int(procurar_numeros(line));
                        // while (getline(in2, line)){
                        //     if (line.find("Pss:") != string::npos) {
                        //         total.ram[i] += extrair_dados_string_int(procurar_numeros(line));
                        //         break;}
                        // }
                        found = 1;
                    }
                }
                if (found == 0){PIDS_Lixo.insert(PID);}
            }
        }
    }
    closedir(dir);
}



int main(){
    tot_use_ava storage;
    ifstream in("/proc/meminfo");
    string line;
    getline(in, line);
    storage.total = extrair_dados_string_int(procurar_numeros(line)) / 1048576.0;
    in.close();
    line.clear();
    
    //const string* nomes = new string[8]{"firefox", "chrome", "chromium", "brave", "vivaldi", "edge", "librewolf", "zen"};
    const string nomes[8] = {"firefox", "chrome", "chromium", "brave", "vivaldi", "edge", "librewolf", "zen"};
    printf("Browsers disponiveis:\n");
    for (unsigned short i = 0; i < 8; i++){
        printf("%d. %s\n", i, nomes[i].c_str());
    }
    printf("Introduza os numeros dos browsers que deseja:\n");
    vector<string> nomes_a_procurar;
    while (true){
        char x;
        printf("Número(introduza . se desejar acabar):");
        cin >> x;
        if (x == '.'){break;}
        if (x < '0' or x > '7'){printf("Valor Inválido!\n");}
        else{nomes_a_procurar.push_back(nomes[x - 48]);}
    }


    initscr();
    int max_y, max_x;
    box(stdscr, 0, 0);
    cpu last_cpu = cpu_usage();
    getmaxyx(stdscr, max_y, max_x);
    box(stdscr, 0, 0);
    getmaxyx(stdscr, max_y, max_x);
    vector <double> browsers_ram(nomes_a_procurar.size());
    unordered_set <int> PIDS_Lixo;
    unordered_set <int> PIDS_UTEIS;
    while (true){
        clear();


        global_memory(storage);
        mvprintw(1,2,"Memory");
        mvprintw(2,4,"Total     : %8.2f GB", storage.total);
        mvprintw(3,4,"Available : %8.2f GB", storage.available);
        mvprintw(4,4,"Used      : %8.2f GB", storage.used);
        

        mvprintw(6,2,"CPU");
        cpu current_cpu = cpu_usage();
        double cpu_percent = (current_cpu.work - last_cpu.work) * 100.0 / (current_cpu.total - last_cpu.total);
        mvprintw(7,4,"CPU Usage: %6.2f%%", (cpu_percent));
        mvaddch(8,4,'[');
        int preenchimento = cpu_percent * 0.5;
        for (int i = 0; i < 50; i++) {
            mvaddch(8,5+i, i < preenchimento ? '#' : ' ');
        }
        mvaddch(8,5+50,']');


        mvprintw(10,2,"Memory Browsers");
        browsers_ram.resize(nomes_a_procurar.size());
        browser_search(nomes_a_procurar, browsers_ram, PIDS_Lixo);
        for (unsigned short i = 0;i < nomes_a_procurar.size(); i++){
            //if (browsers_ram[i] > 10000){
                mvprintw(11 + i,4,"Ram %s: %.4g Gb",nomes_a_procurar[i].c_str(), browsers_ram[i] / 1048576);
                browsers_ram[i] = 0;
            //}
        }
        refresh();
        sleep(1);
        last_cpu = current_cpu;
    }
    endwin();
    return 0;
}
