#!/bin/bash

# Verifica se o script Python existe
if [ ! -f "solve_vrp.py" ]; then
    echo "O arquivo solve_vrp.py não foi encontrado!"
    exit 1
fi

# Cria a pasta resultados/VRPy se não existir
mkdir -p resultados/VRPy

# Loop para resolver VRP para cada grafo gerado
for ((i=2; i<=15; i++))  # Começar do grafo 2, ignorando o grafo 1
do
    caminho_arquivo="grafos/grafo$i.txt"
    if [ ! -f "$caminho_arquivo" ]; then
        echo "O arquivo $caminho_arquivo não foi encontrado!"
        continue
    fi

    # Define capacidade do veículo e número máximo de paradas
    capacidade_veiculo=15
    num_paradas=5
    
    # Mensagem de progresso
    echo "Processando $caminho_arquivo..."
    
    # Executa o script Python para resolver o VRP
    python solve_vrp.py "$caminho_arquivo" $capacidade_veiculo $num_paradas
done

echo "15 grafos processados com sucesso!"
