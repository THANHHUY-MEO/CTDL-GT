#include <iostream>
#include <stdio.h>
#include <fstream>
using namespace std;
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
    int* position; //Dùng để tra vị trí V trong mảng ptrToArray
    QueueNode** ptrToArray;

    //Khởi tạo PriorityQueue
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
    //Đổi chổ 2 Node
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
    // Lấy vị trí của Key Parent
    int getIndexParent(int index) {

        return (index - 1) / 2;
    }
    //Hiệu chỉnh lại priority queue khi có sự thay đổi các giá trị Vy
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
    //Hàm thay đổi giá trị Vy của một V nào đó
    void decreaseKey(int V, int Vy)
    {
        int index = position[V];
        ptrToArray[index]->Vy = Vy;

        //Khôi phục thuộc tính
        while (index && ptrToArray[getIndexParent(index)]->Vy > ptrToArray[index]->Vy) {

            position[ptrToArray[index]->V] = getIndexParent(index);
            position[ptrToArray[getIndexParent(index)]->V] = index;

            swap(ptrToArray[index], ptrToArray[getIndexParent(index)]);
            index = getIndexParent(index);
        }
    }
    //Hàm để thêm Node mới vào priority queue
    void insertData(int V, double Vy)
    {
        int index = currentSize;
        currentSize++;
        QueueNode* nodeTmp = (QueueNode*)malloc(sizeof(QueueNode));
        nodeTmp->init(V, Vy);
        ptrToArray[index] = nodeTmp;
        position[V] = index;

        // Khôi phục thuộc tính nếu nó bị vi phạm bởi việc thêm vào trên
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
    int dest;  //Node đích mà nó đến
    int cost;  //Giá trị của đường đi đến node đích
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
    int N; //Số điểm trong đồ thị
    ListNode* listNode;  //Một mảng động với các phần tử thứ i là danh sách liên kết thể hiện các điểm xung quanh i

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
    bool* Approved = new bool[index]; // Dùng kiểm tra node đã tìm đc đường đi ngắn nhất ch
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
        free(nodeTmp);
        free(p);
    }
    free(Approved);
    free(tmp);
    free(queue);
}
//Chuyển đầu vào là ma trận sang đồ thị
Graph* MatrixToGraph(int** Matrix, int NumberOfNonTerminal)
{
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->CreateGraph(NumberOfNonTerminal);
    for (int i = 0; i < NumberOfNonTerminal; i++)
    {
        for (int j = i + 1; j < NumberOfNonTerminal; j++)
        {
            if (Matrix[i][j] != 0) graph->addEdge(i, j, Matrix[i][j]);
        }
    }
    return graph;
}
int main()
{

    int NumberOfNonTerminal;
    int StartPoint;
    ifstream ifs("Matrix.txt");
    ifs >> NumberOfNonTerminal;
    ifs >> StartPoint;
    int* ResultTanks = new int[NumberOfNonTerminal];
    int* Trace = new int[NumberOfNonTerminal];
    int** Matrix;

    Matrix = (int**)malloc(NumberOfNonTerminal * sizeof(int*));
    for (int i = 0; i < NumberOfNonTerminal; i++)
    {
        Matrix[i] = (int*)malloc(NumberOfNonTerminal * sizeof(int));
    }
    for (int i = 0; i < NumberOfNonTerminal; i++)
    {
        for (int j = 0; j < NumberOfNonTerminal; j++)
        {
            ifs >> Matrix[i][j];
        }
    }

    //
    //         (1) ----5----(0)
    //         /  |       /
    //        2    9     8
    //        /    |    /     Đồ thị để tạo ma trận trong file Matran.txt
    //      (3)---6-- (2)   
    //
    //

    Graph* graph = MatrixToGraph(Matrix, NumberOfNonTerminal);
    Dijkstra(graph, 0, ResultTanks, Trace);
    for (int i = 0; i < NumberOfNonTerminal; i++)
    {
        cout << StartPoint << " -> " << i << " cost: " << ResultTanks[i] << "\n";
        int vt = i;
        cout << vt << " , ";
        while (Trace[vt] != StartPoint)
        {
            cout << Trace[vt] << " , ";
            vt = Trace[vt];
        }
        cout << StartPoint << "\n";
    }

    free(ResultTanks);
    free(Trace);
    for (int i = 0; i < NumberOfNonTerminal; i++)  free(Matrix[i]);
    free(Matrix);
    free(graph->listNode);
    free(graph);
}