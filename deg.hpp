#include <iostream>
#include <algorithm>
#include <vector>
//#include <>
using namespace std;

class Node
{
    public:
        vector<float> dato;
        vector<vector<pair<int,float>>> vecino;

};


class Grafo
{
    private:
        int dimension;
        int grado;
        int numero_nodos;
        vector<Node> todo;
    public:
        Grafo(int in_dim, int in_gra) : dimension(in_dim), grado(in_gra)
        {
            numero_nodos = 0;
        }    
        void borrarN();
        void insertarN(vector<float> & in_dato);
    private:
        void insertarA();
        void borrarA();
        float distancia();
};

void Grafo::insertarN(vector<float> & in_dato)
{

}

//range search
//check mrng
//extend graph
//optimize edge
//dynamic edge optimization

void busquedaRango()
{
    
}