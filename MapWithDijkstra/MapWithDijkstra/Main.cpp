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
    int number;
};

// Mảng chứa các điểm trên Map   // Biến chứa số lượng điểm hiện tại trong mảng 
Point* arrayPoint;  int countPointCurrent;

void drawBorder();
void drawStartEndLocation(int x, int y, int r, int g, int b);
void drawLogoMap(int x, int y);
string intToString(int num);
void drawPoint(Point point);
void drawBackgroup(int r, int g, int b);
void drawPointInArray();
void drawLinebetweenTwoPoint(int point1, int point2, int r, int g, int b);
void addPointToArray();
void drawLineInArray();
bool enterLocation(int& position);
int enterStartLocation();
int enterEndLocation();
void drawMap(int N);

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
void Dijkstra(Graph* graph, int G, int V[], int Trace[])
{
    int index = graph->N;
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
    //for (i = 0; i < index; i++)
    //{
    //    cout << G << " -> " << i << " cost: " << V[i] << "\n";
    //    int vt = i;
    //    cout << vt << " - ";
    //    while (Trace[vt] != G)
    //    {
    //        cout << Trace[vt] << " - ";
    //        vt = Trace[vt];
    //    }
    //    cout << G << "\n";
    //}
}
//int main()
//{
//    Graph* graph = (Graph*)malloc(sizeof(Graph));
//    // graph->CreateGraph(4);
//    // graph->addEdge(0,1,5);
//    // graph->addEdge(0,2,8);
//    // graph->addEdge(1,2,9);
//    // graph->addEdge(1,3,2);
//    // graph->addEdge(2,3,6);
//    int G = 2;
//    int N = 9;
//    graph->CreateGraph(N);
//    graph->addEdge(0, 1, 4);
//    graph->addEdge(0, 7, 8);
//    graph->addEdge(1, 2, 8);
//    graph->addEdge(1, 7, 11);
//    graph->addEdge(2, 3, 7);
//    graph->addEdge(2, 8, 2);
//    graph->addEdge(2, 5, 4);
//    graph->addEdge(3, 4, 9);
//    graph->addEdge(3, 5, 14);
//    graph->addEdge(4, 5, 10);
//    graph->addEdge(5, 6, 2);
//    graph->addEdge(6, 7, 1);
//    graph->addEdge(6, 8, 6);
//    graph->addEdge(7, 8, 7);
//    int* V = new int[N];
//    int* Trace = new int[N];
//    Dijkstra(graph, 2, V, Trace);
//    for (int i = 0; i < N; i++)
//    {
//        cout << G << " -> " << i << " cost: " << V[i] << "\n";
//        int vt = i;
//        cout << vt << " - ";
//        while (Trace[vt] != G)
//        {
//            cout << Trace[vt] << " - ";
//            vt = Trace[vt];
//        }
//        cout << G << "\n";
//    }
//
//
//}

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

// Đánh dấu vị trí điểm bắt đầu và điểm kết thúc
// Có thể tùy chỉnh màu 
void drawStartEndLocation(int x, int y, int r, int g, int b)
{
    x = x - 140;
    y = y - 75;
    PEN = getPen(PS_SOLID, 40, RGB(r, g, b));
    line(x + 145, y + 20, x + 145, y + 21);

    PEN = getPen(PS_SOLID, 14, RGB(r - 100, g, b));
    line(x + 146, y + 20, x + 146, y + 21);

    PEN = getPen(PS_SOLID, 5, RGB(r, g, b));
    for (int i = 0; i < 7; i++)
    {
        line(x + 133 + i * 2, y + 36 + i * 4, x + 157 - i * 2, y + 36 + i * 4);
    }
    
}

// Vẽ Logo Map
// Có thể tùy chỉnh vị trí Logo
void drawLogoMap(int x, int y)
{

    PEN = getPen(PS_SOLID, 30, RGB(0, 230, 100));
    line(x, y, x + 150, y);
    line(x, y, x, y + 150);

    PEN = getPen(PS_SOLID, 10, RGB(50, 50, 255));
    for (int i = 0; i < 15; i++)
    {
        line(x + i * 10, y + i * 10, x + i * 10, y + 150);
    }

    PEN = getPen(PS_SOLID, 30, RGB(50, 50, 255));
    line(x + 7, y + 150, x + 143, y + 150);

    PEN = getPen(PS_SOLID, 30, RGB(160, 160, 160));
    for (int i = 0; i < 15; i++)
    {
        if (i < 5)
            continue;
        line(x + i * 10, y + 20, x + i * 10, y - 10 + i * 10);
    }

    PEN = getPen(PS_SOLID, 30, RGB(160, 160, 160));
    line(x + 150, y + 7, x + 150, y + 143);

    PEN = getPen(PS_SOLID, 10, RGB(0, 230, 100));
    for (int i = 0; i < 15; i++)
    {
        line(x + i * 10, y, x + i * 10, y + 150 - i * 10);
    }     

    PEN = getPen(PS_SOLID, 20, RGB(255, 255, 255));
    line(x + 75, y + 75, x + 155, y + 155);

    PEN = getPen(PS_SOLID, 20, RGB(255, 255, 0));
    line(x - 5, y + 155, x + 155, y - 5);

    PEN = getPen(PS_SOLID, 90, RGB(255, 0, 0));
    line(x + 145, y + 20, x + 145, y + 21);

    PEN = getPen(PS_SOLID, 30, RGB(135, 0, 0));
    line(x + 146, y + 20, x + 146, y + 21);

    PEN = getPen(PS_SOLID, 5, RGB(255, 0, 0));
    for (int i = 0; i < 21; i++)
    {
        if (i < 18)
        {
            line(x + 105 + i * 2, y + 38 + i * 4, x + 185 - i * 2, y + 38 + i * 4);
        }
        if (i == 18)
        {
            line(x + 105 + i * 2, y + 38 + i * 4, x + 185 - i * 2, y + 38 + i * 4);
            line(x + 105 + i * 2, y + 39 + i * 4, x + 185 - i * 2, y + 39 + i * 4);
            line(x + 105 + i * 2, y + 40 + i * 4, x + 185 - i * 2, y + 40 + i * 4);
            line(x + 105 + i * 2, y + 41 + i * 4, x + 185 - i * 2, y + 41 + i * 4);
            line(x + 105 + i * 2, y + 42 + i * 4, x + 185 - i * 2, y + 42 + i * 4);
            line(x + 105 + i * 2, y + 43 + i * 4, x + 185 - i * 2, y + 43 + i * 4);
        }
        if (i == 19)
        {
            line(x + 105 + i * 2, y + 42 + i * 4, x + 185 - i * 2, y + 42 + i * 4);
            line(x + 105 + i * 2, y + 43 + i * 4, x + 185 - i * 2, y + 43 + i * 4);
            line(x + 105 + i * 2, y + 44 + i * 4, x + 185 - i * 2, y + 44 + i * 4);
            line(x + 105 + i * 2, y + 45 + i * 4, x + 185 - i * 2, y + 45 + i * 4);
            line(x + 105 + i * 2, y + 46 + i * 4, x + 185 - i * 2, y + 46 + i * 4);
        }
        if (i == 20)
        {
            line(x + 105 + i * 2, y + 45 + i * 4, x + 185 - i * 2, y + 45 + i * 4);
            line(x + 105 + i * 2, y + 46 + i * 4, x + 185 - i * 2, y + 46 + i * 4);
            line(x + 105 + i * 2, y + 47 + i * 4, x + 185 - i * 2, y + 47 + i * 4);
            line(x + 105 + i * 2, y + 48 + i * 4, x + 185 - i * 2, y + 48 + i * 4);
            line(x + 105 + i * 2, y + 49 + i * 4, x + 185 - i * 2, y + 49 + i * 4);
            line(x + 105 + i * 2, y + 50 + i * 4, x + 185 - i * 2, y + 50 + i * 4);
        }
    }
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
    PEN = getPen(PS_SOLID, 2, RGB(7, 7, 7));
    for (int i = 60; i >= 50; i--)
    {
        circle(point.x, point.y, i);
    }

    PEN = getPen(PS_SOLID, 5, RGB(0, 240, 90));
    for (int i = 50; i >= 0; i--)
    {
        circle(point.x, point.y, i);
    }

    
    if (point.number < 10)
    {
        string str = intToString(point.number);
        char const* strNumber = str.c_str();
        drawText(point.x - 12, point.y - 8, strNumber, 10, 10, 10);
    }
    else
    {
        string str = intToString(point.number);
        char const* strNumber = str.c_str();
        drawText(point.x - 15, point.y - 8, strNumber, 10, 10, 10);
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
    PEN = getPen(PS_SOLID, 12, RGB(r, g, b));
    line(arrayPoint[point1].x, arrayPoint[point1].y, arrayPoint[point2].x, arrayPoint[point2].y);
    //line(arrayPoint[point1].x, arrayPoint[point1].y, midX, midY);
    //line(arrayPoint[point2].x, arrayPoint[point2].y, midX, midY);
    drawPoint(arrayPoint[point1]);
    drawPoint(arrayPoint[point2]);

    if (point1 == point2)
        return;

    int dist = sqrt(abs(arrayPoint[point1].x - arrayPoint[point2].x) * abs(arrayPoint[point1].x - arrayPoint[point2].x) +
                    abs(arrayPoint[point1].y - arrayPoint[point2].y) * abs(arrayPoint[point1].y - arrayPoint[point2].y));
    string str = intToString(dist);
    char const* strDist = str.c_str();

    int midX = (arrayPoint[point1].x + arrayPoint[point2].x) / 2 - 20;
    int midY = (arrayPoint[point1].y + arrayPoint[point2].y) / 2 - 9;
    
    PEN = getPen(PS_SOLID, 2, RGB(230, 230, 230));
    for (int i = 28; i >= 0; i--)
    {
        circle(midX + 20, midY + 9, i);
    }
    if (dist < 100)
        midX = midX + 5;
    drawText(midX, midY, strDist, 0, 50, 250);
    return;
}

// Thêm các điểm trên Map vào arrayPoint 
void addPointToArray()
{
    arrayPoint[countPointCurrent].x = 450;
    arrayPoint[countPointCurrent].y = 550;
    arrayPoint[countPointCurrent].number = 0;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 450;
    arrayPoint[countPointCurrent].y = 700;
    arrayPoint[countPointCurrent].number = 1;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 700;
    arrayPoint[countPointCurrent].y = 750;
    arrayPoint[countPointCurrent].number = 2;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 700;
    arrayPoint[countPointCurrent].y = 550;
    arrayPoint[countPointCurrent].number = 3;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 700;
    arrayPoint[countPointCurrent].y = 400;
    arrayPoint[countPointCurrent].number = 4;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 850;
    arrayPoint[countPointCurrent].y = 750;
    arrayPoint[countPointCurrent].number = 5;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 800;
    arrayPoint[countPointCurrent].y = 350;
    arrayPoint[countPointCurrent].number = 6;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1050;
    arrayPoint[countPointCurrent].y = 750;
    arrayPoint[countPointCurrent].number = 7;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1050;
    arrayPoint[countPointCurrent].y = 350;
    arrayPoint[countPointCurrent].number = 8;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1050;
    arrayPoint[countPointCurrent].y = 150;
    arrayPoint[countPointCurrent].number = 9;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1450;
    arrayPoint[countPointCurrent].y = 750;
    arrayPoint[countPointCurrent].number = 10;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1350;
    arrayPoint[countPointCurrent].y = 350;
    arrayPoint[countPointCurrent].number = 11;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1250;
    arrayPoint[countPointCurrent].y = 150;
    arrayPoint[countPointCurrent].number = 12;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1750;
    arrayPoint[countPointCurrent].y = 750;
    arrayPoint[countPointCurrent].number = 13;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1700;
    arrayPoint[countPointCurrent].y = 350;
    arrayPoint[countPointCurrent].number = 14;
    countPointCurrent++;

    arrayPoint[countPointCurrent].x = 1600;
    arrayPoint[countPointCurrent].y = 150;
    arrayPoint[countPointCurrent].number = 15;
    countPointCurrent++;
}

// Vẽ đường nối giữa các điểm trê Map 
void drawLineInArray()
{
    drawLinebetweenTwoPoint(0, 1, 50, 50, 50);
    drawLinebetweenTwoPoint(0, 3, 50, 50, 50);
    drawLinebetweenTwoPoint(1, 2, 50, 50, 50);
    drawLinebetweenTwoPoint(2, 3, 50, 50, 50);
    drawLinebetweenTwoPoint(2, 5, 50, 50, 50);
    drawLinebetweenTwoPoint(3, 4, 50, 50, 50);
    drawLinebetweenTwoPoint(4, 6, 50, 50, 50);
    drawLinebetweenTwoPoint(5, 6, 50, 50, 50);
    drawLinebetweenTwoPoint(6, 8, 50, 50, 50);
    drawLinebetweenTwoPoint(5, 7, 50, 50, 50);
    drawLinebetweenTwoPoint(7, 8, 50, 50, 50);
    drawLinebetweenTwoPoint(8, 9, 50, 50, 50);
    drawLinebetweenTwoPoint(7, 10, 50, 50, 50);
    drawLinebetweenTwoPoint(8, 11, 50, 50, 50);
    drawLinebetweenTwoPoint(9, 12, 50, 50, 50);
    drawLinebetweenTwoPoint(10, 11, 50, 50, 50);
    drawLinebetweenTwoPoint(11, 12, 50, 50, 50);
    drawLinebetweenTwoPoint(10, 13, 50, 50, 50);
    drawLinebetweenTwoPoint(11, 14, 50, 50, 50);
    drawLinebetweenTwoPoint(12, 15, 50, 50, 50);
    drawLinebetweenTwoPoint(13, 14, 50, 50, 50);
    drawLinebetweenTwoPoint(14, 15, 50, 50, 50);
}

// Nhập vị trí 
bool enterLocation(int& position)
{
    bool reEnter = false;
    try
    {
        reEnter = false;
        string temp;  cin >> temp;

        for (int i = 0; i < temp.length(); i++)
        {
            if (temp[i] < '0' || temp[i] > '9')
            {
                reEnter = true;
                throw(404);
                break;
            }
        }

        if (!reEnter)
        {
            for (int i = 0; i < temp.length(); i++)
            {
                position = position * 10 + (temp[i] - '0');
            }
        }
    }
    catch (...)
    {
        reEnter = true;
        drawText(130, 300, "ERROR!!", 222, 0, 0);
        drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
        _getch();
    }

    return reEnter;
}

// Nhập vị trí bắt đầu 
// Yêu cầu nhập lại khi vị trí không hợp lệ
int enterStartLocation()
{
    int startPoint;
    bool reEnter;
    do
    {
        gotoxy(13, 15);
        drawText(80, 120, "ENTER START LOCATION", 0, 250, 50);
        
        cout << "Location : ";
        startPoint = 0;
        reEnter = enterLocation(startPoint);
        if (!reEnter)
        {
            if (startPoint < 0 || startPoint > 15)
            {
                reEnter = true;
                drawText(130, 300, "ERROR!!", 222, 0, 0);
                drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
                _getch();
            }
        }
        clrscrText();
    } while (reEnter);

    drawText(80, 120, "ENTER START LOCATION", 7, 7, 7);
    return startPoint;
}

// Nhập vị trí kết thúc
// Yêu cầu nhập lại khi vị trí không hợp lệ 
int enterEndLocation()
{
    int endPoint;
    bool reEnter;
    do
    {
        gotoxy(13, 15);
        drawText(80, 120, "ENTER END LOCATION", 0, 250, 50);

        cout << "Location : ";
        endPoint = 0;
        reEnter = enterLocation(endPoint);
        if (!reEnter)
        {
            if (endPoint < 0 || endPoint > 15)
            {
                reEnter = true;
                drawText(130, 300, "ERROR!!", 222, 0, 0);
                drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
                _getch();
            }
        }
        clrscrText();
    } while (reEnter);

    drawText(80, 120, "ENTER END LOCATION", 7, 7, 7);
    clrscrText();
    return endPoint;
}

Graph * MatrixToGraph(int M[][16], int N)
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

    PEN = getPen(PS_SOLID, 7, RGB(50, 50, 50));
    line(x, y, x + 270, y);
    line(x, y + 14 * 5, x + 270, y + 14 * 5);
    line(x, y, x, y + 14 * 5);
    line(x + 270, y, x + 270, y + 14 * 5);

    drawText(420, 150, "The shortest path length is : ", 0, 0, 0);
    char* numString = new char[10];
    snprintf(numString, 10, "%d", minOfRoad);
    const char* numStringConst = numString;
    drawText(610, 150, numStringConst, 0, 10, 250);
}

// Vẽ Map
void drawMap(int N, int M[][16])
{
    // Tạo Map
    // Vẽ Logo Map
    // Tạo thanh loading 
    initgraph();
    Sleep(SLEEP * 25);
    drawLogoMap(390, 150);
    drawText(380, 350, "MAP WITH ", 255, 255, 255);
    drawText(470, 350, "DIJKSTRA", 0, 255, 0);
    for (int i = 0; i < 40; i++)
    {
        PEN = getPen(PS_SOLID, 4, RGB(0, 30 + i * 4, 0));
        line(385 + 4 * i, 375, 389 + 4 * i, 375);
        Sleep(SLEEP * 5);
    }
    Sleep(SLEEP * 50);
    system("cls");

    // Vẽ hộp thoại yêu cầu người dùng mở toàn màn hình
    // Và nhấn Enter để bắt đầu chương trình 
    drawBorder(); Sleep(287);
    drawText(400, 200, "OPEN FULLSCREEN", 250, 250, 250);
    drawText(420, 230, "       AND     ", 250, 250, 250);
    drawText(340, 260, "PRESS ENTRER TO START PROGRAM", 250, 250, 250);
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
            // Biến lưu vị trí bắt đầu, kết thúc 
            exitProgram = false;
            int startLocation;
            int endLocation;

            drawText(90, 720, "MAP WITH ", 255, 255, 255);
            drawText(180, 720, "DIJKSTRA", 0, 255, 0);
            for (int i = 0; i < 40; i++)
            {
                PEN = getPen(PS_SOLID, 4, RGB(0, 30 + i * 4, 0));
                line(90 + 4 * i, 745, 94 + 4 * i, 745);
            }
            drawLogoMap(100, 785);

            // Vẽ BackGroup - Tùy chỉnh được màu
            // Vẽ các điểm tròn arrayPoint lên Console
            // Vẽ đường đi giữa các điểm 
            Sleep(SLEEP * 17);
            drawBackgroup(200, 200, 200);
            drawPointInArray();
            drawLineInArray();
            Sleep(SLEEP * 10);

            // Nhập vị trí bắt đầu
            // Đánh dấu vị trí bắt đầu - Có thể tùy chỉnh màu điểm đánh dấu
            startLocation = enterStartLocation();
            drawStartEndLocation(arrayPoint[startLocation].x, arrayPoint[startLocation].y, 250, 0, 0);

            // Nhập vị trí điểm kết thúc
            // Đánh dấu vị trí kết thúc - Có thể chỉnh màu điểm đánh dấu 
            endLocation = enterEndLocation();
            drawStartEndLocation(arrayPoint[endLocation].x, arrayPoint[endLocation].y, 250, 0, 0);

            //////////////////////////////////////////////////////////////////////////////////////

            // Code xử lí chương trình 
            Graph* graph = (Graph*)malloc(sizeof(Graph));
            graph = MatrixToGraph(M, N);
            int* MinOfRoads = new int[N];
            int* Trace = new int[N];
            Dijkstra(graph, startLocation, MinOfRoads, Trace);
            int posTmp = endLocation;
            do
            {
                 drawLinebetweenTwoPoint(Trace[posTmp], posTmp, 51, 51, 255);
                 posTmp = Trace[posTmp];
            } while (Trace[posTmp] != startLocation);
            drawLinebetweenTwoPoint(Trace[posTmp], posTmp,51, 51, 255);
            drawInformationBox(MinOfRoads[endLocation], 0, 230, 100);

            drawStartEndLocation(arrayPoint[startLocation].x, arrayPoint[startLocation].y, 250, 0, 0);
            drawStartEndLocation(arrayPoint[endLocation].x, arrayPoint[endLocation].y, 250, 0, 0);
            
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
                drawLogoMap(880, 400);

            }
               
        } while (!exitProgram);
    
}

int main()
{
    int N = 16;
    int M[16][16] = { {0, 150, 0, 250, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {150, 0, 254, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 254, 0, 200, 0, 150, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {250, 0, 200, 0, 150, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 150, 0, 0, 111, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 150, 0, 0, 0, 403, 200, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 111, 403, 0, 0, 250 ,0 ,0 ,0 ,0 ,0 ,0 ,0},
                    {0, 0, 0, 0, 0, 200, 0, 0, 400, 0, 400, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 250, 400, 0, 200, 0, 300, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0, 200, 0, 0, 0, 200, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 400, 0, 0, 0, 412, 0, 300, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0, 300, 0, 412, 0, 223, 0, 350, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 200, 0, 223, 0, 0, 0, 350},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 300, 0, 0, 0, 403, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 350, 0, 403, 0, 223},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 350, 0, 223, 0}
    };
    drawMap(N, M);
    getchar();
    return 0;
    return 0;
}



































//#include <iostream>
//#include <stdio.h>
//#include <string.h>
//#include <math.h>
//#include <conio.h>
//#include "graphics.h"
//#define SLEEP 25
//
//using namespace std;
//
// Struct ?i?m tr�n Map
// T?a ?? theo tr? x
// T?a ?? theo tr?c y
// S? th? t? c?a ?i?m 
//struct Point
//{
//    int x;
//    int y;
//    int number;
//};
//
// M?ng ch?a c�c ?i?m tr�n Map   // Bi?n ch?a s? l??ng ?i?m hi?n t?i trong m?ng 
//Point* arrayPoint;  int countPointCurrent;
//
//void drawBorder();
//void drawStartEndLocation(int x, int y, int r, int g, int b);
//void drawLogoMap(int x, int y);
//string intToString(int num);
//void drawPoint(Point point);
//void drawBackgroup(int r, int g, int b);
//void drawPointInArray();
//void drawLinebetweenTwoPoint(int point1, int point2, int r, int g, int b);
//void addPointToArray();
//void drawLineInArray();
//bool enterLocation(int& position);
//int enterStartLocation();
//int enterEndLocation();
//void drawMap(int N, int M[][16]);
//
//struct QueueNode
//{
//    int V; //Node Y
//    int Vy; // Sau khi k?t th�c c�c Vy l� gi� tr? nh? nh?t c?a ?? d�i t? g?c ??n Y
//
//    void init(int V, int Vy) {
//
//        this->V = V;
//        this->Vy = Vy;
//    }
//};
//
//struct PriorityQueue
//{
//    int currentSize;
//    int maxSize;
//    int* position;
//    QueueNode** ptrToArray;
//
//    void init(int maxSize) {
//
//        this->currentSize = 0;
//        this->maxSize = maxSize;
//        position = new int[maxSize + 1];
//        ptrToArray = new QueueNode * [maxSize + 1];
//    }
//
//    bool isEmpty()
//    {
//        return currentSize == 0;
//    }
//
//    void swap(QueueNode*& nodeA, QueueNode*& nodeB)
//    {
//        QueueNode* tmp = nodeA;
//        nodeA = nodeB;
//        nodeB = tmp;
//    }
//     L?y v? tr� c?a Key Left
//    int getIndexLeft(int index) {
//
//        return (2 * index + 1);
//    }
//
//     L?y v? tr� c?a Key Right
//    int getIndexRight(int index) {
//
//        return (2 * index + 2);
//    }
//    int getIndexParent(int index) {
//
//        return (index - 1) / 2;
//    }
//    void Update(int index)
//    {
//        int left = getIndexLeft(index);
//        int right = getIndexRight(index);
//        int smallest = index;
//
//        if (left < currentSize && ptrToArray[left]->Vy < ptrToArray[smallest]->Vy)
//            smallest = left;
//
//        if (right < currentSize && ptrToArray[right]->Vy < ptrToArray[smallest]->Vy)
//            smallest = right;
//        if (smallest != index)
//        {
//            QueueNode* smallestNode = ptrToArray[smallest];
//            QueueNode* indexNode = ptrToArray[index];
//
//            Cap nhat pos de tim kiem Node
//            position[smallestNode->V] = index;
//            position[indexNode->V] = smallest;
//
//            swap(ptrToArray[smallest], ptrToArray[index]);
//            Update(smallest);
//        }
//    }
//
//    QueueNode* getElementMin() {
//
//        QueueNode* root = new QueueNode();
//        if (isEmpty()) {
//
//            root->V = -1;
//            return root;
//        }
//
//        if (currentSize == 1) {
//
//            currentSize = 0;
//            return ptrToArray[0];
//        }
//
//        root = ptrToArray[0];
//        Cap nhat vi tri cua cac V
//        position[root->V] = currentSize - 1;
//        position[ptrToArray[currentSize - 1]->V] = 0;
//
//        ptrToArray[0] = ptrToArray[currentSize - 1];
//        currentSize--;
//         Kh�i ph?c thu?c t�nh MinHeap c?a c�y
//        Update(0);
//
//        return root;
//    }
//    void decreaseKey(int V, int Vy)
//    {
//        int index = position[V];
//        ptrToArray[index]->Vy = Vy;
//
//        while (index && ptrToArray[getIndexParent(index)]->Vy > ptrToArray[index]->Vy) {
//
//            position[ptrToArray[index]->V] = getIndexParent(index);
//            position[ptrToArray[getIndexParent(index)]->V] = index;
//
//            swap(ptrToArray[index], ptrToArray[getIndexParent(index)]);
//            index = getIndexParent(index);
//        }
//    }
//    void insertData(int V, double Vy)
//    {
//        int index = currentSize;
//        currentSize++;
//        QueueNode* nodeTmp = (QueueNode*)malloc(sizeof(QueueNode));
//        nodeTmp->init(V, Vy);
//        ptrToArray[index] = nodeTmp;
//        position[V] = index;
//         Kh�i ph?c thu?c t�nh MinHeap n?u n� b? vi ph?m b?i vi?c th�m v�o tr�n
//        while (index != 0 && ptrToArray[getIndexParent(index)]->Vy > ptrToArray[index]->Vy) {
//            position[ptrToArray[index]->V] = getIndexParent(index);
//            position[ptrToArray[getIndexParent(index)]->V] = index;
//            swap(ptrToArray[index], ptrToArray[getIndexParent(index)]);
//            index = getIndexParent(index);
//        }
//    }
//    void displayQueue() {
//
//        for (int v = 0; v < currentSize; v++) {
//
//            cout << ptrToArray[v]->V << "|" << ptrToArray[v]->Vy << "|" << position[ptrToArray[v]->V] << "   ";
//        }
//        cout << endl;
//    }
//};
//
//struct Node
//{
//    int dest;
//    int cost;
//    Node* next; //Li�n k?t ??n c�c ?�ch kh�c c?a src
//    bool CreateNode(int dest, int cost)
//    {
//        this->dest = dest;
//        this->cost = cost;
//        this->next = nullptr;
//        return true;
//    }
//};
//struct ListNode
//{
//    Node* head;
//};
//struct Graph
//{
//    int N;
//    ListNode* listNode;
//    bool CreateGraph(int N)
//    {
//        this->N = N;
//        T?o m?ng ??ng 
//        listNode = new ListNode[N];
//        Kh?i t?o c�c src 
//        for (int i = 0; i < N; i++)
//        {
//            listNode[i].head = nullptr;
//        }
//        return true;
//    }
//    void addEdge(int src, int dest, int cost)
//    {   //Th�m m?t ???ng ?i t? src ??n dest c?ng nh? dest t?i src
//        Node* node1 = (Node*)malloc(sizeof(Node));
//
//        node1->CreateNode(dest, cost);
//        node1->next = listNode[src].head;
//        listNode[src].head = node1;
//
//        Node* node2 = (Node*)malloc(sizeof(Node));
//        node2->CreateNode(src, cost);
//        node2->next = listNode[dest].head;
//        listNode[dest].head = node2;
//
//    }
//};
//void Dijkstra(Graph* graph, int G, int V[], int Trace[])
//{
//    int index = graph->N;
//    bool* Approved = new bool[index]; // D�ng ki?m tra node ?� t�m ?c ???ng ?i ng?n nh?t ch?a 
//    int i;
//    Node* tmp = graph->listNode[G].head;
//    PriorityQueue* queue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
//    queue->init(index);
//    G�n gi� tr? ban ??u cho c�c ?i?m V[Y]
//    for (i = 0; i < index; i++)
//    {
//        V[i] = INT_MAX;
//        Approved[i] = false;
//        Trace[i] = G;
//    }
//    while (tmp != nullptr)
//    {
//        V[tmp->dest] = tmp->cost;
//        tmp = tmp->next;
//    }
//    V[G] = 0;
//    for (i = 0; i < index; i++)
//    {
//        queue->insertData(i, V[i]);
//    }
//    while (queue->isEmpty() == false)
//    {
//        QueueNode* nodeTmp = queue->getElementMin(); //node ?� t�m ?c ?? d�i nh? nh?t ??n src
//        Approved[nodeTmp->V] = true;
//        Node* p = graph->listNode[nodeTmp->V].head; //tr? p ?? duy?t qua c�c node xung quanh 
//        Duy?t c�c node xung quanh
//        while (p != nullptr)
//        {
//            int Y = p->dest;
//            int d = p->cost;  // = nodeTmp -> Vy; (Kho?ng c�ch a ??n b t??ng t? b ??n a)
//            if (V[nodeTmp->V] != INT_MAX && Approved[Y] == false && V[nodeTmp->V] + d < V[Y])
//            {
//                V[Y] = V[nodeTmp->V] + d;
//                queue->decreaseKey(Y, V[Y]);
//                Trace[Y] = nodeTmp->V;
//                N?u gi� tr? ???ng ?i nh? h?n th� thay ??i trong queue, ?�nh d?u l?i b?i Trace
//            }
//            p = p->next;
//        }
//    }
//}
//
//
//Graph* MatrixToGraph(int M[][16], int N)
//{
//    Graph* graph = (Graph*)malloc(sizeof(Graph));
//    graph->CreateGraph(N);
//    for (int i = 0; i < N; i++)
//    {
//        for (int j = i + 1; j < N; j++)
//        {
//            if (M[i][j] != 0) graph->addEdge(i, j, M[i][j]);
//        }
//    }
//    return graph;
//}
//void drawInformationBox(int minOfRoad, int r, int g, int b)
//{
//    int x = 400;
//    int y = 260;
//
//    PEN = getPen(PS_SOLID, 5, RGB(r, g, b));
//    for (int i = 0; i < 30; i++)
//    {
//        line(x, y, x + 400, y);
//        y -= 5;
//    }
//    drawText(420, 150, "The shortest path length is : ", 0, 0, 0);
//    char* numString = new char[10];
//    snprintf(numString, 10, "%d", minOfRoad);
//    const char* numStringConst = numString;
//    drawText(610, 150, numStringConst, 255, 0, 0);
//
//
//}
// Chuy?n ??ng v? khung vi?n khi b?t ??u ch??ng tr�nh 
//void drawBorder()
//{
//    Sleep(4 * SLEEP);
//
//     V? c?nh ph�a tr�n 
//    for (int i = 0; i < 25; i++)
//    {
//        PEN = getPen(PS_SOLID, 12, RGB(0, i * 4, 0));
//        line(300 + 14 * i, 160, 314 + 14 * i, 160);
//        Sleep(SLEEP / 2);
//    }
//
//     V? c?nh b�n ph?i 
//    for (int i = 0; i < 11; i++)
//    {
//        PEN = getPen(PS_SOLID, 12, RGB(0, i * 3 + 100, 0));
//        line(650, 160 + 14 * i, 650, 175 + 14 * i);
//        Sleep(SLEEP);
//    }
//
//     V? c?nh b�n tr�i 
//    for (int i = 0; i < 25; i++)
//    {
//        PEN = getPen(PS_SOLID, 12, RGB(0, i * 4 + 120, 0));
//        line(300 + 14 * (24 - i), 320, 314 + 14 * (24 - i), 320);
//        Sleep(SLEEP / 2);
//    }
//
//     V? c?nh ph�a d??i 
//    for (int i = 0; i < 11; i++)
//    {
//        PEN = getPen(PS_SOLID, 12, RGB(0, i * 3 + 220, 0));
//        line(300, 160 + 14 * (10 - i), 300, 175 + 14 * (10 - i));
//        Sleep(SLEEP);
//    }
//}
//
// ?�nh d?u v? tr� ?i?m b?t ??u v� ?i?m k?t th�c
// C� th? t�y ch?nh m�u 
//void drawStartEndLocation(int x, int y, int r, int g, int b)
//{
//    x = x - 145;
//    y = y - 115;
//    PEN = getPen(PS_SOLID, 70, RGB(r, g, b));
//    line(x + 145, y + 20, x + 145, y + 21);
//
//    PEN = getPen(PS_SOLID, 20, RGB(r - 100, g, b));
//    line(x + 146, y + 20, x + 146, y + 21);
//
//    PEN = getPen(PS_SOLID, 5, RGB(r, g, b));
//    for (int i = 0; i < 14; i++)
//    {
//        line(x + 118 + i * 2, y + 38 + i * 4, x + 172 - i * 2, y + 38 + i * 4);
//    }
//    line(x + 118 + 13 * 2, y + 38 + 13 * 4, x + 172 - 13 * 2, y + 38 + 13 * 4);
//    line(x + 118 + 13 * 2, y + 39 + 13 * 4, x + 172 - 13 * 2, y + 39 + 13 * 4);
//    line(x + 118 + 13 * 2, y + 40 + 13 * 4, x + 172 - 13 * 2, y + 40 + 13 * 4);
//}
//
// V? Logo Map
// C� th? t�y ch?nh v? tr� Logo
//void drawLogoMap(int x, int y)
//{
//
//    PEN = getPen(PS_SOLID, 30, RGB(0, 230, 100));
//    line(x, y, x + 150, y);
//    line(x, y, x, y + 150);
//
//    PEN = getPen(PS_SOLID, 10, RGB(50, 50, 255));
//    for (int i = 0; i < 15; i++)
//    {
//        line(x + i * 10, y + i * 10, x + i * 10, y + 150);
//    }
//
//    PEN = getPen(PS_SOLID, 30, RGB(50, 50, 255));
//    line(x + 7, y + 150, x + 143, y + 150);
//
//    PEN = getPen(PS_SOLID, 30, RGB(160, 160, 160));
//    for (int i = 0; i < 15; i++)
//    {
//        if (i < 5)
//            continue;
//        line(x + i * 10, y + 20, x + i * 10, y - 10 + i * 10);
//    }
//
//    PEN = getPen(PS_SOLID, 30, RGB(160, 160, 160));
//    line(x + 150, y + 7, x + 150, y + 143);
//
//    PEN = getPen(PS_SOLID, 10, RGB(0, 230, 100));
//    for (int i = 0; i < 15; i++)
//    {
//        line(x + i * 10, y, x + i * 10, y + 150 - i * 10);
//    }
//
//    PEN = getPen(PS_SOLID, 20, RGB(255, 255, 255));
//    line(x + 75, y + 75, x + 155, y + 155);
//
//    PEN = getPen(PS_SOLID, 20, RGB(255, 255, 0));
//    line(x - 5, y + 155, x + 155, y - 5);
//
//    PEN = getPen(PS_SOLID, 90, RGB(255, 0, 0));
//    line(x + 145, y + 20, x + 145, y + 21);
//
//    PEN = getPen(PS_SOLID, 30, RGB(135, 0, 0));
//    line(x + 146, y + 20, x + 146, y + 21);
//
//    PEN = getPen(PS_SOLID, 5, RGB(255, 0, 0));
//    for (int i = 0; i < 21; i++)
//    {
//        if (i < 18)
//        {
//            line(x + 105 + i * 2, y + 38 + i * 4, x + 185 - i * 2, y + 38 + i * 4);
//        }
//        if (i == 18)
//        {
//            line(x + 105 + i * 2, y + 38 + i * 4, x + 185 - i * 2, y + 38 + i * 4);
//            line(x + 105 + i * 2, y + 39 + i * 4, x + 185 - i * 2, y + 39 + i * 4);
//            line(x + 105 + i * 2, y + 40 + i * 4, x + 185 - i * 2, y + 40 + i * 4);
//            line(x + 105 + i * 2, y + 41 + i * 4, x + 185 - i * 2, y + 41 + i * 4);
//            line(x + 105 + i * 2, y + 42 + i * 4, x + 185 - i * 2, y + 42 + i * 4);
//            line(x + 105 + i * 2, y + 43 + i * 4, x + 185 - i * 2, y + 43 + i * 4);
//        }
//        if (i == 19)
//        {
//            line(x + 105 + i * 2, y + 42 + i * 4, x + 185 - i * 2, y + 42 + i * 4);
//            line(x + 105 + i * 2, y + 43 + i * 4, x + 185 - i * 2, y + 43 + i * 4);
//            line(x + 105 + i * 2, y + 44 + i * 4, x + 185 - i * 2, y + 44 + i * 4);
//            line(x + 105 + i * 2, y + 45 + i * 4, x + 185 - i * 2, y + 45 + i * 4);
//            line(x + 105 + i * 2, y + 46 + i * 4, x + 185 - i * 2, y + 46 + i * 4);
//        }
//        if (i == 20)
//        {
//            line(x + 105 + i * 2, y + 45 + i * 4, x + 185 - i * 2, y + 45 + i * 4);
//            line(x + 105 + i * 2, y + 46 + i * 4, x + 185 - i * 2, y + 46 + i * 4);
//            line(x + 105 + i * 2, y + 47 + i * 4, x + 185 - i * 2, y + 47 + i * 4);
//            line(x + 105 + i * 2, y + 48 + i * 4, x + 185 - i * 2, y + 48 + i * 4);
//            line(x + 105 + i * 2, y + 49 + i * 4, x + 185 - i * 2, y + 49 + i * 4);
//            line(x + 105 + i * 2, y + 50 + i * 4, x + 185 - i * 2, y + 50 + i * 4);
//        }
//    }
//}
//
// Chuy?n ??i Int sang const *char 
//string intToString(int num)
//{
//    string numAsStr;
//    bool isNegative = num < 0;
//    if (isNegative) num *= -1;
//
//    do
//    {
//        char toInsert = (num % 10) + 48;
//        numAsStr.insert(0, 1, toInsert);
//
//        num /= 10;
//    } while (num);
//
//    return isNegative ? numAsStr.insert(0, 1, '-') : numAsStr;
//}
//
// V? ?i?m tr�n Map 
// ??i s? c?n truy?n l� bi?n Point c� ??:
// T?a ?? theo tr?c x
// T?a ?? theo tr?c Y
// S? th? t? c?a ?i?m tr�n Map
//void drawPoint(Point point)
//{
//    PEN = getPen(PS_SOLID, 2, RGB(7, 7, 7));
//    for (int i = 60; i >= 50; i--)
//    {
//        circle(point.x, point.y, i);
//    }
//
//    PEN = getPen(PS_SOLID, 5, RGB(0, 240, 90));
//    for (int i = 50; i >= 0; i--)
//    {
//        circle(point.x, point.y, i);
//    }
//
//
//    if (point.number < 10)
//    {
//        string str = intToString(point.number);
//        char const* strNumber = str.c_str();
//        drawText(point.x - 12, point.y - 8, strNumber, 10, 10, 10);
//    }
//    else
//    {
//        string str = intToString(point.number);
//        char const* strNumber = str.c_str();
//        drawText(point.x - 15, point.y - 8, strNumber, 10, 10, 10);
//    }
//}
//
// V? Backgroup c?a Map
// C� th? t�y ch?nh m�u s?c
//void drawBackgroup(int r, int g, int b)
//{
//    PEN = getPen(PS_SOLID, 10, RGB(r, g, b));
//    for (int i = 0; i < 90; i++)
//    {
//        line(350, 50 + i * 10, 1850, 50 + i * 10);
//    }
//    line(350, 50, 350, 940);
//    line(1850, 50, 1850, 940);
//}
//
// v? c�c ?i?m ?� ??a v�o m?ng arrayPoint l�n Map
//void drawPointInArray()
//{
//    for (int i = 0; i < countPointCurrent; i++)
//    {
//        drawPoint(arrayPoint[i]);
//    }
//}
//
// V? ???ng n?i gi?a hai ?i?m tr�n Map
// ??i s? c?n ??a v�o:
// S? th? t? c?a hai ?i?m tr�n Map ( Ki?u int ) - Hai ?i? ??u thu?c arayPoint 
// Ba d?i s? R, G, B t�y ch?nh m�u s?c
// H�m t? t�nh to�n v� v? kho?ng c�ch th?c t? gi?a hai ?i?m tr�n Map 
//void drawLinebetweenTwoPoint(int point1, int point2, int r, int g, int b)
//{
//    PEN = getPen(PS_SOLID, 12, RGB(r, g, b));
//    line(arrayPoint[point1].x, arrayPoint[point1].y, arrayPoint[point2].x, arrayPoint[point2].y);
//    line(arrayPoint[point1].x, arrayPoint[point1].y, midX, midY);
//    line(arrayPoint[point2].x, arrayPoint[point2].y, midX, midY);
//    drawPoint(arrayPoint[point1]);
//    drawPoint(arrayPoint[point2]);
//
//    int dist = abs(sqrt(arrayPoint[point1].x * arrayPoint[point1].x + arrayPoint[point1].y * arrayPoint[point1].y) -
//        sqrt(arrayPoint[point2].x * arrayPoint[point2].x + arrayPoint[point2].y * arrayPoint[point2].y));
//
//    string str = intToString(dist);
//    char const* strDist = str.c_str();
//
//    int midX = (arrayPoint[point1].x + arrayPoint[point2].x) / 2 - 20;
//    int midY = (arrayPoint[point1].y + arrayPoint[point2].y) / 2 - 9;
//
//    PEN = getPen(PS_SOLID, 2, RGB(230, 230, 230));
//    for (int i = 28; i >= 0; i--)
//    {
//        circle(midX + 20, midY + 9, i);
//    }
//    if (dist < 100)
//        midX = midX + 5;
//    drawText(midX, midY, strDist, 0, 50, 250);
//}
//
// Th�m c�c ?i?m tr�n Map v�o arrayPoint 
//void addPointToArray()
//{
//    arrayPoint[countPointCurrent].x = 450;
//    arrayPoint[countPointCurrent].y = 550;
//    arrayPoint[countPointCurrent].number = 0;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 450;
//    arrayPoint[countPointCurrent].y = 700;
//    arrayPoint[countPointCurrent].number = 1;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 700;
//    arrayPoint[countPointCurrent].y = 750;
//    arrayPoint[countPointCurrent].number = 2;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 700;
//    arrayPoint[countPointCurrent].y = 550;
//    arrayPoint[countPointCurrent].number = 3;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 700;
//    arrayPoint[countPointCurrent].y = 400;
//    arrayPoint[countPointCurrent].number = 4;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 850;
//    arrayPoint[countPointCurrent].y = 750;
//    arrayPoint[countPointCurrent].number = 5;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 800;
//    arrayPoint[countPointCurrent].y = 350;
//    arrayPoint[countPointCurrent].number = 6;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 1050;
//    arrayPoint[countPointCurrent].y = 750;
//    arrayPoint[countPointCurrent].number = 7;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 1050;
//    arrayPoint[countPointCurrent].y = 350;
//    arrayPoint[countPointCurrent].number = 8;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 1050;
//    arrayPoint[countPointCurrent].y = 150;
//    arrayPoint[countPointCurrent].number = 9;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 1450;
//    arrayPoint[countPointCurrent].y = 750;
//    arrayPoint[countPointCurrent].number = 10;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 1350;
//    arrayPoint[countPointCurrent].y = 350;
//    arrayPoint[countPointCurrent].number = 11;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 1250;
//    arrayPoint[countPointCurrent].y = 150;
//    arrayPoint[countPointCurrent].number = 12;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 1750;
//    arrayPoint[countPointCurrent].y = 750;
//    arrayPoint[countPointCurrent].number = 13;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 1700;
//    arrayPoint[countPointCurrent].y = 350;
//    arrayPoint[countPointCurrent].number = 14;
//    countPointCurrent++;
//
//    arrayPoint[countPointCurrent].x = 1600;
//    arrayPoint[countPointCurrent].y = 150;
//    arrayPoint[countPointCurrent].number = 15;
//    countPointCurrent++;
//}
//
// V? ???ng n?i gi?a c�c ?i?m tr� Map 
//void drawLineInArray()
//{
//    drawLinebetweenTwoPoint(0, 1, 70, 70, 70);
//    drawLinebetweenTwoPoint(0, 3, 70, 70, 70);
//    drawLinebetweenTwoPoint(1, 2, 70, 70, 70);
//    drawLinebetweenTwoPoint(2, 3, 70, 70, 70);
//    drawLinebetweenTwoPoint(2, 5, 70, 70, 70);
//    drawLinebetweenTwoPoint(3, 4, 70, 70, 70);
//    drawLinebetweenTwoPoint(4, 6, 70, 70, 70);
//    drawLinebetweenTwoPoint(5, 6, 70, 70, 70);
//    drawLinebetweenTwoPoint(6, 8, 70, 70, 70);
//    drawLinebetweenTwoPoint(5, 7, 70, 70, 70);
//    drawLinebetweenTwoPoint(7, 8, 70, 70, 70);
//    drawLinebetweenTwoPoint(8, 9, 70, 70, 70);
//    drawLinebetweenTwoPoint(7, 10, 70, 70, 70);
//    drawLinebetweenTwoPoint(8, 11, 70, 70, 70);
//    drawLinebetweenTwoPoint(9, 12, 70, 70, 70);
//    drawLinebetweenTwoPoint(10, 11, 70, 70, 70);
//    drawLinebetweenTwoPoint(11, 12, 70, 70, 70);
//    drawLinebetweenTwoPoint(10, 13, 70, 70, 70);
//    drawLinebetweenTwoPoint(11, 14, 70, 70, 70);
//    drawLinebetweenTwoPoint(12, 15, 70, 70, 70);
//    drawLinebetweenTwoPoint(13, 14, 70, 70, 70);
//    drawLinebetweenTwoPoint(14, 15, 70, 70, 70);
//}
//
// Nh?p v? tr� 
//bool enterLocation(int& position)
//{
//    bool reEnter = false;
//    try
//    {
//        reEnter = false;
//        string temp;  cin >> temp;
//
//        for (int i = 0; i < temp.length(); i++)
//        {
//            if (temp[i] < '0' || temp[i] > '9')
//            {
//                reEnter = true;
//                throw(404);
//                break;
//            }
//        }
//
//        if (!reEnter)
//        {
//            for (int i = 0; i < temp.length(); i++)
//            {
//                position = position * 10 + (temp[i] - '0');
//            }
//        }
//    }
//    catch (...)
//    {
//        reEnter = true;
//        drawText(130, 300, "ERROR!!", 222, 0, 0);
//        drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
//        _getch();
//    }
//
//    return reEnter;
//}
//
// Nh?p v? tr� b?t ??u 
// Y�u c?u nh?p l?i khi v? tr� kh�ng h?p l?
//int enterStartLocation()
//{
//    int startPoint;
//    bool reEnter;
//    do
//    {
//        gotoxy(13, 15);
//        drawText(80, 120, "ENTER START LOCATION", 0, 250, 50);
//
//        cout << "Location : ";
//        startPoint = 0;
//        reEnter = enterLocation(startPoint);
//        if (!reEnter)
//        {
//            if (startPoint < 0 || startPoint > 15)
//            {
//                reEnter = true;
//                drawText(130, 300, "ERROR!!", 222, 0, 0);
//                drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
//                _getch();
//            }
//        }
//        clrscrText();
//    } while (reEnter);
//
//    drawText(80, 120, "ENTER START LOCATION", 7, 7, 7);
//    return startPoint;
//}
//
// Nh?p v? tr� k?t th�c
// Y�u c?u nh?p l?i khi v? tr� kh�ng h?p l? 
//int enterEndLocation()
//{
//    int endPoint;
//    bool reEnter;
//    do
//    {
//        gotoxy(13, 15);
//        drawText(80, 120, "ENTER END LOCATION", 0, 250, 50);
//
//        cout << "Location : ";
//        endPoint = 0;
//        reEnter = enterLocation(endPoint);
//        if (!reEnter)
//        {
//            if (endPoint < 0 || endPoint > 15)
//            {
//                reEnter = true;
//                drawText(130, 300, "ERROR!!", 222, 0, 0);
//                drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
//                _getch();
//            }
//        }
//        clrscrText();
//    } while (reEnter);
//
//    drawText(80, 120, "ENTER END LOCATION", 7, 7, 7);
//    clrscrText();
//    return endPoint;
//}
//
// V? Map
//void drawMap(int N, int M[][16])
//{
//     T?o Map
//     V? Logo Map
//     T?o thanh loading 
//    initgraph();
//    Sleep(SLEEP * 25);
//    drawLogoMap(390, 150);
//    drawText(380, 350, "MAP WITH ", 255, 255, 255);
//    drawText(470, 350, "DIJKSTRA", 0, 255, 0);
//    for (int i = 0; i < 40; i++)
//    {
//        PEN = getPen(PS_SOLID, 4, RGB(0, 30 + i * 4, 0));
//        line(385 + 4 * i, 375, 389 + 4 * i, 375);
//        Sleep(SLEEP * 5);
//    }
//    Sleep(SLEEP * 50);
//    system("cls");
//
//     V? h?p tho?i y�u c?u ng??i d�ng m? to�n m�n h�nh
//     V� nh?n Enter ?? b?t ??u ch??ng tr�nh 
//    drawBorder(); Sleep(287);
//    drawText(400, 200, "OPEN FULLSCREEN", 250, 250, 250);
//    drawText(420, 230, "       AND     ", 250, 250, 250);
//    drawText(340, 260, "PRESS ENTRER TO START PROGRAM", 250, 250, 250);
//    getchar();
//
//     T?o m?ng arrayPoint ch?a c�c ?i?m tr�n Map
//     Setup s? ?i?m hi?n t?i trong arrayPoint l� 0 ?i?m
//     Ti?n h�nh th�m c�c ?i?m v�o arrayPoint 
//    arrayPoint = new Point[N];
//    countPointCurrent = 0;
//    addPointToArray();
//
//     Bi?n tho�t ch??ng tr�nh d�ng cho v�ng l?p 
//    bool exitProgram;
//    do
//    {
//         exitProgram lu�n l� false khi b?t ??u 
//         Bi?n l?u v? tr� b?t ??u, k?t th�c 
//        exitProgram = false;
//        int startLocation;
//        int endLocation;
//
//        drawText(90, 720, "MAP WITH ", 255, 255, 255);
//        drawText(180, 720, "DIJKSTRA", 0, 255, 0);
//        for (int i = 0; i < 40; i++)
//        {
//            PEN = getPen(PS_SOLID, 4, RGB(0, 30 + i * 4, 0));
//            line(90 + 4 * i, 745, 94 + 4 * i, 745);
//        }
//        drawLogoMap(100, 785);
//
//         V? BackGroup - T�y ch?nh ???c m�u
//         V? c�c ?i?m tr�n arrayPoint l�n Console
//         V? ???ng ?i gi?a c�c ?i?m 
//        Sleep(SLEEP * 17);
//        drawBackgroup(200, 200, 200);
//        drawPointInArray();
//        drawLineInArray();
//        Sleep(SLEEP * 10);
//
//         Nh?p v? tr� b?t ??u
//         ?�nh d?u v? tr� b?t ??u - C� th? t�y ch?nh m�u ?i?m ?�nh d?u
//        startLocation = enterStartLocation();
//        drawStartEndLocation(arrayPoint[startLocation].x, arrayPoint[startLocation].y, 250, 0, 0);
//
//         Nh?p v? tr� ?i?m k?t th�c
//         ?�nh d?u v? tr� k?t th�c - C� th? ch?nh m�u ?i?m ?�nh d?u 
//        endLocation = enterEndLocation();
//        drawStartEndLocation(arrayPoint[endLocation].x, arrayPoint[endLocation].y, 250, 0, 0);
//
//        ////////////////////////////////////////////////////////////////////////////////////
//
//         Code x? l� ch??ng tr�nh 
//        Graph* graph = (Graph*)malloc(sizeof(Graph));
//        graph = MatrixToGraph(M, N);
//        int* MinOfRoads = new int[N];
//        int* Trace = new int[N];
//        Dijkstra(graph, startLocation, MinOfRoads, Trace);
//        int posTmp = endLocation;
//        do
//        {
//            drawLinebetweenTwoPoint(Trace[posTmp], posTmp, 51, 153, 255);
//            posTmp = Trace[posTmp];
//        } while (Trace[posTmp] != startLocation);
//        drawLinebetweenTwoPoint(Trace[posTmp], posTmp, 51, 153, 255);
//        drawInformationBox(MinOfRoads[endLocation], 200, 200, 200);
//
//
//
//        ///////////////////////////////////////////////////////////////////////////////////
//
//         Xu?t ch? th? t??ng t�c v?i ng??i d�ng 
//        drawText(100, 100, "Restart program [ R ]", 0, 222, 0);
//        drawText(110, 130, "Exit Program [ X ]", 222, 0, 0);
//
//         Nh?p ch? th? c?a ng??i d�ng 
//        gotoxy(2, 15);
//        string userSelect = "";
//        cout << "Your select : ";
//        cin >> userSelect;
//        cout << userSelect << endl;
//
//         X�a m�n h�nh Console 
//        system("cls");
//
//         Tho�t ch??ng tr�nh 
//        if (userSelect == "x" || userSelect == "X")
//        {
//             X�a m�n h�nh
//             G�n exitProgram = true
//             X�a m?ng arrayPoint
//             V? Logo Map 
//            delete(arrayPoint);
//            exitProgram = true;
//            system("cls");
//            drawLogoMap(880, 400);
//
//        }
//
//    } while (!exitProgram);
//
//}
//
//int main()
//{
//    int N = 16;
//    int M[16][16] = { {0, 121, 0, 179, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//                    {121, 0, 193, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//                    {0, 193, 0, 135, 0, 107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//                    {179, 0, 135, 0, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//                    {0, 0, 0, 83, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//                    {0, 0, 107, 0, 0, 0, 260, 156, 0, 0, 0, 0, 0, 0, 0, 0},
//                    {0, 0, 0, 0, 66, 260, 0, 0, 233 ,0 ,0 ,0 ,0 ,0 ,0 ,0},
//                    {0, 0, 0, 0, 0, 156, 0, 0, 183, 0, 342, 0, 0, 0, 0, 0},
//                    {0, 0, 0, 0, 0, 0, 233, 183, 0, 46, 0, 287, 0, 0, 0, 0},
//                    {0, 0, 0, 0, 0, 0, 0, 0, 46, 0, 0, 0, 198, 0, 0, 0},
//                    {0, 0, 0, 0, 0, 0, 0, 342, 0, 0, 0, 237, 0, 271, 0, 0},
//                    {0, 0, 0, 0, 0, 0, 0, 0, 287, 0, 237, 0, 135, 0, 341, 0},
//                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 198, 0, 135, 0, 0, 0, 348},
//                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 271, 0, 0, 0, 168, 0},
//                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 341, 0, 168, 0, 128},
//                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 348, 0, 128, 0}
//    };
//    drawMap(N, M);
//    getchar();
//    return 0;
//    return 0;
//}