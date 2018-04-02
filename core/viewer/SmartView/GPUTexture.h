

/*
ʹ�õ�ȫ��ͷ�ļ�
GL/glew.h

ʹ�õľ�̬���ӿ�
opengl32.lib glu32.lib glew32.lib

GPU_CheckSupported() //����Կ������Ƿ�֧��
GPU_InitGL() //��ʼ��������shader�����
GPU_EnableTexture	//��������ľ���Ŵ�
GPU_SetImageAttrib //��������ĳߴ�
GPU_DrawTexture() //��������ĺ���

*/


bool GPU_CheckSupported();
bool GPU_InitGL();
void GPU_EnableTexture( bool flag);
void GPU_SetImageAttrib( int ImageWidth, int TextureIndex );//���л�����ʱ������ԴͼƬ��С���͸�ͼƬ��Ӧ����������
void GPU_DrawTexture( float x0, float y0, float z0, float tx0, float ty0,
	 float x1, float y1, float z1, float tx1, float ty1,
	 float x2, float y2, float z2, float tx2, float ty2,
	 float x3, float y3, float z3, float tx3, float ty3 );
void GPU_Destroy();

