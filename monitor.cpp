//#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>


using namespace std;


struct tot_use_ava{
    double total;
    double used;
    double available;
};


unsigned long int  extrair_dados_string_int(const string texto){
    unsigned long int resultado = 0;
    for (unsigned short i = 0; i < texto.length(); i++){
        //std::cout << texto[i] << '\n';
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


tot_use_ava global_memory (){
    tot_use_ava storage;
    ifstream in("/proc/meminfo");
    string line;
    getline(in, line);
    storage.total = extrair_dados_string_int(procurar_numeros(line)) / 1048576.0;
    getline(in, line);
    getline(in,line);
    storage.available = extrair_dados_string_int(procurar_numeros(line)) / 1048576.0;
    storage.used = storage.total - storage.available;
    return storage; 
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
        //cout << line.find(' ', contagem) << '\n';
        if (line.find(' ', contagem) != string::npos){fim = line.find(' ', contagem);}
        unsigned long int novo = extrair_dados_string_int(line.substr(contagem, fim - contagem));
        total += novo;
        if (contagem_somas < 3){work += novo; contagem_somas++;}
        contagem = fim + 1;
        //cout<< contagem<<'\n';
    }
    return {total, work};
}


int main(){
    cpu last_cpu = cpu_usage();
    while (true){
        tot_use_ava storage = global_memory();
        printf("Total: %.4g Gb  Available: %.4g Gb  Used: %.4g Gb\n", storage.total, storage.available, storage.used);
        //cout << "Total: " << storage.total << " Gb  Available: " << storage.available << " Gb  Used: " << storage.used << " Gb\n"; 
        cpu current_cpu = cpu_usage();
        printf("cpu usage: %.4g %% \n\n", (current_cpu.work - last_cpu.work) * 100.0 / (current_cpu.total - last_cpu.total));
        //cout << "cpu usage: " << (current_cpu.work - last_cpu.work) * 100.0 / (current_cpu.total - last_cpu.total) << " %\n\n";
        sleep(1);
        last_cpu = current_cpu;
    }
    return 0;
}
