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

class Grafo {
    public:
    void adicionarArestaNaoDirigida(unsigned int verticeOrigem, unsigned int verticeDestino, bool somarTamanho);
    unsigned int obtemGrau(unsigned int vertice);
    void setOrdem(unsigned int ordem);
    void printSeqGrau(ofstream &arquivoSaida);
    unsigned int getOrdem();
    unsigned int getTamanho();
    void realizarBuscaProfundidade(unsigned int verticeOrigem, bool exportar, string arquivo);
    bool realizarBuscaLargura(unsigned int verticeOrigem, bool exportar, string arquivo);
    unsigned int getNivelV(unsigned int vertice);
};

class GrafoLista : public Grafo {
    protected:
    unsigned int ordem;
    unsigned int tamanho;
    list<unsigned int> *adjacencia;
    unsigned int *nivelV;

    // método que utiliza a recursão para percorrer toda a lista de adjacencia do vértice em questão
    void realizarVisitaVertice(unsigned int vertice, bool visitados[], unsigned int pai[], int nivel, unsigned int nivelV[]) {
        // oficialmente ele foi visitado
        visitados[vertice] = true;
        //cout << "Vertice " << (vertice+1) << " foi visitado." << endl;

        // vamos visitar todas as adjacencias do vertice agora....
        list<unsigned int>::iterator i;
        nivel++;
        for (i = this->adjacencia[vertice].begin(); i != this->adjacencia[vertice].end(); ++i) {
            // a adjacencia ainda não foi visitada?
            if (!visitados[*i]) {
                // pois agora será...
                pai[*i] = vertice;
                nivelV[*i] = nivel;
                realizarVisitaVertice(*i, visitados, pai, nivel, nivelV);
            }
        }
    }

    public:
    ~GrafoLista() {
        delete this->adjacencia;
    }

    void setOrdem(unsigned int ordem) {
        this->ordem = ordem;
        this->tamanho = 0;
        this->adjacencia = new list<unsigned int>[ordem];
    }

    void setTamanho(unsigned int tamanho) {
        this->tamanho = tamanho;
    }

    void adicionarArestaNaoDirigida(unsigned int verticeOrigem, unsigned int verticeDestino, bool somarTamanho) {
        this->adjacencia[verticeOrigem-1].push_back(verticeDestino-1);
        this->adjacencia[verticeDestino-1].push_back(verticeOrigem-1);
        if (somarTamanho) {
            this->tamanho++;
        }
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

    void printSeqGrau(ofstream &arquivoSaida) {
        arquivoSaida << "(";
        vector<unsigned int> graus;
        
        for (unsigned int i = 1; i <= ordem; i++) {
            graus.push_back(this->obtemGrau(i));
        }
        std::sort(graus.begin(), graus.end());
        for (unsigned int i = 0, len = graus.size(); i < len; i++) {
            arquivoSaida << graus[i];
            if ((i+1) < len) arquivoSaida << ",";
        }
        arquivoSaida << ")";
    }

    void realizarBuscaProfundidade(unsigned int verticeOrigem, bool exportar, string arquivo) {
        //cout << "BFS ... Origem: " << verticeOrigem << endl;
        
        // inicialmente nenhum vértice foi visitado
        bool *visitados = new bool[ordem]{false};
        unsigned int *pai = new unsigned int[ordem]{0};
        unsigned int nivel = 0;
        this->nivelV = new unsigned int[ordem]{0};

        // iremos buscar à partir de um vertice origem
        this->realizarVisitaVertice((verticeOrigem-1), visitados, pai, nivel, this->nivelV);
        
        if (exportar) {
            ofstream arquivoSaida;
            arquivoSaida.open(arquivo);
            if (arquivoSaida.is_open()) {
                for (unsigned int i = 0; i < ordem; i++) {
                    arquivoSaida << "[V] " << (i+1) << " [P] ";
                    if (this->nivelV[i] == 0) {
                        arquivoSaida << "n";
                    } else {
                        arquivoSaida << (pai[i]+1);
                    }
                    arquivoSaida << " [L] " << this->nivelV[i] << endl;
                }
                arquivoSaida.close();
            }
        }
    }

    bool realizarBuscaLargura(unsigned int verticeOrigem, bool exportar, string arquivo) {
        //cout << "BFS ... Origem: " << verticeOrigem << endl;

        // inicialmente nenhum vértice foi visitado
        bool *visitados = new bool[ordem]{false};
        unsigned int *pai = new unsigned int[ordem]{0};
        unsigned int nivel = 0;
        this->nivelV = new unsigned int[ordem]{0};

        // o algoritmo se baseia na fila
        list<unsigned int> fila;

        // oficialmente ele foi visitado
        visitados[(verticeOrigem-1)] = true;
        // empilha o vertice visitado no final
        fila.push_back((verticeOrigem-1));

        list<unsigned int>::iterator i;
        while (!fila.empty()) {
            // vertice à ser explorado
            unsigned int vertice = fila.front();
            //cout << "Vertice " << (vertice+1) << " foi visitado." << endl;
            // desempilha o vertice que está sendo explorado
            fila.pop_front();
            nivel++;

            // vamos visitar todas as adjacencias do vertice agora....
            for (i = this->adjacencia[vertice].begin(); i != this->adjacencia[vertice].end(); ++i) {
                // a adjacencia ainda não foi visitada?
                if (!visitados[*i]) {
                    pai[*i] = vertice;
                    this->nivelV[*i] = nivel;
                    // pois agora será...
                    visitados[*i] = true;
                    // empilha ela para percorrer também
                    fila.push_back(*i);
                }
            }
        }

        if (exportar) {
            ofstream arquivoSaida;
            arquivoSaida.open(arquivo);
            if (arquivoSaida.is_open()) {
                for (unsigned int i = 0; i < ordem; i++) {
                    arquivoSaida << "[V] " << (i+1) << " [P] ";
                    if (this->nivelV[i] == 0) {
                        arquivoSaida << "n";
                    } else {
                        arquivoSaida << (pai[i]+1);
                    }
                    arquivoSaida << " [L] " << this->nivelV[i] << endl;
                }
                arquivoSaida.close();
            }
        }

        for (int i = 0; i < ordem; i++) {
            if (!visitados[i]) {
                return false;
            }
        }
        return true;
    }

    unsigned int getNivelV(unsigned int vertice) {
        return this->nivelV[vertice];
    }
};

void importarArquivoLista(string arquivo, GrafoLista &grafo) {
    string linha;
    unsigned int nLinha = 0;
    ifstream arquivoEntrada;

    arquivoEntrada.open(arquivo);
    if (arquivoEntrada.is_open()) {
        while (getline(arquivoEntrada, linha)) {
            if (nLinha == 0) {
                grafo.setOrdem(std::stoull(linha));
                nLinha++; 
                continue;
            }
            unsigned int pos = linha.find(" ");
            grafo.adicionarArestaNaoDirigida(std::stoul(linha.substr(0, pos)), std::stoul(linha.substr(pos+1, sizeof(linha)+1)), true);
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
        grafo.printSeqGrau(arquivoSaida);
        arquivoSaida.close();
    }
}

int main() {
    GrafoLista g1;
    importarArquivoLista("grafo5.txt", g1);

    return 0;
}