#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

/*Структура опису положення камери*/
struct Scamera{
    float x,y,z;
    float Xrot, Zrot;
}camera;


// Функція керування камерою (крутіння с-ми координат)
void Camera_Apply();

// Повертання камери вздовж осей X, Z
void Camera_Rotation(float xAngle, float zAngle);

//Функція автоматичного обертання камери за допомогою миші
void Camera_AutoMoveByMouse(int centerX, int centerY, float speed);

// Функція руху камери
void Camera_MoveDirection(int forwardMove, int rightMove, float speed);

#endif // CAMERA_H_INCLUDED
