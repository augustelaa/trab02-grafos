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
    protected:
    unsigned int ordem;
    unsigned int tamanho;
    list<pair<unsigned int, unsigned int>> *adjacencia;
    unsigned int *nivelV;
    unsigned int custoEuleriano;

    public:
    void setOrdem(unsigned int ordem) {
        this->ordem = ordem;
        this->tamanho = 0;
        this->custoEuleriano = 0;
        this->adjacencia = new list<pair<unsigned int, unsigned int>>[ordem];
    }

    void setTamanho(unsigned int tamanho) {
        this->tamanho = tamanho;
    }

    void adicionarArestaValoradaNaoDirigida(unsigned int verticeOrigem, unsigned int verticeDestino, unsigned int valor) {
        this->adjacencia[verticeOrigem].push_back(make_pair(verticeDestino, valor));
        this->adjacencia[verticeDestino].push_back(make_pair(verticeOrigem, valor));
        this->tamanho++;
        this->custoEuleriano += valor;
    }

    unsigned int dijkstra(unsigned int verticeOrigem, unsigned int verticeDestino, list<unsigned int> &rota) {
        unsigned int distancia[this->getOrdem()];
        bool visitados[this->getOrdem()];
        unsigned int roteamento[this->getOrdem()];

        // Inicializa os valores
        rota.clear();
        for (unsigned int i = 0, len = this->getOrdem(); i < len; i++) {
            distancia[i] = INT64_MAX; // infinito para o CPP
            visitados[i] = false;
            roteamento[i] = INT64_MAX;
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
                        roteamento[verticeAdjacente] = vertice;
                    }

                }
            }
        }

        if (distancia[verticeDestino] < this->getOrdem()) {
            unsigned int vertice = verticeDestino;
            while (true) {
                rota.push_back(vertice);
                if (roteamento[vertice] <= this->getOrdem()) {
                    vertice = roteamento[vertice];
                } else {
                    break;
                }
            }

            // retorna a distância mínima até o destino
            return distancia[verticeDestino];
        } else {
            return -1;
        }
    }

    unsigned int obtemGrau(unsigned int vertice) {
        return this->adjacencia[vertice].size();
    }

    unsigned int getCustoEuleriano() {
        return this->custoEuleriano;
    }

    bool isEuleriano() {
        if (this->isDesconexo()) {
            return false;
        }
        for (unsigned int i = 0, len = this->getOrdem(); i < len; i++) {
            if (this->obtemGrau(i) % 2 != 0) {
                return false;
            }
        }
        return true;
    }

    bool fleury(unsigned int origem, list<unsigned int> &ciclo) {
        list<unsigned int> *temp = new list<unsigned int>[this->getOrdem()];
        list<pair<unsigned int, unsigned int> >::iterator j;
        unsigned int tamanho = this->getTamanho();
        int custo = 0;

        for (unsigned int i = 0, len = this->getOrdem(); i < len; i++) {
            for (j = this->adjacencia[i].begin(); j != this->adjacencia[i].end(); ++j) {
                temp[i].push_back(j->first);
            }
        }

        // Criaremos uma fila para controlar o percorrimento... O par sera: quem é...sua distancia
        list<unsigned int> fila;

        // Inicio da busca
        fila.push_back(origem);

        while (!fila.empty()) {
            // Primeiro elemento da fila
            unsigned int vertice = fila.front();
            tamanho--;
            ciclo.push_back(vertice);
            fila.pop_front();

            // Percorre cada vertice
            if (temp[vertice].size() == 0) {
                break;
            }
            unsigned int verticeAdjacente = temp[vertice].front();

            // remove aresta
            temp[vertice].remove(verticeAdjacente);
            temp[verticeAdjacente].remove(vertice);

            fila.push_back(verticeAdjacente);
        }

        // Se ao fim nao passou por todas as arestas, nao é um ciclo Euleriano
        if (tamanho > 0) {
            return false;
        }
        return true;
    }

    void fleury(list<unsigned int> &ciclo) {
        // Faz vertice a vertice até encontrar um fluxo Euleriano
        for (unsigned int i = 0, len = this->getOrdem(); i < len; i++) {
            ciclo.clear();
            if(this->fleury(i, ciclo)) {
                break;
            }
        }
    }

    bool isDesconexo() {
        // Aplicação do algoritmo de goodman
        list<unsigned int> *temp = new list<unsigned int>[this->getOrdem()];
        list<pair<unsigned int, unsigned int> >::iterator j;

        for (unsigned int i = 0, len = this->getOrdem(); i < len; i++) {
            for (j = this->adjacencia[i].begin(); j != this->adjacencia[i].end(); ++j) {
                temp[i].push_back(j->first);
            }
        }

        // Criaremos uma fila para controlar o percorrimento... O par sera: quem é...sua distancia
        list<unsigned int> fila;
        // Inicio da busca
        fila.push_back(0); //TODO:CRIAR FUNCAO PEGA PRIMEIRO,ZERO É INSEGURO

        bool removidos[this->getOrdem()] = {false};
        
        list<unsigned int>::iterator k;

        while (!fila.empty()) {
            // Primeiro elemento da fila
            unsigned int vertice = fila.front();
            fila.pop_front();

            // Percorre cada vertice
            unsigned int verticeAdjacente = 0;
            for (k = temp[vertice].begin(); k != temp[vertice].end(); ++k) {
                if (removidos[*k] || *k == vertice) {
                    continue;
                }
                if (verticeAdjacente == 0) {
                    verticeAdjacente = *k;
                    fila.push_back(verticeAdjacente);
                    continue; // Não cria a ele mesmo
                }
                temp[verticeAdjacente].push_back(*k);
            }
            removidos[vertice] = true;
        }

        for (unsigned int i = 0, len = this->getOrdem(); i < len; i++) {
            if (!removidos[i]) {
                return true;
            }
        }

        return false;
    }

    void eulerizar() {
        bool impares[this->getOrdem()] = {false};
        bool usados[this->getOrdem()] = {false};

        // Encontra os vertices com grau impar
        for (unsigned int i = 0, len = this->getOrdem(); i < len; i++) {
            if (this->obtemGrau(i) % 2 != 0) {
                impares[i] = true;
            }
        }
        unsigned int **distancia = new unsigned int*[this->getOrdem()];
        list<unsigned int> **rotas = new list<unsigned int>*[this->getOrdem()];
        for(unsigned int i = 0, len = this->getOrdem(); i < len; i++) {
            if (impares[i] == true) {
                distancia[i] = new unsigned int[this->getOrdem()]{INT32_MAX};
                rotas[i] = new list<unsigned int>[this->getOrdem()];
            }
        }
        list<unsigned int> rota;
        

        // Calcula a distancia entre eles
        for (unsigned int i = 0, len = this->getOrdem(); i < len; i++) {
            if (impares[i] == false) {
                continue;
            }
            for (unsigned int j = 0, len = this->getOrdem(); j < len; j++) {
                if (i == j || impares[j] == false) {
                    continue;
                }
                distancia[i][j] = this->dijkstra(i, j, rota);
                rotas[i][j] = rota;
                /*{
                    1: 21
                    2: 12
                    3: 43
                    4: 34                    
                }*/
            }
        }

        // Determina as melhores ligações e cria os caminhos virtuais
        vector<pair<unsigned int, unsigned int> > vp;  
        for (unsigned int i = 0; i < this->getOrdem(); i++) {
            if (usados[i] == true || !impares[i]) {
                continue;
            }
            unsigned int *colunas = distancia[i];
            vp.clear();
            for (unsigned int j = 0; j < this->getOrdem(); j++) {
                if (i == j || usados[j] == true || !impares[j]) {
                    continue;
                }
                vp.push_back(make_pair(colunas[j], j));
            }
            sort(vp.begin(), vp.end());
            unsigned int vertice = vp[0].second;
            unsigned int valor = vp[0].first;
            usados[i] = true;
            usados[vertice] = true;

            list<unsigned int> rotaVertice = rotas[i][vertice];
            while (!rotaVertice.empty()) {
                unsigned int origem = rotaVertice.front();
                rotaVertice.pop_front();
                unsigned int destino = rotaVertice.front();
                rotaVertice.pop_front();
                cout << "Caminho virtual de " << origem;
                cout << " para " << destino << " com custo " << valor << " adicionado." << endl;
                this->adicionarArestaValoradaNaoDirigida(origem, destino, valor);
            }
        }
    }

    unsigned int getOrdem() {
        return ordem;
    }

    unsigned int getTamanho() {
        return tamanho;
    }

    void carteiroChines() {
        list<unsigned int> ciclo;
        if (this->isEuleriano()) {
            cout << "Euleriano" << endl;
            this->fleury(ciclo);
            while(!ciclo.empty()) {
                cout << ciclo.front() << " - ";
                ciclo.pop_front();
            }
            cout << "Custo: " << this->getCustoEuleriano();
        } else {
            cout << "Não euleriano" << endl;
            this->eulerizar();
            cout << "Realizando eularização" << endl;
            if (this->isEuleriano()) {
                cout << "Euleriano" << endl;
                this->fleury(ciclo);
                while(!ciclo.empty()) {
                    cout << ciclo.front() << " - ";
                    ciclo.pop_front();
                }
                cout << "Custo: " << this->getCustoEuleriano();
            } else {
                cout << "Deu ruim";
            }
        }
    }
};

int main() {
    /*Grafo g1;
    list<unsigned int> rota;
    g1.setOrdem(6);

    g1.adicionarArestaValoradaNaoDirigida(0, 1, 1);
    g1.adicionarArestaValoradaNaoDirigida(0, 3, 4);
    g1.adicionarArestaValoradaNaoDirigida(0, 4, 2);
    g1.adicionarArestaValoradaNaoDirigida(1, 2, 5);
    g1.adicionarArestaValoradaNaoDirigida(1, 5, 3);
    g1.adicionarArestaValoradaNaoDirigida(2, 5, 5);
    g1.adicionarArestaValoradaNaoDirigida(3, 4, 2);
    g1.adicionarArestaValoradaNaoDirigida(3, 5, 5);
    g1.adicionarArestaValoradaNaoDirigida(4, 5, 8);

    unsigned int distancia = g1.dijkstra(0, 5, rota);
    cout << "Distancia: " << distancia << endl << "Rota: ";
    while(!rota.empty()) {
        cout << rota.front() << " - ";
        rota.pop_front();
    }*/

    Grafo g2;
    
    /*
    g2.setOrdem(7);
    g2.adicionarArestaValoradaNaoDirigida(0, 1, 1);
    g2.adicionarArestaValoradaNaoDirigida(0, 4, 2);
    g2.adicionarArestaValoradaNaoDirigida(1, 2, 2);
    g2.adicionarArestaValoradaNaoDirigida(1, 3, 5);
    g2.adicionarArestaValoradaNaoDirigida(1, 4, 3);
    g2.adicionarArestaValoradaNaoDirigida(2, 6, 5);
    g2.adicionarArestaValoradaNaoDirigida(2, 5, 2);
    g2.adicionarArestaValoradaNaoDirigida(2, 3, 5);
    g2.adicionarArestaValoradaNaoDirigida(3, 4, 8);
    g2.adicionarArestaValoradaNaoDirigida(3, 5, 8);
    g2.adicionarArestaValoradaNaoDirigida(4, 5, 8);
    g2.adicionarArestaValoradaNaoDirigida(5, 6, 8);
    */

    g2.setOrdem(6);
    g2.adicionarArestaValoradaNaoDirigida(0, 1, 1);
    g2.adicionarArestaValoradaNaoDirigida(0, 4, 1);
    g2.adicionarArestaValoradaNaoDirigida(1, 2, 1);
    g2.adicionarArestaValoradaNaoDirigida(2, 3, 1);
    g2.adicionarArestaValoradaNaoDirigida(3, 4, 1);
    g2.adicionarArestaValoradaNaoDirigida(4, 1, 1);
    g2.adicionarArestaValoradaNaoDirigida(2, 5, 1);
    g2.adicionarArestaValoradaNaoDirigida(3, 5, 1);

    /*
    g2.setOrdem(3);
    g2.adicionarArestaValoradaNaoDirigida(0, 1, 1);
    g2.adicionarArestaValoradaNaoDirigida(1, 2, 1);
    g2.adicionarArestaValoradaNaoDirigida(2, 0, 1);

    if (g2.isDesconexo()) {
        cout << "Desconexo";
    } else {
        cout << "Conexo";
    }*/

    /*g2.setOrdem(7);
    g2.adicionarArestaValoradaNaoDirigida(0, 3, 1);
    g2.adicionarArestaValoradaNaoDirigida(0, 4, 1);
    g2.adicionarArestaValoradaNaoDirigida(1, 4, 1);
    g2.adicionarArestaValoradaNaoDirigida(1, 5, 1);
    g2.adicionarArestaValoradaNaoDirigida(2, 5, 1);
    g2.adicionarArestaValoradaNaoDirigida(2, 6, 1);
    g2.adicionarArestaValoradaNaoDirigida(3, 4, 1);
    g2.adicionarArestaValoradaNaoDirigida(4, 5, 1);
    g2.adicionarArestaValoradaNaoDirigida(5, 6, 1);*/

    g2.carteiroChines();

    return 0;
}