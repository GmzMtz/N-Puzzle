//-----Compilación de la cabecera-----//.
#ifndef Puzzle_hpp
#define Puzzle_hpp

//-----Librerías-----//.
#include "Allegro.hpp"
#include "Nodo.hpp"

//-----Estructuras-----//.
struct comp { 
    //Se verifica cuál nodo tiene un mayor costo y tiempo de procesamiento.
    bool operator()(const Nodo* lhs, const Nodo* rhs) const { 
        return (lhs->costo + lhs->nivel) > (rhs->costo + rhs->nivel); 
    } 
};

//-----Clase 'Puzzle'-----//.
class Puzzle {
protected:
    //---Atributos---//.
    int tipo_puzzle;
    vector<vector<string>> *puzzle;
    int x, y;
    int contador;
    bool parpadeo;
    ALLEGRO_FONT* letra;
    ALLEGRO_DISPLAY* pantalla;

    //---Funciones privadas---//.
    char validar_entrada(ALLEGRO_EVENT); //Se verifica que el caracter ingresado sea numérico.
    bool entrada_valida(int, int); //Se verifica si la entrada actual es repetida/válida o no.
    int calcula_Costo(vector<vector<string>>&, vector<vector<string>>&); //Se calcula el costo que requiere resolver un puzzle inicial a uno meta.
    bool esCoordenada(int, int); //Se verifica si dos elementos son contiguos, según el tamaño del puzzle.
    void muestra_Solucion(Nodo*, ALLEGRO_SAMPLE*); //Se imprime la ruta más optima para resolver el puzzle. 
    int convertir_string(string); //Se convierte un string a un valor numérico.
public:
    //---Constructor y destructor---//.
    Puzzle(ALLEGRO_FONT*, ALLEGRO_DISPLAY*, int); //Constructor con argumentos.
    ~Puzzle(); //Destructor.

    //---Métodos---//.
    void generaPuzzle(); //Se genera el puzzle de forma aleatoria.
    Espacio getEspacioVacio(); //Devuelve las coordenadas del espacio vacio del puzzle.
    bool resuelve(Puzzle *); //Si tiene solución, se resuelve el puzzle por medio del algoritmo branch and bound.
    void sugerir_movimiento(Puzzle*); //Realiza un movimiento en el puzzle que acerca al usuario a la solución.
    bool operator==(Puzzle &); //Se compara si ambos puzzles son exactamente iguales.
    vector<string> &operator[](int); //Se retorna un vector relacionado con el tablero.
    void swap(int); //Se intercambian dos valores dentro del tablero.
    int inversiones(int *); //Se retorna la cantidad de permutaciones del tablero actual.
    virtual bool resolvible_manual() { return true; } //Se verifica si el puzzle puede resolverse con la meta predeterminada.
    void imprime_Puzzle(vector<vector<string>>*); //Se imprime el contenido del puzzle.
    void pantalla_solucion(); //Se genera la pantalla de solución de puzzles.
    void pantalla_puzzleResuelto(Nodo *,int); //Se generan mensajes que indican si el puzzle fue resuelto o no.
    inline vector<vector<string>> *matriz() { return puzzle; } //Se retorna el puzzle. 
};

//-----Clase 'Puzzle_facil', hija de 'Puzzle'-----//.
class Puzzle_facil : public Puzzle {
public:
    //---Constructor y destructor---//.
    Puzzle_facil(ALLEGRO_FONT* letra, ALLEGRO_DISPLAY* pantalla, int valor) : Puzzle(letra, pantalla, valor) {} //Constructor con argumentos.
    ~Puzzle_facil() { Puzzle::~Puzzle(); } //Destructor.

    //---Métodos---//.
    bool resolvible_manual(); //Se verifica si el 8-puzzle puede resolverse con la meta predeterminada.
};

//-----Clase 'Puzzle_medio', hija de 'Puzzle'-----//.
class Puzzle_medio : public Puzzle {
private:
    int posicion_x_cero(); //Se retorna la posición horizontal del espacio.
public:
    //---Constructor y destructor---//.
    Puzzle_medio(ALLEGRO_FONT* letra, ALLEGRO_DISPLAY* pantalla, int valor) : Puzzle(letra, pantalla, valor) {} //Constructor con argumentos.
    ~Puzzle_medio() { Puzzle::~Puzzle(); } //Destructor.

    //---Métodos---//.
    bool resolvible_manual(); //Se verifica si el 15-puzzle puede resolverse con la meta predeterminada.
};

//-----Clase 'Puzzle_dificil', hija de 'Puzzle'-----//.
class Puzzle_dificil : public Puzzle {
public:
    //---Constructor y destructor---//.
    Puzzle_dificil(ALLEGRO_FONT* letra, ALLEGRO_DISPLAY* pantalla, int valor) : Puzzle(letra, pantalla, valor) {} //Constructor con argumentos.
    ~Puzzle_dificil() { Puzzle::~Puzzle(); } //Destructor.

    //---Métodos---//.
    bool resolvible_manual(); //Se verifica si el 24-puzzle puede resolverse con la meta predeterminada.
};
#endif