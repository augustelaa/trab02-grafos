#include <iostream>
#include <list>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <chrono>

using namespace std;

/**
 * Augusto Henrique da Conceição
 * Marcelo Luis Jung
 **/

class GrafoLista {
    protected:
    unsigned int ordem;
    unsigned int tamanho;
    list<pair<unsigned int, unsigned int>> *adjacencia;
    unsigned int *nivelV;

    public:
    void setOrdem(unsigned int ordem) {
        this->ordem = ordem;
        this->tamanho = 0;
        this->adjacencia = new list<pair<unsigned int, unsigned int>>[ordem];
    }

    void setTamanho(unsigned int tamanho) {
        this->tamanho = tamanho;
    }

    void adicionarArestaValoradaNaoDirigida(unsigned int verticeOrigem, unsigned int verticeDestino, unsigned int valor) {
        this->adjacencia[verticeOrigem].push_back(make_pair(verticeDestino, valor));
        this->adjacencia[verticeDestino].push_back(make_pair(verticeOrigem, valor));
        this->tamanho++;
    }

    unsigned int dijkstra(unsigned int verticeOrigem, unsigned int verticeDestino) {
        unsigned int distancia[this->getOrdem()];
        bool visitados[this->getOrdem()];

        // Inicializa os valores
        for (unsigned int i = 0, len = this->getOrdem(); i < len; i++) {
            distancia[i] = INT64_MAX; // infinito para o CPP
            visitados[i] = false;
        }
        // A distancia depende da origem, no entanto na origem é zero
        distancia[verticeOrigem] = 0;

        // Criaremos uma fila para controlar o percorrimento... O par sera: quem é...sua distancia
        list<pair<unsigned int, unsigned int>> fila;
        
        // Inicio da busca ocorre na origem
        fila.push_back(make_pair(verticeOrigem, distancia[verticeOrigem]));
        
        list<pair<unsigned int, unsigned int> >::iterator i;
        while (!fila.empty()) {
            // Primeiro elemento da fila
            pair<unsigned int, unsigned int> item = fila.front();            
            // Vertice é o primeiro do par
            unsigned int vertice = item.first;
            // Remove da fila
            fila.pop_front();

            // Se não foi visitado, então...
            if (!visitados[vertice]) {
                // Agora foi!
                visitados[vertice] = true;

                // Realiza o percorrimento dos adjacentes
                for (i = this->adjacencia[vertice].begin(); i != this->adjacencia[vertice].end(); i++) {
                    unsigned int verticeAdjacente = i->first;
                    unsigned int valorAdjacente = i->second;

                    // Hora do relaxamento 4i20
                    if (distancia[verticeAdjacente] > (distancia[vertice] + valorAdjacente)) {
                        distancia[verticeAdjacente] = distancia[vertice] + valorAdjacente;
                        fila.push_back(make_pair(verticeAdjacente, distancia[verticeAdjacente]));
                    }

                }
            }
        }

        // retorna a distância mínima até o destino
		return distancia[verticeDestino];
    }

    unsigned int obtemGrau(unsigned int vertice) {
        return this->adjacencia[vertice-1].size();
    }

    unsigned int getOrdem() {
        return ordem;
    }

    unsigned int getTamanho() {
        return tamanho;
    }
};

void importarArquivoLista(string arquivo, GrafoLista &grafo) {
    string linha;
    unsigned int nLinha = 0;
    ifstream arquivoEntrada;

    arquivoEntrada.open(arquivo);
    if (arquivoEntrada.is_open()) {
        while (getline(arquivoEntrada, linha)) {
            unsigned int pos = linha.find(" ");
            if (nLinha == 0) {
                grafo.setOrdem(std::stoul(linha.substr(0, pos)));
                nLinha++; 
                continue;
            }
            //grafo.adicionarArestaNaoDirigida(std::stoul(linha.substr(0, pos)), std::stoul(linha.substr(pos+1, sizeof(linha)+1)), true);
        }
        arquivoEntrada.close();
    }
}

void exportarArquivoLista(string arquivo, GrafoLista &grafo) {
    ofstream arquivoSaida;
    arquivoSaida.open(arquivo);
    if (arquivoSaida.is_open()) {
        arquivoSaida << grafo.getOrdem() << endl;
        arquivoSaida << grafo.getTamanho() << endl;
        arquivoSaida.close();
    }
}

int main() {
    GrafoLista g1;
    g1.setOrdem(6);


    /*
    4 9
    0 1 1
    0 3 4
    0 4 2
    1 2 5
    1 5 3
    2 5 5
    3 4 2
    3 5 5
    4 5 8
    
    */

    g1.adicionarArestaValoradaNaoDirigida(0, 1, 1);
    g1.adicionarArestaValoradaNaoDirigida(0, 3, 4);
    g1.adicionarArestaValoradaNaoDirigida(0, 4, 2);
    g1.adicionarArestaValoradaNaoDirigida(1, 2, 5);
    g1.adicionarArestaValoradaNaoDirigida(1, 5, 3);
    g1.adicionarArestaValoradaNaoDirigida(2, 5, 5);
    g1.adicionarArestaValoradaNaoDirigida(3, 4, 2);
    g1.adicionarArestaValoradaNaoDirigida(3, 5, 5);
    g1.adicionarArestaValoradaNaoDirigida(4, 5, 8);


    unsigned int distancia = g1.dijkstra(0, 5);
    cout << "Distancia: " << distancia;

    return 0;
}