#pragma once
#include <cstdlib>
#include <random>
#include "hnswlib/hnswlib/hnswlib.h"
#include <numeric>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <cfloat>
#include <climits>
#include <cassert>
#include <cmath>
#include <string>
#include <stdexcept>
#include <fstream>

//#include <>
//#define DIMENSION 2
#define GRADO 30 
#define LIMITE_RANGE 60 
#define EPS 0.2f
#define MAX_SWAP 5           
#define OPT_CADA 50          
#define OPT_FINAL 0
#ifndef OPT_NUEVOS
#define OPT_NUEVOS 0
#endif          // 1 = ademas un lote extra al terminar
#define K_OPT 30             // mejorAR
#define EPS_OPT 0.001f       // mejorarAR
#ifndef N_PIVOTES
#define N_PIVOTES 64
#endif
//ESP del paper

using namespace std;
struct Temporal 
{ 
    bool agregada; 
    int ind1; 
    int ind2; 
};

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

        int num;
        vector<Vertice> todo;
        vector<int> now; 
        
        int entrada = 0;    
        mt19937 gen{42};
        int iter = 0;

        
        hnswlib::L2Space* esp = nullptr;
        hnswlib::DISTFUNC<float> fdist = nullptr;
        void* fparam = nullptr;

        bool compacto = false;
        int dimension = 0;
        vector<float> plano_dato;     
        vector<int> plano_vec;        
        vector<int> pivotes;          
        vector<Pair> cola_c;          
        vector<Pair> cola_r;


    public:
        int get_num_nodos()
        {return num;;}
        Vertice get_vertice(int & i)
        {
            return todo[i];
        }

        Grafo( )
        { num = 0; }
        ~Grafo() { delete esp; }
        Grafo(const Grafo&) = delete;              // esp es un puntero: no copiar el grafo
        Grafo& operator=(const Grafo&) = delete;
        long long n_dist = 0;
        void borrarN();
        double peso_promedio();
        void insertar(vector<float> & in_dato);
        vector<Pair> buscar(vector<float> &datqu, int k, float eps);
        void verificar();
        void dynamicMeAr();
        void optimizarVertice(int v1);
        static int cuantas_opt(size_t N);
        void optimizarLote(int n_opt = -1, int estrategia = 0);
        bool conexo();
        void calcular_entrada();   // [CAMBIO] llamar una vez al terminar de construir
        void compactar();
        bool guardar(string archivo, double tiempo_construccion);
        bool cargar(string archivo, double& tiempo_construccion);
    private:
        void insertarA(int &, int &);
        void busquedaR(vector<float> &datqu, vector<Pair> &final, vector<int >& seeds_begin,
                     int k = LIMITE_RANGE, float eps = EPS);
        void borrarA();
        float distancia(const vector<float>& , const vector<float>& );
        bool revMRNG( int &in_2, float& dentre, vector<Pair>& vecinos);
        float dis_ono(int& ind, int& ); 
        bool existec(int &ind, int &veci);
        void balanceo(int& indice, vector<int>& seeds);//extender el grafo pero para mi balanceo
        void quitarAr(int inde1, int inde2);
        
        bool mejorarAr(int v1, int v2);
        void busquedaPlana(vector<float> &datqu, vector<Pair> &final, int k, float eps);
};

double Grafo::peso_promedio()
{
    double s = 0; long c = 0;
    for (Vertice& v : todo)
    {
        for (Pair& p : v.vecino)
        { 
            s += p.distancia; 
            c++; 
        }
    }  
    if (c == 0)
        return 0;
    return s / c;
}
bool Grafo::conexo()
{
    if (todo.empty()) 
        return true;
    vector<bool> vis(todo.size(), false);
    queue<int> q; q.push(0); vis[0] = true; int cont = 1;
    while (!q.empty()) 
    {
        int u = q.front(); q.pop();
        for (Pair& p : todo[u].vecino)
            if (!vis[p.indice]) 
            { 
                vis[p.indice] = true; 
                cont++; 
                q.push(p.indice); 
            }
    }
    return cont == (int)todo.size();
}

void Grafo::calcular_entrada()
{
    if (todo.empty()) 
        return;

    int dim = todo[0].dato.size();

    vector<double> c(dim, 0.0);

    for (Vertice& v : todo)
        for (int i = 0; i < dim; i++) 
            c[i] += v.dato[i];

    vector<float> cf(dim);
    for (int i = 0; i < dim; i++) 
        cf[i] = (float)(c[i] / todo.size());


    float mejor = FLT_MAX;
    for (int i = 0; i < todo.size(); i++)
    {
        float d = distancia(todo[i].dato, cf);
        if (d < mejor) 
        { 
            mejor = d; 
            
            entrada = i;
        }
    }
}

void falla(string m)
{
    throw runtime_error("verificar(): " + m);
}

void Grafo::verificar()
{
    int esperado = min(GRADO, (int)todo.size() - 1);
    for (int i = 0; i < (int)todo.size(); i++)
    {
        if ((int)todo[i].vecino.size() != esperado)
            falla("vertice " + to_string(i) + " tiene grado " + to_string(todo[i].vecino.size()));
        vector<int> indi;
        for (Pair& tp : todo[i].vecino)
        {
            int j = tp.indice;
            if (j == i) falla("lazo en " + to_string(i));
            if (!existec(j, i)) falla("arista no simetrica " + to_string(i) + "-" + to_string(j));
            indi.push_back(j);
        }
        sort(indi.begin(), indi.end());
        if (adjacent_find(indi.begin(), indi.end()) != indi.end())
            falla("arista repetida en " + to_string(i));
    }
}
vector<Pair> Grafo::buscar(vector<float> &datqu, int kconsulta, float eps)
{
    vector<Pair> final;
    if (compacto)
    {
        busquedaPlana(datqu, final, kconsulta, eps);   
        return final;
    }
    vector<int >seeds = {entrada};  
    busquedaR(datqu, final, seeds, kconsulta, eps);

    return final;
}

bool Grafo::guardar(string archivo, double tiempo_construccion)
{
    if (!compacto) return false;
    ofstream f(archivo, ios::binary);
    if (!f.is_open()) return false;
    char marca_archivo[4] = {'D', 'E', 'G', '1'};
    int grado = GRADO;
    int npiv = pivotes.size();
    f.write(marca_archivo, 4);
    f.write((char*)&num, sizeof(int));
    f.write((char*)&dimension, sizeof(int));
    f.write((char*)&grado, sizeof(int));
    f.write((char*)&entrada, sizeof(int));
    f.write((char*)&npiv, sizeof(int));
    f.write((char*)&tiempo_construccion, sizeof(double));
    f.write((char*)plano_dato.data(), plano_dato.size() * sizeof(float));
    f.write((char*)plano_vec.data(), plano_vec.size() * sizeof(int));
    f.write((char*)pivotes.data(), pivotes.size() * sizeof(int));
    return f.good();
}

bool Grafo::cargar(string archivo, double& tiempo_construccion)
{
    ifstream f(archivo, ios::binary);
    if (!f.is_open()) return false;
    char marca_archivo[4];
    int grado = 0;
    int npiv = 0;
    f.read(marca_archivo, 4);
    if (marca_archivo[0] != 'D' || marca_archivo[1] != 'E' || marca_archivo[2] != 'G' || marca_archivo[3] != '1')
        return false;
    f.read((char*)&num, sizeof(int));
    f.read((char*)&dimension, sizeof(int));
    f.read((char*)&grado, sizeof(int));
    f.read((char*)&entrada, sizeof(int));
    f.read((char*)&npiv, sizeof(int));
    f.read((char*)&tiempo_construccion, sizeof(double));
    if (grado != GRADO) return false;

    plano_dato.resize((size_t)num * dimension);
    plano_vec.resize((size_t)num * GRADO);
    pivotes.resize(npiv);
    f.read((char*)plano_dato.data(), plano_dato.size() * sizeof(float));
    f.read((char*)plano_vec.data(), plano_vec.size() * sizeof(int));
    f.read((char*)pivotes.data(), pivotes.size() * sizeof(int));
    if (!f.good()) return false;

    if (!esp)
    {
        esp = new hnswlib::L2Space(dimension);
        fdist = esp->get_dist_func();
        fparam = esp->get_dist_func_param();
    }
    now.assign(num, 0);
    iter = 0;
    compacto = true;
    return true;
}

void Grafo::compactar()
{
    if (todo.empty()) 
        return;
    dimension = todo[0].dato.size();
    plano_dato.assign((int)num * dimension, 0.0f);
    plano_vec.assign((int)num * GRADO, 0);
    for (int i = 0; i < num; i++)
    {
        for (int t = 0; t < dimension; t++)
        {
            plano_dato[(int)i * dimension + t] = todo[i].dato[t];
        }
        int g = todo[i].vecino.size();
        for (int j = 0; j < GRADO; j++)
        {
            if (j < g)
                plano_vec[(int)i * GRADO + j] = todo[i].vecino[j].indice;
            else
                plano_vec[(int)i * GRADO + j] = todo[i].vecino[0].indice;
        }
    }

    pivotes.clear();
    pivotes.push_back(entrada);
    for (int p = 0; p < N_PIVOTES && p < num; p++)
        pivotes.push_back((int)(gen() % num));

    if (now.size() < (size_t)num) now.resize(num, 0);
    compacto = true;
}

void Grafo::busquedaPlana(vector<float> &datqu, vector<Pair> &final, int k, float eps)
{
    iter++;
    float* q = datqu.data();
    float factor = (1 + eps) * (1 + eps);

    cola_c.clear();     
    cola_r.clear();     

    int semilla = entrada;
    float dsem = FLT_MAX;
    for (int p = 0; p < (int)pivotes.size(); p++)
    {
        int idx = pivotes[p];
        if (now[idx] == iter) continue;
        now[idx] = iter;
        n_dist++;
        float d = fdist(q, &plano_dato[(size_t)idx * dimension], fparam);
        if (d < dsem) { dsem = d; semilla = idx; }
        cola_r.push_back({idx, d});
        push_heap(cola_r.begin(), cola_r.end());
        if ((int)cola_r.size() > k) { pop_heap(cola_r.begin(), cola_r.end()); cola_r.pop_back(); }
    }
    cola_c.push_back({semilla, dsem});

    float di_lejana = FLT_MAX;
    float limite = FLT_MAX;
    if ((int)cola_r.size() >= k) { di_lejana = cola_r.front().distancia; limite = di_lejana * factor; }

    int nuevos[GRADO];
    while (!cola_c.empty())
    {
        pop_heap(cola_c.begin(), cola_c.end(), greater<Pair>());
        Pair temp = cola_c.back();
        cola_c.pop_back();
        if (temp.distancia > limite)
            break;

        int* veci = &plano_vec[(size_t)temp.indice * GRADO];
        int m = 0;
        for (int j = 0; j < GRADO; j++)
        {
            int ind = veci[j];
            if (now[ind] == iter) continue;
            now[ind] = iter;
            nuevos[m++] = ind;
            char* pm = (char*)&plano_dato[(size_t)ind * dimension];
            for (int o = 0; o < dimension * 4; o += 64)
                __builtin_prefetch(pm + o);
        }
        for (int t = 0; t < m; t++)
        {
            int ind = nuevos[t];
            n_dist++;
            float dis = fdist(q, &plano_dato[(size_t)ind * dimension], fparam);
            if (dis <= limite)
            {
                cola_c.push_back({ind, dis});
                push_heap(cola_c.begin(), cola_c.end(), greater<Pair>());
                if (dis <= di_lejana)
                {
                    cola_r.push_back({ind, dis});
                    push_heap(cola_r.begin(), cola_r.end());
                    if ((int)cola_r.size() > k) { pop_heap(cola_r.begin(), cola_r.end()); cola_r.pop_back(); }
                    if ((int)cola_r.size() >= k) { di_lejana = cola_r.front().distancia; limite = di_lejana * factor; }
                }
            }
        }
    }

    sort_heap(cola_r.begin(), cola_r.end());
    final.resize(cola_r.size());
    for (int i = 0; i < (int)cola_r.size(); i++)
        final[i] = {cola_r[i].indice, sqrt(cola_r[i].distancia)};
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
        vector<int> semi = {0};      
        balanceo(indice, semi);
    }
    num++;
}

float Grafo::distancia(const vector<float>& uno, const vector<float>& dos)
{
    n_dist++;
    if (!esp)
    {
        esp = new hnswlib::L2Space(uno.size());
        fdist = esp->get_dist_func();
        fparam = esp->get_dist_func_param();
    }
    return sqrt(fdist(uno.data(), dos.data(), fparam));
}


//eleccion de SEED????? how no se ayuda todavia
//es random noma, fue pero puedo mejorarlo para la busqueda omg

//nNOTAS PROFE ERICK
//MPI libreria en C, gestionar paralelismo omaiga
//para que no sea tan lento , DESPUES omg

void Grafo::busquedaR(vector<float> &datqu, vector<Pair> &final,
                      vector<int>& seeds_begin, int k, float eps)
{
    priority_queue<Pair> final_re;//max hea[p] vammmoooo
    priority_queue<Pair, vector<Pair>, greater<Pair>> csemillas;

    if (now.size() < todo.size()) 
    {   
        now.resize(todo.size(), 0); }
    iter++;

    for (int i : seeds_begin)
    {
        if (now[i] != iter)
        {
            now[i] = iter;
            float temp = distancia(todo[i].dato, datqu);
            csemillas.push({i, temp});
            final_re.push({i, temp});
            if (final_re.size() > k) 
            { 
                final_re.pop();
            }
        }
    }
  

    float di_lejana = FLT_MAX;
    if(final_re.size() >= k)
    {
        di_lejana = final_re.top().distancia;
    }

    while (!csemillas.empty())//mas cercano a da
    {
        Pair temp = csemillas.top();
        csemillas.pop();
        if (temp.distancia > di_lejana*(1 + eps)) 
            break;

        for (Pair& veci : todo[temp.indice].vecino)
        {
            int ind = veci.indice;
            if (now[ind] == iter) //avoid visitados sino circulo
                continue;

            now[ind] = iter;
            float dis = distancia(todo[ind].dato, datqu);
            if (dis <= di_lejana * (1 + eps))
            {
            csemillas.push({ind, dis});
            if (dis <= di_lejana)
            {
                final_re.push({ind, dis});
            
                if (final_re.size() > k) 
                {
                     final_re.pop();
                } 
                if (final_re.size() >= k) 
                {
                    di_lejana = final_re.top().distancia;    }    
            }
            
        
        
        
        }
        }
    }
    while (!final_re.empty()) 
    { 
        final.push_back(final_re.top()); 
        final_re.pop(); 
    }
    reverse(final.begin(), final.end());
}

bool Grafo::revMRNG(int &in_2, float& dentre,
                    vector<Pair>& vecinos)
{
    for (Pair& temp : vecinos)
    {
        int in_3 = temp.indice;

        if (!existec(in_2, in_3))
            continue;

        if (dentre > max(temp.distancia, dis_ono(in_2, in_3)))
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

bool ya_esta(int x, vector<Pair> &vecinos) {
    for (Pair e : vecinos)
        if (e.indice == x) 
        {
            return true;
        }
    return false;
}

bool exister(vector<int> & temp, int &xa)
{
    for(int t : temp)
    {
        if(t == xa)
        {
            return true;
        }
    }
    return false;
}

void Grafo::balanceo(int& indice, vector<int>& seeds)
{
    vector<Pair> vecinos;
    // no va pero si va poer no

    vector<int> final;

    int k_busqueda = LIMITE_RANGE;
    while (true)
    {
        final.clear();//limpiar final sino se repite
        vector<Pair> mascerca;
        busquedaR(todo[indice].dato, mascerca, seeds, k_busqueda, EPS);

        for (Pair& f : mascerca)
        {
            final.push_back(f.indice);
        } 

        bool sinrng = false;

        while (vecinos.size() < GRADO)
        {
            int contar = vecinos.size();
            for (int i = 0; i < mascerca.size() && vecinos.size() < GRADO; i++)
            {
                int ind_buq = mascerca[i].indice;
                if (ya_esta(ind_buq, vecinos)) 
                {    continue;   }
                if (!(sinrng || revMRNG(ind_buq, mascerca[i].distancia, vecinos))) 
                    continue;

                int lejano = -1;
                float dlejos = -1;
                for (Pair& temp : todo[ind_buq].vecino)
                {
                    if (!ya_esta(temp.indice, vecinos) && temp.distancia > dlejos)
                    {
                        dlejos = temp.distancia;
                        lejano = temp.indice;
                    }
                }    
                if (lejano == -1) //no arista entonces fue
                    continue;

                quitarAr(ind_buq, lejano);
                vecinos.push_back({ind_buq, mascerca[i].distancia});
                vecinos.push_back({lejano, distancia(todo[indice].dato, todo[lejano].dato)});
            }
            
            if (sinrng && vecinos.size() == contar) 
                break;
            
            sinrng = true;
        }
        if (vecinos.size() == GRADO) 
        {   
            break;   
        }


        k_busqueda = k_busqueda* 2;
        if (k_busqueda > todo.size()) 
        { break; }
    }

    for (Pair& e : vecinos) 
    {   
        insertarA(indice, e.indice); 
    }


    if (OPT_NUEVOS)
    {
        for (Pair& ve : vecinos)
        {
            if (!exister(final, ve.indice) && existec(indice, ve.indice))
            {
                mejorarAr(indice, ve.indice);
            } 
        }
            
    }
        
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

bool contiene( vector<Pair>& S, int x)
{
    for ( Pair& p : S) 
        if (p.indice == x) 
            return true;
    return false;
}

bool Grafo::mejorarAr(int in_ind1, int in_ind2)
{
    vector<Temporal> cambioste;             //temporal muy muy muy necesario
    float sin_arista = dis_ono(in_ind1, in_ind2);


    quitarAr(in_ind1, in_ind2);
    cambioste.push_back({false, in_ind1, in_ind2});
    int inicio = in_ind1;
    int extre = in_ind2;

    while (cambioste.size() < MAX_SWAP)
    {

        float temp = sin_arista;
        vector<Pair> mascercanos;
        vector<int> sem = {inicio, extre};
        busquedaR(todo[in_ind2].dato, mascercanos , sem, K_OPT, EPS_OPT);   

        int possi_1 = inicio;
        int possi_2 = extre;
        for (Pair& m : mascercanos)
        {
            int indt = m.indice;
            if (indt == in_ind1 ||indt == in_ind2 ||  existec(in_ind2, indt)) 
            {
                continue;
            }
                
            for (Pair& pn : todo[indt].vecino)
            {
                int n = pn.indice;
                if (n == in_ind2) 
                    continue;
                
                float supmejora = sin_arista - m.distancia + pn.distancia;
                
                if (temp < supmejora) 
                {   
                    temp = supmejora; 
                    possi_1 = indt; 
                    possi_2 = n; 
                }
            }
        }
        if (temp == sin_arista) 
            break;  

        inicio = possi_1; 
        extre = possi_2; 
        sin_arista = temp;

        insertarA(in_ind2, inicio);  
        cambioste.push_back({true,  in_ind2, inicio});
        quitarAr(inicio, extre);   
        cambioste.push_back({false, inicio, extre});

        if (in_ind1 == extre)
        {
            float b = 0; 
            
            int v5 = -1;
            int v6 = -1;
            vector<Pair> S2;
            vector<int> sem2 = {in_ind2, inicio};
            busquedaR(todo[in_ind1].dato, S2, sem2, K_OPT, EPS_OPT);   
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
                    if (b < cand) 
                    {   b = cand; 
                        v5 = s; 
                        v6 = n; 
                    }
                }
            }
            if (b > 0)
            {
                insertarA(in_ind1, v5);
                insertarA(in_ind1, v6);
                quitarAr(v5, v6);
                return true;
            }
        }
        else if (!existec(in_ind1, extre) &&
         sin_arista - distancia(todo[in_ind1].dato, todo[extre].dato) > 0)
        {
            vector<Pair> final1, final2;
            vector<int> semte = {in_ind2, inicio};
            busquedaR(todo[in_ind1].dato, final1, semte, K_OPT, EPS_OPT);   
            busquedaR(todo[extre].dato, final2, semte, K_OPT, EPS_OPT);   

            if (contiene(final1, in_ind1) || contiene(final2, extre))
            {
                insertarA(in_ind1, extre);
                return true;
            }
        }

        int v = extre; 
        extre = inicio; 
        inicio = in_ind2; 
        in_ind2 = v;
    }

    for (int i = cambioste.size() - 1; i >= 0; i--)
    {
        if (cambioste[i].agregada) 
        {
            quitarAr(cambioste[i].ind1, cambioste[i].ind2);
        }
        else  
        {
            insertarA(cambioste[i].ind1, cambioste[i].ind2);
        }             
    }
    return false;
}

void Grafo::dynamicMeAr()
{
    if (todo.size() <= GRADO + 1)
        return;
    optimizarVertice((int)(gen() % todo.size()));
}

void Grafo::optimizarVertice(int v1)
{
    if (todo.size() <= GRADO + 1)
        return;

    vector<int> N;                                
    for (Pair& p : todo[v1].vecino) 
    {   N.push_back(p.indice);}

    for (int v2 : N)
    {
        if (!existec(v1, v2)) continue;
        float dentre = dis_ono(v1, v2);
        if (!revMRNG(v2, dentre, todo[v1].vecino))  //correcion
        {
            mejorarAr(v1, v2);
        }
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

// cuantas optimizaciones en total segun N: 100% hasta ~1000, ~40% en 10 000, ~20% en 50 000, ~5% en 1 millon
int Grafo::cuantas_opt(size_t N)
{
    double n = 25.6 * pow((double)N, 0.55);
    if (n > (double)N)
        n = (double)N;
    return (int)(n * 10);
}

void Grafo::optimizarLote(int n_opt, int estrategia)
{
    int N = todo.size();
    if (N <= GRADO + 1 || n_opt <= 0) return;

    if (estrategia == 0)
    {
        for (int k = 0; k < n_opt; k++)  
        {
            optimizarVertice((gen() % N));
        }
    
        return;
    }

    
    n_opt = min(n_opt, N);
    vector<int> orden(N);
    iota(orden.begin(), orden.end(), 0);
    vector<float> score(N, 0);
    for (int i = 0; i < N; i++)
        for (Pair& p : todo[i].vecino) score[i] += p.distancia;
    partial_sort(orden.begin(), orden.begin() + n_opt, orden.end(),
                 [&](int a, int b) { return score[a] > score[b]; });
    for (int k = 0; k < n_opt; k++)
        optimizarVertice(orden[k]);
}