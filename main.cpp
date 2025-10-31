#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

using namespace std;

// estructura de nodo generico para la lista enlazada
template <typename T>
struct Nodo {
    T dato;
    Nodo<T>* siguiente;
    
    Nodo(T valor) : dato(valor), siguiente(nullptr) {}
};

// lista enlazada simple generica
template <typename T>
class ListaSensor {
private:
    Nodo<T>* cabeza;
    int cantidad;
    
public:
    // constructor
    ListaSensor() : cabeza(nullptr), cantidad(0) {}
    
    // constructor de copia
    ListaSensor(const ListaSensor<T>& otra) : cabeza(nullptr), cantidad(0) {
        Nodo<T>* actual = otra.cabeza;
        while (actual != nullptr) {
            insertar(actual->dato);
            actual = actual->siguiente;
        }
    }
    
    // operador de asignacion
    ListaSensor<T>& operator=(const ListaSensor<T>& otra) {
        if (this == &otra) return *this;
        
        // liberar memoria actual
        Nodo<T>* actual = cabeza;
        while (actual != nullptr) {
            Nodo<T>* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
        
        cabeza = nullptr;
        cantidad = 0;
        
        // copiar de la otra lista
        actual = otra.cabeza;
        while (actual != nullptr) {
            insertar(actual->dato);
            actual = actual->siguiente;
        }
        
        return *this;
    }
    
    // destructor - libera toda la memoria
    ~ListaSensor() {
        Nodo<T>* actual = cabeza;
        while (actual != nullptr) {
            Nodo<T>* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
    }
    
    // insertar un valor al final
    void insertar(T valor) {
        Nodo<T>* nuevo = new Nodo<T>(valor);
        
        if (cabeza == nullptr) {
            cabeza = nuevo;
        } else {
            Nodo<T>* actual = cabeza;
            while (actual->siguiente != nullptr) {
                actual = actual->siguiente;
            }
            actual->siguiente = nuevo;
        }
        cantidad++;
    }
    
    // calcular promedio de los valores
    T calcularPromedio() const {
        if (cantidad == 0) return 0;
        
        T suma = 0;
        Nodo<T>* actual = cabeza;
        while (actual != nullptr) {
            suma += actual->dato;
            actual = actual->siguiente;
        }
        return suma / cantidad;
    }
    
    // eliminar el valor minimo
    void eliminarMinimo() {
        if (cabeza == nullptr) return;
        
        Nodo<T>* actual = cabeza;
        Nodo<T>* minNodo = cabeza;
        Nodo<T>* prevMin = nullptr;
        Nodo<T>* prev = nullptr;
        
        // buscar el minimo
        while (actual != nullptr) {
            if (actual->dato < minNodo->dato) {
                minNodo = actual;
                prevMin = prev;
            }
            prev = actual;
            actual = actual->siguiente;
        }
        
        // eliminar el minimo
        if (prevMin == nullptr) {
            cabeza = cabeza->siguiente;
        } else {
            prevMin->siguiente = minNodo->siguiente;
        }
        
        delete minNodo;
        cantidad--;
    }
    
    // obtener cantidad de elementos
    int getCantidad() const {
        return cantidad;
    }
    
    // mostrar todos los valores
    void mostrar() const {
        Nodo<T>* actual = cabeza;
        cout << "lecturas: ";
        while (actual != nullptr) {
            cout << actual->dato << " ";
            actual = actual->siguiente;
        }
        cout << endl;
    }
};

// clase base abstracta para sensores
class SensorBase {
protected:
    char nombre[50];
    
public:
    // constructor
    SensorBase(const char* id) {
        strncpy(nombre, id, 49);
        nombre[49] = '\0';
    }
    
    // destructor virtual - critico para polimorfismo correcto
    virtual ~SensorBase() {}
    
    // metodos virtuales puros - deben implementarse en clases derivadas
    virtual void procesarLectura() = 0;
    virtual void imprimirInfo() const = 0;
    
    // obtener nombre del sensor
    const char* getNombre() const {
        return nombre;
    }
};

// sensor de temperatura - maneja valores float
class SensorTemperatura : public SensorBase {
private:
    ListaSensor<float> historial;
    
public:
    SensorTemperatura(const char* id) : SensorBase(id) {}
    
    ~SensorTemperatura() {}
    
    // registrar nueva lectura
    void registrarLectura(float valor) {
        historial.insertar(valor);
    }
    
    // procesar las lecturas del sensor
    void procesarLectura() override {
        cout << "\nprocesando sensor de temperatura: " << nombre << endl;
        
        if (historial.getCantidad() == 0) {
            cout << "no hay lecturas" << endl;
            return;
        }
        
        // eliminar la lectura mas baja si hay mas de una
        if (historial.getCantidad() > 1) {
            historial.eliminarMinimo();
        }
        
        float promedio = historial.calcularPromedio();
        cout << "promedio de temperaturas: " << promedio << " grados" << endl;
        cout << "lecturas actuales: " << historial.getCantidad() << endl;
    }
    
    void imprimirInfo() const override {
        cout << "[temperatura] id: " << nombre 
             << " | lecturas: " << historial.getCantidad() << endl;
    }
    
    void mostrarHistorial() const {
        historial.mostrar();
    }
};

// sensor de presion - maneja valores int
class SensorPresion : public SensorBase {
private:
    ListaSensor<int> historial;
    
public:
    SensorPresion(const char* id) : SensorBase(id) {}
    
    ~SensorPresion() {}
    
    // registrar nueva lectura
    void registrarLectura(int valor) {
        historial.insertar(valor);
    }
    
    // procesar las lecturas del sensor
    void procesarLectura() override {
        cout << "\nprocesando sensor de presion: " << nombre << endl;
        
        if (historial.getCantidad() == 0) {
            cout << "no hay lecturas" << endl;
            return;
        }
        
        int promedio = historial.calcularPromedio();
        cout << "promedio de presiones: " << promedio << " psi" << endl;
        cout << "lecturas totales: " << historial.getCantidad() << endl;
    }
    
    void imprimirInfo() const override {
        cout << "[presion] id: " << nombre 
             << " | lecturas: " << historial.getCantidad() << endl;
    }
    
    void mostrarHistorial() const {
        historial.mostrar();
    }
};

// nodo para la lista de gestion polimorfica
struct NodoGestion {
    SensorBase* sensor;
    NodoGestion* siguiente;
    
    NodoGestion(SensorBase* s) : sensor(s), siguiente(nullptr) {}
};

// lista de gestion - almacena punteros a la clase base
class ListaGestion {
private:
    NodoGestion* cabeza;
    int cantidad;
    
public:
    ListaGestion() : cabeza(nullptr), cantidad(0) {}
    
    // destructor - libera toda la memoria en cascada
    ~ListaGestion() {
        cout << "\nliberando memoria del sistema..." << endl;
        NodoGestion* actual = cabeza;
        while (actual != nullptr) {
            NodoGestion* siguiente = actual->siguiente;
            delete actual->sensor;
            delete actual;
            actual = siguiente;
        }
    }
    
    // insertar un sensor
    void insertarSensor(SensorBase* sensor) {
        NodoGestion* nuevo = new NodoGestion(sensor);
        
        if (cabeza == nullptr) {
            cabeza = nuevo;
        } else {
            NodoGestion* actual = cabeza;
            while (actual->siguiente != nullptr) {
                actual = actual->siguiente;
            }
            actual->siguiente = nuevo;
        }
        cantidad++;
    }
    
    // buscar un sensor por nombre
    SensorBase* buscarSensor(const char* nombre) {
        NodoGestion* actual = cabeza;
        while (actual != nullptr) {
            if (strcmp(actual->sensor->getNombre(), nombre) == 0) {
                return actual->sensor;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }
    
    // procesar todos los sensores usando polimorfismo
    void procesarTodos() {
        cout << "\n=== procesamiento polimorfico ===" << endl;
        NodoGestion* actual = cabeza;
        while (actual != nullptr) {
            actual->sensor->procesarLectura();
            actual = actual->siguiente;
        }
    }
    
    // listar todos los sensores
    void listarSensores() const {
        cout << "\n--- sensores registrados ---" << endl;
        NodoGestion* actual = cabeza;
        int contador = 1;
        while (actual != nullptr) {
            cout << contador++ << ". ";
            actual->sensor->imprimirInfo();
            actual = actual->siguiente;
        }
        cout << "total: " << cantidad << " sensores" << endl;
    }
    
    int getCantidad() const {
        return cantidad;
    }
};

// clase simple para comunicacion con arduino
class ArduinoSerial {
private:
    int fd;
    
public:
    ArduinoSerial(const char* puerto) {
        fd = open(puerto, O_RDWR | O_NOCTTY);
        
        if (fd < 0) {
            cout << "error abriendo puerto" << endl;
            return;
        }
        
        // configurar puerto serial
        struct termios opciones;
        tcgetattr(fd, &opciones);
        cfsetispeed(&opciones, B9600);
        cfsetospeed(&opciones, B9600);
        opciones.c_cflag &= ~PARENB;
        opciones.c_cflag &= ~CSTOPB;
        opciones.c_cflag &= ~CSIZE;
        opciones.c_cflag |= CS8;
        tcsetattr(fd, TCSANOW, &opciones);
        
        usleep(2000000); // esperar 2 segundos para arduino
    }
    
    ~ArduinoSerial() {
        if (fd >= 0) close(fd);
    }
    
    // leer una linea del puerto
    bool leerLinea(char* buffer, int maxSize) {
        if (fd < 0) return false;
        
        int bytesLeidos = 0;
        char c;
        
        while (bytesLeidos < maxSize - 1) {
            int resultado = read(fd, &c, 1);
            if (resultado <= 0) break;
            if (c == '\n') break;
            if (c != '\r') buffer[bytesLeidos++] = c;
        }
        
        buffer[bytesLeidos] = '\0';
        return bytesLeidos > 0;
    }
};

// estructura para datos del sensor
struct DatoSensor {
    char tipo[10];
    char id[50];
    float valor;
    bool valido;
};

// parsear linea del formato: "TEMP,T-001,23.5"
DatoSensor parsearLinea(const char* linea) {
    DatoSensor dato;
    dato.valido = false;
    
    char copia[256];
    strncpy(copia, linea, 255);
    copia[255] = '\0';
    
    char* token = strtok(copia, ",");
    if (token == nullptr) return dato;
    strncpy(dato.tipo, token, 9);
    dato.tipo[9] = '\0';
    
    token = strtok(nullptr, ",");
    if (token == nullptr) return dato;
    strncpy(dato.id, token, 49);
    dato.id[49] = '\0';
    
    token = strtok(nullptr, ",");
    if (token == nullptr) return dato;
    dato.valor = atof(token);
    
    dato.valido = true;
    return dato;
}

int main() {
    // crear sistema de gestion
    ListaGestion* sistema = new ListaGestion();
    
    cout << "\n=== sistema iot de sensores ===" << endl;
    
    int opcion = 0;
    
    while (opcion != 7) {
        cout << "\n--- menu ---" << endl;
        cout << "1. crear sensor de temperatura" << endl;
        cout << "2. crear sensor de presion" << endl;
        cout << "3. registrar lectura" << endl;
        cout << "4. listar sensores" << endl;
        cout << "5. procesar sensores" << endl;
        cout << "6. leer desde arduino" << endl;
        cout << "7. salir" << endl;
        cout << "opcion: ";
        cin >> opcion;
        
        switch (opcion) {
            case 1: {
                // crear sensor de temperatura
                char id[50];
                cout << "id del sensor: ";
                cin >> id;
                
                SensorTemperatura* temp = new SensorTemperatura(id);
                sistema->insertarSensor(temp);
                cout << "sensor de temperatura creado" << endl;
                break;
            }
            
            case 2: {
                // crear sensor de presion
                char id[50];
                cout << "id del sensor: ";
                cin >> id;
                
                SensorPresion* pres = new SensorPresion(id);
                sistema->insertarSensor(pres);
                cout << "sensor de presion creado" << endl;
                break;
            }
            
            case 3: {
                // registrar lectura en un sensor
                if (sistema->getCantidad() == 0) {
                    cout << "no hay sensores registrados" << endl;
                    break;
                }
                
                char id[50];
                cout << "id del sensor: ";
                cin >> id;
                
                SensorBase* sensor = sistema->buscarSensor(id);
                
                if (sensor == nullptr) {
                    cout << "sensor no encontrado" << endl;
                    break;
                }
                
                // usar dynamic_cast para determinar el tipo
                SensorTemperatura* temp = dynamic_cast<SensorTemperatura*>(sensor);
                if (temp != nullptr) {
                    float valor;
                    cout << "temperatura: ";
                    cin >> valor;
                    temp->registrarLectura(valor);
                    cout << "lectura registrada" << endl;
                } else {
                    SensorPresion* pres = dynamic_cast<SensorPresion*>(sensor);
                    if (pres != nullptr) {
                        int valor;
                        cout << "presion: ";
                        cin >> valor;
                        pres->registrarLectura(valor);
                        cout << "lectura registrada" << endl;
                    }
                }
                break;
            }
            
            case 4: {
                // listar todos los sensores
                sistema->listarSensores();
                break;
            }
            
            case 5: {
                // procesar todos los sensores
                sistema->procesarTodos();
                break;
            }
            
            case 6: {
                // leer desde arduino
                cout << "\nconectando a arduino..." << endl;
                cout << "puerto (ejemplo: /dev/ttyUSB0 o /dev/ttyACM0): ";
                char puerto[50];
                cin >> puerto;
                
                ArduinoSerial arduino(puerto);
                
                char buffer[256];
                int lecturas = 0;
                int maxLecturas = 10;
                
                cout << "leyendo " << maxLecturas << " datos del arduino..." << endl;
                
                while (lecturas < maxLecturas) {
                    if (arduino.leerLinea(buffer, 256)) {
                        cout << "recibido: " << buffer << endl;
                        
                        DatoSensor dato = parsearLinea(buffer);
                        
                        if (dato.valido) {
                            SensorBase* sensor = sistema->buscarSensor(dato.id);
                            
                            // crear sensor si no existe
                            if (sensor == nullptr) {
                                if (strcmp(dato.tipo, "TEMP") == 0) {
                                    sensor = new SensorTemperatura(dato.id);
                                    sistema->insertarSensor(sensor);
                                    cout << "sensor temperatura creado: " << dato.id << endl;
                                } else if (strcmp(dato.tipo, "PRES") == 0) {
                                    sensor = new SensorPresion(dato.id);
                                    sistema->insertarSensor(sensor);
                                    cout << "sensor presion creado: " << dato.id << endl;
                                }
                            }
                            
                            // registrar lectura
                            if (sensor != nullptr) {
                                SensorTemperatura* temp = dynamic_cast<SensorTemperatura*>(sensor);
                                if (temp != nullptr) {
                                    temp->registrarLectura(dato.valor);
                                } else {
                                    SensorPresion* pres = dynamic_cast<SensorPresion*>(sensor);
                                    if (pres != nullptr) {
                                        pres->registrarLectura((int)dato.valor);
                                    }
                                }
                            }
                        }
                        
                        lecturas++;
                    }
                    usleep(100000);
                }
                
                cout << "lecturas completadas: " << lecturas << endl;
                break;
            }
            
            case 7: {
                cout << "\ncerrando sistema..." << endl;
                break;
            }
            
            default:
                cout << "opcion invalida" << endl;
                break;
        }
    }
    
    // liberar memoria del sistema
    delete sistema;
    
    cout << "\nsistema cerrado" << endl;
    return 0;
}
