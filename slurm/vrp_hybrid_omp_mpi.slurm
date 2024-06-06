#!/bin/bash

#SBATCH --job-name=vrp_hybrid_omp_mpi         # Nome do job
#SBATCH --output=vrp_hybrid_omp_mpi_%j.txt    # Salva a saída em um arquivo de texto
#SBATCH --partition=espec                     # Especifica a partição
#SBATCH --nodes=1                             # Solicita 1 nó
#SBATCH --ntasks=4                            # Solicita 4 tarefas (processos MPI)
#SBATCH --cpus-per-task=4                     # Solicita 4 CPUs por tarefa
#SBATCH --mem=2G                              # Define a memória total de 2GB

# Executa o aplicativo com o arquivo de entrada especificado
mpirun ./vrp_hybrid_omp_mpi grafos/grafox.txt
