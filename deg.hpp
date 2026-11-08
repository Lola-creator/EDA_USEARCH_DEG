#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <cfloat>
#include <climits>
#include <cassert>

//#include <>
#define DIMENSION 4
#define GRADO 4 //de cada nodo
#define LIMITE_RANGE 5 //numero maximo de resultados que queremos devolver
#define RADIO_MAX 6.000
#define ESP 0.001
//ESP del paper

using namespace std;

class Vertice
{
    public:
        vector<float> dato;
        vector<pair<int,float>> vecino;

};

class Grafo
{
    private:

        int numero_nodos;
        vector<Vertice> todo;
    public:
        int get_num_nodos()
        {return numero_nodos;}
        Vertice get_vertice(int & i)
        {
            return todo[i];
        }

        Grafo( )
        { numero_nodos = 0; }    

        void borrarN();
        void insertar(vector<float> & in_dato);
    private:
        void insertarA(int &, int &);
        void busquedaR(vector<float> & , queue<Vertice>&);
        void borrarA();
        float distancia(const vector<float>& , const vector<float>& );
};

void Grafo::insertarA(int& indice1, int& indice2 )
{
    float peso = distancia(todo[indice1].dato,  todo[indice2].dato);
    todo[indice1].vecino.push_back({indice2, peso});
    todo[indice2].vecino.push_back({indice1, peso});
}

void Grafo::insertar(vector<float> & in_dato)
{
    Vertice insertar;
    insertar.dato = in_dato;
    todo.push_back(insertar);
    int indice = todo.size() -1;
    if(GRADO+1>=todo.size() )
    {
        for(int indi = 0;indi<indice ;indi++)
        {
            insertarA(indi, indice);
            
        }
        numero_nodos++;
         for (auto& v : todo)
            //assert(v.vecino.size() == GRADO);
        return;
    }
   


}

float Grafo::distancia(const vector<float>& uno, const vector<float>& dos)
{
    float suma = 0.0f;
    for(int i = 0;i<uno.size();i++)
    suma+= (uno[i] - dos[i]) * (uno[i] - dos[i]);
    return suma;

}
/* //range search
//check mrng
//extend graph
//optimize edge
//dynamic edge optimization */

//eleccion de SEED????? how
void Grafo::busquedaR(vector<float> &datqu,vector<pair<int, float>>& seeds)
{
    vector<pair<int, float>> final = seeds;
    float di_lejana = FLT_MAX;//del resultado, es el r chiquito
    vector<pair<int, float>> no_explorado = seeds;

    while( seeds.size())
    {
        int indice = -1;
        float minimo_local = FLT_MAX;
        int borrar = -1;
        for(int i = 0;seeds.size();i++)
        {
            if(minimo_local>distancia(todo[seeds[i].first].dato, datqu))
            {
                indice = seeds[i].first;
                minimo_local = distancia(todo[seeds[i].first].dato, datqu);
                borrar = i;
            }
        }

        seeds.erase(seeds.begin()+borrar);

        //condicion de parada

        for(int i = 0;i<seed.vecino.size();i++)
        {
            Vertice temp{todo[seed.vecino[i].first]};
            if(distancia(temp.dato, datqu) <= (di_lejana * (1+ESP)) )
                no_explorado.push();
        }
            //que seria el nuevo seed
        Vertice cercano{no_explorado.front()};
        float minimo_local = INT_MAX;
        while(no_explorado.size())
        {
            //elejirr vertice
            Vertice valor{no_explorado.front()};
            no_explorado.pop();
            
            float local = distancia(valor.dato, datqu);
            
            if(minimo_local>local)
            {
                minimo_local = local;
                cercano = valor;
            }
            
            
        }
        seed = cercano;
    }

}