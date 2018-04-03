

/*
使用的全部头文件
GL/glew.h

使用的静态链接库
opengl32.lib glu32.lib glew32.lib

GPU_CheckSupported() //检查显卡驱动是否支持
GPU_InitGL() //初始化，编译shader程序等
GPU_EnableTexture	//激活纹理的卷积放大
GPU_SetImageAttrib //设置纹理的尺寸
GPU_DrawTexture() //绘制纹理的函数

*/


bool GPU_CheckSupported();
bool GPU_InitGL();
void GPU_EnableTexture( bool flag);
void GPU_SetImageAttrib( int ImageWidth, int TextureIndex );//当切换纹理时，设置源图片大小，和该图片对应的纹理索引
void GPU_DrawTexture( float x0, float y0, float z0, float tx0, float ty0,
	 float x1, float y1, float z1, float tx1, float ty1,
	 float x2, float y2, float z2, float tx2, float ty2,
	 float x3, float y3, float z3, float tx3, float ty3 );
void GPU_Destroy();

