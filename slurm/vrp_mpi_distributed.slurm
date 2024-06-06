#!/bin/bash

#SBATCH --job-name=vrp_mpi_distributed         # Nome do job
#SBATCH --output=vrp_mpi_distributed_%j.txt    # Salva a saída em um arquivo de texto
#SBATCH --partition=espec                      # Especifica a partição
#SBATCH --nodes=1                              # Solicita 1 nó
#SBATCH --ntasks=4                             # Solicita 4 tarefas (processos MPI)
#SBATCH --cpus-per-task=1                      # Solicita 1 CPU por tarefa
#SBATCH --mem=2G                               # Define a memória total de 2GB

# Executa o aplicativo com o arquivo de entrada especificado
mpirun ./vrp_mpi_distributed grafos/grafox.txt
