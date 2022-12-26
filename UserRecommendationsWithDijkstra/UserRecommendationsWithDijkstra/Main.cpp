#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include "graphics.h"
#define SLEEP 25

using namespace std;

// Struct điểm trên Map
// Tọa độ theo trụ x
// Tọa độ theo trục y
// Số thứ tự của điểm 
struct Point
{
    int x;
    int y;
    char name;
};

// Mảng chứa các điểm trên Map   // Biến chứa số lượng điểm hiện tại trong mảng 
Point* arrayPoint;  int countPointCurrent;
char arrayName[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'M', 'N', 'O', 'P', 'Q' };

struct QueueNode
{
    int V; //Node Y
    int Vy; // Sau khi kết thúc các Vy là giá trị nhỏ nhất của độ dài từ gốc đến Y

    void init(int V, int Vy) {

        this->V = V;
        this->Vy = Vy;
    }
};

struct PriorityQueue
{
    int currentSize;
    int maxSize;
    int* position;
    QueueNode** ptrToArray;

    void init(int maxSize) {

        this->currentSize = 0;
        this->maxSize = maxSize;
        position = new int[maxSize + 1];
        ptrToArray = new QueueNode * [maxSize + 1];
    }

    bool isEmpty()
    {
        return currentSize == 0;
    }

    void swap(QueueNode*& nodeA, QueueNode*& nodeB)
    {
        QueueNode* tmp = nodeA;
        nodeA = nodeB;
        nodeB = tmp;
    }
    // Lấy vị trí của Key Left
    int getIndexLeft(int index) {

        return (2 * index + 1);
    }

    // Lấy vị trí của Key Right
    int getIndexRight(int index) {

        return (2 * index + 2);
    }
    int getIndexParent(int index) {

        return (index - 1) / 2;
    }
    void Update(int index)
    {
        int left = getIndexLeft(index);
        int right = getIndexRight(index);
        int smallest = index;

        if (left < currentSize && ptrToArray[left]->Vy < ptrToArray[smallest]->Vy)
            smallest = left;

        if (right < currentSize && ptrToArray[right]->Vy < ptrToArray[smallest]->Vy)
            smallest = right;
        if (smallest != index)
        {
            QueueNode* smallestNode = ptrToArray[smallest];
            QueueNode* indexNode = ptrToArray[index];

            //Cap nhat pos de tim kiem Node
            position[smallestNode->V] = index;
            position[indexNode->V] = smallest;

            swap(ptrToArray[smallest], ptrToArray[index]);
            Update(smallest);
        }
    }

    QueueNode* getElementMin() {

        QueueNode* root = new QueueNode();
        if (isEmpty()) {

            root->V = -1;
            return root;
        }

        if (currentSize == 1) {

            currentSize = 0;
            return ptrToArray[0];
        }

        root = ptrToArray[0];
        //Cap nhat vi tri cua cac V
        position[root->V] = currentSize - 1;
        position[ptrToArray[currentSize - 1]->V] = 0;

        ptrToArray[0] = ptrToArray[currentSize - 1];
        currentSize--;
        // Khôi phục thuộc tính MinHeap của cây
        Update(0);

        return root;
    }
    void decreaseKey(int V, int Vy)
    {
        int index = position[V];
        ptrToArray[index]->Vy = Vy;

        while (index && ptrToArray[getIndexParent(index)]->Vy > ptrToArray[index]->Vy) {

            position[ptrToArray[index]->V] = getIndexParent(index);
            position[ptrToArray[getIndexParent(index)]->V] = index;

            swap(ptrToArray[index], ptrToArray[getIndexParent(index)]);
            index = getIndexParent(index);
        }
    }
    void insertData(int V, double Vy)
    {
        int index = currentSize;
        currentSize++;
        QueueNode* nodeTmp = (QueueNode*)malloc(sizeof(QueueNode));
        nodeTmp->init(V, Vy);
        ptrToArray[index] = nodeTmp;
        position[V] = index;
        // Khôi phục thuộc tính MinHeap nếu nó bị vi phạm bởi việc thêm vào trên
        while (index != 0 && ptrToArray[getIndexParent(index)]->Vy > ptrToArray[index]->Vy) {
            position[ptrToArray[index]->V] = getIndexParent(index);
            position[ptrToArray[getIndexParent(index)]->V] = index;
            swap(ptrToArray[index], ptrToArray[getIndexParent(index)]);
            index = getIndexParent(index);
        }
    }
    void displayQueue() {

        for (int v = 0; v < currentSize; v++) {

            cout << ptrToArray[v]->V << "|" << ptrToArray[v]->Vy << "|" << position[ptrToArray[v]->V] << "   ";
        }
        cout << endl;
    }
};

struct Node
{
    int dest;
    int cost;
    Node* next; //Liên kết đến các đích khác của src
    bool CreateNode(int dest, int cost)
    {
        this->dest = dest;
        this->cost = cost;
        this->next = nullptr;
        return true;
    }
};
struct ListNode
{
    Node* head;
};
struct Graph
{
    int N;
    ListNode* listNode;
    bool CreateGraph(int N)
    {
        this->N = N;
        //Tạo mảng động 
        listNode = new ListNode[N];
        //Khởi tạo các src 
        for (int i = 0; i < N; i++)
        {
            listNode[i].head = nullptr;
        }
        return true;
    }
    void addEdge(int src, int dest, int cost)
    {   //Thêm một đường đi từ src đến dest cũng như dest tới src
        Node* node1 = (Node*)malloc(sizeof(Node));

        node1->CreateNode(dest, cost);
        node1->next = listNode[src].head;
        listNode[src].head = node1;

        Node* node2 = (Node*)malloc(sizeof(Node));
        node2->CreateNode(src, cost);
        node2->next = listNode[dest].head;
        listNode[dest].head = node2;

    }
};
int Dijkstra(Graph* graph, int G, int V[], int Trace[])
{
    int index = graph->N;
    int indexUserRecommendation = -1;
    bool* Approved = new bool[index]; // Dùng kiểm tra node đã tìm đc đường đi ngắn nhất chưa 
    int i;
    Node* tmp = graph->listNode[G].head;
    PriorityQueue* queue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    queue->init(index);
    //Gán giá trị ban đầu cho các điểm V[Y]
    for (i = 0; i < index; i++)
    {
        V[i] = INT_MAX;
        Approved[i] = false;
        Trace[i] = G;
    }
    while (tmp != nullptr)
    {
        V[tmp->dest] = tmp->cost;
        tmp = tmp->next;
    }
    V[G] = 0;
    for (i = 0; i < index; i++)
    {
        queue->insertData(i, V[i]);
    }
    while (queue->isEmpty() == false)
    {
        QueueNode* nodeTmp = queue->getElementMin(); //node đã tìm đc độ dài nhỏ nhất đến src
        Approved[nodeTmp->V] = true;
        Node* p = graph->listNode[nodeTmp->V].head; //trỏ p để duyệt qua các node xung quanh 
        //Duyệt các node xung quanh
        while (p != nullptr)
        {
            int Y = p->dest;
            int d = p->cost;  // = nodeTmp -> Vy; (Khoảng cách a đến b tương tự b đến a)
            if (V[nodeTmp->V] != INT_MAX && Approved[Y] == false && V[nodeTmp->V] + d < V[Y])
            {
                V[Y] = V[nodeTmp->V] + d;
                queue->decreaseKey(Y, V[Y]);
                Trace[Y] = nodeTmp->V;
                //Nếu giá trị đường đi nhỏ hơn thì thay đổi trong queue, đánh dấu lại bởi Trace
            }
            p = p->next;
        }
    }
    int min = INT_MAX;
    for (i = 0; i < index; i++)
    {
        if (Trace[i] != G && V[i] < min)
        {
            min = V[i];
            indexUserRecommendation = i;
        }
    }
    return indexUserRecommendation;
}

Graph* MatrixtoGraph(int M[][16], int N)
{
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->CreateGraph(N);
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            if (M[i][j] != 0) graph->addEdge(i, j, M[i][j]);
        }
    }
    return graph;
}

// Chuyển động vẽ khung viền khi bắt đầu chương trình 
void drawBorder()
{
    Sleep(4 * SLEEP);

    // Vẽ cạnh phía trên 
    for (int i = 0; i < 25; i++)
    {
        PEN = getPen(PS_SOLID, 12, RGB(0, i * 4, 0));
        line(300 + 14 * i, 160, 314 + 14 * i, 160);
        Sleep(SLEEP / 2);
    }

    // Vẽ cạnh bên phải 
    for (int i = 0; i < 11; i++)
    {
        PEN = getPen(PS_SOLID, 12, RGB(0, i * 3 + 100, 0));
        line(650, 160 + 14 * i, 650, 175 + 14 * i);
        Sleep(SLEEP);
    }

    // Vẽ cạnh bên trái 
    for (int i = 0; i < 25; i++)
    {
        PEN = getPen(PS_SOLID, 12, RGB(0, i * 4 + 120, 0));
        line(300 + 14 * (24 - i), 320, 314 + 14 * (24 - i), 320);
        Sleep(SLEEP / 2);
    }

    // Vẽ cạnh phía dưới 
    for (int i = 0; i < 11; i++)
    {
        PEN = getPen(PS_SOLID, 12, RGB(0, i * 3 + 220, 0));
        line(300, 160 + 14 * (10 - i), 300, 175 + 14 * (10 - i));
        Sleep(SLEEP);
    }
}

// Vẽ Backgroup của Map
// Có thể tùy chỉnh màu sắc
void drawBackgroup(int r, int g, int b)
{
    PEN = getPen(PS_SOLID, 10, RGB(r, g, b));
    for (int i = 0; i < 90; i++)
    {
        line(350, 50 + i * 10, 1850, 50 + i * 10);
    }
    line(350, 50, 350, 940);
    line(1850, 50, 1850, 940);
}

void drawBackgroupLogo()
{
    PEN = getPen(PS_SOLID, 10, RGB(210, 210, 210));
    for (int i = 0; i < 100; i++)
        line(0, i * 10, 1000, i * 10);
}

// Vẽ Logo Map
// Có thể tùy chỉnh vị trí Logo
void drawLogo(int x, int y)
{
    PEN = getPen(PS_SOLID, 4, RGB(51, 51, 250));
    for (int i = 360; i >= 0; i --)
        circle(x, y + 110, i);

    PEN = getPen(PS_SOLID, 4, RGB(250, 250, 250));
    for (int i = 125; i >= 0; i--)
        circle(x, y, i);

    PEN = getPen(PS_SOLID, 4, RGB(51, 51, 250));
    for (int i = 50; i >= 0; i--)
        circle(x, y - 20, i);

    PEN = getPen(PS_SOLID, 7, RGB(250, 250, 250));
    line(x - 37, y + 20, x - 100, y + 150);
    line(x + 37, y + 20, x + 100, y + 150);

    PEN = getPen(PS_SOLID, 4, RGB(51, 51, 250));
    for (int i = 90; i >= 0; i--)
        circle(x, y + 60, i);

    PEN = getPen(PS_SOLID, 4, RGB(250, 250, 250));
    for (int i = 125; i >= 0; i--)
        circle(x - 100, y + 150, i);

    PEN = getPen(PS_SOLID, 4, RGB(51, 51, 250));
    for (int i = 50; i >= 0; i--)
        circle(x - 100, y + 150 - 20, i);

    PEN = getPen(PS_SOLID, 4, RGB(51, 51, 250));
    for (int i = 90; i >= 0; i--)
        circle(x - 100, y + 150 + 60, i);

    PEN = getPen(PS_SOLID, 4, RGB(250, 250, 250));
    for (int i = 125; i >= 0; i--)
        circle(x + 100, y + 150, i);

    PEN = getPen(PS_SOLID, 4, RGB(51, 51, 250));
    for (int i = 50; i >= 0; i--)
        circle(x + 100, y + 150 - 20, i);

    PEN = getPen(PS_SOLID, 4, RGB(51, 51, 250));
    for (int i = 90; i >= 0; i--)
        circle(x + 100, y + 150 + 60, i);
}

// Chuyển đổi Int sang const *char 
string intToString(int num)
{
    string numAsStr;
    bool isNegative = num < 0;
    if (isNegative) num *= -1;

    do
    {
        char toInsert = (num % 10) + 48;
        numAsStr.insert(0, 1, toInsert);

        num /= 10;
    } while (num);

    return isNegative ? numAsStr.insert(0, 1, '-') : numAsStr;
}

// Vẽ điểm trên Map 
// Đối số cần truyền là biến Point có đủ:
// Tọa độ theo trục x
// Tọa độ theo trục Y
// Số thứ tự của điẻm trên Map
void drawPoint(Point point)
{
    PEN = getPen(PS_SOLID, 10, RGB(0, 128, 255));
    for (int i = 75; i >= 0; i--)
    {
        circle(point.x, point.y, i);
    }


    PEN = getPen(PS_SOLID, 10, RGB(250, 250, 250));
    for (int i = 20; i >= 0; i--)
    {
        circle(point.x, point.y - 20, i);
    }
    string str;
    str[0] = point.name;
    char const* strName = str.c_str();
    if(point.name == 'I')
        drawText(point.x - 10, point.y - 29, strName, 0, 0, 0);
    else
        drawText(point.x - 13, point.y - 29, strName, 0, 0, 0);

    PEN = getPen(PS_SOLID, 10, RGB(250, 250, 250));
    for (int i = 50; i >= 0; i--)
    {
        circle(point.x, point.y + 28, i);
    }
}

// vẽ các điểm đã đưa vào mảng arrayPoint lên Map
void drawPointInArray()
{
    for (int i = 0; i < countPointCurrent; i++)
    {
        drawPoint(arrayPoint[i]);
    }
}

// Vẽ đường nối giữa hai điểm trên Map
// Đối số cần đưa vào:
// Số thứ tự của hai điểm trên Map ( Kiểu int ) - Hai điể đều thuộc arayPoint 
// Ba dối số R, G, B tùy chỉnh máu sắc
// Hàm tự tính toán và vẽ khoảng cách thực tế giữa hai điẻm trên Map 
void drawLinebetweenTwoPoint(int point1, int point2, int r, int g, int b)
{
    int x = abs(arrayPoint[point1].y - arrayPoint[point2].y);
    int y = abs(arrayPoint[point1].x - arrayPoint[point2].x);
    int z = sqrt(x * x + y * y);

    int xMulLine = (50 * y) / z;
    int yMulLine = (50 * x) / z;

    int x1, x2, y1, y2;

    if (arrayPoint[point1].x > arrayPoint[point2].x)
    {
        x1 = arrayPoint[point1].x - xMulLine;
        x2 = arrayPoint[point2].x + xMulLine;
    }
    else
    {
        x1 = arrayPoint[point1].x + xMulLine;
        x2 = arrayPoint[point2].x - xMulLine;
    }

    if (arrayPoint[point1].y > arrayPoint[point2].y)
    {
        y1 = arrayPoint[point1].y - yMulLine;
        y2 = arrayPoint[point2].y + yMulLine;
    }
    else
    {
        y1 = arrayPoint[point1].y + yMulLine;
        y2 = arrayPoint[point2].y - yMulLine;
    }



    PEN = getPen(PS_SOLID, 12, RGB(r, g, b));
    line(x1, y1, x2, y2);

    if (point1 == point2)
        return;

    int dist = sqrt(abs(arrayPoint[point1].x - arrayPoint[point2].x) * abs(arrayPoint[point1].x - arrayPoint[point2].x) +
        abs(arrayPoint[point1].y - arrayPoint[point2].y) * abs(arrayPoint[point1].y - arrayPoint[point2].y)) / 3;

    string str = intToString(dist);
    char const* strDist = str.c_str();

    int midX = (arrayPoint[point1].x + arrayPoint[point2].x) / 2 - 20;
    int midY = (arrayPoint[point1].y + arrayPoint[point2].y) / 2 - 9;

    PEN = getPen(PS_SOLID, 2, RGB(240, 240, 240));
    for (int i = 28; i >= 0; i--)
    {
        circle(midX + 20, midY + 9, i);
    }
    if (dist < 100)
        midX = midX + 5;
    drawText(midX, midY, strDist, 0, 100, 250);
    return;
}

// Thêm các điểm trên Map vào arrayPoint 
void addPointToArray()
{
    arrayPoint[countPointCurrent].x = 800;
    arrayPoint[countPointCurrent].y = 200;
    arrayPoint[countPointCurrent].name = 'A';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 450;
    arrayPoint[countPointCurrent].y = 700;
    arrayPoint[countPointCurrent].name = 'B';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 700;
    arrayPoint[countPointCurrent].y = 850;
    arrayPoint[countPointCurrent].name = 'C';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 800;
    arrayPoint[countPointCurrent].y = 600;
    arrayPoint[countPointCurrent].name = 'D';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 750;
    arrayPoint[countPointCurrent].y = 450;
    arrayPoint[countPointCurrent].name = 'E';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1150;
    arrayPoint[countPointCurrent].y = 500;
    arrayPoint[countPointCurrent].name = 'F';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 550;
    arrayPoint[countPointCurrent].y = 250;
    arrayPoint[countPointCurrent].name = 'G';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1050;
    arrayPoint[countPointCurrent].y = 850;
    arrayPoint[countPointCurrent].name = 'H';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1050;
    arrayPoint[countPointCurrent].y = 400;
    arrayPoint[countPointCurrent].name = 'I';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1050;
    arrayPoint[countPointCurrent].y = 150;
    arrayPoint[countPointCurrent].name = 'J';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1350;
    arrayPoint[countPointCurrent].y = 700;
    arrayPoint[countPointCurrent].name = 'K';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1450;
    arrayPoint[countPointCurrent].y = 400;
    arrayPoint[countPointCurrent].name = 'M';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1250;
    arrayPoint[countPointCurrent].y = 150;
    arrayPoint[countPointCurrent].name = 'N';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1660;
    arrayPoint[countPointCurrent].y = 750;
    arrayPoint[countPointCurrent].name = 'O';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1800;
    arrayPoint[countPointCurrent].y = 450;
    arrayPoint[countPointCurrent].name = 'P';
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1650;
    arrayPoint[countPointCurrent].y = 150;
    arrayPoint[countPointCurrent].name = 'Q';
    countPointCurrent++;
}

// Vẽ đường nối giữa các điểm trê Map 
void drawLineInArray(int r, int g, int b)
{
    // A 0
    // B 1
    // C 2
    // D 3
    // E 4
    // F 5
    // G 6
    // H 7
    // I 8
    // J 9
    // K 10
    // M 11
    // N 12
    // O 13
    // P 14
    // Q 15

    drawLinebetweenTwoPoint(0, 6, r, g, b);
    drawLinebetweenTwoPoint(0, 4, r, g, b);
    drawLinebetweenTwoPoint(0, 9, r, g, b);
    drawLinebetweenTwoPoint(0, 8, r, g, b);
    drawLinebetweenTwoPoint(1, 6, r, g, b);
    drawLinebetweenTwoPoint(4, 6, r, g, b);
    drawLinebetweenTwoPoint(3, 4, r, g, b);
    drawLinebetweenTwoPoint(2, 3, r, g, b);
    drawLinebetweenTwoPoint(8, 3, r, g, b);
    drawLinebetweenTwoPoint(7, 3, r, g, b);
    drawLinebetweenTwoPoint(1, 2, r, g, b);
    drawLinebetweenTwoPoint(1, 4, r, g, b);
    drawLinebetweenTwoPoint(0, 9, r, g, b);
    drawLinebetweenTwoPoint(8, 9, r, g, b);
    drawLinebetweenTwoPoint(5, 7, r, g, b);
    drawLinebetweenTwoPoint(5, 8, r, g, b);
    drawLinebetweenTwoPoint(11, 8, r, g, b);
    drawLinebetweenTwoPoint(7, 10, r, g, b);
    drawLinebetweenTwoPoint(5, 10, r, g, b);
    drawLinebetweenTwoPoint(9, 12, r, g, b);
    drawLinebetweenTwoPoint(15, 12, r, g, b);
    drawLinebetweenTwoPoint(12, 11, r, g, b);
    drawLinebetweenTwoPoint(11, 10, r, g, b);
    drawLinebetweenTwoPoint(13, 10, r, g, b);
    drawLinebetweenTwoPoint(11, 13, r, g, b);
    drawLinebetweenTwoPoint(15, 11, r, g, b);
    drawLinebetweenTwoPoint(14, 15, r, g, b);
    drawLinebetweenTwoPoint(14, 13, r, g, b);

}

Graph* MatrixToGraph(int M[][16], int N)
{
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->CreateGraph(N);
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            if (M[i][j] != 0) graph->addEdge(i, j, M[i][j]);
        }
    }
    return graph;
}

void drawInformationBox(int minOfRoad, int r, int g, int b)
{
    int x = 400;
    int y = 125;

    PEN = getPen(PS_SOLID, 5, RGB(r, g, b));
    for (int i = 0; i < 15; i++)
    {
        line(x, y + i * 5, x + 270, y + i * 5);
    }

    PEN = getPen(PS_SOLID, 3, RGB(50, 50, 50));
    line(x, y, x + 270, y);
    line(x, y + 14 * 5, x + 270, y + 14 * 5);
    line(x - 2, y, x - 2, y + 14 * 5);
    line(x + 272, y, x + 272, y + 14 * 5);

    drawText(400, 150, "The min weight to recomment is : ", 250, 250, 250);
    char* numString = new char[10];
    snprintf(numString, 10, "%d", minOfRoad);
    const char* numStringConst = numString;
    drawText(620, 150, numStringConst, 0, 250, 10);
}

void drawBigPoint(int x, int y)
{
    PEN = getPen(PS_SOLID, 7, RGB(50, 50, 250));
    for (int i = 300; i >= 0; i--)
    {
        circle(x, y, i);
    }

    PEN = getPen(PS_SOLID, 7, RGB(250, 250, 250));
    for (int i = 100; i >= 0; i--)
    {
        circle(x, y - 50, i);
    }

    for (int i = 200; i >= 0; i--)
    {
        circle(x, y + 120, i);
    }

    PEN = getPen(PS_SOLID, 10, RGB(11, 11, 11));
    for (int i = 0; i < 20; i++)
        line(x - 120, y + 120 + 10 * i, x + 120, y + 120 + 10 * i);


    drawText(x - 160, y + 130, "USER RECOMMENDATIONS WITH ", 2500, 250, 250);
    drawText(x + 80, y + 130, "DIJKSTRA", 0, 0, 250);
}

int enterUserName()
{
    bool reEnter;
    do
    {
        gotoxy(13, 15);
        reEnter = false;
        string strUserName = "";
      
        drawText(80, 120, "ENTER USER NAME", 0, 50, 250);

        cout << "Name : ";
        cin >> strUserName;

        if (strUserName.length() != 1)
        {
            reEnter = true;
            drawText(130, 300, "ERROR!!", 222, 0, 0);
            drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
            _getch();
            clrscrText();
            continue;
        }

        if (!reEnter)
        {
            for (int i = 0; i < 16; i++)
            {
                if (strUserName[0] == arrayName[i])
                {

                    drawText(80, 120, "ENTER USER NAME", 7, 7, 7);
                    clrscrText();
                    return i;
                }
                else
                    reEnter = true;
            }
        }
        drawText(130, 300, "ERROR!!", 222, 0, 0);
        drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
        _getch();

        clrscrText();
    } while (reEnter);
}

void drawBoderChat(int indexName)
{
    int x = arrayPoint[indexName].x;
    int y = arrayPoint[indexName].y;

    PEN = getPen(PS_SOLID, 4, RGB(127, 0, 255));
    for (int i = 0; i < 13; i++)
    {
        line(x - 10 - i, y - 20 - i * 2, x - 10 - i * 2, y - 20 - i * 2);
    }

    PEN = getPen(PS_SOLID, 20, RGB(127, 0, 255));
    for (int i = 13; i < 25; i++)
    {
        line(x - 10, y - 20 - i * 2, x - 50, y - 20 - i * 2);
    }
    drawText(x - 50, y - 65, "Hiii", 250, 250, 250);
}



void drawBoderRepChat(int indexName)
{
    int x = arrayPoint[indexName].x;
    int y = arrayPoint[indexName].y;

    PEN = getPen(PS_SOLID, 4, RGB(127, 0, 255));
    for (int i = 0; i < 13; i++)
    {
        line(x - 10 - i, y - 20 - i * 2, x - 10 - i * 2, y - 20 - i * 2);
    }

    PEN = getPen(PS_SOLID, 50, RGB(127, 0, 255));
    line(x - 30, y - 60, x - 54, y - 60);

    PEN = getPen(PS_SOLID, 10, RGB(250, 250, 250));
    line(x - 60, y - 65, x - 60, y - 50);

    PEN = getPen(PS_SOLID, 7, RGB(250, 250, 250));
    line(x - 45, y - 65, x - 27, y - 65);
    line(x - 45, y - 60, x - 25, y - 60);
    line(x - 45, y - 55, x - 27, y - 55);
    line(x - 45, y - 50, x - 29, y - 50);
    line(x - 46, y - 65, x - 46, y - 50);
    line(x - 46, y - 65, x - 40, y - 70);
    line(x - 40, y - 72, x - 40, y - 77);
}

// Vẽ Map
void drawMap(int N, int M[][16])
{
    // Tạo Map
    // Vẽ Logo Map
    // Tạo thanh loading 
    initgraph();
    Sleep(SLEEP * 25);
    drawBackgroupLogo();
    drawLogo(465, 120);

    PEN = getPen(PS_SOLID, 4, RGB(210, 210, 210));
    for (int i = 0; i < 20; i++)
        line(465 - 200, 120 + 218 + i * 4, 465 + 200, 120 + 218 + i * 4);
         drawText(465 - 155, 120 + 230, "USER RECOMMENDATIONS WITH ", 0, 0, 0);
    drawText(465 + 75, 120 + 230, "DIJKSTRA", 0, 0, 250);
    for (int i = 0; i < 60; i++)
    {
        int blue = (30 + i * 4 > 255) ? 255 : (30 + i * 4);
        PEN = getPen(PS_SOLID, 4, RGB(0, 0, blue));
        line(465 - 120 + 4 * i, 120 + 255, 465 - 116 + 4 * i, 120 + 255);
        Sleep(SLEEP);
    }
    Sleep(SLEEP * 50);
    system("cls");

    // Vẽ hộp thoại yêu cầu người dùng mở toàn màn hình
    // Và nhấn Enter để bắt đầu chương trình 
    drawBackgroupLogo();
    drawBorder(); Sleep(287);
    drawText(400, 200, "OPEN FULLSCREEN", 0, 50, 250);
    drawText(420, 230, "       AND     ", 0, 50, 250);
    drawText(340, 260, "PRESS ENTRER TO START PROGRAM", 0, 50, 250);
    getchar();

    // Tạo mảng arrayPoint chứa các điểm trên Map
    // Setup số điểm hiện tại trong arrayPoint là 0 điểm
    // Tiến hành thêm các điểm vào arrayPoint 
    arrayPoint = new Point[N];
    countPointCurrent = 0;
    addPointToArray();

    // Biến thoát chương trình dùng cho vòng lặp 
    bool exitProgram;
    do
    {
        // exitProgram luôn là false khi bắt đầu 
        exitProgram = false;

        drawBigPoint(180, 750);
        drawBackgroup(250, 250, 250);
        drawPointInArray();
        drawLineInArray(170, 170, 170);

        int indexUser = enterUserName();
        Sleep(SLEEP * 10);
        drawBoderChat(indexUser);

        //////////////////////////////////////////////////////////////////////////////////////

        // Code xử lí chương trình 
        Sleep(SLEEP * 30);
        Graph* graph = (Graph*)malloc(sizeof(Graph));
        graph = MatrixToGraph(M, N);
        int* MinOfRoads = new int[N];
        int* Trace = new int[N];
        int indexUserRecommendation = Dijkstra(graph, indexUser, MinOfRoads, Trace);
        int posTmp = indexUserRecommendation;

        do
        {
            drawLinebetweenTwoPoint(Trace[posTmp], posTmp, 51, 255, 51);
            posTmp = Trace[posTmp];
        } while (Trace[posTmp] != indexUser);
        drawLinebetweenTwoPoint(Trace[posTmp], posTmp, 51, 255, 51);
        drawInformationBox(MinOfRoads[indexUserRecommendation], 0, 100, 230);
        drawBoderChat(indexUser);
        Sleep(SLEEP * 30);
        drawBoderRepChat(indexUserRecommendation);

        /////////////////////////////////////////////////////////////////////////////////////

        // Xuất chỉ thị tương tác với người dùng 
        drawText(100, 100, "Restart program [ R ]", 0, 222, 0);
        drawText(110, 130, "Exit Program [ X ]", 222, 0, 0);

        // Nhập chỉ thị của người dùng 
        gotoxy(2, 15);
        string userSelect = "";
        cout << "Your select : ";
        cin >> userSelect;

        // Xóa màn hình Console 
        system("cls");

        // Thoát chương trình 
        if (userSelect == "x" || userSelect == "X")
        {
            // Xóa màn hình
            // Gán exitProgram = true
            // Xóa mảng arrayPoint
            // Vẽ Logo Map 
            delete(arrayPoint);
            exitProgram = true;
            system("cls");
            drawBigPoint(950, 450);
        }

    } while (!exitProgram);

}

int main()
{
    int N = 16;   // A    B    C    D    E    F    G    H    I    J    K    M    N    O    P    Q
    int M[16][16] ={{0  , 0  , 0  , 0  , 84 , 0  , 84 , 0  , 106, 84 , 0  , 0  , 0  , 0  , 0  , 0  }, // A
                    {0  , 0  , 97 , 0  , 130, 0  , 153, 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  }, // B
                    {0  , 97 , 0  , 89 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  }, // C
                    {0  , 0  , 89 , 0  , 52 , 0  , 0  , 117, 106, 0  , 0  , 0  , 0  , 0  , 0  , 0  }, // D 
                    {84 , 130, 0  , 52 , 0  , 0  , 94 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  }, // E
                    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 121, 47 , 0  , 94 , 0  , 0  , 0  , 0  , 0  }, // F
                    {84 , 153, 0  , 0  , 94 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  }, // G
                    {0  , 0  , 0  , 117, 0  , 121, 0  , 0  , 0  , 0  , 111, 0  , 0  , 0  , 0  , 0  }, // H
                    {106, 0  , 0  , 106, 0  , 47 , 0  , 0  , 0  , 83 , 0  , 133, 0  , 0  , 0  , 0  }, // I
                    {84 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 83 , 0  , 0  , 0  , 66 , 0  , 0  , 0  }, // J
                    {0  , 0  , 0  , 0  , 0  , 94 , 0  , 111, 0  , 0  , 0  , 105, 0  , 104, 0  , 0  }, // K
                    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 133, 0  , 105, 0  , 106, 136, 0  , 106}, // M
                    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 66 , 0  , 106, 0  , 0  , 0  , 133}, // N
                    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 104, 136, 0  , 0  , 110, 0  }, // O
                    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 110, 0  , 111}, // P
                    {0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 106, 133, 0  , 111, 0  }, // Q
    };
    drawMap(N, M);
    getchar();
    return 0;
    return 0;
}



































