#include <windows.h>
#include <gl/gl.h>
#include <math.h>

#include "../open_gl/camera.h"


#define STB_IMAGE_IMPLEMENTATION
#include "../open_gl/stb-master/stb_image.h" // файл для роботи з завантаженням текстури

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

#include "../open_gl/main.h"


void LoadTexture(char *file_name, int *target)
{
    // target - вказівник на змінну з номером текстури
    int width, height, cnt;

    //filename - ім'я файлу текстури
    // cnt - к-ть компонентів кольору на 1 піксель
    // при cnt = 4 - використовуємо RGBA, інакше RGB
    unsigned char *data = stbi_load(file_name, &width, &height, &cnt, 0);
    glGenTextures(1, target); // створюємо 1 текстуру
    glBindTexture(GL_TEXTURE_2D, *target); // робимо її активною

        // Параметри текстури:
        // 2D
        //GL_TEXTURE_WRAP_S / GL_TEXTURE_WRAP_T - заповнення текстури по горизонталі/вертикалі
        // GL_TEXTURE_MIN_FILTER / GL_TEXTURE_MAG_FILTER - звуження/розширення текстури
        //GL_REPEAT - повторення текстури при виході за її межі
        //GL_NEAREST - беремо текстуру сусідньої точки
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // завантаження текстури у відеокарту
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                      0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data); // звільнення пам'яті після завантаження текстури
}




#define sqr(a) (a)*(a)

//Функція розрахунку нормалей за 3-ма точками(для кожної клітинки поверхні)
// 4-й параметр - вказівник на нормаль
void CalcNormals(TCell a, TCell b, TCell c, TCell *n)
{
    float wrki;
    TCell v1, v2;

    v1.x = a.x - b.x;
    v1.y = a.y - b.y;
    v1.z = a.z - b.z;
    v2.x = b.x - c.x;
    v2.y = b.y - c.y;
    v2.z = b.z - c.z;

    n->x = (v1.y * v2.z - v1.z * v2.y);
    n->y = (v1.z * v2.x - v1.x * v2.z);
    n->z = (v1.x * v2.y - v1.y * v2.x);

    wrki = sqrt(sqr(n->x) + sqr(n->y) + sqr(n->z));
    n -> x /= wrki;
    n -> y /= wrki;
    n -> z /= wrki;
}

// Перевірка виходу гравця за межі поверхні
BOOL IsCoordInMap(float x, float y)
{
    return (x >= 0) && (x < mapW) && (y >= 0) && (y < mapH);
}

// Створення пагорбів на поверхні (зміненням висоти окремих вершин )
void Map_CreateHill(int posX, int posY, int rad, int height)
{
    for(int i = posX - rad; i <= posX + rad; i++)
        for(int j = posY - rad; j <= posY + rad; j++)
            if(IsCoordInMap(i,j))
            {
                float len = sqrt( pow(posX - i, 2) + pow(posY - j, 2));
                if(len < rad)
                {
                    len = len / rad * M_PI_2;
                    map[i][j].z += cos(len) * height;
                }
            }
}

// Розрахунок висоти над поверхнею (за координатами)
float Map_GetHeight(float x, float y) // на якій висоті гравець над поверхнею
{
    if (!IsCoordInMap(x,y)) return 0;
    int cX = (int)x;
    int cY = (int)y;
    float h1 = ( (1 - (x - cX))*map[cX][cY].z + (x - cX)*map[cX+1][cY].z); //X - нижні ребра клітинки
    float h2 = ( (1 - (x - cX))*map[cX][cY + 1].z + (x - cX)*map[cX+1][cY+1].z); //Y - верхні ребра клітинки

    return (1 - (y - cY))*h1 + (y - cY)*h2;
}

//Ініціалізація поверхні
void Map_Init()
{
    LoadTexture("textures/pole.png", & tex_pole);
    LoadTexture("textures/trava.png", & tex_trava);
    LoadTexture("textures/flower.png", & tex_flower);
    LoadTexture("textures/flower2.png", & tex_flower2);
    LoadTexture("textures/grib.png", & tex_grib);
    LoadTexture("textures/tree.png", & tex_tree);
    LoadTexture("textures/tree2.png", & tex_tree2);

   // Налаштування освітлення 111-114
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE); // для плавного освітлення

    // Не показуємо прозорі пікселі
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.99);



    for(int i = 0; i < mapW; i++)
        for(int j = 0; j < mapH; j++)
        {
           /* float dc = (rand()%20) * 0.01;
            mapCol[i][j].r = 0.21 + dc; //0.31
            mapCol[i][j].g = 0.15 + dc; //0.5
            mapCol[i][j].b = 0.73 + dc; //0.13*/



            map[i][j].x = i;
            map[i][j].y = j;
            map[i][j].z = (rand() % 10 ) * 0.05; // імітація нерівностей (мікрорельєф)



            mapUV[i][j].u = i;
            mapUV[i][j].v = j;

        }

        //масив індексів, за якими малюємо поверхню
    for(int i = 0; i < mapW - 1; i++)
    {
        int pos = i*mapH; //номер ячейки масиву(скрізна нумерація - як в пам'яті)
        for(int j = 0; j < mapH - 1; j++)
        {
            mapInd[i][j][0] = pos;
            mapInd[i][j][1] = pos + 1; //перехід на сусідню вершину (по осі Y)
            mapInd[i][j][2] = pos + 1 + mapH; //перехід на сусідню вершину (по осі X)

            mapInd[i][j][3] = pos + 1 + mapH;
            mapInd[i][j][4] = pos + mapH;
            mapInd[i][j][5] = pos;

            pos++;
        }
    }

    // Генерація пагорбів (10 шт.)
    for(int i = 0; i < 10; i++)
    {
        Map_CreateHill(rand() % mapW, rand() % mapH, rand() % 50, rand() % 10);
    }

    //Імітація освітлення
    for(int i = 0; i < mapW - 1; i++)
            for(int j = 0; j < mapH - 1; j++)
            {
                // Розрахунок нормалі (точки передаються за порядком проти годинникової стрілки)
                CalcNormals(map[i][j], map[i+1][j], map[i][j+1], &mapNormal[i][j]);
            }

     // Ініціалізація об'єктів
     int travaN = 2000;
     int gribN = 30;
     int treeN = 40;

     plantCnt = travaN + gribN + treeN;
     plantMas = realloc(plantMas, sizeof(*plantMas) * plantCnt);
     for (int i = 0; i < plantCnt; i++)
     {
         if (i < travaN)
         {
             plantMas[i].type = rand() % 10 != 0 ? tex_trava:
                 (rand()%2 == 0 ? tex_flower : tex_flower2);

             plantMas[i].scale = 0.7 + (rand() % 5) * 0.1;
         }
         else if (i < travaN + gribN)
         {
             plantMas[i].type = tex_grib;
             plantMas[i].scale = 0.2 + (rand() % 10) * 0.01;
         }
         else
         {
             plantMas[i].type = rand() % 2 == 0 ? tex_tree : tex_tree2;
             plantMas[i].scale = 4 + (rand() % 14);
         }

         plantMas[i].x = rand() % mapW;
         plantMas[i].y = rand() % mapH;
         plantMas[i].z = Map_GetHeight(plantMas[i].x, plantMas[i].y);
     }


}

// Відображення поверхні
void Map_Show()
{
    // СХІД ТА ЗАХІД СОНЦЯ
    static float alfa = 0;
    alfa += 0.3;
    if(alfa > 180) alfa -= 360;

    glClearColor(0.6f, 0.8f, 1.0f, 0.0f); // Фон кольору неба
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);

    glPushMatrix();

    Camera_Apply();

    glPushMatrix();
        glRotatef(alfa, 0,1,0);
        GLfloat position[] = {0, 0, 1, 0};
        glLightfv(GL_LIGHT0, GL_POSITION, position); // позиція джерела світла
    glPopMatrix();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, map);
        glTexCoordPointer(2, GL_FLOAT, 0, mapUV);
        glColor3f(0.7, 0.7, 0.7);
        glNormalPointer(GL_FLOAT, 0, mapNormal);
        glBindTexture(GL_TEXTURE_2D, tex_pole);
        glDrawElements(GL_TRIANGLES, mapIndCnt, GL_UNSIGNED_INT, mapInd);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    // РОСЛИНИ НА ПОВЕРХНІ
        glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, plant);
        glTexCoordPointer(2, GL_FLOAT, 0, plantUV);
        glColor3f(0.7, 0.7, 0.7);
        glNormal3f(0,0,1);
        //glNormalPointer(GL_FLOAT, 0, mapNormal);

        for(int i = 0; i < plantCnt; i++)
        {
            glBindTexture(GL_TEXTURE_2D, plantMas[i].type);
            glPushMatrix();
                glTranslatef(plantMas[i].x, plantMas[i].y, plantMas[i].z);
                glScalef(plantMas[i].scale, plantMas[i].scale, plantMas[i].scale);
                glDrawElements(GL_TRIANGLES, plantIndCnt, GL_UNSIGNED_INT, plantInd);
            glPopMatrix();
        }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //glDisableClientState(GL_COLOR_ARRAY);
   // glDisableClientState(GL_NORMAL_ARRAY);

    glPopMatrix();
}

// Рух гравця (клавіші "W", "A", "S", "D")
void Player_Move()
{
    Camera_MoveDirection(GetKeyState('W') < 0 ? 1 : (GetKeyState('S') < 0 ? -1: 0),
                         GetKeyState('D') < 0 ? 1 : (GetKeyState('A') < 0 ? -1: 0),
                         0.1);
    Camera_AutoMoveByMouse(400,400,0.2);

    camera.z = Map_GetHeight(camera.x, camera.y) + 1.7;
}

/*Функція зміни налаштувань проекції в залежності від розмірів вікна*/
// При зміні розмірів вікна картина не "розвалюється"
void WndResize(int x, int y)
{
    glViewport(0,0,x,y);
    float k = x / (float) y; // Кут огляду
    float sz = 0.1;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-k*sz, k*sz, -sz,sz, sz*2,1000); // 1000 - відстань від камери до лінії горизонту
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1100,
                          700,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    RECT rct;
    GetClientRect(hwnd, &rct);
    WndResize(rct.right, rct.bottom);
    Map_Init();
    glEnable(GL_DEPTH_TEST);

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {

        // Якщо вікно знаходиться у фокусі миші, вона утримується
        // (для зручності гортання)
            if (GetForegroundWindow() == hwnd)
            {
                Player_Move();
            }

            Map_Show();
            SwapBuffers(hDC);

            theta += 1.0f;
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_SIZE: // Реакція на зміну розмірів вікна
            WndResize(LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_SETCURSOR: //Приховання курсора миші всередині вікна
            ShowCursor(FALSE);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

