#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <cfloat>
#include <climits>
#include <cassert>
#include <cmath>

//#include <>
#define DIMENSION 4
#define GRADO 4 //de cada nodo
#define LIMITE_RANGE 3 //numero maximo de resultados que queremos devolver
#define RADIO_MAX 6.000
#define ESP 0.5
//ESP del paper

using namespace std;

struct Pair
{
    int indice;
    float distancia;

    // max-heap por defecto: el tope es el de mayor distancia
    bool operator<(const Pair& o) const { return distancia < o.distancia; }
    // necesario para greater<Pair> (min-heap)
    bool operator>(const Pair& o) const { return distancia > o.distancia; }
};

class Vertice
{
    public:
        vector<float> dato;
        vector<Pair> vecino;

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
        void busqueda();
    private:
        void insertarA(int &, int &);
        void busquedaR(vector<float> &datqu, vector<Pair> &final, vector<int >& seeds_begin);
        void borrarA();
        float distancia(const vector<float>& , const vector<float>& );
        bool revMRNG(int &in_1, int &in_2);
        float dis_ono(int& ind, int& ); 
        bool existec(int &ind, int &veci);
};

void Grafo::busqueda()
{

}

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


//eleccion de SEED????? how no se ayuda todavia


//MPI libreria en C, gestionar paralelismo omaiga
//para que no sea tan lento 
//max heap para que no sea tan lento pero haz la bsuqueda en paralelo
void Grafo::busquedaR(vector<float> &datqu,
    vector<Pair> &final,
    vector<int >& seeds_begin)
{
    priority_queue<Pair> final_re;
    priority_queue<Pair, vector<Pair>, greater<Pair>> csemillas ;
    vector<bool> explorado(todo.size(), false);
    for(int i:seeds_begin)
    {
        if(!explorado[i])
        {
            explorado[i] = true;
            float temp = distancia(todo[i].dato , datqu);
            csemillas.push({i , temp});
            final_re.push({i, temp});
            if(final_re.size() > LIMITE_RANGE ) final_re.pop();
        }
        
    
    }
    float di_lejana;//es el r chiquito
    if(final_re.size()>= LIMITE_RANGE) 
    {
        di_lejana = final_re.top().distancia;
    }
    else
    {
        di_lejana = FLT_MAX;
    }
    
    while( csemillas.empty())
    {
        Pair temp = csemillas.top();
        csemillas.pop();
        if(temp.distancia > (di_lejana)*(1+ESP)) 
        return;

        for(Pair & vect: todo[temp.indice].vecino)
        {
            int indice_vis = vect.indice;
            if(explorado[indice_vis]) //si esta explorado ya no
                continue;
            explorado[indice_vis] = true;
//agregas a csemilla si es menor dela distancia permitida y un poco mas
            float dis = sqrt(distancia(todo[indice_vis].dato, datqu));
            if(dis <= (di_lejana) * (1+ESP))
            {
                csemillas.push({indice_vis, dis});
                //agregas a resultado si es mejor que el mas lejos de los resutlados
                if(dis<= di_lejana)
                {
                    final_re.push({indice_vis, dis});
                    if(final_re.size() > LIMITE_RANGE)
                    {
                        final_re.pop();   }
                    //hasta que se llene
                    if(final_re.size() >= LIMITE_RANGE)
                    {
                        di_lejana = final_re.top().distancia;     }
                }

            }

        }   
    }

    
    while (!final_re.empty()) 
    { 
        final.push_back(final_re.top()); 
        final_re.pop(); }
    
    reverse(final.begin(), final.end());  


}

bool Grafo::revMRNG(int &in_1, int &in_2)
{
    float d12 = distancia(todo[in_1].dato, todo[in_2].dato);
    for (Pair & temp : todo[in_1].vecino) {
        int in_3 = temp.indice;
        if (!sonVecinos(in_2, in_2)) 
            continue;      
        if (d12 > max(in_3.distancia, dis_ono(v2, u)))
            return false;
    }
    return true;
}

float Grafo::dis_ono(int& ind, int& ind2) 
{
    for (Pair& temp : todo[ind].vecino)
    {
        if (temp.indice == ind2) 
            return temp.distancia;            
    }
    return FLT_MAX;
    
}

bool Grafo::existec(int &ind, int &veci)
{
    for (Pair & temp : todo[ind].vecino) 
    {
        if (temp.indice == veci) 
    return true;
    }
    
    return false;
}