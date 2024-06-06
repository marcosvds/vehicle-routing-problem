import os

# Caminho base para os diretórios de soluções
base_path = os.path.join(os.getcwd(), 'solucoes')

# Diretórios das diferentes soluções
directories = {
    'brute_force': 'brute_force',
    'hybrid_omp_mpi': 'hybrid_omp_mpi',
    'local_search': 'local_search',
    'mpi_distributed': 'mpi_distributed',
    'openmp_threads': 'openmp_threads'
}

# Prefixo dos nomes de arquivos em cada diretório
file_prefixes = {
    'brute_force': 'solucao_brute_force_grafo',
    'hybrid_omp_mpi': 'solucao_hybrid_omp_mpi_grafo',
    'local_search': 'local_search_grafo',
    'mpi_distributed': 'mpi_distributed_grafo',
    'openmp_threads': 'openmp_threads_grafo'
}

# Sufixo dos nomes de arquivos (ordem)
file_suffixes = [str(i) + ".txt" for i in range(3, 13)]

# Função para extrair o tempo de execução de um arquivo
def extrair_tempo(file_path):
    with open(file_path, 'r') as f:
        try:
            tempo = int(f.read().strip())
            return tempo
        except ValueError:
            print(f"Erro ao extrair tempo do arquivo: {file_path}")
            return None

# Dicionário para armazenar os vetores de tempos de execução
tempos_execucao = {key: [] for key in directories.keys()}

# Percorrer os diretórios e extrair os tempos de execução
for key, dir_name in directories.items():
    dir_path = os.path.join(base_path, dir_name)
    if os.path.exists(dir_path):
        print(f"Processando diretório: {dir_path}")
        for suffix in file_suffixes:
            file_name = file_prefixes[key] + suffix
            file_path = os.path.join(dir_path, file_name)
            if os.path.isfile(file_path):
                print(f"Processando arquivo: {file_path}")
                tempo = extrair_tempo(file_path)
                if tempo is not None:
                    tempos_execucao[key].append(tempo)

# Imprimir os vetores no console
for key, tempos in tempos_execucao.items():
    print(f"vetor_{key} = {tempos}")

print("Soluções salvas com sucesso.")
