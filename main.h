#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

// Структура клітинок поверхні
typedef struct
{
    float x,y,z;
}TCell;

// Структура кольорів клітинок
typedef struct
{
    float r,g,b;
}TColor;

// Для кожної клітинки  - свій колір (при ініціалізації поверхні)


// Тип текстурних координат
typedef struct
{
 float u, v;
}TUV;

// Структура об'єктів
typedef struct
{
    float x,y,z; // координати
    int type; // тип
    float scale // розмір
} TObject;

/*КАРТА ПОВЕРХНІ 100 на 100*/

#define mapW 100
#define mapH 100

TCell map[mapW][mapH]; // Масив вершин
TCell mapNormal [mapW][mapH]; // Масив нормалей
//TColor mapCol [mapW][mapH]; // Масив кольорів для вершин

TUV mapUV[mapW][mapH];


// Масив індексів
/*
-1 - к-ть клітинок по кожній осі на 1 од. менше к-ті вершин по кожній осі
6 (к-ть вершин)- кожна клітинка малюється з 2 трикутників =>
1 клітинка - 6 вершин
*/
GLuint mapInd[mapW-1][mapH-1][6];
int mapIndCnt = sizeof(mapInd) / sizeof(GLuint);



//Вершини
float plant[] = {-0.5,0,0, 0.5,0,0, 0.5,0,1, -0.5,0,1,
        0,-0.5,0, 0,0.5,0, 0,0.5,1, 0,-0.5,1};

// Текстурні координати(з цих точок відбувається накладання на поверхню)
float plantUV[] = {0,1, 1,1, 1,0, 0,0, 0,1, 1,1, 1,0, 0,0};

//Індекси
GLuint plantInd[] = {0,1,2, 2,3,0, 4,5,6, 6,7,4}; // 4 трикутники
int plantIndCnt = sizeof(plantInd) / sizeof(GLuint);


int tex_pole, tex_trava, tex_flower, tex_flower2,
tex_grib, tex_tree, tex_tree2;

TObject *plantMas = NULL;
int plantCnt = 0;

#endif // MAIN_H_INCLUDED
