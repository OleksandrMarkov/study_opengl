#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

// ��������� ������� �������
typedef struct
{
    float x,y,z;
}TCell;

// ��������� ������� �������
typedef struct
{
    float r,g,b;
}TColor;

// ��� ����� �������  - ��� ���� (��� ����������� �������)


// ��� ���������� ���������
typedef struct
{
 float u, v;
}TUV;

// ��������� ��'����
typedef struct
{
    float x,y,z; // ����������
    int type; // ���
    float scale // �����
} TObject;

/*����� ������Ͳ 100 �� 100*/

#define mapW 100
#define mapH 100

TCell map[mapW][mapH]; // ����� ������
TCell mapNormal [mapW][mapH]; // ����� ��������
//TColor mapCol [mapW][mapH]; // ����� ������� ��� ������

TUV mapUV[mapW][mapH];


// ����� �������
/*
-1 - �-�� ������� �� ����� �� �� 1 ��. ����� �-� ������ �� ����� ��
6 (�-�� ������)- ����� ������� ��������� � 2 ���������� =>
1 ������� - 6 ������
*/
GLuint mapInd[mapW-1][mapH-1][6];
int mapIndCnt = sizeof(mapInd) / sizeof(GLuint);



//�������
float plant[] = {-0.5,0,0, 0.5,0,0, 0.5,0,1, -0.5,0,1,
        0,-0.5,0, 0,0.5,0, 0,0.5,1, 0,-0.5,1};

// �������� ����������(� ��� ����� ���������� ���������� �� ��������)
float plantUV[] = {0,1, 1,1, 1,0, 0,0, 0,1, 1,1, 1,0, 0,0};

//�������
GLuint plantInd[] = {0,1,2, 2,3,0, 4,5,6, 6,7,4}; // 4 ����������
int plantIndCnt = sizeof(plantInd) / sizeof(GLuint);


int tex_pole, tex_trava, tex_flower, tex_flower2,
tex_grib, tex_tree, tex_tree2;

TObject *plantMas = NULL;
int plantCnt = 0;

#endif // MAIN_H_INCLUDED
