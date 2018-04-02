#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "e00IO.h"
char linea[128];
int g_id = 0;
int id = 0;

const char *msgReadingARC = "reading arc coordinates and topology            ";
const char *msgReadingCNT = "reading polygon centroid coordinates            ";
const char *msgReadingLAB = "reading label points                            ";
const char *msgReadingLOG = "reading coverage history                        ";
const char *msgReadingPAL = "reading polygon topology                        ";
const char *msgReadingPRJ = "reading projection parameters                   ";
const char *msgReadingSIN = "reading spatial index                           ";
const char *msgReadingTOL = "reading tolerance status                        ";
const char *msgReadingTXT = "reading txt annotations                         ";
const char *msgReadingTX6 = "reading tx6 annotations                         ";
const char *msgReadingTX7 = "reading tx7 annotations                         ";
const char *msgReadingRXP = "reading rxp specific to regions                 ";
const char *msgReadingRPL = "reading rpl specific to regions                 ";
const char *msgReadingIFO = "reading info file formats                       ";


static void ChangeExponent(CString& str)
{
	str.Replace("e+0","e+");
	str.Replace("e-0","e-");
}


FILE * openE00File(const char *fileName,int mode)
{
	if (mode==0)
	{
		FILE *file = fopen(fileName,"r");
		if (file)
		{
			//EXP信息			
			fprintf(file,"EXP  0  %s\n",fileName);
			return file;
		}
		return NULL;
	}
	else
	{
		FILE *file = fopen(fileName,"w");
		if (file)
		{
			//EXP信息			
			fprintf(file,"EXP  0  %s\n",fileName);
			return file;
		}
		return NULL;
	}
}

void closeE00File(FILE *archivo)
{
	if (archivo)
	{
		fprintf(archivo,"EOS\n");
		fclose(archivo);
		archivo = NULL;
	}
}

char *readString (char *texto, char *linea, int posicion, int numero)
{
	memcpy (texto, linea + posicion, numero);
    texto[numero] = '\0';
	
	while (texto[numero - 1] == ' ' && numero > 0) {
		texto[--numero] = '\0';
	}
	
	return texto;
}

void readDoublePrecision (FILE *archivo, int number, double *x, double *y)
{
	int index = 0;
	
	for (; index < number; index++)
	{
		fgets (linea, 128, archivo);
		sscanf (linea, "%le %le", x + index, y + index );
	}
}

void writeDoublePrecision (FILE *archivo, int number, double *x, double *y)
{
	CString line;
	int index = 0;	
	for (; index < number; index++)
	{
		line.Format("%15.14e %15.14e",x[index],y[index]);
		ChangeExponent(line);

		fprintf(archivo,"%s\n",(LPCTSTR)line);	
	}
}

void readSinglePrecision (FILE *archivo, int number, double *x, double *y)
{
	int index = 0;

	if (number > 1)
	for (; index < number - 1; index += 2)
	{
		fgets (linea, 128, archivo);
		sscanf (linea, "%le %le %le %le", x + index, y + index, x + index + 1, y + index + 1);
	}
	
	if (number % 2 == 1)
	{
		fgets (linea, 128, archivo);
		sscanf (linea, "%le %le", x + index, y + index);
	}
}

void writeSinglePrecision (FILE *archivo, int number, double *x, double *y)
{
	int index = 0;
	
	if (number > 1)
		for (; index < number - 1; index += 2)
		{	
			fprintf (archivo, " %e %e %e %e\n", x[index], y[index], x[index + 1], y[index + 1]);
		}
		
	if (number % 2 == 1)
	{
		fprintf(archivo, " %e %e\n", x[index], y[index]);
	}
}


void readARCSection (FILE *archivo, strArcInfo *arcinfo, int precision)
{
	int v2, v3, v4, v5, v6, num_coor;

	printf ("%s\r", msgReadingARC);
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		sscanf (linea, "%i %i %i %i %i %i %i", &id, &v2, &v3, &v4, &v5, &v6, &num_coor);

		if (id == -1)
		{
			printf ("%s[DONE]\n", msgReadingARC);
			return;
		}

		arcinfo->arcs = (shapeArcObject*)realloc (arcinfo->arcs, (arcinfo->arcsCount + 1) * sizeof (shapeArcObject));

		arcinfo->arcs[arcinfo->arcsCount].id = id;
		arcinfo->arcs[arcinfo->arcsCount].elements = num_coor;
		arcinfo->arcs[arcinfo->arcsCount].x = (double*)malloc (num_coor * sizeof (double));
		arcinfo->arcs[arcinfo->arcsCount].y = (double*)malloc (num_coor * sizeof (double));

		if (precision == 2)
			readSinglePrecision (archivo, num_coor,
				arcinfo->arcs[arcinfo->arcsCount].x, arcinfo->arcs[arcinfo->arcsCount].y);
		else if (precision == 3)
			readDoublePrecision (archivo, num_coor,
				arcinfo->arcs[arcinfo->arcsCount].x, arcinfo->arcs[arcinfo->arcsCount].y);
		else
			exit (1);

		arcinfo->arcsCount++;
	}
	printf ("%s[FAIL]\n", msgReadingARC);
}

void writeARCSection (FILE *archivo, strArcInfo *arcinfo, int precision,int flag)
{
	//开始标记
	if (precision==2)
	{
		fprintf(archivo,"ARC  2\n");
	}
	else
		fprintf(archivo,"ARC  3\n");

	//弧段数据

	for (int  i=0;i<arcinfo->arcsCount;i++)
	{	
		if((flag==0&&arcinfo->arcs[i].isPartofPolygon)||(flag==1&&arcinfo->arcs[i].isPartofPolygon==false))continue;
		fprintf(archivo,"%10i%10i%10i%10i%10i%10i%10i\n",arcinfo->arcs[i].id,0,0,0,0,0,arcinfo->arcs[i].elements);
		if (precision==2)
		{
			writeSinglePrecision(archivo,arcinfo->arcs[i].elements,arcinfo->arcs[i].x,arcinfo->arcs[i].y);
		}
		else
			writeDoublePrecision(archivo,arcinfo->arcs[i].elements,arcinfo->arcs[i].x,arcinfo->arcs[i].y);
	}
	//结束标记
	fprintf(archivo,"        -1         0         0         0         0         0         0\n");
}


void readCNTSection (FILE *archivo, strArcInfo *arcinfo)
{
	printf ("%s\r", msgReadingCNT);
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		sscanf (linea, "%10i", &id);
		if (id == -1)
		{
			printf ("%s[DONE]\n", msgReadingCNT);
			return;
		}
	}

	printf ("%s[FAIL]\n", msgReadingCNT);
}

void writeCNTSection (FILE *archivo, strArcInfo *arcinfo)
{
// 	printf ("%s\r", msgReadingCNT);
// 	while (!feof (archivo))
// 	{
// 		fgets (linea, 128, archivo);
// 		sscanf (linea, "%10i", &id);
// 		if (id == -1)
// 		{
// 			printf ("%s[DONE]\n", msgReadingCNT);
// 			return;
// 		}
// 	}
// 	
// 	printf ("%s[FAIL]\n", msgReadingCNT);
}

void readLABSection (FILE *archivo, strArcInfo *arcinfo, int precision)
{
	int polygon;
	double x, y;

	printf ("%s\r", msgReadingLAB);
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		sscanf (linea, "%10i%10i%le%le", &id, &polygon, &x, &y);

		if (id == -1)
		{
			printf ("%s[DONE]\n", msgReadingLAB);
			return;
		}
		if (precision==2)
		{
			fgets (linea, 128, archivo);
		}
		else
		{
			fgets (linea, 128, archivo);
			fgets (linea, 128, archivo);
		}
		
		arcinfo->points = (shapePointObject*)realloc(arcinfo->points, (arcinfo->pointsCount + 1) * sizeof (shapePointObject));

		arcinfo->points[arcinfo->pointsCount].id= id;
		arcinfo->points[arcinfo->pointsCount].x = x;
		arcinfo->points[arcinfo->pointsCount].y = y;

		arcinfo->pointsCount++;
	}

	printf ("%s[FAIL]\n", msgReadingLAB);
}

void writeLABSection (FILE *archivo, strArcInfo *arcinfo, int precision)
{
	//开始标记
	if (precision==2)
	{
		fprintf(archivo,"LAB  2\n");
	}
	else
		fprintf(archivo,"LAB  3\n");

	CString line;
	
	//点数据
	for (int i=0;i<arcinfo->pointsCount;i++)
	{	
		if (precision==2)
		{
			fprintf(archivo,"%10i%10i %e %e\n",arcinfo->points[i].id,0,arcinfo->points[i].x,arcinfo->points[i].y);
			fprintf(archivo," %e %e %e %e\n",arcinfo->points[i].x,arcinfo->points[i].y,arcinfo->points[i].x,arcinfo->points[i].y);
		}
		else
		{
			line.Format("%10i%10i %15.14e %15.14e",arcinfo->points[i].id,0,arcinfo->points[i].x,arcinfo->points[i].y);
			ChangeExponent(line);
			fprintf(archivo,"%s\n",(LPCTSTR)line);

			line.Format(" %15.14e %15.14e",arcinfo->points[i].x,arcinfo->points[i].y);
			ChangeExponent(line);

			fprintf(archivo,"%s\n",(LPCTSTR)line);
			fprintf(archivo,"%s\n",(LPCTSTR)line);
		}		
	}
	//结束标记
	if (precision==2)
	{
		fprintf(archivo,"        -1         0 0.0000000E+00 0.0000000E+00\n");
	}
	else
		fprintf(archivo,"        -1         0 0.00000000000000E+00 0.00000000000000E+00\n");
}

void readLOGSection (FILE *archivo, strArcInfo *arcinfo)
{
	printf ("%s\r", msgReadingLOG);

	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		if (!strncmp (linea, "EOL", 3))
		{
			printf ("%s[DONE]\n", msgReadingLOG);
			return;
		}
	}

	printf ("%s[FAIL]\n", msgReadingLOG);
}

void writeLOGSection (FILE *archivo, strArcInfo *arcinfo)
{

}

void readPALSection (FILE *archivo, strArcInfo *arcinfo, int precision)
{
	int coorno, i;
	int id1, id2, to1, to2, from1, from2;

	printf ("%s\r", msgReadingPAL);
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);		
		sscanf (linea, "%d", &coorno);
		if (coorno == -1)
		{
			if(precision==3)
			{
				fgets (linea, 128, archivo);
			}
			printf ("%s[DONE]\n", msgReadingPAL);
			return;
		}

		arcinfo->polygons = (shapePolygonObject*)realloc (arcinfo->polygons, (arcinfo->polygonsCount + 1) * sizeof (shapePolygonObject));
		arcinfo->polygons[arcinfo->polygonsCount].elements = coorno;
		arcinfo->polygons[arcinfo->polygonsCount].arcs = (int*)malloc (coorno * sizeof (int));

		int k = arcinfo->polygonsCount;
		sscanf (linea, "%d %le %le %le %le",
			&arcinfo->polygons[k].elements,&arcinfo->polygons[k].xmin,&arcinfo->polygons[k].ymin,&arcinfo->polygons[k].xmax,&arcinfo->polygons[k].ymax);
		
		i = 0;
		if (coorno > 1)
		for (; i < (coorno - 1); i += 2)
		{
			fgets (linea, 128, archivo);
			sscanf (linea, "%d %d %d %d %d %d", &id1, &from1, &to1, &id2, &from2, &to2);

			arcinfo->polygons[k].arcs[i] = id1;
			arcinfo->polygons[k].arcs[i + 1] = id2;
		}

		if (coorno % 2 == 1)
		{
			fgets (linea, 128, archivo);
			sscanf (linea, "%d %d %d", &id1, &from1, &to1);
			arcinfo->polygons[k].arcs[i] = id1;
		}

		arcinfo->polygonsCount++;
	}

	printf ("%s[FAIL]\n", msgReadingPAL);
}

void writePALSection (FILE *archivo, strArcInfo *arcinfo, int precision)
{
	//开始标记
	if (precision==2)
	{
		fprintf(archivo,"PAL  2\n");
	}
	else
		fprintf(archivo,"PAL  3\n");

	CString line;
	
	//面数据
	for (int i=0;i<arcinfo->polygonsCount;i++)
	{
		if (precision==2)
		{
			fprintf(archivo,"%10i %e %e %e %e\n",arcinfo->polygons[i].elements,arcinfo->polygons[i].xmin,arcinfo->polygons[i].ymin,arcinfo->polygons[i].xmax,arcinfo->polygons[i].ymax);
		}
		else
		{
			line.Format("%10i %15.14e %15.14e %15.14e %15.14e",arcinfo->polygons[i].elements,arcinfo->polygons[i].xmin,arcinfo->polygons[i].ymin,arcinfo->polygons[i].xmax,arcinfo->polygons[i].ymax);
			ChangeExponent(line);

			fprintf(archivo,"%s\n",(LPCTSTR)line);
		}
		if(arcinfo->polygons[i].elements>1)
			for (int j=0;j<arcinfo->polygons[i].elements-1;j+=2)
			{
				fprintf(archivo,"%10i%10i%10i%10i%10i%10i\n",arcinfo->polygons[i].arcs[j],arcinfo->polygons[i].arcs[j],1,arcinfo->polygons[i].arcs[j+1],arcinfo->polygons[i].arcs[j+1],1);
			}
		if (arcinfo->polygons[i].elements%2 == 1)
		{
			fprintf(archivo,"%10i%10i%10i\n",arcinfo->polygons[i].arcs[arcinfo->polygons[i].elements-1],arcinfo->polygons[i].arcs[arcinfo->polygons[i].elements-1],1);
		}
		
	}
	//结束标记
	fprintf(archivo,"        -1         0         0         0         0         0         0\n");
	if(precision==3)
		fprintf(archivo," 0.00000000000000E+00 0.00000000000000E+00\n");
}

void readPRJSection (FILE *archivo, strArcInfo *arcinfo)
{
	printf ("%s\r", msgReadingPRJ);
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		if (!strncmp (linea, "EOP", 3))
		{
			printf ("%s[DONE]\n", msgReadingPRJ);
			return;
		}
	}

	printf ("%s[FAIL]\n", msgReadingPRJ);
}

void writePRJSection (FILE *archivo, strArcInfo *arcinfo)
{

}

void readSINSection (FILE *archivo, strArcInfo *arcinfo)
{
	printf ("%s\r", msgReadingSIN);
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo );
		if (!strncmp (linea, "EOX", 3))
		{
			printf ("%s[DONE]\n", msgReadingSIN);
			return;
		}
	}

	printf ("%s[FAIL]\n", msgReadingSIN);
}

void writeSINSection (FILE *archivo, strArcInfo *arcinfo)
{
	
}

void readTOLSection (FILE *archivo, strArcInfo *arcinfo)
{
	printf ("%s\r", msgReadingTOL);
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		sscanf (linea, "%10i", &id);
		if (id == -1)
		{
			printf ("%s[DONE]\n", msgReadingTOL);
			return;
		}
	}

	printf ("%s[FAIL]\n", msgReadingTOL);
}

void writeTOLSection (FILE *archivo, strArcInfo *arcinfo)
{

}

void readTXTSection (FILE *archivo, strArcInfo *arcinfo, int precision)
{
	int LEVEL,num_vertices1,num_vertices2,SYMBOL ,Number ;
	double x,y,temp0,temp1,temp2,temp,charhei;
	string str;
	printf ("%s\r", msgReadingTXT);
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		sscanf (linea, "%10i", &id);
		if (id == -1)
		{
			printf ("%s[DONE]\n", msgReadingTXT);
			return;
		}
		sscanf(linea,"%10i%10i%10i%10i%10i",&LEVEL,&num_vertices1,&num_vertices2,&SYMBOL,&Number);
		if (precision==2)
		{
			fgets(linea, 128, archivo);
			sscanf(linea," %le %le %le %le %le",&x,&temp0,&temp1,&temp2,&y);
			fgets(linea, 128, archivo);
			fgets(linea, 128, archivo);
			sscanf(linea," %le %le %le %le %le",&temp,&temp0,&temp1,&temp2,&charhei);
			fgets(linea, 128, archivo);			
		}
		else
		{
			fgets(linea, 128, archivo);
			sscanf(linea," %le",&x);
			fgets(linea, 128, archivo);
			sscanf(linea," %le %le %le",&temp1,&y,&temp2);
			fgets(linea, 128, archivo);
			fgets(linea, 128, archivo);
			fgets(linea, 128, archivo);
			sscanf(linea," %le %le %le",&temp1,&temp2,&charhei);
			fgets(linea, 128, archivo);
		}
		str = readFieldValueAsString(archivo,Number);
		arcinfo->texts = (shapeTextObject*)realloc (arcinfo->texts, (arcinfo->textsCount + 1) * sizeof (shapeTextObject));
		arcinfo->texts[arcinfo->textsCount].height = charhei;
		arcinfo->texts[arcinfo->textsCount].x = x;
		arcinfo->texts[arcinfo->textsCount].y = y;
		if(str.size()>127)
		{
			strncpy(arcinfo->texts[arcinfo->textsCount].text,str.c_str(),127);
			arcinfo->texts[arcinfo->textsCount].text[127] = 0;
		}
		else
			strcpy(arcinfo->texts[arcinfo->textsCount].text,str.c_str());
		
		arcinfo->textsCount++;		

	}
}

void writeTXTSection (FILE *archivo, strArcInfo *arcinfo, int precision)
{

}

void readTX6Section (FILE *archivo, strArcInfo *arcinfo, int precision)
{
	printf ("%s\r", msgReadingTX6);
	fgets (linea, 128, archivo);
	if (strncmp (linea, "TEXT", 4)!=0)
	{
		printf ("%s[FIALED]\n", msgReadingTX6);
		return;
	}
	char line[128];
	int charNum;
	double charhei,x,y;
	int num_vertices1,num_vertices2,LEVEL;
	string str;
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		sscanf (linea, "%10i", &id);
		if (id == -1)
		{
			printf ("%s[DONE]\n", msgReadingTX6);
			fgets (linea, 128, archivo);
			return;
		}
// 		if (!strncmp (linea, "JABBERWOCKY", 11))
// 		{
// 			printf ("%s[DONE]\n", msgReadingTX6);
// 			return;
// 		}
		sscanf (linea, "%10i%10i%10i%10i", &id,&LEVEL,&num_vertices1,&num_vertices2);
		readString(line,linea,60,10);		
		charNum = atoi(line);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		if (precision==2)
		{
			sscanf(linea, " %le", &charhei);
			fgets (linea, 128, archivo);
			sscanf(linea, " %le %le", &x,&y);
		}
		else
		{
			sscanf(linea, " %le", &charhei);
			fgets (linea, 128, archivo);
			sscanf(linea, " %le %le", &x,&y);
		}
		for (int i=num_vertices1+num_vertices2-1;i>0;i--)
		{
			fgets (linea, 128, archivo);
		}
		str = readFieldValueAsString(archivo,charNum);
		arcinfo->texts = (shapeTextObject*)realloc (arcinfo->texts, (arcinfo->textsCount + 1) * sizeof (shapeTextObject));
		arcinfo->texts[arcinfo->textsCount].height = charhei;
		arcinfo->texts[arcinfo->textsCount].x = x;
		arcinfo->texts[arcinfo->textsCount].y = y;
		if(str.size()>127)
		{
			strncpy(arcinfo->texts[arcinfo->textsCount].text,str.c_str(),127);
			arcinfo->texts[arcinfo->textsCount].text[127] = 0;
		}
		else
			strcpy(arcinfo->texts[arcinfo->textsCount].text,str.c_str());

		fgets (linea, 128, archivo);
	
		arcinfo->textsCount++;
	}

	printf ("%s[FAIL]\n", msgReadingTX6);
}

void writeTX6Section (FILE *archivo, strArcInfo *arcinfo, int precision)
{
	if (precision==2)
	{
		fprintf(archivo,"TX6  2\nTEXT\n");
	}
	else
		fprintf(archivo,"TX6  3\nTEXT\n");

	CString line;
	
	g_id = 0;
	for (int i=0;i<arcinfo->textsCount;i++)
	{
		fprintf(archivo,"%10i%10i%10i%10i%10i%10i%10i\n",++g_id,1,3,0,1,0,strlen(arcinfo->texts[i].text));
		fprintf(archivo,"         1         0         0         0         0         0         0\n         0         0         0         0         0         0         0\n         0         0         0         0         0         0\n");
		fprintf(archivo,"         1         0         0         0         0         0         0\n         0         0         0         0         0         0         0\n         0         0         0         0         0         0\n");
		fprintf(archivo,"-1.0000000E+02\n");
		if (precision==2)
		{
			fprintf(archivo," %e 0.0000000E+00 0.0000000E+00\n",arcinfo->texts[i].height);
			fprintf(archivo," %e %e\n %e %e\n %e %e\n",arcinfo->texts[i].x,arcinfo->texts[i].y,arcinfo->texts[i].x,arcinfo->texts[i].y,arcinfo->texts[i].x,arcinfo->texts[i].y);
		}
		else
		{
			line.Format(" %15.14e 0.00000000000000E+00 0.00000000000000E+00",arcinfo->texts[i].height);
			ChangeExponent(line);
			fprintf(archivo,"%s\n",(LPCTSTR)line);

			line.Format(" %15.14e %15.14e\n %15.14e %15.14e\n %15.14e %15.14e\n",arcinfo->texts[i].x,arcinfo->texts[i].y,arcinfo->texts[i].x,arcinfo->texts[i].y,arcinfo->texts[i].x,arcinfo->texts[i].y);
			ChangeExponent(line);
			fprintf(archivo,"%s\n",(LPCTSTR)line);

		}
		fprintf(archivo,"%s\n",arcinfo->texts[i].text);		
	}
	//结束标记
	fprintf(archivo,"        -1         0         0         0         0         0         0\nJABBERWOCKY\n");
}


void readTX7Section (FILE *archivo, strArcInfo *arcinfo, int precision)
{
// 	printf ("%s\r", msgReadingTX7);
// 	while (!feof (archivo))
// 	{
// 		fgets (linea, 128, archivo);
// 		if (!strncmp (linea, "JABBERWOCKY", 11))
// 		{
// 			printf ("%s[DONE]\n", msgReadingTX7);
// 			return;
// 		}
// 	}
// 
// 	printf ("%s[FAIL]\n", msgReadingTX7);
	printf ("%s\r", msgReadingTX6);
	fgets (linea, 128, archivo);
	if (strncmp (linea, "TEXT", 4)!=0)
	{
		printf ("%s[FIALED]\n", msgReadingTX7);
		return;
	}
	char line[128];
	int charNum;
	double charhei,x,y;
	int num_vertices1,num_vertices2,LEVEL;
	string str;
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		sscanf (linea, "%10i", &id);
		if (id == -1)
		{
			printf ("%s[DONE]\n", msgReadingTX7);
			fgets (linea, 128, archivo);
			return;
		}
		// 		if (!strncmp (linea, "JABBERWOCKY", 11))
		// 		{
		// 			printf ("%s[DONE]\n", msgReadingTX6);
		// 			return;
		// 		}
		sscanf (linea, "%10i%10i%10i%10i", &id,&LEVEL,&num_vertices1,&num_vertices2);
		readString(line,linea,60,10);		
		charNum = atoi(line);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		fgets (linea, 128, archivo);
		if (precision==2)
		{
			sscanf(linea, " %le", &charhei);
			fgets (linea, 128, archivo);
			sscanf(linea, " %le %le", &x,&y);
		}
		else
		{
			sscanf(linea, " %le", &charhei);
			fgets (linea, 128, archivo);
			sscanf(linea, " %le %le", &x,&y);
		}
		for (int i=num_vertices1+num_vertices2-1;i>0;i--)
		{
			fgets (linea, 128, archivo);
		}
		str = readFieldValueAsString(archivo,charNum+1);
		arcinfo->texts = (shapeTextObject*)realloc (arcinfo->texts, (arcinfo->textsCount + 1) * sizeof (shapeTextObject));
		arcinfo->texts[arcinfo->textsCount].height = charhei;
		arcinfo->texts[arcinfo->textsCount].x = x;
		arcinfo->texts[arcinfo->textsCount].y = y;
		if(str.size()>127)
		{
			strncpy(arcinfo->texts[arcinfo->textsCount].text,str.c_str(),127);
			arcinfo->texts[arcinfo->textsCount].text[127] = 0;
		}
		else
			strcpy(arcinfo->texts[arcinfo->textsCount].text,str.c_str());
		
		arcinfo->textsCount++;
	}
	
	printf ("%s[FAIL]\n", msgReadingTX7);
}

void writeTX7Section (FILE *archivo, strArcInfo *arcinfo)
{

}

void readRXPSection (FILE *archivo, strArcInfo *arcinfo)
{
	printf ("%s\r", msgReadingRXP);
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		if (!strncmp (linea, "JABBERWOCKY", 11))
		{
			printf ("%s[DONE]\n", msgReadingRXP);
			return;
		}
	}

	printf ("%s[FAIL]\n", msgReadingRXP);
}

void writeRXPSection (FILE *archivo, strArcInfo *arcinfo)
{
	
}

void readRPLSection (FILE *archivo, strArcInfo *arcinfo)
{
	printf ("%s\r", msgReadingRPL);
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		if (!strncmp (linea, "JABBERWOCKY", 11))
		{
			printf ("%s[DONE]\n", msgReadingRPL);
			return;
		}
	}

	printf ("%s[FAIL]\n", msgReadingRPL);
}

void writeRPLSection (FILE *archivo, strArcInfo *arcinfo)
{

}

void readIFOSection (FILE *archivo, strArcInfo *arcinfo)
{
	char name[32];
	int validAttrib, totalAttrib, lengthData, numRecords;

	printf ("%s\r", msgReadingIFO);
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		if (!strncmp (linea, "EOI", 3))
		{
			printf ("%s[DONE]\n", msgReadingIFO);
			return;
		}

		sscanf (linea + 34, "%4i%4i%4i%10i", &validAttrib,
			&totalAttrib, &lengthData, &numRecords);
		readString (name, linea, 0, 32);

		readIFORecords (archivo, name, totalAttrib, numRecords);
	}

	printf ("%s[FAIL]\n", msgReadingIFO);
}

void readFieldsInfo(FILE *archivo,E00FieldInfoArray &arry, vector<int> &widths, int precision,int flag)
{
	char  precision0[3], type[4];
	char name[32], width[5], bytes[4];

	int nchar;
	int validAttrib, totalAttrib, lengthData, numRecords ,field;	
	E00FieldInfo fieldInfo;
	while (!feof (archivo))
	{
		fgets (linea, 128, archivo);
		if (strncmp (linea, "EOI", 3)==0)
		{
			//printf ("%s[DONE]\n", msgReadingIFO);
			return;
		}
		readString (name, linea, 0, 31);
		if(flag==0&&strncmp(name+strlen(name)-3,"PAT",3)==0||
			flag==1&&strncmp(name+strlen(name)-3,"AAT",3)==0||
			flag==2&&strncmp(name+strlen(name)-3,"PAT",3)==0||
			flag==3&&strncmp(name+strlen(name)-7,"TATTEXT",7)==0)
		{
			sscanf (linea + 34, "%4i%4i%4i%10i", &validAttrib,
			&totalAttrib, &lengthData, &numRecords);
			for (field = 0; field < totalAttrib; field++)
			{
				if (field==4)
				{
					int fdfd=0;
				}
				fgets (linea, 128, archivo);
				readString (name, linea, 0, 16);
				readString (width, linea, 28, 4);
				readString (bytes, linea, 16, 3);
				readString (precision0, linea, 32, 2);
				readString (type, linea, 34, 3);
				strcpy(fieldInfo.fieldName,name);
				if (!strcmp (type, " 60"))
				{					
					fieldInfo.fieldType = E00Double;
					if(atoi(bytes)==4)
						widths.push_back(14);
// 					if(precision==2)
// 						widths.push_back(14);
					else if (atoi(bytes)>4)
					{
						widths.push_back(24);
					}

				}
				else if (!strcmp (type, " 50"))
				{
					fieldInfo.fieldType = E00Integer;
					widths.push_back(11);
				}
				else if (!strcmp (type, " 40"))
				{
					fieldInfo.fieldType = E00Double;
					widths.push_back(14);
				}
				else if (!strcmp (type, " 30"))
				{
					fieldInfo.fieldType = E00Integer;
					widths.push_back(atoi (width));
				}
				else if (!strcmp (type, " 20"))
				{
					fieldInfo.fieldType = E00String;
					widths.push_back(atoi (width));
				}
				arry.push_back(fieldInfo);			
			}
			return;
		}
	}
}

int readFieldValueAsInteger(FILE *archivo,int width)
{
	int ivalue = 0;
	char *p = new char[width+1];
	int n = 0;
	do 
	{
		if(fgets(p+n,width-n+1,archivo)==NULL) 
		{
			delete []p;
			return 0;
		}		
		n = strlen(p);
		if (p[n-1]=='\n')
		{
			p[n-1] = 0;
			n--;
		}
		
	} while(width!=n);
	ivalue = atoi(p);
	delete []p;
	return ivalue;
}

double readFieldValueAsDouble(FILE *archivo,int width)
{
	double lfvalue = 0;
	char *p = new char[width+1];
	int n = 0;
	do 
	{
		if(fgets(p+n,width-n+1,archivo)==NULL) 
		{
			delete []p;
			return 0;
		}		
		n = strlen(p);
		if (p[n-1]=='\n')
		{
			p[n-1] = 0;
			n--;
		}
		
	} while(width!=n);
	lfvalue = atof(p);
	delete []p;
	return lfvalue;
}

string readFieldValueAsString(FILE *archivo,int width)
{
	char *p = new char[width+1];
	int n = 0;
	do 
	{
		if(fgets(p+n,width-n+1,archivo)==NULL) 
		{
			delete []p;
			return 0;
		}		
		n = strlen(p);
		if (p[n-1]=='\n')
		{
			p[n-1] = 0;
			n--;
		}
		
	} while(width!=n);
	string str = p;
	delete []p;
	return str;
}

void beginWriteIFOSection(FILE *archivo, strArcInfo *arcinfo, int precision)
{
	if (precision==2)
	{
		fprintf(archivo,"IFO  2\n");
	}
	else
		fprintf(archivo,"IFO  3\n");
}

void writeAATIFOField(FILE *archivo, const E00FieldInfoArray *fieldArray, strArcInfo *arcinfo, int precision, int flag)
{
	char *p = (char *)malloc(strlen(arcinfo->attributeName)+5);
	strcpy(p,arcinfo->attributeName);
//	if (flag==1)
	{
		int doublesize = 4;
		if (precision==3)
		{
			doublesize = 8;
		}
		int byteNum=0;
		for (int i=0;i<fieldArray->size();i++)
		{
			if (((*fieldArray)[i]).fieldType==E00Integer)
			{
				byteNum+=4;
			}
			else if (((*fieldArray)[i]).fieldType==E00Double)
			{				
				byteNum+=doublesize;
			}
			else if (((*fieldArray)[i]).fieldType==E00String)
			{
				byteNum+=320;
			}
		}		
		strcat(p,".AAT");
		if (flag==1)
		{
			fprintf(archivo,"%-32sXX%4d%4d%4d%10d\n",p,fieldArray->size(),fieldArray->size(),byteNum,arcinfo->arcsCount-arcinfo->polygonsCount);
		}
		else if (flag==2)
		{
			fprintf(archivo,"%-32sXX%4d%4d%4d%10d\n",p,fieldArray->size(),fieldArray->size(),byteNum,arcinfo->polygonsCount);
		}
		
		int startPos=1;
		for (i=0;i<fieldArray->size();i++)
		{
			if (((*fieldArray)[i]).fieldType==E00Integer)
			{
				fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1                %5d-\n",
					((*fieldArray)[i]).fieldName,4,startPos,5,-1,50,i+1);
				startPos+=4;
			}
			else if (((*fieldArray)[i]).fieldType==E00Double)
			{
				if(precision==2)
					fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1                %5d-\n",
						((*fieldArray)[i]).fieldName,4,startPos,12,3,60,i+1);
				else
					fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1                %5d-\n",
						((*fieldArray)[i]).fieldName,8,startPos,18,5,60,i+1);
				startPos+=doublesize;
			}
			else if (((*fieldArray)[i]).fieldType==E00String)
			{
				fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1                %5d-\n",
					((*fieldArray)[i]).fieldName,320,startPos,320,-1,20,i+1);
				startPos+=320;
			}
		}		
	}
// 	else if (flag==2)
// 	{
// 		strcat(p,".AAT");
// 	}
	if(p)free(p);
}

void writeBNDIFOField(FILE *archivo,  const E00FieldInfoArray *fieldArray, strArcInfo *arcinfo, int precision, int flag)
{
	char *p = (char *)malloc(strlen(arcinfo->attributeName)+5);
	strcpy(p,arcinfo->attributeName);
	//	if (flag==1)
	{
		int doublesize = 4;
		if (precision==3)
		{
			doublesize = 8;
		}
		int byteNum=0;
		for (int i=0;i<fieldArray->size();i++)
		{
			if (((*fieldArray)[i]).fieldType==E00Integer)
			{
				byteNum+=4;
			}
			else if (((*fieldArray)[i]).fieldType==E00Double)
			{				
				byteNum+=doublesize;
			}
			else if (((*fieldArray)[i]).fieldType==E00String)
			{
				byteNum+=320;
			}
		}		
		strcat(p,".BND");
		fprintf(archivo,"%-32sXX%4d%4d%4d%10d\n",p,fieldArray->size(),fieldArray->size(),byteNum,1);
		int startPos=1;
		for (i=0;i<fieldArray->size();i++)
		{
			if (((*fieldArray)[i]).fieldType==E00Integer)
			{
				fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1               %5d-\n",
					((*fieldArray)[i]).fieldName,4,startPos,5,-1,50,i+1);
				startPos+=4;
			}
			else if (((*fieldArray)[i]).fieldType==E00Double)
			{
				if(precision==2)
					fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1               %5d-\n",
					((*fieldArray)[i]).fieldName,4,startPos,12,3,60,i+1);
				else
					fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1               %5d-\n",
					((*fieldArray)[i]).fieldName,8,startPos,18,5,60,i+1);
				startPos+=doublesize;
			}
			else if (((*fieldArray)[i]).fieldType==E00String)
			{
				fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1               %5d-\n",
					((*fieldArray)[i]).fieldName,320,startPos,320,-1,20,i+1);
				startPos+=320;
			}
		}		
	}
	// 	else if (flag==2)
	// 	{
	// 		strcat(p,".AAT");
	// 	}
	if(p)free(p);
}

void writePATIFOField(FILE *archivo,  const E00FieldInfoArray *fieldArray, strArcInfo *arcinfo, int precision, int flag)
{
	char *p = (char *)malloc(strlen(arcinfo->attributeName)+5);
	strcpy(p,arcinfo->attributeName);
	//	if (flag==1)
	{
		int doublesize = 4;
		if (precision==3)
		{
			doublesize = 8;
		}
		int byteNum=0;
		for (int i=0;i<fieldArray->size();i++)
		{
			if (((*fieldArray)[i]).fieldType==E00Integer)
			{
				byteNum+=4;
			}
			else if (((*fieldArray)[i]).fieldType==E00Double)
			{				
				byteNum+=doublesize;
			}
			else if (((*fieldArray)[i]).fieldType==E00String)
			{
				byteNum+=320;
			}
		}		
		strcat(p,".PAT");
		if (flag==0)
		{
			fprintf(archivo,"%-32sXX%4d%4d%4d%10d\n",p,fieldArray->size(),fieldArray->size(),byteNum,arcinfo->pointsCount);
		}
		else if (flag==2)
		{
			fprintf(archivo,"%-32sXX%4d%4d%4d%10d\n",p,fieldArray->size(),fieldArray->size(),byteNum,arcinfo->polygonsCount);
		}
		
		int startPos=1;
		for (i=0;i<fieldArray->size();i++)
		{
			if (((*fieldArray)[i]).fieldType==E00Integer)
			{
				fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1               %5d-\n",
					((*fieldArray)[i]).fieldName,4,startPos,5,-1,50,i+1);
				startPos+=4;
			}
			else if (((*fieldArray)[i]).fieldType==E00Double)
			{
				if(precision==2)
					fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1               %5d-\n",
					((*fieldArray)[i]).fieldName,4,startPos,12,3,60,i+1);
				else
					fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1               %5d-\n",
					((*fieldArray)[i]).fieldName,8,startPos,18,5,60,i+1);
				startPos+=doublesize;
			}
			else if (((*fieldArray)[i]).fieldType==E00String)
			{
				fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1               %5d-\n",
					((*fieldArray)[i]).fieldName,320,startPos,320,-1,20,i+1);
				startPos+=320;
			}
		}		
	}
	// 	else if (flag==2)
	// 	{
	// 		strcat(p,".AAT");
	// 	}
	if(p)free(p);
}

void writeTATTEXTField(FILE *archivo,  const E00FieldInfoArray *fieldArray, strArcInfo *arcinfo, int precision, int flag)
{
	char *p = (char *)malloc(strlen(arcinfo->attributeName)+9);
	strcpy(p,arcinfo->attributeName);
	//	if (flag==1)
	{
		int doublesize = 4;
		if (precision==3)
		{
			doublesize = 8;
		}
		int byteNum=0;
		for (int i=0;i<fieldArray->size();i++)
		{
			if (((*fieldArray)[i]).fieldType==E00Integer)
			{
				byteNum+=4;
			}
			else if (((*fieldArray)[i]).fieldType==E00Double)
			{				
				byteNum+=doublesize;
			}
			else if (((*fieldArray)[i]).fieldType==E00String)
			{
				byteNum+=320;
			}
		}		
		strcat(p,".TATTEXT");
		fprintf(archivo,"%-32sXX%4d%4d%4d%10d\n",p,fieldArray->size(),fieldArray->size(),byteNum,arcinfo->arcsCount-arcinfo->polygonsCount);
		int startPos=1;
		for (i=0;i<fieldArray->size();i++)
		{
			if (((*fieldArray)[i]).fieldType==E00Integer)
			{
				fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1               %5d-\n",
					((*fieldArray)[i]).fieldName,4,startPos,5,-1,50,i+1);
				startPos+=4;
			}
			else if (((*fieldArray)[i]).fieldType==E00Double)
			{
				if(precision==2)
					fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1               %5d-\n",
					((*fieldArray)[i]).fieldName,4,startPos,12,3,60,i+1);
				else
					fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1               %5d-\n",
					((*fieldArray)[i]).fieldName,8,startPos,18,5,60,i+1);
				startPos+=doublesize;
			}
			else if (((*fieldArray)[i]).fieldType==E00String)
			{
				fprintf(archivo,"%-16s%3d-1%4d4-1%4d%2d%3d-1  -1  -1-1               %5d-\n",
					((*fieldArray)[i]).fieldName,320,startPos,320,-1,20,i+1);
				startPos+=320;
			}
		}		
	}
	// 	else if (flag==2)
	// 	{
	// 		strcat(p,".AAT");
	// 	}
	if(p)free(p);
}

void sprintfIntegerBlank(string &str)
{
	for (int i=0;i<11;i++)
	{
		str+=' ';
	}
}

void sprintfDoubleBlank(string &str,int precision)
{
	if (precision==2)
	{
		for (int i=0;i<14;i++)
		{
			str+=' ';
		}	
	}
	else
	{
		for (int i=0;i<24;i++)
		{
			str+=' ';
		}	
	}		
}

void sprintfIntegerValue(string &str, int value)
{
	char line[12];
	sprintf(line,"%11d",value);
	str.append(line);
}

void sprintfBlank(string &str,int cnt)
{
	for (int i=0;i<cnt;i++)
	{
		str+=' ';
	}	
}

void sprintfDoubleValue(string &str, double value, int precision)
{
	if (precision==2)
	{
		char line[15];		
		sprintf(line,"%15.6e",value);
		
		str.append(line);
	}
	else
	{
		char line[25];
		sprintf(line,"%24.14le",value);

		CString line2 = line;
		ChangeExponent(line2);

		str.append((LPCTSTR)line2);
	}	
}

void sprintfStringValue(string &str, char value[128])
{
	char line[321];
	sprintf(line,"%-320s",value);
	str.append(line);
}

void writeIFOFieldValue(FILE *archivo,string &str)
{
	const char *p = str.c_str();
	char line[81]={0};
	for (int i=0;i<str.length();i+=80)
	{
		strncpy(line,p,80);
		fprintf(archivo,"%s\n",line);
		p+=80;
	}
}

void endWriteIFOSection(FILE *archivo, strArcInfo *arcinfo, int precision, int flag)
{
	fprintf(archivo,"EOI\n");
}


void readIFORecords (FILE *archivo, char *filename, int fields, int recno)
{
// 	DBFFieldType types[16];
// 	DBFHandle dbase;
// 	int offset, fieldprecision, widths[16], length = 0, field, record;
// 	char registro[256], fname[32], name[20], width[5], precision[3], type[4], buffer[256];;
// 
// 	sscanf (filename, "%s", fname);
// 	dbase = DBFCreate ("temp.dbf");
// 
// 	for (field = 0; field < fields; field++)
// 	{
// 		fgets (linea, 128, archivo);
// 		readString (name, linea, 0, 16);
// 		readString (width, linea, 28, 4);
// 		readString (precision, linea, 32, 2);
// 		readString (type, linea, 34, 3);
// 
// 		if (!strcmp (type, " 60"))
// 		{
// 			widths[field] = 14;
// 			types[field] = FTDouble;
// 		}
// 		else if (!strcmp (type, " 50"))
// 		{
// 			widths[field] = 11;
// 			types[field] = FTInteger;
// 		}
// 		else if (!strcmp (type, " 40"))
// 		{
// 			widths[field] = 14;
// 			types[field] = FTDouble;
// 		}
// 		else if (!strcmp (type, " 30"))
// 		{
// 			widths[field] = atoi (width);
// 			types[field] = FTInteger;
// 		}
// 		else if (!strcmp (type, " 20"))
// 		{
// 			widths[field] = atoi (width);
// 			types[field] = FTString;
// 		}
// 
// 		fieldprecision = (atoi (precision) < 1) ? 0 : atoi (precision);
// 		if (strlen(name) > 11)
// 			DBFAddField (dbase, name + strlen(name) - 11, types[field], widths[field], fieldprecision);
// 		else
// 			DBFAddField (dbase, name, types[field], widths[field], fieldprecision);
// 		length += widths[field];
// 	}
// 
// 	for (record = 0; record < recno; record++)
// 	{
// 		memset (registro, ' ', 255);
// 		for (offset = 0; offset < length; offset += 80)
// 		{
// 			fgets (linea, 128, archivo);
// 			memcpy (registro + offset, linea, strlen (linea) - 1);
// 		}
// 
// 		for (field = 0, offset = 0; field < fields; offset += widths[field++])
// 		{
// 			readString (buffer, registro, offset, widths[field]);
// 			if (types[field] == FTInteger)
// 				DBFWriteIntegerAttribute (dbase, record, field, atoi (buffer));
//			else if (types[field] == FTDouble)
// 				DBFWriteDoubleAttribute (dbase, record, field, atof (buffer));
// 			else if (types[field] == FTString)
// 				DBFWriteStringAttribute (dbase, record, field, buffer);
// 		}
// 	}
// 
// 	DBFClose (dbase);
// 	rename ("temp.dbf", fname);
}


shapeArcObject findARC(strArcInfo *pInfo, int id)
{
	for( int i=0; i<pInfo->arcsCount; i++)
	{
		if( pInfo->arcs[i].id==id )
		{
			return pInfo->arcs[i];
		}
	}

	shapeArcObject info;
	memset(&info,0,sizeof(info));
	return info;
}