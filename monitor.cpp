//#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <ncurses.h>


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


struct browsers_data
{   const string nomes[8] = {"firefox", "chrome", "chromium", "brave", "vivaldi", "edge", "librewolf", "zen"};
    double ram[8] ={0,0,0,0,0,0,0,0};
};
 

browsers_data browser_search(){
    browsers_data total;
    DIR* dir = opendir("/proc");
    struct dirent* entrada;
    while ((entrada = readdir(dir)) != NULL){
        if (entrada->d_name[0] >= '0' and entrada->d_name[0] <= '9'){
            string entrada_str = entrada->d_name;
            entrada_str = "/proc/" + entrada_str + '/';
            ifstream in( entrada_str + "cmdline");
            string line;
            getline(in, line);
            for (unsigned char i = 0; i < 8; i++){
                if (line.find(total.nomes[i]) != string::npos){
                    ifstream in2(entrada_str + "smaps_rollup");
                    getline(in2, line);
                    getline(in2, line);
                    getline(in2, line);
                    total.ram[i] += extrair_dados_string_int(procurar_numeros(line));
                    // while (getline(in2, line)){
                    //     if (line.find("Pss:") != string::npos) {
                    //         total.ram[i] += extrair_dados_string_int(procurar_numeros(line));
                    //         break;}
                    // }
                }
            }
        }
    }
    closedir(dir);
    return total;
}


int main(){
    tot_use_ava storage;
    ifstream in("/proc/meminfo");
    string line;
    getline(in, line);
    storage.total = extrair_dados_string_int(procurar_numeros(line)) / 1048576.0;
    in.close();
    line.clear();
    initscr();
    int max_y, max_x;
    box(stdscr, 0, 0);
    cpu last_cpu = cpu_usage();
    getmaxyx(stdscr, max_y, max_x);
    box(stdscr, 0, 0);
    getmaxyx(stdscr, max_y, max_x);
    while (true){
        clear();
        global_memory(storage);
        mvprintw(1,2,"Total: %.4g Gb  Available: %.4g Gb  Used: %.4g Gb", storage.total, storage.available, storage.used);
        cpu current_cpu = cpu_usage();
        mvprintw(2,2,"cpu usage: %.4g %% ", (current_cpu.work - last_cpu.work) * 100.0 / (current_cpu.total - last_cpu.total));
        browsers_data browsers = browser_search();
        for (unsigned char i = 0;i < 8; i++){
            if (browsers.ram[i] > 10000){
                mvprintw(3,2,"Ram %s: %.4g Gb",browsers.nomes[i].c_str(), browsers.ram[i] / 1048576);
            }
        }
        refresh();
        sleep(1);
        last_cpu = current_cpu;
    }
    endwin();
    return 0;
}
