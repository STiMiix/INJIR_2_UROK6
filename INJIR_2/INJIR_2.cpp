#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>								
#include "math_3d.h"

GLuint VBO;

void RenderSceneCB() {									// функция для обратного вызова

	glClear(GL_COLOR_BUFFER_BIT);						// очистили буфер кадра, используя заданный цвет

	glEnableVertexAttribArray(0);					// задали нулевую связь между координатами вершин и параметрами шейдера

	glBindBuffer(GL_ARRAY_BUFFER, VBO);				// обратно привязали буфер для отрисовки

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// сообщили конвейеру, как он будет воспринимать данные буфера:
	/* индекс атрибута, количество компонентов в нём (у нас 3 - X, Y и Z), тип компонентов, параметр нормализации атрибута,
	*  число байт между 2 экземплярами атрибута, смещение в структуре
	*/

	glDrawArrays(GL_TRIANGLES, 0, 3);					// (порядковая) функция для отрисовки: точки, индекс первой вершины	и их количество

	glDisableVertexAttribArray(0);					// отключили каждый атрибут вершины
	glutSwapBuffers();									// поменяли местами фоновый буфер и буфер кадра
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);								// инициализировали glut


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		// настрили опции: двойную буферизацию и буфер цвета


	glutInitWindowSize(1024, 768);						// установили размер окна 1024 на 768 пикселей
	glutInitWindowPosition(100, 100);					// установили позицию на экране
	glutCreateWindow("Tutorial 01");					// заголовок окна


	glutDisplayFunc(RenderSceneCB);						// функция, где мы будем отрисовывать один кадр


	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// установили цвет, который будет использован во время след. очистки буфера кадра
			//	 red   green blue alpha-channel

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// сменили цвет, используемый при очистке
	GLenum res = glewInit();
	if (res != GLEW_OK)									// проверка на ошибки
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	Vector3f vecArrTrngl[3];
	vecArrTrngl[0] = Vector3f(-1.0f, -1.0f, 0.0f);
	vecArrTrngl[1] = Vector3f(1.0f, -1.0f, 0.0f);
	vecArrTrngl[2] = Vector3f(0.0f, 1.0f, 0.0f);

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);				// задали, что буфер будет хранить массив вершин

	glBufferData(GL_ARRAY_BUFFER, sizeof(vecArrTrngl), vecArrTrngl, GL_STATIC_DRAW);	/* наполнили объект данными:
	название цели, размер данных(байт), адрес массива, флаг использования паттернов: без изменений значений буфера */

	glutMainLoop();									// передали контроль GLUT
}
