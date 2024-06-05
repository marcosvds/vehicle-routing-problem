#!/bin/bash

# Verifica se o script Python existe
if [ ! -f "instance_generator.py" ]; then
    echo "O arquivo instance_generator.py não foi encontrado!"
    exit 1
fi

# Loop para gerar 15 grafos
for ((i=1; i<=15; i++))
do
    # Gera um valor aleatório para a probabilidade entre 0.1 e 0.3
    probabilidade=$(awk -v min=0.1 -v max=0.3 'BEGIN{srand(); print min+rand()*(max-min)}')
    
    # Executa o script Python com os parâmetros atuais
    python instance_generator.py $i $probabilidade
done

echo "15 grafos gerados com sucesso!"
