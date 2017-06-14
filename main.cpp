#include <iostream>

#include <windows.h>
#include <fstream>

using namespace std;

struct PropertyRecord
{
    double length;
    double re;
    double im;
};

struct MaterialRecord
{
    int propertyCount;
    PropertyRecord * propertyRecord;
    char * name;
};

struct MaterialList
{
    int materialCount;
    MaterialRecord * materialRecord;
};

void readMaterialList(MaterialList &ml, char * path, char * exp);
void readMaterialRecord(MaterialRecord &mr, char * path, char * exp);
void readPropertyRecord(PropertyRecord &pr, char * line);

void saveMaterialList(MaterialList &ml, char * path, char * fileName);
void loadMaterialList(MaterialList &ml, char * path, char * fileName);

void printMaterialList(MaterialList &ml, int type = 0);

int main(int argc, char *argv[])
{
    MaterialList ml;
    ml.materialCount = 0;
    ml.materialRecord = new MaterialRecord[1000];

    char path[] = "c:\\MaterialListCreate";
    char exp[] = "nk";
    char fileNameData[] = "_mlData.bin";

    readMaterialList(ml, path, exp);
    saveMaterialList(ml, path, fileNameData);

    MaterialList mlLoad;

    loadMaterialList(mlLoad, path, fileNameData);
    printMaterialList(mlLoad, 1);

    return 0;
}

void readMaterialList(MaterialList &ml, char * path, char * exp)
{
    char fullPath[1000] = "";
    strcat(fullPath, path);
    strcat(fullPath, "\\*.");
    strcat(fullPath, exp);

    WIN32_FIND_DATA FindFileData;
	HANDLE hf;
	hf=FindFirstFile(fullPath, &FindFileData);
	if (hf!=INVALID_HANDLE_VALUE)
	{
		do
		{
		    ml.materialRecord[ml.materialCount].name = new char[strlen(FindFileData.cFileName) - 2];
            strncpy(ml.materialRecord[ml.materialCount].name, FindFileData.cFileName, strlen(FindFileData.cFileName) - 3);
            ml.materialRecord[ml.materialCount].name[strlen(FindFileData.cFileName) - 3] = '\0';

		    readMaterialRecord(ml.materialRecord[ml.materialCount], path, exp);

		    ml.materialCount++;
		}
		while (FindNextFile(hf,&FindFileData)!=0);
		FindClose(hf);
	}
}

void readMaterialRecord(MaterialRecord &mr, char * path, char * exp)
{
    char buff[1000];

    char fullPath[1000] = "";
    strcat(fullPath, path);
    strcat(fullPath, "\\");
    strcat(fullPath, mr.name);
    strcat(fullPath, ".");
    strcat(fullPath, exp);

    mr.propertyCount = 0;
    mr.propertyRecord = new PropertyRecord[10000];

    ifstream fin(fullPath);

    while(fin.getline(buff, 1000))
    {
        if(buff[0] != ';')
        {
            readPropertyRecord(mr.propertyRecord[mr.propertyCount], buff);
            mr.propertyCount++;
        }
    }

    fin.close();
}

void readPropertyRecord(PropertyRecord &pr, char * line)
{
    char l[100], n[100], k[100];
    double in, ik;
    int index = 0;

    while(line[index] == ' ') index++;

    int index2 = 0;
    do
    {
        l[index2] = line[index];
        index2++;
        index++;
    }while(line[index] != ' ');

    while(line[index] == ' ') index++;

    index2 = 0;
    do
    {
        n[index2] = line[index];
        index2++;
        index++;
    }while(line[index] != ' ');

    while(line[index] == ' ') index++;

    index2 = 0;
    do
    {
        k[index2] = line[index];
        index2++;
        index++;
    }while(line[index] != ' ' && line[index] != '\0');

    in = atof(n);
    ik = atof(k);

    pr.length = atof(l) / 10.0;
    pr.re = in*in - ik*ik;
    pr.im = 2*in*ik;
}

void saveMaterialList(MaterialList &ml, char * path, char * fileName)
{
    char fullPath[1000] = "";
    strcat(fullPath, path);
    strcat(fullPath, "\\");
    strcat(fullPath, fileName);

    ofstream fout(fullPath, ios_base::binary);

    fout.write((char*)&ml.materialCount, sizeof ml.materialCount);

    for(int i = 0; i < ml.materialCount; i++)
    {
        int len = strlen(ml.materialRecord[i].name) + 1;

        fout.write((char*)&len, sizeof len);
        fout.write(ml.materialRecord[i].name, len);

        fout.write((char*)&ml.materialRecord[i].propertyCount, sizeof ml.materialRecord[i].propertyCount);

        for(int j = 0; j < ml.materialRecord[i].propertyCount; j++)
        {
            fout.write((char*)&ml.materialRecord[i].propertyRecord[j].length, sizeof ml.materialRecord[i].propertyRecord[j].length);
            fout.write((char*)&ml.materialRecord[i].propertyRecord[j].re, sizeof ml.materialRecord[i].propertyRecord[j].re);
            fout.write((char*)&ml.materialRecord[i].propertyRecord[j].im, sizeof ml.materialRecord[i].propertyRecord[j].im);
        }
    }
    fout.close();
}

void loadMaterialList(MaterialList &ml, char * path, char * fileName)
{
    char fullPath[1000] = "";
    strcat(fullPath, path);
    strcat(fullPath, "\\");
    strcat(fullPath, fileName);

    ifstream fin(fullPath, ios_base::binary);

    fin.read((char*)&ml.materialCount, sizeof ml.materialCount);
    ml.materialRecord = new MaterialRecord[ml.materialCount];

    for(int i = 0; i < ml.materialCount; i++)
    {
        int len;
        fin.read((char*)&len, sizeof len);
        ml.materialRecord[i].name = new char[len];
        fin.read(ml.materialRecord[i].name, len);

        fin.read((char*)&ml.materialRecord[i].propertyCount, sizeof ml.materialRecord[i].propertyCount);
        ml.materialRecord[i].propertyRecord = new PropertyRecord[ml.materialRecord[i].propertyCount];

        for(int j = 0; j < ml.materialRecord[i].propertyCount; j++)
        {
            fin.read((char*)&ml.materialRecord[i].propertyRecord[j].length, sizeof ml.materialRecord[i].propertyRecord[j].length);
            fin.read((char*)&ml.materialRecord[i].propertyRecord[j].re, sizeof ml.materialRecord[i].propertyRecord[j].re);
            fin.read((char*)&ml.materialRecord[i].propertyRecord[j].im, sizeof ml.materialRecord[i].propertyRecord[j].im);
        }
    }
    fin.close();
}

void printMaterialList(MaterialList &ml, int type)
{
    cout << "MaterialList count: " << ml.materialCount << endl;

    if(type == 0) return;

    for(int i = 0; i < ml.materialCount; i++)
    {
        cout << i+1 << ". " << ml.materialRecord[i].name;
        cout << " [count: " << ml.materialRecord[i].propertyCount;
        cout << "] [len: " << ml.materialRecord[i].propertyRecord[ml.materialRecord[i].propertyCount - 1].length;
        cout << "]" << endl;

        if(type == 1) continue;

        for(int j = 0; j < ml.materialRecord[i].propertyCount; j++)
        {
            cout << "  " << ml.materialRecord[i].propertyRecord[j].length;
            cout << "  " << ml.materialRecord[i].propertyRecord[j].re;
            cout << "  " << ml.materialRecord[i].propertyRecord[j].im;
            cout << endl;
        }
    }
}
