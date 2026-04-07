#include <iostream>
#include <cstring>
#include <cmath>
#include <thread>
using namespace std;

class Vector
{
    public:

    float x, y, z;
    
    float Magnitude() const { return sqrt(x*x + y*y + z*z); }
    Vector Normalized() const { return Magnitude() == 0 ? Vector{0,0,0} : *this / Magnitude(); }
	float Distance(const Vector& other) const { return sqrt((x-other.x)*(x-other.x) + (y-other.y)*(y-other.y) + (z-other.z)*(z-other.z));}
    Vector cross(const Vector &other) const { return { y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x}; }

    Vector operator+(const Vector &other) const { return {x+other.x, y+other.y, z+other.z}; }
    Vector operator-(const Vector &other) const { return {x-other.x, y-other.y, z-other.z}; }
    Vector operator/(float f) const { return {(x/f), (y/f), (z/f)}; }
    Vector operator*(float f) const { return {(x*f), (y*f), (z*f)}; }
    
};

class Edge
{
    public:
    Vector a, b;
};

Vector WORLDUP = {0, 1, 0};

const int sizeW = 200, sizeO = 200, pSizeW = 200, mThreads = 8;
//               y     x     z
char (*worldMatrix)[sizeW][sizeW] = new char[sizeW][sizeW][sizeW],
output[sizeO][sizeO];
// string shade = " .:-=+*#%@";
inline void WriteCell(Vector v, char ch)
{
    int x=(int)roundf(v.x), y=(int)roundf(v.y), z=(int)roundf(v.z);

    if (x < 0 || x >= sizeW) return;
    if (y < 0 || y >= sizeW) return;
    if (z < 0 || z >= sizeW) return;
    
    worldMatrix[y][x][z] = ch;
}

void Initialize()
{
    memset(worldMatrix, ' ', sizeW*sizeW*sizeW);
    memset(output, ' ', sizeO*sizeO);
}

void Interpolate(Vector &v, Vector tar, char toWrite)
{
    while (v.Distance(tar)>=0.01f)
    {
        Vector dir = tar - v;
        if (dir.Magnitude() == 0)
            return;

        if (dir.Magnitude()>1) dir = dir.Normalized();
        
        v = v + dir*0.5f;
        WriteCell(v, toWrite);
    }
}

void FillTriangle(Vector a, Vector b, Vector c, char ch) {
    
    if (a.Distance(b) < 1.0f && b.Distance(c) < 1.0f && c.Distance(a) < 1.0f) {
        WriteCell(a, ch);
        return;
    }

    Vector ab = (a + b) * 0.5f;
    Vector bc = (b + c) * 0.5f;
    Vector ca = (c + a) * 0.5f;

    FillTriangle(a, ab, ca, ch);
    FillTriangle(b, ab, bc, ch);
    FillTriangle(c, bc, ca, ch);
    FillTriangle(ab, bc, ca, ch);
}

void DrawCube(Vector c, float size, char ch, Vector lookAt = {sizeW/2, sizeW/2, sizeW/2})
{
    float halfSize = size / 2;

    Vector fwd = (lookAt - c).Normalized(),
    right = fwd.cross(WORLDUP).Normalized(),
    up = right.cross(fwd).Normalized(),
    verticesI[8] =
    {
        {-halfSize,-halfSize,-halfSize},
        { halfSize,-halfSize,-halfSize},
        { halfSize, halfSize,-halfSize},
        {-halfSize, halfSize,-halfSize},
        {-halfSize,-halfSize, halfSize},
        { halfSize,-halfSize, halfSize},
        { halfSize, halfSize, halfSize},
        {-halfSize, halfSize, halfSize}
    };

    Vector verticesF[8];
    for(int i=0;i<8;i++)
        verticesF[i] = c + right * verticesI[i].x + up * verticesI[i].y + fwd * verticesI[i].z;


    Edge edges[12] =
    {
        {verticesF[0],verticesF[1]},
        {verticesF[1],verticesF[2]},
        {verticesF[2],verticesF[3]},
        {verticesF[3],verticesF[0]},

        {verticesF[4],verticesF[5]},
        {verticesF[5],verticesF[6]},
        {verticesF[6],verticesF[7]},
        {verticesF[7],verticesF[4]},

        {verticesF[0],verticesF[4]},
        {verticesF[1],verticesF[5]},
        {verticesF[2],verticesF[6]},
        {verticesF[3],verticesF[7]}
    };
    
    
    for(int i=0;i<12;i++)
        Interpolate(edges[i].a, edges[i].b, ch);    

    // for (int i = 0; i < 4; i++)
    //     FillTriangle(verticesF[i], verticesF[i+1], verticesF[i+2], '.');
}

void DrawTetra(Vector c, float size, char ch, Vector lookAt = {sizeW/2, sizeW/2, sizeW/2})
{
    float halfSize = size / 2;
    Vector verticesI[4] =
    {
        {0, -halfSize, 0},
        {halfSize, halfSize, 0},
        {-halfSize, halfSize, 0},
        {0, halfSize, halfSize}
    }, fwd = (lookAt - c).Normalized(),
    right = fwd.cross(WORLDUP).Normalized(),
    up = right.cross(fwd).Normalized();

    Vector verticesF[8];
    for(int i=0;i<8;i++)
        verticesF[i] = c + right * verticesI[i].x + up * verticesI[i].y + fwd * verticesI[i].z;

    Edge edges[6] =
    {
        {verticesF[0], verticesF[1]},
        {verticesF[0], verticesF[2]},
        {verticesF[0], verticesF[3]},
        {verticesF[1], verticesF[2]},
        {verticesF[1], verticesF[3]},
        {verticesF[2], verticesF[3]}
    };

    for (int i = 0; i < 2; i++)
        FillTriangle(verticesF[i], verticesF[i+1], verticesF[i+2], '.');
    for(int i=0;i<6;i++)
        Interpolate(edges[i].a, edges[i].b, ch);

}

Vector GetNormal(float x, float y, float z)
{
    float nx, ny, nz;
    if (x > 0 && worldMatrix[(int)y][(int)x-1][(int)z] != ' ') nx = 1;
    else if (x < sizeW-1 && worldMatrix[(int)y][(int)x+1][(int)z] != ' ') nx = -1;
    else nx = 0;
    
    if (y > 0 && worldMatrix[(int)y-1][(int)x][(int)z] != ' ') ny = 1;
    else if (y < sizeW-1 && worldMatrix[(int)y+1][(int)x][(int)z] != ' ') ny = -1;
    else ny = 0;
    
    if (z > 0 && worldMatrix[(int)y][(int)x][(int)z-1] != ' ') nz = 1;
    else if (z < sizeW-1 && worldMatrix[(int)y][(int)x][(int)z+1] != ' ') nz = -1;
    else nz = 0;

    return Vector{nx, ny, nz};
}

inline char Raycast(float ox, float oy, float oz, float dx, float dy, float dz) {

    float tx = ox, ty = oy, tz = oz;
    for (int i = 0; i<pSizeW+100; i++)
    {
        tx += dx; ty += dy; tz += dz;
        
        int ix = (int)tx, iy = (int)ty, iz = (int)tz;
        
        if (ix < 0 || ix >= sizeW || iy < 0 || iy >= sizeW || iz < 0 || iz >= sizeW) 
            return ' ';

        char hit = worldMatrix[iy][ix][iz];
        if (hit != ' ') return hit;
    }
    return ' ';
}

Vector lookAt = {pSizeW/2, pSizeW/2, pSizeW/2};
void Render(const Vector &cPos, float fov = 1.5f, int iter = 0)
{
    Vector forward = (lookAt - cPos).Normalized();
    Vector right = forward.cross(WORLDUP).Normalized();
    Vector up = right.cross(forward).Normalized();

    int x, y;
    for(y=iter*sizeO/mThreads; y<(iter+1)*sizeO/mThreads; y++)
        for(x=0; x<sizeO; x++)
        {
            Vector dir = (forward + right*(x/(float)sizeO - 0.5f)*fov + up*(y/(float)sizeO - 0.5f)*fov).Normalized();
            output[y][x] = Raycast(cPos.x, cPos.y, cPos.z, dir.x, dir.y, dir.z);
        }
}

string t = "";
void Display()
{
    t = "";
    printf("\x1b[H");
    // system("cls");
    for (int i = 0; i<sizeO; i++)
    {
        for (int j = 0;j<sizeO; j++)
            t+=output[i][j];
        t+= '\n';
    }
    cout<<t;
}

void DrawFloor(char ch) {
    for (int x = 0; x < sizeW; x += 10) 
        for (int z = 0; z < sizeW; z++) WriteCell({(float)x, pSizeW/2+40, (float)z}, ch);
    
    for (int z = 0; z < sizeW; z += 10) 
        for (int x = 0; x < sizeW; x++) WriteCell({(float)x, pSizeW/2+40, (float)z}, ch);
}

int main()
{
    float n = 0;
    system("pause");
    Initialize();

    DrawCube({pSizeW/3, pSizeW/2, pSizeW/2}, 40, '@');
    DrawTetra({pSizeW*2/3, pSizeW/2, pSizeW/2}, 40, '#');
    DrawFloor('.');

    while(n<=200)
    {
        // n=200;
        thread threads[mThreads];
        for (int i = 0; i < mThreads; i++)
            threads[i] = thread(Render, Vector{10,200-n,  sizeW/2+n/10}, n/400, i);

        for (int i = 0; i < mThreads; i++)
            threads[i].join();
        Display();
        n++;
        memset(output, ' ', sizeO*sizeO);
        this_thread::sleep_for(chrono::milliseconds(16));
    }

    delete[] worldMatrix;
    system("pause");
}