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

struct in_dis
{
    int todo_indice;
    float distancia;

    in_dis(){}
};

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
        void busquedaR(vector<float>&, vector<pair<int, float>> & ,vector<pair<int, float>>& );
        void borrarA();
        float distancia(const vector<float>& , const vector<float>& );
        float best_lejano(int & indice, vector<pair<int , float>> &vec);
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



/*
vector<int> con búsqueda lineal será lentísimo con 10000 datos. 
Un vector<bool> del tamaño de todo indexado por id es igual de 
simple y es instantáneo.

*/

//eleccion de SEED????? how
void Grafo::busquedaR(vector<float> &datqu,vector<pair<int, float>> & final_re, vector<pair<int, float>>& seeds)
{
    final_re = seeds;
    vector<int> explorado;
    for(int i = 0;i<seeds.size();i++)
    {
        explorado.push_back(seeds[i].first);
    }
    float di_lejana = FLT_MAX;//del resultado, es el r chiquito
    
    while( seeds.size())
    {
        int indice = -1;
        float minimo_local = FLT_MAX;
        int borrar = -1;
        for(int i = 0;i<seeds.size();i++)
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
        if(distancia(todo[indice].dato, datqu) > di_lejana*(1+ESP))
            return ;
        //vecions de s chiquita
        //lineal por ahora 
        vector<pair<int, float>> no_explorado;//guardamos distancia a query
        for(int i = 0;i<todo[indice].vecino.size();i++)
        {
            int vecino_in = todo[indice].vecino[i].first;
            float dis_qu = distancia(todo[vecino_in].dato, datqu);
            no_explorado.push_back({vecino_in, dis_qu});
        }

        for(pair<int, float> hola:no_explorado)
        {
            for(int i = 0;i<explorado.size();i++)
            {
                if(explorado[i] == hola.first)
                {
                    no_explorado.erase(no_explorado.begin() + i);
                }
            }
        }

        for(int i = 0;i<no_explorado.size();i++)
        {
            float temp = distancia(todo[no_explorado[i].first].dato, datqu);
            if(temp<=di_lejana * (1+ESP))
            {
                seeds.push_back({no_explorado[i].first, temp});
                if(temp<= di_lejana)
                {
                    final_re.push_back({no_explorado[i].first, temp});
                    if(final_re.size() > LIMITE_RANGE )
                    {
                        /*
                        int borrar2 = -1;
                        float temp2 = 0;
                        for(int a = 0;a<final.size();a++)
                        {
                            if(temp< final[a].second)
                            {
                                borrar2 = a;
                                temp2 = final[a].second;
                            }
                        }
                        final.erase(final.begin() + borrar2);
                        temp2 = FLT_MAX;
                        for(int a = 0;a<final.size();a++)
                        {
                            if(temp< final[a].second)
                            {
                                temp2 = final[a].second;
                            }
                        }
                        di_lejana = temp2;
                        
                        */
                        int borrar2 = -1;
                        best_lejano(borrar2, final_re);
                        final_re.erase(final_re.begin() + borrar);
                        di_lejana = best_lejano(borrar2, final_re);
                    }
                }
            }
        }
    
        for(pair<int, float> hola:no_explorado)
        explorado.push_back(hola.first);
    }

}

float Grafo::best_lejano(int & indice, vector<pair<int , float>> &vec)
{
    float temp2 = 0;
    for(int a = 0;a<vec.size();a++)
    {
        if(temp2< vec[a].second)
        {
            indice = a;
            temp2 = vec[a].second;
        }
    }
    return temp2;
}