#include "GL/gl.h"
#include <windows.h>
#include <math.h>
#include "camera.h"

//Ініціалізація камери на висоті 1.7 од. над поверхнею
struct Scamera camera = {0,0,1.7,0,0};


void Camera_Apply()
{
    glRotatef(-camera.Xrot, 1,0,0);
    glRotatef(-camera.Zrot, 0,0,1);
    glTranslatef(-camera.x, -camera.y, -camera.z);
}

void Camera_Rotation(float xAngle, float zAngle)
{
    camera.Zrot += zAngle;

    if(camera.Zrot < 0) camera.Zrot += 360;
    if(camera.Zrot > 360) camera.Zrot -= 360;

    camera.Xrot += xAngle;

    if(camera.Xrot < 0) camera.Xrot = 0;
    if(camera.Xrot > 180) camera.Xrot = 180;
}

void Camera_AutoMoveByMouse(int centerX, int centerY, float speed)
{
    // УТРИМАННЯ МИШКИ + ПОВОРОТ КАМЕРИ
    POINT cur;
    POINT base = {centerX, centerY};
    GetCursorPos(&cur); // Глобальна позиція курсора миші

    // повертаємо камеру
    Camera_Rotation( (base.y - cur.y)*speed, (base.x - cur.x)*speed);

    // повернення курсору в вихідну позицію
    SetCursorPos(base.x, base.y);
}

void Camera_MoveDirection(int forwardMove, int rightMove, float speed)
{
    //forwardMove - напрям руху по осі(куди дивиться гравець)
    //rightMove - поперечна вісь
    //speed - швидкість

    float ugol = -camera.Zrot / 180 * M_PI; //кут, за яким дивиться гравець (в радіанах)

    if(forwardMove > 0) // рух вперед
        ugol += rightMove > 0 ? M_PI_4 : (rightMove < 0 ? -M_PI_4 : 0);
    if(forwardMove < 0) // рух назад
        ugol += M_PI + (rightMove > 0 ? -M_PI_4 : (rightMove < 0 ? M_PI_4 : 0));
    if(forwardMove == 0) // стоїмо нерухомо
    {
        ugol += rightMove > 0 ? M_PI_2 : -M_PI_2;
        if (rightMove == 0) speed = 0; // гравець не рухається, а направляє камеру
    }

    if(speed != 0) // при русі змінюється положення
    {
        camera.x += sin(ugol) * speed;
        camera.y += cos(ugol) * speed;
    }
}




