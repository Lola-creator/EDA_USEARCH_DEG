#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <cfloat>
#include <climits>
#include <cassert>
#include <cmath>

//#include <>
#define DIMENSION 2
#define GRADO 4 //de cada nodo
#define LIMITE_RANGE 60 //numero maximo de resultados que queremos devolver, es el k
#define EPS 0.2f
#define MAX_SWAP 12
//ESP del paper

using namespace std;

struct Pair
{
    int indice;
    float distancia;
  
    bool operator<(const Pair& o) const { return distancia < o.distancia; }
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
        vector<Pair> busqueda(vector<float> &datqu);
        void verificar();
    private:
        void insertarA(int &, int &);
        void busquedaR(vector<float> &datqu, vector<Pair> &final, vector<int >& seeds_begin);
        void borrarA();
        float distancia(const vector<float>& , const vector<float>& );
        bool revMRNG(int &in_1, int &in_2);
        float dis_ono(int& ind, int& ); 
        bool existec(int &ind, int &veci);
        void balanceo(int& indice, vector<int>& seeds);//extender el grafo pero para mi balanceo
        void quitarAr(int inde1, int inde2);
        void dynamicMeAr();
        bool mejorarAr(int v1, int v2);
};
void Grafo::verificar()
{
    int esperado = min(GRADO, (int)todo.size() - 1);   
    for (int i = 0; i < (int)todo.size(); i++)
    {
        assert((int)todo[i].vecino.size() == esperado);   
        vector<int> indi;//ces
        for (Pair& tp : todo[i].vecino)
        {
            int j = tp.indice;
            assert(j != i);
            assert(existec(j, i));
            indi.push_back(j);
        }
        sort(indi.begin(), indi.end());
        assert(adjacent_find(indi.begin(), indi.end()) == indi.end());
    }
}
vector<Pair> Grafo::busqueda(vector<float> &datqu)
{
    vector<Pair> final;
    vector<int >seeds = {0};
    busquedaR(datqu, final, seeds);

    for(int i = 0; i<final.size();i++)
    {
        int indice = final[i].indice;
        for(int z = 0;z<DIMENSION;z++)
        {
            cout<<todo[indice].dato[z]<<"  ";
        }
        cout<<endl;
    }
    return final;
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

    }
    else
    {
        vector<int> semillas = {0};      // semilla arbitraria, comdice el paper
        balanceo(indice, semillas);
    }
    numero_nodos++;
}

float Grafo::distancia(const vector<float>& uno, const vector<float>& dos)
{
    float suma = 0.0f;
    for(int i = 0;i<uno.size();i++)
    suma+= (uno[i] - dos[i]) * (uno[i] - dos[i]);
    return sqrt(suma);
    
}


//eleccion de SEED????? how no se ayuda todavia
//es random noma, fue pero puedo mejorarlo para la busqueda omg

//nNOTAS PROFE ERICK
//MPI libreria en C, gestionar paralelismo omaiga
//para que no sea tan lento , DESPUES omg

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
    
    while(!csemillas.empty())
    {
        Pair temp = csemillas.top();
        csemillas.pop();
        if(temp.distancia > (di_lejana)*(1+EPS)) 
        break;

        for(Pair & vect: todo[temp.indice].vecino)
        {
            int indice_vis = vect.indice;
            if(explorado[indice_vis]) //si esta explorado ya no
                continue;
            explorado[indice_vis] = true;
//agregas a csemilla si es menor dela distancia permitida y un poco mas
            float dis = (distancia(todo[indice_vis].dato, datqu));
            if(dis <= (di_lejana) * (1+EPS))
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
    float dentre = distancia(todo[in_1].dato, todo[in_2].dato);
    for (Pair & temp : todo[in_1].vecino) {
        int in_3 = temp.indice;
        if (!existec(in_2, in_3)) //vuelves al incio de bucle pue
            continue;      
        if (dentre > max(temp.distancia, dis_ono(in_2, in_3)))
            return false;//significa que no es suficientemente importante
            //por es o eliminas
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

bool ya_esta(int x, vector<int> &vecinos) {
    for (int e : vecinos)
        if (e == x) return true;
    return false;
}
void Grafo::balanceo(int& indice, vector<int>& seeds)
{
    vector<Pair> mascerca;
    busquedaR(todo[indice].dato, mascerca, seeds);

    vector<int> vecinos;      // vecinos del dato agregado osea indice
    bool noRNG = false;

    while (vecinos.size() < GRADO)
    {
        int contar = vecinos.size();

        for (int i = 0; i < mascerca.size() && vecinos.size() < GRADO; i++)
        {
            int b = mascerca[i].indice;
            //ya esta dentro de vecinos
            if (ya_esta(b, vecinos)) 
                continue;                 
            if (!(noRNG || revMRNG(indice, b))) 
                continue;  // filtro de calidad

            int n = -1;
            float mejor = -1;
            for (Pair& temp : todo[b].vecino)
                if (!ya_esta(temp.indice, vecinos) && temp.distancia > mejor)
                {
                    mejor = temp.distancia;
                    n = temp.indice;
                }
            if (n == -1) continue;

            quitarAr(b, n);
            vecinos.push_back(n);
            vecinos.push_back(b);
        }

        if (noRNG && vecinos.size() == contar) 
            break;  // no avanzo: salir
        noRNG = true;                                   
    }

    assert((int)vecinos.size() == GRADO);

    for (int e : vecinos)
        insertarA(indice, e);
}

void Grafo::quitarAr(int inde1, int inde2)
{
    vector<Pair>& lista_a = todo[inde1].vecino;
    for (int i = 0; i < lista_a.size(); i++)
        if (lista_a[i].indice == inde2)
        {
            lista_a[i] = lista_a.back();   
            lista_a.pop_back();           
            break;
        }

    vector<Pair>& lista_b = todo[inde2].vecino;
    for (int i = 0; i < lista_b.size(); i++)
        if (lista_b[i].indice == inde1)
        {
            lista_b[i] = lista_b.back();
            lista_b.pop_back();
            break;
        }
}


struct Cambio { bool agregada; int ind1; int ind2; };

static bool contiene(const vector<Pair>& S, int x)
{
    for (const Pair& p : S) if (p.indice == x) return true;
    return false;
}

bool Grafo::mejorarAr(int in_ind1, int in_ind2)
{
    //ind1 y ind2 es una arista larga y mal ayudaa
    vector<Cambio> cambiost;//temporal muy muy muy necesario
    float sin_arista = dis_ono(in_ind1, in_ind2);//lo que gano sin esta arista
    quitarAr(in_ind1, in_ind2);
    cambiost.push_back({false, in_ind1, in_ind2});
    int ult_1 = in_ind1;//ultimas extremos
    int ult_2 = in_ind2;

    while (cambiost.size() < MAX_SWAP)
    {

        float b = sin_arista;//hacems una copia
        vector<Pair> mascercanos;
        vector<int> sem = {ult_1, ult_2};//
        busquedaR(todo[in_ind2].dato, mascercanos , sem);
//cercanos a indidce 2, las semilla son el mismo 1 y 2
        int nv3 = ult_1;
        int nv4 = ult_2;
        for (Pair& ps : mascercanos)
        {
            int s = ps.indice;
            if (s == in_ind1 || s == in_ind2 || 
                existec(in_ind2, s)) 
                continue;
            for (Pair& pn : todo[s].vecino)
            {
                int n = pn.indice;
                if (n == in_ind2) 
                    continue;
                
                float cand = sin_arista - ps.distancia + pn.distancia;
                
                if (b < cand) 
                {   
                    b = cand; 
                    nv3 = s; 
                    nv4 = n; 
                }
            }
        }
        if (b == sin_arista) 
            break;          // sin mejora debemos revertir porque no se pudo mejorar
        ult_1 = nv3; 
        ult_2 = nv4; 
        sin_arista = b;

        insertarA(in_ind2, ult_1);  
        cambiost.push_back({true,  in_ind2, ult_1});
        quitarAr(ult_1, ult_2);   
        cambiost.push_back({false, ult_1, ult_2});

        if (in_ind1 == ult_2)
        {
            float b2 = 0; int v5 = -1, v6 = -1;
            vector<Pair> S2;
            vector<int> sem2 = {in_ind2, ult_1};
            busquedaR(todo[in_ind1].dato, S2, sem2);
            for (Pair& ps : S2)
            {
                int s = ps.indice;
                if (s == in_ind1 || existec(in_ind1, s)) continue;
                for (Pair& pn : todo[s].vecino)
                {
                    int n = pn.indice;
                    if (n == in_ind1 || existec(in_ind1, n)) 
                        continue;   
                    float cand = sin_arista + pn.distancia - ps.distancia
                               - distancia(todo[n].dato, todo[in_ind1].dato);
                    if (b2 < cand) 
                    {   b2 = cand; 
                        v5 = s; 
                        v6 = n; 
                    }
                }
            }
            if (b2 > 0)
            {
                insertarA(in_ind1, v5);
                insertarA(in_ind1, v6);
                quitarAr(v5, v6);
                return true;
            }
        }
        else if (!existec(in_ind1, ult_2) &&
         sin_arista - distancia(todo[in_ind1].dato, todo[ult_2].dato) > 0)
        {
            vector<Pair> S1, S4;
            vector<int> sem2 = {in_ind2, ult_1};
            busquedaR(todo[in_ind1].dato, S1, sem2);
            busquedaR(todo[ult_2].dato, S4, sem2);

            if (contiene(S1, in_ind1) || contiene(S4, ult_2))
            {
                insertarA(in_ind1, ult_2);
                return true;
            }
        }

        int v = ult_2; 
        ult_2 = ult_1; 
        ult_1 = in_ind2; 
        in_ind2 = v;
    }

    for (int i = cambiost.size() - 1; i >= 0; i--)
    {
        if (cambiost[i].agregada) 
        {
            quitarAr(cambiost[i].ind1, cambiost[i].ind2);
        }
        else  
        {
            insertarA(cambiost[i].ind1, cambiost[i].ind2);
        }             
    }
    return false;
}

void Grafo::dynamicMeAr()
{
    if (todo.size() <= GRADO + 1) 
        return;   

    int v1 = rand() % todo.size();
    vector<int> N;                                
    for (Pair& p : todo[v1].vecino) 
    {   N.push_back(p.indice);}

    for (int v2 : N)
        if (existec(v1, v2) && !revMRNG(v1, v2))  
        {
            mejorarAr(v1, v2);
        }

    int v2 = -1; float mx = -1;                   
    for (Pair& p : todo[v1].vecino)
    {
        if (p.distancia > mx) 
        { 
            mx = p.distancia; 
            v2 = p.indice; 
        }
    }
        
    if (v2 != -1) 
    {
        mejorarAr(v1, v2);
    }
}