# Monitor de Recursos Linux

Um monitor minimalista e eficiente de recursos do sistema em Linux, desenvolvido em C++ puro. Mostra em tempo real a memória global, uso de CPU e consumo de RAM por navegador web.

## O que faz

Monitor os seguintes recursos em tempo real:

### Memória Global
- **Total**: Memória RAM total do sistema em GB
- **Disponível**: Memória livre disponível em GB
- **Usada**: Memória consumida em GB

### CPU
- Percentagem de CPU usada em tempo real
- Calcula com base em `/proc/stat`
- Atualização a cada segundo

### Navegadores Web
Monitora automaticamente o consumo de RAM para:
- **Firefox** - Mozilla Firefox
- **Chrome** - Google Chrome
- **Chromium** - Chromium open-source
- **Brave** - Brave Privacy Browser
- **Vivaldi** - Vivaldi Browser
- **Edge** - Microsoft Edge
- **LibreWolf** - Privacy-focused Firefox fork
- **Zen** - Zen Browser

*Nota: Filtra browsers com menos de 10 MB para evitar falsos positivos (ex: Chrome process do Brave)*

## Exemplo de Output
Total: 13.48 Gb  Available: 6.187 Gb  Used: 7.294 Gb
cpu usage: 1.307 % 
Ram firefox: 4.467 Gb

## Performance

- **RAM consumida**: ~1.8 MB
- **Binário compilado (static)**: ~1.9 MB
- **Tempo de atualização**: 1 segundo

## Requisitos

- Linux com `/proc` filesystem
- GCC/G++ com suporte a C++17
- `make` (opcional, mas recomendado)

## Instalação

### Com Makefile

```bash
make
```

### Manualmente

```bash
g++ -O3 -static monitor.cpp -o monitor
strip -s monitor
```

## Uso

```bash
./monitor
```

O programa executa continuamente, atualizando as informações a cada segundo. Para parar, pressione `Ctrl+C`.

## Compilação e Flags

- `-O3`: Otimizações máximas
- `-static`: Linking estático (portabilidade)
- `strip -s`: Remove símbolos de debug (reduz tamanho)

## Estrutura do Código

### Funções Principais

- **`extrair_dados_string_int()`** - Converte strings em números
- **`procurar_numeros()`** - Extrai sequências numéricas de strings
- **`global_memory()`** - Lê `/proc/meminfo` para memória global
- **`cpu_usage()`** - Lê `/proc/stat` para percentagem de CPU
- **`browser_search()`** - Itera `/proc/[pid]/` para encontrar browsers

### Estruturas de Dados

- **`tot_use_ava`** - Armazena total, usado e disponível de memória
- **`cpu`** - Armazena total de ticks e work ticks de CPU
- **`browsers_data`** - Armazena nomes e RAM de browsers

## Como Funciona

1. **Inicialização**: Lê CPU inicial em `/proc/stat`
2. **Loop Principal** (a cada segundo):
   - Lê memória global de `/proc/meminfo`
   - Calcula percentagem de CPU (delta dos últimos 2 pontos)
   - Itera `/proc/` procurando browsers por PID
   - Lê `/proc/[pid]/cmdline` para identificar browser
   - Extrai `VmRSS` de `/proc/[pid]/status` para memória
   - Imprime resultados

## Customização

Para adicionar/remover browsers, edite o array `nomes[]`:

```cpp
const string nomes[X] = {"firefox", "chrome", ...};
double ram[X] = {0,0,...};
```

E atualize o loop:

```cpp
for (unsigned char i = 0; i < X; i++){  // X = número de browsers
```

## Limpeza

```bash
make clean
```

Remove o binário compilado.

## Notas Técnicas

- Utiliza `/proc` filesystem (disponível em qualquer Linux)
- Sem dependências externas
- Código em C++ puro (sem bibliotecas third-party)
- Memory-safe com `string` e `ifstream`
- Static linking para portabilidade máxima

## Futuras Melhorias (v2.0)

- Interface interativa com `ncurses`
- CPU por browser
- Monitorização de disco
- Histórico de dados
- Gráficos ASCII

## Versão

**v1.0** - Monitor básico funcional e otimizado

## Autor

Tomás

---

**Desenvolvido como projeto educacional em C++ para aprender programação de sistemas em Linux.**