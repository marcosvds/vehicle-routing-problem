import os
import sys
from networkx import DiGraph, set_node_attributes
from vrpy import VehicleRoutingProblem

def ler_arquivo_grafo(caminho_arquivo):
    with open(caminho_arquivo, 'r', encoding='utf-8') as arquivo:
        # Lê o número de nós
        N = int(arquivo.readline().strip())

        # Lê as demandas dos nós
        demandas = {}
        for _ in range(N-1):  # Desconsidera o depósito (0)
            linha = arquivo.readline().strip().split()
            id_no, demanda = int(linha[0]), int(linha[1])
            demandas[id_no] = demanda

        # Lê o número de arestas
        K = int(arquivo.readline().strip())

        # Lê as arestas
        grafo = []
        for _ in range(K):
            linha = arquivo.readline().strip().split()
            origem, destino, peso = int(linha[0]), int(linha[1]), int(linha[2])
            grafo.append((origem, destino, peso))

    return demandas, grafo

def resolver_vrp(caminho_arquivo, capacidade_veiculo, num_paradas):
    demandas, arestas = ler_arquivo_grafo(caminho_arquivo)

    G = DiGraph()
    for inicio, fim, custo in arestas:
        if inicio == 0:
            inicio = "Source"
        if fim == 0:
            fim = "Sink"
        G.add_edge(inicio, fim, cost=custo)

    set_node_attributes(G, values=demandas, name="demand")

    # Adiciona nós "Source" e "Sink" com demanda zero
    G.add_node("Source", demand=0)
    G.add_node("Sink", demand=0)

    prob = VehicleRoutingProblem(G, load_capacity=capacidade_veiculo)
    prob.num_stops = num_paradas
    prob.solve()

    # Salva a solução em um arquivo de resultado
    resultado_arquivo = os.path.join("resultados", "VRPy", f"{os.path.basename(caminho_arquivo).replace('.txt', '_result.txt')}")
    with open(resultado_arquivo, 'w', encoding='utf-8') as arquivo:
        arquivo.write(f"Solução para {caminho_arquivo}:\n")
        arquivo.write(f"Rotas: {prob.best_routes}\n")
        arquivo.write(f"Custo total: {prob.best_value}\n")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Uso: python solve_vrp.py <caminho_arquivo> <capacidade_veiculo> <num_paradas>")
        sys.exit(1)

    caminho_arquivo = sys.argv[1]
    capacidade_veiculo = int(sys.argv[2])
    num_paradas = int(sys.argv[3])

    resolver_vrp(caminho_arquivo, capacidade_veiculo, num_paradas)
