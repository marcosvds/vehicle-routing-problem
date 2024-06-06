#!/bin/bash

#SBATCH --job-name=vrp_openmp_threads          # Nome do job
#SBATCH --output=vrp_openmp_threads_%j.txt     # Salva a saída em um arquivo de texto
#SBATCH --partition=espec                      # Especifica a partição
#SBATCH --nodes=1                              # Solicita 1 nó
#SBATCH --ntasks=1                             # Solicita 1 tarefa
#SBATCH --cpus-per-task=4                      # Solicita 4 CPUs por tarefa
#SBATCH --mem=2G                               # Define a memória total de 2GB

# Executa o aplicativo com o arquivo de entrada especificado
./vrp_openmp_threads grafos/grafox.txt