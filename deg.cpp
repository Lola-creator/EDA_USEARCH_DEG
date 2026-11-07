#include "deg.hpp"

using namespace std;
int main()
{
    Grafo g;                                  // grado 4
    for (int i = 0; i < 5; i++)
    {
        vector<float> dato = {(float)i, (float)(i*i), 1.0f, 0.5f };
        g.insertar(dato);
    }
        

    for (int i = 0; i < g.get_num_nodos(); i++)
        cout << "nodo " << i << " tiene " << g.get_vertice(i).vecino.size() << " vecinos\n";
    return 0;
}