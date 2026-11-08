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
        void busquedaR(vector<float> &dato, 
                    vector<Vertice> );
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
void Grafo::busquedaR(vector<float> &datqu, 
                    vector<Vertice> resultado)
{
    //no explorado == seeds osea respecto a seed NO LO ENTENDI MAL AYUDAAAA
    //agarras y agregas los vecionos
    float di_lejana = FLT_MAX;
    vector<Vertice> ya_revisado;//se supone que esta vacio
    float limite_D = FLT_MAX;
    Vertice seed;
    while(//encontramos el valor o se salio del radio
        true)
    {
        queue<Vertice> no_explorado;
        for(int i = 0;i<seed.vecino.size();i++)
        {
            Vertice temp{todo[seed.vecino[i].first]};
            no_explorado.push(temp);
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