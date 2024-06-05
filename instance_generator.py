import random
import os

def gerar_dicionario_demandas(N):
    """
    Gera um dicionário onde a chave é um int de 1 até N e o valor é um inteiro aleatório de 1 até 10.
    """
    return {i: random.randint(1, 10) for i in range(1, N)}

def gerar_entradas_grafo(num_nos, max_peso=100, probabilidade=0.25):
    """
    Gera um grafo para o problema de otimização de rotas de veículos.
    """
    grafo = {}
    # Gerar pesos para arestas entre o depósito e outros nós
    for i in range(1, num_nos):
        grafo[(0, i)] = random.randint(1, max_peso)
        grafo[(i, 0)] = grafo[(0, i)]  # Assume que a distância de volta ao depósito é a mesma

    # Gerar pesos para arestas entre todos os outros pares de nós
    for i in range(1, num_nos):
        for j in range(i+1, num_nos):
            if random.random() <= probabilidade:  # Verifica a probabilidade
                peso = random.randint(1, max_peso)
                grafo[(i, j)] = peso
                grafo[(j, i)] = peso  # Assume que a distância de volta é a mesma

    return grafo

def main(num_nos, probabilidade):
    if num_nos == 1:
        return  # Ignora o caso de apenas um nó

    demandas = gerar_dicionario_demandas(num_nos)
    grafo = gerar_entradas_grafo(num_nos, probabilidade=probabilidade)

    # Salva o grafo em um arquivo TXT
    os.makedirs('grafos', exist_ok=True)
    filename = f'grafos/grafo{num_nos}.txt'
    with open(filename, 'w') as arquivo:
        arquivo.write(str(num_nos) + "\n")  # Número de nós, incluindo depósito
        for local, demanda in demandas.items():
            linha = f"{local} {demanda}\n"  # Par LOCAL DEMANDA
            arquivo.write(linha)

        arquivo.write(str(len(grafo)) + "\n")  # Número de arestas
        for aresta, peso in grafo.items():
            linha = f"{aresta[0]} {aresta[1]} {peso}\n"  # Trio: ORIGEM DESTINO CUSTO
            arquivo.write(linha)

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Uso: python instance_generator.py <num_nos> <probabilidade>")
        sys.exit(1)

    num_nos = int(sys.argv[1])
    probabilidade = float(sys.argv[2])
    main(num_nos, probabilidade)
