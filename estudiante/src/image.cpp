/**
 * @file image.cpp
 * @brief Fichero con definiciones para los métodos primitivos de la clase Image
 *
 */

#include <cstring>
#include <cassert>
#include <iostream>
#include <cmath>

#include <image.h>
#include <imageIO.h>

using namespace std;

/********************************
      FUNCIONES PRIVADAS
********************************/
void Image::Allocate(int nrows, int ncols, byte * buffer){
    rows = nrows;
    cols = ncols;

    img = new byte * [rows];

    if (buffer != 0)
        img[0] = buffer;
    else
        img[0] = new byte [rows * cols];

    for (int i=1; i < rows; i++)
        img[i] = img[i-1] + cols;
}

// Función auxiliar para inicializar imágenes con valores por defecto o a partir de un buffer de datos
void Image::Initialize (int nrows, int ncols, byte * buffer){
    if ((nrows == 0) || (ncols == 0)){
        rows = cols = 0;
        img = 0;
    }
    else Allocate(nrows, ncols, buffer);
}

// Función auxiliar para copiar objetos Imagen

void Image::Copy(const Image & orig){
    Initialize(orig.rows,orig.cols);
    for (int k=0; k<rows*cols;k++)
        set_pixel(k,orig.get_pixel(k));
}

// Función auxiliar para destruir objetos Imagen
bool Image::Empty() const{
    return (rows == 0) || (cols == 0);
}

void Image::Destroy(){
    if (!Empty()){
        delete [] img[0];
        delete [] img;
    }
}

LoadResult Image::LoadFromPGM(const char * file_path){
    if (ReadImageKind(file_path) != IMG_PGM)
        return LoadResult::NOT_PGM;

    byte * buffer = ReadPGMImage(file_path, rows, cols);
    if (!buffer)
        return LoadResult::READING_ERROR;

    Initialize(rows, cols, buffer);
    return LoadResult::SUCCESS;
}

/********************************
       FUNCIONES PÚBLICAS
********************************/

// Constructor por defecto

Image::Image(){
    Initialize();
}

// Constructores con parámetros
Image::Image (int nrows, int ncols, byte value){
    Initialize(nrows, ncols);
    for (int k=0; k<rows*cols; k++){
        set_pixel(k,value);
    }
}

bool Image::Load (const char * file_path) {
    Destroy();
    return LoadFromPGM(file_path) == LoadResult::SUCCESS;
}

// Constructor de copias

Image::Image (const Image & orig){
    assert (this != &orig);
    Copy(orig);
}

// Destructor

Image::~Image(){
    Destroy();
}

// Operador de Asignación

Image & Image::operator= (const Image & orig){
    if (this != &orig){
        Destroy();
        Copy(orig);
    }
    return *this;
}

// Métodos de acceso a los campos de la clase

int Image::get_rows() const {
    return rows;
}

int Image::get_cols() const {
    return cols;
}

int Image::size() const{
    return get_rows()*get_cols();
}

// Métodos básicos de edición de imágenes
void Image::set_pixel (int i, int j, byte value) {
    img[i][j] = value;
}
byte Image::get_pixel (int i, int j) const {
    return img[i][j];
}

// This doesn't work if representation changes
void Image::set_pixel (int k, byte value) {
    // TODO this makes assumptions about the internal representation
    // TODO Can you reuse set_pixel(i,j,value)?
    int i = k/cols;
    int j = k%cols;

    set_pixel(i, j, value);
}

// This doesn't work if representation changes
byte Image::get_pixel (int k) const {
    // TODO this makes assumptions about the internal representation
    // TODO Can you reuse get_pixel(i,j)?
    return get_pixel(k/cols, k%cols);
}

// Métodos para almacenar y cargar imagenes en disco
bool Image::Save (const char * file_path) const {
    // TODO this makes assumptions about the internal representation
    byte * p = img[0];
    return WritePGMImage(file_path, p, rows, cols);
}

void Image::Invert() {
    const int MAX_BYTE_VALUE = 255;

    for (int i=0; i < get_rows(); i++)
        for (int j=0; j < get_cols(); j++)
            set_pixel( i, j, MAX_BYTE_VALUE - get_pixel(i, j));
}

double Image::Mean(int i, int j, int height, int width) const {
    return 0;
}

Image Image::Subsample(int factor) const {
    return Image();
}

Image Image::Crop(int nrow, int ncol, int height, int width) const {
    //Compruebo la columna
    if (ncol < 0){
        width = width + ncol;
        ncol = 0;

    } else if (ncol < cols){
        if (ncol+width > cols){
            width = width - ((ncol+width) - cols);
        }

    } else {
        width = 0;
    }

    //Compruebo la fila
    if (nrow < 0){
        height = height + nrow;
        nrow = 0;

    } else if (nrow < rows){
        if (nrow+height > rows){
            height = height - ((nrow+height) - rows);
        }

    } else {
        height = 0;

    }

    Image result(width, height);

    for (int f = 0; f < width; f++){
        for (int c = 0; c < height; c++){
            result.set_pixel(f, c, get_pixel(nrow+f, ncol+c));
        }
    }

    return (result);
}

Image Image::Zoom2X() const {

    //Creamos una imagen con 2n-1 columnas y la completamos
    Image zoom_(get_rows(), (get_cols() * 2) - 1);
    cout << "Creada Imagen Zoom_: " << zoom_.get_rows() << "x" << zoom_.get_cols() << endl;

    for(int j=0; j<zoom_.get_cols(); ++j) {

        if ((j % 2) == 0){
            //Rellenamos las columnas antiguas
            for (int i = 0; i < zoom_.get_rows(); ++i) {

                zoom_.set_pixel(i, j, get_pixel(i , (j / 2)));

            }

        } else {
            //Rellenamos las columnas nuevas interpolando

            for (int i = 0; i < zoom_.get_rows(); ++i){
                zoom_.set_pixel(i, j, round((get_pixel(i, (j-1)/2) + get_pixel(i, (j+1)/2)) / 2.0));
            }

        }
    }

    //Ahora creamos la imagen definitiva y repetimos el proceso
    Image zoom((get_rows()*2) - 1, zoom_.get_cols());
    cout << "Creda Imagen Zoom: " << zoom.get_rows() << "x" << zoom.get_cols() << endl;

    //Copiamos las filas de zoom_ en zoom
    for(int i = 0; i < zoom.get_rows(); i++) {

        if((i%2)==0){

            for (int j = 0; j < zoom.get_cols(); j++) {

                zoom.set_pixel(i, j, zoom_.get_pixel((i / 2), j ));

            }

        } else {
            //Interpolamos las filas restantes

            for (int j = 0; j < zoom.get_cols(); j++) {

                zoom.set_pixel(i, j, round((zoom_.get_pixel((i - 1) / 2, j) + zoom_.get_pixel((i + 1) / 2, j)) / 2.0));

            }
        }

    }

    return (zoom);
}

void Image::AdjustContrast (byte in1, byte in2, byte out1, byte out2){

    const double TRAMO1 = 1.0*out1/in1;
    const double TRAMO2 = 1.0*(out2 - out1)/(in2 - in1);
    const double TRAMO3 = 1.0*(255 - out2)/(255 - in2);

    for(int k=0; k<get_rows()*get_cols(); k++){

        if(get_pixel(k) < in1 ){

            set_pixel(k, round((TRAMO1 * get_pixel(k) )));

        }

        else if(get_pixel(k) <= in2){

            set_pixel(k, round(out1 + (TRAMO2 * (get_pixel(k)-in1) )));

        }

        else{

            set_pixel(k, round(out2 + (TRAMO3 * (get_pixel(k)-in2) )));

        }

    }



}

void Image::ShuffleRows() {

}

