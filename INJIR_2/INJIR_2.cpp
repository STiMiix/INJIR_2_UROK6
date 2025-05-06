#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>								
#include "math_3d.h"
#include <assert.h>

GLuint VBO;
GLuint gWorldLocation;
static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWorld * vec4(Position, 1.0);                                     \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);                                           \n\
}";
void RenderSceneCB() {									// функция для обратного вызова

	glClear(GL_COLOR_BUFFER_BIT);						// очистили буфер кадра, используя заданный цвет
	static float Scale = 0.0f;
	Scale += 0.001f;

	Matrix4f World;
	World.m[0][0] = cosf(Scale); World.m[0][1] = -sinf(Scale); World.m[0][2] = 0.0f; World.m[0][3] = 0.0f;
	World.m[1][0] = sinf(Scale); World.m[1][1] = cosf(Scale); World.m[1][2] = 0.0f; World.m[1][3] = 0.0f;
	World.m[2][0] = 0.0f; World.m[2][1] = 0.0f; World.m[2][2] = 1.0f; World.m[2][3] = 0.0f;
	World.m[3][0] = 0.0f; World.m[3][1] = 0.0f; World.m[3][2] = 0.0f; World.m[3][3] = 1.0f;
	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]);

	glEnableVertexAttribArray(0);					// задали нулевую связь между координатами вершин и параметрами шейдера

	glBindBuffer(GL_ARRAY_BUFFER, VBO);				// обратно привязали буфер для отрисовки

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// сообщили конвейеру, как он будет воспринимать данные буфера:
	/* индекс атрибута, количество компонентов в нём (у нас 3 - X, Y и Z), тип компонентов, параметр нормализации атрибута,
	*  число байт между 2 экземплярами атрибута, смещение в структуре
	*/

	glDrawArrays(GL_TRIANGLES, 0, 3);					// (порядковая) функция для отрисовки: точки, индекс первой вершины	и их количество

	glDisableVertexAttribArray(0);					// отключили каждый атрибут вершины
	glutSwapBuffers();									// поменяли местами фоновый буфер и буфер кадра
	glutPostRedisplay();
}
static void CreateVertexBuffer()
{
	Vector3f vecArrTrngl[3];
	vecArrTrngl[0] = Vector3f(-1.0f, -1.0f, 0.0f);
	vecArrTrngl[1] = Vector3f(1.0f, -1.0f, 0.0f);
	vecArrTrngl[2] = Vector3f(0.0f, 1.0f, 0.0f);

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);				// задали, что буфер будет хранить массив вершин

	glBufferData(GL_ARRAY_BUFFER, sizeof(vecArrTrngl), vecArrTrngl, GL_STATIC_DRAW);	/* наполнили объект данными:
	название цели, размер данных(байт), адрес массива, флаг использования паттернов: без изменений значений буфера */
}
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	glShaderSource(ShaderObj, 1, p, Lengths);
	glCompileShader(ShaderObj);
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	glAttachShader(ShaderProgram, ShaderObj);
}
static void CompileShaders()
{
	GLuint ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
	AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glValidateProgram(ShaderProgram);

	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glUseProgram(ShaderProgram);

	gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	assert(gWorldLocation != 0xFFFFFFFF);
}
int main(int argc, char** argv) {

	glutInit(&argc, argv);								// инициализировали glut


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);		// настрили опции: двойную буферизацию и буфер цвета


	glutInitWindowSize(1024, 768);						// установили размер окна 1024 на 768 пикселей
	glutInitWindowPosition(100, 100);					// установили позицию на экране
	glutCreateWindow("UROK6");					// заголовок окна


	glutDisplayFunc(RenderSceneCB);						// функция, где мы будем отрисовывать один кадр

	GLenum res = glewInit();
	if (res != GLEW_OK)									// проверка на ошибки
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// сменили цвет, используемый при очистке
	CreateVertexBuffer();
	CompileShaders();
	glutMainLoop();									// передали контроль GLUT
}
