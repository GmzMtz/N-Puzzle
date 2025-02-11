//-----Compilación del archivo fuente-----//.
#ifndef Puzzle_cpp
#define Puzzle_cpp

//-----Librerías-----//.
#include "Puzzle.hpp"

//-----Métodos de la clase 'Puzzle'-----//.
//---Funciones privadas---//.
//Se verifica que el caracter ingresado sea numérico.
char Puzzle::validar_entrada(ALLEGRO_EVENT evento) {
    char letra;
    switch (evento.keyboard.keycode) {
    case ALLEGRO_KEY_1: letra = '1'; break; case ALLEGRO_KEY_2: letra = '2'; break; case ALLEGRO_KEY_3: letra = '3'; break;
    case ALLEGRO_KEY_4: letra = '4'; break; case ALLEGRO_KEY_5: letra = '5'; break; case ALLEGRO_KEY_6: letra = '6'; break;
    case ALLEGRO_KEY_7: letra = '7'; break; case ALLEGRO_KEY_8: letra = '8'; break; case ALLEGRO_KEY_9: letra = '9'; break;
    case ALLEGRO_KEY_0: letra = '0'; break;
    case ALLEGRO_KEY_BACKSPACE: letra = '-'; break; default: letra = '+'; break;
    }
    return letra;
}

//Se verifica si la entrada actual es repetida/válida o no.
bool Puzzle::entrada_valida(int fila, int columna) {
    int limite = 0;

    switch (this->tipo_puzzle) {
    case 3: limite = 9; break;
    case 4: limite = 16; break;
    case 5: limite = 25; break;
    }

    int auxiliar = stoi(this->puzzle[0][fila][columna]);
    if (auxiliar > limite || auxiliar < 1) {
        return false;
    }

    for (int i = 0; i < this->tipo_puzzle; i++) {
        for (int j = 0; j < this->tipo_puzzle; j++) {
            if (!(i == fila && j == columna)) {
                if (this->puzzle[0][i][j] == this->puzzle[0][fila][columna]) {
                    return false;
                }
            }
        }
    }
    return true;
}

//Se calcula el costo que requiere resolver un puzzle inicial a uno meta.
int Puzzle::calcula_Costo(vector<vector<string>>& puzzle_inicial, vector<vector<string>>& puzzle_final) {
    int costo = 0;
    for (int i = 0; i < this->tipo_puzzle; i++) {
        for (int j = 0; j < this->tipo_puzzle; j++) {
            if (puzzle_inicial[i][j] != "0" && puzzle_inicial[i][j] != puzzle_final[i][j]) {
                costo++;
            }
        }
    }
    return costo;
}

//Se verifica si dos elementos son contiguos, según el tamaño del puzzle.
bool Puzzle::esCoordenada(int x, int y) {
    return ((x >= 0 && x < this->tipo_puzzle) && (y >= 0 && y < this->tipo_puzzle));
}

//Se imprime la ruta más optima para resolver el puzzle.
void Puzzle::muestra_Solucion(Nodo* raiz, ALLEGRO_SAMPLE* deslizar) {
    if (raiz == NULL) return;
    this->muestra_Solucion(raiz->padre, deslizar);
    this->imprime_Puzzle(&(raiz->mat));
    al_play_sample(deslizar, 2.0, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
    al_rest(1);
}

//Se convierte un string a un valor numérico.  
int Puzzle::convertir_string(string valor) {
    if (valor == "1") return 0;         else if (valor == "2") return 1;    else if (valor == "3") return 2;
    else if (valor == "4") return 3;    else if (valor == "5") return 4;    else if (valor == "6") return 5;
    else if (valor == "7") return 6;    else if (valor == "8") return 7;    else if (valor == "9") return 8;
    else if (valor == "10") return 9;   else if (valor == "11") return 10;  else if (valor == "12") return 11;
    else if (valor == "13") return 12;  else if (valor == "14") return 13;  else if (valor == "15") return 14;
    else if (valor == "16") return 15;  else if (valor == "17") return 16;  else if (valor == "18") return 17;
    else if (valor == "19") return 18;  else if (valor == "20") return 19;  else if (valor == "21") return 20;
    else if (valor == "22") return 21;  else if (valor == "23") return 22;  else if (valor == "24") return 23;
    else return 0;
}       

//---Constructor y destructor---//.
//Constructor con argumentos.
Puzzle::Puzzle(ALLEGRO_FONT* formato, ALLEGRO_DISPLAY* ventana, int tipo_puzzle) {
    this->x = this->y = this->contador = 0;
    this->parpadeo = true;
    this->letra = formato;
    this->pantalla = ventana;
    this->tipo_puzzle = tipo_puzzle;
    this->puzzle = new vector<vector<string>>(this->tipo_puzzle, vector<string>(this->tipo_puzzle, ""));
}

//Destructor.
Puzzle::~Puzzle() {
    delete this->puzzle;
}

//---Métodos---//.
//Se genera el puzzle de forma aleatoria.
void Puzzle::generaPuzzle() {
	string random;
	int contador_y = 0, contador_x = 0;
    vector<string>* num_aleatorios = new vector<string>;

    for (int i = 0; i < pow(this->tipo_puzzle, 2); i++) num_aleatorios->push_back(to_string(i));
    int k = pow(this->tipo_puzzle, 2) - 1;

    for (int i = 0; i < pow(this->tipo_puzzle, 2); i++) {
        int j = rand() % (k + 1);

        string intercambio = num_aleatorios[0][j + i];
        num_aleatorios[0][j + i] = num_aleatorios[0][i];
        num_aleatorios[0][i] = intercambio;

        k--;
    }
    for (int i = 0; i < this->tipo_puzzle; i++) {
        for (int j = 0; j < this->tipo_puzzle; j++) {
            this->puzzle[0][i][j] = num_aleatorios->back();
            num_aleatorios->pop_back();
        }
    } //Se genera el puzzle con fichas aleatorias y únicas.
    delete num_aleatorios;
	return;
}

//Devuelve el espacio vacio del puzzle.
Espacio Puzzle::getEspacioVacio() {
    Espacio espacio_vacio;
    for (int i = 0; i < this->tipo_puzzle; i++) {
        for (int j = 0; j < this->tipo_puzzle; j++) {
            if (this->puzzle[0][i][j] == "0") {
                espacio_vacio.x = i;
                espacio_vacio.y = j;
                break;
            }
        }
    } //Se determina la posición matricial del espacio.
    return espacio_vacio;
}

//Si tiene solución, se resuelve el puzzle por medio del algoritmo branch and bound.
bool Puzzle::resuelve(Puzzle *puzzle_final) {
    // Para los movimientos de las fichas.
    // Abajo, Izq, Arriba, Der.
    int fil[] = { 1, 0, -1, 0 };
    int col[] = { 0, -1, 0, 1 };

    ALLEGRO_SAMPLE* deslizar = al_load_sample("Sounds/smw_fireball.wav");

    // Coordenadas del espacio vacio del puzzle.
    Espacio espacio_vacio = this->getEspacioVacio();
    int x = espacio_vacio.x;
    int y = espacio_vacio.y;

    priority_queue<Nodo*, vector<Nodo*>, comp> *nodosVivos = new priority_queue<Nodo*, vector<Nodo*>, comp>; // Creamos una cola de prioridad para almacenar los nodos vivos del arbol de búsqeda.

    // Creamos un nuevo nodo y calculamos su costo.
    Nodo* raiz = new Nodo(*(this->puzzle), x, y, x, y, 0, NULL);  
    raiz->costo = this->calcula_Costo(*(this->puzzle), *(puzzle_final->puzzle)); 

    nodosVivos->push(raiz);// Agregamos la raiz a la cola de prioridad (lista de nododos vivos).
 
    // Buscamos el nodo vivo de menor costo, agregamos a sus hijos a la lista de nodos vivos y lo eliminamos de la lista.
    this->pantalla_solucion();
    while (!nodosVivos->empty()) {
        Nodo* min = nodosVivos->top(); //Extraemos el nodo vivo con menor costo estimado.
        nodosVivos->pop(); //Eliminamos al nodo extraido de la lista de nodos vivos.

        if (min->costo == 0 || min->nivel >= 30) { // Si el nodo extraido es un nodo solucion.
            this->muestra_Solucion(min, deslizar);
            al_destroy_sample(deslizar);
            if (min->costo == 0)
                this->pantalla_puzzleResuelto(min, 1);
            else
                this->pantalla_puzzleResuelto(min, 2);

            delete min;
            while (!nodosVivos->empty()) {
                min = nodosVivos->top();
                nodosVivos->pop();
                delete min;
            }
            delete raiz;
            return true;
        } 

        //Generamos los nodos hijo del elemento extraido (máximo cuatro hijos).
        for (int i = 0; i < 4; i++) {
            if (this->esCoordenada(min->espacio_vacio.x + col[i], min->espacio_vacio.y + fil[i])) {
                Nodo* hijo = new Nodo(min->mat, min->espacio_vacio.x, min->espacio_vacio.y, min->espacio_vacio.x + col[i], min->espacio_vacio.y + fil[i], min->nivel + 1, min);
                if (hijo->padre != raiz) {
                    if (hijo->mat == hijo->padre->padre->mat)
                        delete hijo;
                    else {
                        hijo->costo = this->calcula_Costo(hijo->mat, *(puzzle_final->puzzle));
                        nodosVivos->push(hijo);
                    }
                }
                else {
                    hijo->costo = this->calcula_Costo(hijo->mat, *(puzzle_final->puzzle));
                    nodosVivos->push(hijo);
                }
            }
        }
    } 
    return false;
}

//Realiza un movimiento en el puzzle que acerca al usuario a la solución.
void Puzzle::sugerir_movimiento(Puzzle* puzzle_final) {
   // Para los movimientos de las fichas.
   // Abajo, Izq, Arriba, Der.
    int fil[] = { 1, 0, -1, 0 };
    int col[] = { 0, -1, 0, 1 };

    // Coordenadas del espacio vacio del puzzle.
    Espacio espacio_vacio = this->getEspacioVacio();
    int x = espacio_vacio.x;
    int y = espacio_vacio.y;

    priority_queue<Nodo*, vector<Nodo*>, comp> *nodosVivos = new priority_queue<Nodo*, vector<Nodo*>, comp>; // Creamos una cola de prioridad para almacenar los nodos vivos del arbol de búsqeda.

    // Creamos un nuevo nodo y calculamos su costo.
    Nodo* raiz = new Nodo(*(this->puzzle), x, y, x, y, 0, NULL);
    raiz->costo = this->calcula_Costo(*(this->puzzle), *(puzzle_final->puzzle));
    nodosVivos->push(raiz); // Agregamos la raiz a la cola de prioridad (lista de nodos vivos).

    Nodo* min = nodosVivos->top(); //Extraemos el nodo vivo con menor costo estimado.
    nodosVivos->pop(); //Eliminamos al nodo extraido de la lista de nodos vivos.

    //Generamos todos los nodos hijo del elemento extraido (máximo cuatro hijos).
    for (int i = 0; i < 4; i++) {
        if (this->esCoordenada(min->espacio_vacio.x + col[i], min->espacio_vacio.y + fil[i])) {
            Nodo* hijo = new Nodo(min->mat, min->espacio_vacio.x, min->espacio_vacio.y, min->espacio_vacio.x + col[i], min->espacio_vacio.y + fil[i], min->nivel + 1, min);
            hijo->costo = this->calcula_Costo(hijo->mat, *(puzzle_final->puzzle));
            nodosVivos->push(hijo);
        }
    }

    min = nodosVivos->top();
    nodosVivos->pop();
    for (int i = 0; i < this->tipo_puzzle; i++)
        for (int j = 0; j < this->tipo_puzzle; j++)
            this->puzzle[0][i][j] = min->mat[i][j];

    delete min;
    delete raiz;
    delete nodosVivos;
    return;
}

//Se compara si ambos puzzles son exactamente iguales.
bool Puzzle::operator==(Puzzle &puzzle) {
    for (int i = 0; i < this->tipo_puzzle; i++) {
        for (int j = 0; j < this->tipo_puzzle; j++) {
            if (this->puzzle[0][i][j] != puzzle.puzzle[0][i][j]) return false;
        }
    }
    return true;
}

//Se retorna un vector relacionado con el tablero.
vector<string> &Puzzle::operator[](int elemento) {
    return this->puzzle[0][elemento];
}

//Se intercambian dos valores dentro del tablero.
void Puzzle::swap(int direccion) {
    string auxiliar;

    for (int i = 0; i < this->tipo_puzzle; i++) {
        for (int j = 0; j < this->tipo_puzzle; j++) {
            if (this->puzzle[0][i][j] == "0") {
                switch (direccion) {
                case 1: //Intercambio con el elemento superior.
                    auxiliar = this->puzzle[0][i][j];
                    this->puzzle[0][i][j] = this->puzzle[0][i - 1][j];
                    this->puzzle[0][i - 1][j] = auxiliar;
                    break;
                case 2: //Intercambio con el elemento inferior.
                    auxiliar = this->puzzle[0][i][j];
                    this->puzzle[0][i][j] = this->puzzle[0][i + 1][j];
                    this->puzzle[0][i + 1][j] = auxiliar;
                    break;
                case 3: //Intercambio con el elemento a la izquierda.
                    auxiliar = this->puzzle[0][i][j];
                    this->puzzle[0][i][j] = this->puzzle[0][i][j - 1];
                    this->puzzle[0][i][j - 1] = auxiliar;
                    break;
                case 4: //Intercambio con el elemento a la derecha.
                    auxiliar = this->puzzle[0][i][j];
                    this->puzzle[0][i][j] = this->puzzle[0][i][j + 1];
                    this->puzzle[0][i][j + 1] = auxiliar;
                    break;
                }
                return;
            }
        }
    }
}


//Se retorna la cantidad de permutaciones del tablero actual.
int Puzzle::inversiones(int* arreglo) {
    int inversiones = 0;

    for (int i = 0; i < this->tipo_puzzle * this->tipo_puzzle - 1; i++) {
        for (int j = i + 1; j < this->tipo_puzzle * this->tipo_puzzle; j++) {
            if (arreglo[i] && arreglo[j] && arreglo[i] > arreglo[j]) {
                inversiones++;
            }
        }
    }
    return inversiones;
}

//Se imprime el contenido del puzzle.
void Puzzle::imprime_Puzzle(vector<vector<string>> *puzzle) {
    al_flip_display();

    ALLEGRO_BITMAP* puzzle_3x3 = al_load_bitmap("Sources/TableroFacil3x3.png");
    ALLEGRO_BITMAP* puzzle_4x4 = al_load_bitmap("Sources/TableroMedio4x4.png");
    ALLEGRO_BITMAP* puzzle_5x5 = al_load_bitmap("Sources/TableroDificil5x5.png");

    vector<ALLEGRO_BITMAP*>* fichas = new vector<ALLEGRO_BITMAP*>;
    for (int i = 1; i < 25; i++) {
        string aux = "Sources/Fichas/F";
        ALLEGRO_BITMAP* elemento = al_load_bitmap((aux + to_string(i) + ".png").c_str());
        fichas->push_back(elemento);
    }
        
    switch (this->tipo_puzzle) {
    case 3:
        al_draw_bitmap(puzzle_3x3, 319, 180, NULL);
        break;
    case 4:
        al_draw_bitmap(puzzle_4x4, 293, 150, NULL);
        break;
    case 5:
        al_draw_bitmap(puzzle_5x5, 266, 120, NULL);
        break;
    }

    for (int i = 0; i < this->tipo_puzzle; i++) {
        for (int j = 0; j < this->tipo_puzzle; j++) {
            if (puzzle[0][i][j] != "0") {
                int auxiliar2 = this->convertir_string(puzzle[0][i][j]);

                switch (this->tipo_puzzle) {
                case 3: al_draw_bitmap(fichas[0][auxiliar2], 319 + j * 53 + 3, 180 + i * 53 + 3, NULL); break;
                case 4: al_draw_bitmap(fichas[0][auxiliar2], 293 + j * 53 + 3, 150 + i * 53 + 3, NULL); break;
                case 5: al_draw_bitmap(fichas[0][auxiliar2], 266 + j * 53 + 3, 120 + i * 53 + 3, NULL); break;
                }
            }
        }
    } //Se imprimen todas las fichas del puzzle.
    al_flip_display();
    al_destroy_bitmap(puzzle_3x3); 
    al_destroy_bitmap(puzzle_4x4);
    al_destroy_bitmap(puzzle_5x5);  
    delete fichas;
}

//Se genera la pantalla de solución de puzzles.
void Puzzle::pantalla_solucion() {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();

    ALLEGRO_BITMAP* cubo1 = al_load_bitmap("Sources/Cubo chico 1.png");
    ALLEGRO_BITMAP* cubo2 = al_load_bitmap("Sources/Cubo chico 2.png");
    ALLEGRO_BITMAP* cubo3 = al_load_bitmap("Sources/Cubo chico 3.png");
    ALLEGRO_BITMAP* cubo4 = al_load_bitmap("Sources/Cubo grande.png");

    al_draw_text(this->letra, al_map_rgb(164, 255, 255), 400, 50, ALLEGRO_ALIGN_CENTRE, "RESOLVIENDO PUZZLE...");

    al_draw_bitmap(cubo4, 50, 415, NULL);
    al_draw_bitmap(cubo1, 210, 435, NULL);
    al_draw_bitmap(cubo2, 330, 435, NULL);
    al_draw_bitmap(cubo3, 450, 435, NULL);
    al_draw_bitmap(cubo1, 570, 435, NULL);
    al_draw_bitmap(cubo2, 690, 435, NULL);

    al_flip_display();
    al_destroy_bitmap(cubo1); al_destroy_bitmap(cubo2);
    al_destroy_bitmap(cubo3); al_destroy_bitmap(cubo4);
}

//Se generan mensajes que indican si el puzzle fue resuelto o no.
void Puzzle::pantalla_puzzleResuelto(Nodo *puzzle, int caso) {
    al_flip_display();
    al_draw_filled_rectangle(0, 0, 800, 100, al_map_rgb(0, 0, 0));
    if (caso == 1)
        al_draw_text(this->letra, al_map_rgb(164, 255, 255), 400, 50, ALLEGRO_ALIGN_CENTRE, "PUZZLE RESUELTO");
    if (caso == 2)
        al_draw_text(this->letra, al_map_rgb(164, 255, 255), 400, 50, ALLEGRO_ALIGN_CENTRE, "SOLUCION NO CALCULABLE");
    al_draw_text(this->letra, al_map_rgb(189, 249, 201), 400, 100, ALLEGRO_ALIGN_CENTRE, ("TOTAL DE MOVIMIENTOS: " + to_string(puzzle->nivel)).c_str());
    al_flip_display();
    return;
}

//-----Métodos de la clase 'Puzzle_facil'-----//.
//---Métodos---//.
//Se verifica si el 8-puzzle puede resolverse con la meta predeterminada.
bool Puzzle_facil::resolvible_manual() {
    int inversiones = 0;
    int* arreglo = new int[9];

    for (int i = 0, contador = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++){
            arreglo[contador++] = stoi(this->puzzle[0][i][j]);
        }
    }

    inversiones = this->inversiones(arreglo);
    delete [] arreglo;
    return inversiones % 2 == 0;
}

//-----Métodos de la clase 'Puzzle_medio'-----//.
//---Funciones privadas---//.
//Se retorna la posición horizontal del espacio.
int Puzzle_medio::posicion_x_cero() {
    for (int i = 3; i >= 0; i--) {
        for (int j = 3; j >= 0; j--) {
            if (this->puzzle[0][i][j] == "0") {
                return this->tipo_puzzle - i;
            }
        }
    }
    return this->tipo_puzzle;
}

//---Métodos---//.
//Se verifica si el 15-puzzle puede resolverse con la meta predeterminada.
bool Puzzle_medio::resolvible_manual() {
    int inversiones = 0;
    int* arreglo = new int[16];

    for (int i = 0, contador = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            arreglo[contador++] = stoi(this->puzzle[0][i][j]);
        }
    }

    inversiones = this->inversiones(arreglo);
    delete [] arreglo;

    if (this->posicion_x_cero() % 2 != 0) {
        return (inversiones % 2 == 0);
    }
    else {
        return (inversiones % 2 != 0);
    }
}

//-----Métodos de la clase 'Puzzle_dificil'-----//.
//---Métodos---//.
//Se verifica si el 24-puzzle puede resolverse con la meta predeterminada.
bool Puzzle_dificil::resolvible_manual() {
    int inversiones;
    int* arreglo = new int[25];
        ;
    for (int i = 0, contador = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            arreglo[contador++] = stoi(this->puzzle[0][i][j]);
        }
    }

    inversiones = this->inversiones(arreglo);
    delete [] arreglo;
    return inversiones % 2 == 0;
}
#endif