#include <iostream>
#include <cstring>
#include <cmath>
using namespace std;

enum GraphType
{
    Sin,
    Cos,
    Tan,
    Log,
    NLog,
    Linear,
    NegOne,
    Exp,
    Square,
    Cube,
    Mod,
    Abs
};

// Fields
const int CELL = 1; // horizontal cell's size
char output[2000][10000];
int size, columnLength, rowLength, yOrigin;
// Structures
struct Vector2D
{
    int x, y;
    bool Equals(Vector2D v)
    {
        return x == v.x && y == v.y;
    }
};

// Functions
void ToCharArray(const string &str, char ch[CELL])
{
    memset(ch, ' ', CELL);
    for (int i = 0; i < (int)str.length() && i < CELL; i++)
        ch[i] = str[i];
}

void WriteCell(int r, int c, char ch[CELL])
{
    if (r < 0 || r >= columnLength)
        return;
    if (c < 0 || c >= rowLength / CELL)
        return;

    for (int i = 0; i < CELL; i++)
        output[r][c * CELL + i] = ch[i];
}

void DisplayPattern()
{
    system("cls");
    for (int i = 0; i < columnLength; i++)
    {
        string row;
        for (int j = 0; j < rowLength; j++)
            row = row + output[i][j];
        std::cout << row;
        std::cout << "\n";
    }
}

void MoveTowards(Vector2D &a, Vector2D b, string toWrite)
{
    float fx = a.x;
    float fy = a.y;

    char ch[CELL];
    ToCharArray(toWrite, ch);

    while (!( (int)roundf(fx) == b.x && (int)roundf(fy) == b.y ))
    {
        float dx = b.x - fx;
        float dy = b.y - fy;

        float mag = sqrt(dx * dx + dy * dy);
        if (mag == 0)
            break;

        dx /= mag;
        dy /= mag;

        fx += dx * 0.5f;
        fy += dy * 0.5f;

        WriteCell((int)roundf(fy), (int)roundf(fx), ch);
    }

    a = b;
}

void Initializations()
{
    memset(output, ' ', sizeof(output));

    size = 100;
    columnLength = 7 * size;
    rowLength = 7 * size * CELL;

    Vector2D origin = {0, columnLength / 2};
    yOrigin = -columnLength / 2;
    MoveTowards(origin, {rowLength - 1, columnLength / 2}, "--");
    origin = {-yOrigin, 0};
    MoveTowards(origin, {-yOrigin, columnLength - 1}, "|");
}

void ExitPrompt()
{
    cout << "Enter any character to exit...";
    char c;
    cin >> c;
}

bool IsBetween(int num, int upp, int low)
{
    return num >= low && num <= upp;
}

void Circle()
{
    // Vector2D center = {yOrigin, columnLength / 2};
    Vector2D center = {size, size};
    int a = size;
    char ch[CELL];
    ToCharArray("*", ch);
    WriteCell(center.y, center.x, ch);
    for (int r = 0; r < rowLength; r++)
        for (int c = 0; c < columnLength; c++)
        {
            char ch[CELL];
            ToCharArray("*", ch);
            if (IsBetween(pow(c - center.x, 2) + pow(r - center.y, 2), pow(size, 2) + a, pow(size, 2) - a))
            {
                WriteCell(r, c, ch);
                // DisplayPattern();
            }
        }
}

float Function(float x, GraphType gT)
{
    switch (gT)
    {
    case Sin:
        return sin(1 * x * 3.1415 / 180) * (float)size;
        break;

    case Cos:
        return cos(1 * x * 3.1415 / 180) * (float)size;
        break;

    case Tan:
        return tan(1 * x * 3.1415 / 180) * (float)size;
        break;

    case Log:
        return (x <= 0 ? 0 : log10f(x) * 10);
        break;

    case NLog:
        return (x <= 0 ? 0 : log(x) * 10);
        break;

    case Linear:
        return x;
        break;

    case NegOne:
        return pow(-1, (float)x / (float)size) * size;
        break;

    case Exp:
        return pow(2, x / 3);
        break;

    case Square:
        return pow(x, 2) / (float)size;
        break;

    case Cube:
        return pow(x, 3) / (float)size;
        break;

    case Mod:
        return (int)x % size;
        break;

    case Abs:
        return abs(x);
        break;

    default:
        return 0;
    }
}

void GraphGen(GraphType gT)
{
    int x = yOrigin;
    int y = Function(x, gT);
    Vector2D start = {x - yOrigin, columnLength / 2 - y};
    for (x = yOrigin; x <= -yOrigin; x++)
    {
        char ch[CELL];
        ToCharArray("*", ch);
        y = Function(x, gT);

        if (y > columnLength / 2 || y < -columnLength / 2)
            continue;

        Vector2D coord = {x - yOrigin, columnLength / 2 - y};
        WriteCell(coord.y, coord.x, ch); // For raw points
        // MoveTowards(start, coord, "*"); // for each point to be connected
        start = coord;
        // DisplayPattern(); // for the animation, scroll all the way up and zoom out for it to work properly
    }
}

int main()
{
    Initializations();
    GraphGen(Sin);
    GraphGen(Cos);
    GraphGen(Tan);
    GraphGen(Log);
    GraphGen(NLog);
    GraphGen(Square);
    GraphGen(Cube);
    GraphGen(Linear);
    GraphGen(Mod);
    GraphGen(Abs);
    GraphGen(Exp);
    GraphGen(NegOne);
    Circle();
    DisplayPattern();
    ExitPrompt();
    return 0;
}
