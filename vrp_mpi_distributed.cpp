#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <map>
#include <queue>
#include <utility>  // Para std::pair
#include <algorithm>  // Para std::next_permutation
#include <cmath>  // Para pow
#include <climits>
#include <chrono>  // Inclui a biblioteca chrono
#include <mpi.h>  // Inclui a biblioteca MPI

using namespace std::chrono;  // Usa o namespace chrono para facilitar

// Define o tipo Aresta como uma tupla de três inteiros (origem, destino, custo)
using Aresta = std::tuple<int, int, int>;

// Define um grafo como um mapa de inteiros para conjuntos de inteiros
using Grafo = std::map<int, std::set<int>>;

// Define um caminho como um vetor de inteiros
using Caminho = std::vector<int>;

// Define uma demanda como um mapa de inteiros para inteiros
using Demanda = std::map<int, int>;

// Define arestas como um mapa de pares de inteiros para inteiros
using Arestas = std::map<std::pair<int, int>, int>;

// Função para converter lista de arestas em matriz de adjacência
Grafo gerarMatrizAdjacente(const std::vector<Aresta>& listaArestas) {
    Grafo grafo;
    for (const auto& aresta : listaArestas) {
        int origem = std::get<0>(aresta);
        int destino = std::get<1>(aresta);
        if (origem == 0) origem = -1;  // -1 para 'source'
        if (destino == 0) destino = -2;  // -2 para 'sink'
        grafo[origem].insert(destino);
    }
    return grafo;
}

// Função para encontrar todos os caminhos possíveis no grafo
std::vector<Caminho> encontrarTodosCaminhos(const Grafo& grafo) {
    std::vector<Caminho> caminhos;
    std::queue<Caminho> fila;
    Caminho inicio = {-1};  // Começa com 'source' representado por -1
    fila.push(inicio);

    while (!fila.empty()) {
        Caminho caminho = fila.front();
        fila.pop();
        int no = caminho.back();

        if (no == -2) {  // -2 para 'sink'
            caminhos.push_back(caminho);
        } else {
            for (int vizinho : grafo.at(no)) {
                Caminho novoCaminho(caminho);
                novoCaminho.push_back(vizinho);
                fila.push(novoCaminho);
            }
        }
    }
    return caminhos;
}

// Função para verificar se a capacidade do caminho é respeitada
bool verificarCapacidade(const Caminho& caminho, const Demanda& demandas, int capacidadeMax) {
    int cargaTotal = 0;
    for (int no : caminho) {
        if (no != -1 && no != -2) {  // Ignora 'source' e 'sink'
            cargaTotal += demandas.at(no);
        }
    }
    return cargaTotal <= capacidadeMax;
}

// Função para calcular o custo de um caminho
int calcularCusto(const Caminho& caminho, const Arestas& mapaArestas) {
    int custoTotal = 0;

    for (size_t i = 0; i < caminho.size() - 1; ++i) {
        int origem = caminho[i];
        if (origem == -1) {
            origem = 0;
        }

        int destino = caminho[i + 1];
        if (destino == -2) {
            destino = 0;
        }

        custoTotal += mapaArestas.at({origem, destino});
    }
    return custoTotal;
}

// Função para gerar o conjunto das partes (power set) de todos os caminhos
std::vector<std::vector<Caminho>> gerarPowerSet(const std::vector<Caminho>& todosCaminhos) {
    std::vector<std::vector<Caminho>> powerSet;
    size_t tamanhoSet = todosCaminhos.size();
    size_t tamanhoPowerSet = std::pow(2, tamanhoSet);

    for (size_t contador = 0; contador < tamanhoPowerSet; contador++) {
        std::vector<Caminho> subconjunto;
        for (size_t i = 0; i < tamanhoSet; i++) {
            if (contador & (1 << i)) {
                subconjunto.push_back(todosCaminhos[i]);
            }
        }
        if (!subconjunto.empty()) {
            powerSet.push_back(subconjunto);
        }
    }
    return powerSet;
}

// Função para verificar se a combinação de caminhos atende à demanda e respeita o número máximo de paradas
bool verificarCombinacaoDemanda(const std::vector<Caminho>& combinacao, const Demanda& demandas, int maxParadas) {
    std::vector<int> nosComDemanda;
    Demanda copiaDemandas = demandas;
    for (const auto& [no, demanda] : demandas) {
        if (demanda > 0) {
            nosComDemanda.push_back(no);
        }
    }

    for (const auto& caminho : combinacao) {
        if (caminho.size() - 2 > maxParadas) {
            return false;
        }
        for (int no : caminho) {
            if (no != -1 && no != -2) {  // Ignora 'source' e 'sink'
                auto it = std::find(nosComDemanda.begin(), nosComDemanda.end(), no);
                if (it != nosComDemanda.end()) {
                    copiaDemandas[no] = -3;
                }
            }
        }
    }

    for (const auto& [no, demanda] : copiaDemandas) {
        if (demanda != -3) {
            return false;
        }
    }

    return true;
}

// Função para calcular o custo total de uma combinação de caminhos
int calcularCustoTotalCombinacao(const std::vector<Caminho>& combinacao, const Arestas& mapaArestas) {
    int custoTotal = 0;
    for (const auto& caminho : combinacao) {
        custoTotal += calcularCusto(caminho, mapaArestas);
    }
    return custoTotal;
}

// Função para encontrar a melhor rota que atende à demanda com o menor custo
std::vector<Caminho> encontrarMelhorRota(const std::vector<std::vector<Caminho>>& powerSet, const Arestas& mapaArestas, const Demanda& demandas, int argc, char** argv, int size, int rank, int maxParadas) {
    
    int tamanhoPowerSet = powerSet.size();
    int chunkSize = tamanhoPowerSet / size;
    int remainder = tamanhoPowerSet % size;
    int padding = size - remainder;
    int elementosPorProcesso = (tamanhoPowerSet + padding) / size;

    int custoMinimo = INT_MAX;
    int idxMelhorRota = -1;

    int idxInicio = rank * elementosPorProcesso;
    int idxFim = (rank + 1) * elementosPorProcesso;
    if (rank == size - 1) {
        idxFim = tamanhoPowerSet;
    }

    std::vector<int> melhorRotaIdx(1);
    melhorRotaIdx[0] = -1;

    for (int i = idxInicio; i < idxFim; i += 1) {
        const auto& combinacao = powerSet[i];
        if (verificarCombinacaoDemanda(combinacao, demandas, maxParadas)) {
            int custo = calcularCustoTotalCombinacao(combinacao, mapaArestas);
            if (custo < custoMinimo) {
                custoMinimo = custo;
                melhorRotaIdx[0] = i;
            }
        }
    }

    std::vector<int> melhoresRotas(size);
    MPI_Gather(melhorRotaIdx.data(), 1, MPI_INT, melhoresRotas.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    int melhorIdx = -1;
    int melhorCusto = INT_MAX;
    if (rank == 0) {
        for (int i = 0; i < size; i += 1) {
            if (melhoresRotas[i] != -1) {
                const auto& combinacao = powerSet[melhoresRotas[i]];
                int custo = calcularCustoTotalCombinacao(combinacao, mapaArestas);
                if (custo < melhorCusto) {
                    melhorCusto = custo;
                    melhorIdx = melhoresRotas[i];
                }
            }
        }
    }

    std::vector<Caminho> melhorRota;
    if (rank == 0) {
        melhorRota = powerSet[melhorIdx];
    }
    return melhorRota;
}

int main(int argc, char* argv[]) {

    int maxParadas = 10;  // Número máximo de paradas permitidas

    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {  // Verifica se o caminho do arquivo foi fornecido
        std::cerr << "Uso: " << argv[0] << " <caminho_para_o_arquivo>" << std::endl;
        return 1;
    }

    const char* caminhoArquivo = argv[1];  // Pega o caminho do arquivo do primeiro argumento
    std::ifstream arquivo(caminhoArquivo);  // Abre o arquivo

    if (!arquivo) {
        std::cerr << "Não foi possível abrir o arquivo." << std::endl;
        return 1;
    }
    if (rank == 0) {
        std::cout << "Arquivo aberto com sucesso.\n" << "Caminho do arquivo: " << caminhoArquivo << "\n";
        std::cout << "Solução MPI" << "\n";
        std::cout << "Rank: " << rank << std::endl;
        std::cout << "Size: " << size << std::endl;
    }

    auto inicio = high_resolution_clock::now();
    int numeroNos, numeroArestas;
    arquivo >> numeroNos;  // Lê o número de nós

    Demanda demandas;
    int indiceNo, demandaNo;

    // Ler as demandas dos nós
    for (int i = 0; i < numeroNos - 1; ++i) {
        arquivo >> indiceNo >> demandaNo;
        demandas[indiceNo] = demandaNo;
    }

    arquivo >> numeroArestas;  // Lê o número de arestas
    std::vector<Aresta> listaArestas;
    int origem, destino, custo;

    // Ler as arestas
    for (int i = 0; i < numeroArestas; ++i) {
        arquivo >> origem >> destino >> custo;
        listaArestas.push_back(std::make_tuple(origem, destino, custo));
    }

    arquivo.close();

    // Criar o mapa de arestas
    Arestas mapaArestas;
    for (const auto& e : listaArestas) {
        mapaArestas[{std::get<0>(e), std::get<1>(e)}] = std::get<2>(e);
    }

    // Continuar com a lógica anterior para gerar o grafo, encontrar caminhos, etc.
    Grafo grafo = gerarMatrizAdjacente(listaArestas);
    std::vector<Caminho> todosCaminhos = encontrarTodosCaminhos(grafo);

    // Gerar o conjunto das partes de todos os caminhos
    std::vector<std::vector<Caminho>> powerSetCaminhos = gerarPowerSet(todosCaminhos);

    // Encontrar a melhor rota que atende à demanda com o menor custo
    std::vector<Caminho> melhorRota = encontrarMelhorRota(powerSetCaminhos, mapaArestas, demandas, argc, argv, size, rank, maxParadas);
    MPI_Finalize();

    // Finaliza o timer e calcula a duração
    auto fim = high_resolution_clock::now();
    auto duracao = duration_cast<milliseconds>(fim - inicio);

    if (rank == 0) {
        std::cout << "Melhor rota encontrada:\n";
        for (const auto& caminho : melhorRota) {
            for (int no : caminho) {
                std::cout << no << " -> ";
            }
            std::cout << "Fim\n";
        }
        std::cout << "Tempo total de execução: " << duracao.count() << " ms" << std::endl;
    }

    return 0;
}
