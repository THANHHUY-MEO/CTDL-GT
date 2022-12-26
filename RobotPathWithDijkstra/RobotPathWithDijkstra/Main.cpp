#include "graphics.h"
#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define COL 61
#define ROW 37
#define SLEEP 20
#define NUMBERBARRIER 600

	// Loang trên ma trận hai chiều 
	int arrayIndexI[] = { 0, -1, 0, 1 };
	int arrayIndexJ[] = { -1, 0, 1, 0 };
	int startX, startY;

	struct Point
	{
		// Tọa độ i trong ma trận 
		// Tọa độ j trong ma trận 
		// Khoảng cách từ gốc đến điểm 
		// Điểm đã đi qua hay chưa 
		int i;
		int j;
		int dist;
		bool empty;
	};

	struct MinHeap {

		// Con trỏ trỏ đến mảng lưu trữ MinHeap
		// Kích thướt max của MinHeap
		// Kích thướt hiện tại của MinHeap
		Point* ptrToArray;
		int maxSize;
		int currentSize;

		// Khởi tạo các giá trị ban đầu của MinHeap
		void init()
		{
			this->currentSize = 0;
			this->maxSize = COL * ROW;
			this->ptrToArray = new Point[COL * ROW];
		}

		bool empty()
		{
			return (currentSize == 0) ? true : false;
		}

		// Lấy index của Key Parent
		int getIndexParent(int index)
		{
			return (index - 1) / 2;
		}

		// Lấy vị trí của Key Left
		int getIndexLeft(int index)
		{
			return (2 * index + 1);
		}

		// Lấy vị trí của Key Right
		int getIndexRight(int index)
		{
			return (2 * index + 2);
		}

		// Lấy phần tử Min của MinHeap
		Point getMin()
		{
			return ptrToArray[0];
		}

		// Hoán vị
		void swap(Point& a, Point& b)
		{
			Point hV = a; a = b; b = hV;
			return;
		}

		// Thêm Key vào MinHeap
		void insertData(Point point)
		{
			ptrToArray[currentSize] = point;
			int index = currentSize;
			currentSize++;

			// Khôi phục thuộc tính MinHeap nếu nó bị vi phạm bởi việc thêm vào trên
			while (index != 0 && ptrToArray[getIndexParent(index)].dist > ptrToArray[index].dist) {

				swap(ptrToArray[index], ptrToArray[getIndexParent(index)]);
				index = getIndexParent(index);
			}
		}

		// Lấy phần tử min và xóa nó khỏi MinHeap
		void deleteElementMinOfHeap() {

			if (currentSize == 1)
				currentSize = 0;
			else
			{
				ptrToArray[0] = ptrToArray[currentSize - 1];
				currentSize--;

				// Khôi phục thuộc tính MinHeap của cây
				minHeapify(0);
			}
		}

		// Khôi phục thuộc tính MinHeap của cây
		void minHeapify(int index) {

			int left = getIndexLeft(index);
			int right = getIndexRight(index);
			int smallest = index;

			if (left < currentSize && ptrToArray[left].dist < ptrToArray[index].dist)
				smallest = left;

			if (right < currentSize && ptrToArray[right].dist < ptrToArray[smallest].dist)
				smallest = right;

			if (smallest != index) {

				swap(ptrToArray[index], ptrToArray[smallest]);
				minHeapify(smallest);
			}
		}
	};

	//struct QueuePoint
	//{
	//	Point point;
	//	QueuePoint* next;
	//};
	//
	//struct Queue
	//{
	//	QueuePoint* front;
	//	QueuePoint* rear;
	//	void init()
	//	{
	//		front = nullptr;
	//		rear = nullptr;
	//	}
	//
	//	bool empty()
	//	{
	//		return (front == nullptr) ? true : false;
	//	}
	//
	//	void enQueue(Point point)
	//	{
	//		struct QueuePoint *newPoint =new QueuePoint();
	//		newPoint->point = point;
	//		newPoint->next = nullptr;
	//
	//		if (front == nullptr && rear == nullptr)
	//		{
	//			front = newPoint;
	//			rear  = newPoint;
	//		}
	//		else
	//		{
	//			rear->next = newPoint;
	//			rear = newPoint;
	//		}
	//	}
	//
	//	Point deQueue()
	//	{
	//		struct QueuePoint* delPoint = new QueuePoint();
	//		struct Point point = front->point;
	//		delPoint = front;
	//		front = front->next;
	//		delete(delPoint);
	//		return point;
	//	}
	//};

	void drawBorder();
	string intToString(int num);
	void drawTable(int x, int y);
	void randomBarrier(int x, int y, Point * *table);
	void drawBarrier(int x, int y, int indexI, int indexJ, int r, int g, int b);
	void drawPoint(int x, int y, int indexI, int indexJ, int r, int g, int b);
	void dijkstra(MinHeap minHeap, Point * *table, bool& found);
	void drawPathRobot(int desI, int desJ, Point * *table);
	void drawEyeRobot(int X, int Y, int r, int g, int b);
	void drawEyeRobotSleep(int X, int Y, int r, int g, int b);
	void drawRobot(int x, int y);
	bool enterPosition(int& position);
	void setUpTable(Point * *table);
	void enterDestination(int x, int y, Point * *table, int& desX, int& desY);
	void startProgram(int x, int y);
	void robotPath();

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

	// Chuyển đổi từ int sang const char*
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

	// Vẽ chướng ngại vật 
	void drawBarrier(int x, int y, int indexI, int indexJ, int r, int g, int b)
	{
		int X = x + 5 + 25 * indexJ;
		int Y = y + 5 + 25 * indexI;
		PEN = getPen(PS_SOLID, 5, RGB(r, g, b));
		for (int i = 0; i < 11; i++)
		{
			line(X - 3, Y - 3 + i * 2, X + 17, Y - 3 + i * 2);
		}
	}

	// Random tạo chướng ngại vật 
	void randomBarrier(int x, int y, Point * *table)
	{
		// Số chướng ngại vật không được vượt quá COL*ROW/2
		if ((COL * ROW) / 2 <= NUMBERBARRIER)
			return;

		srand(time(nullptr));
		for (int i = 0; i < NUMBERBARRIER;)
		{
			int indexX = rand() % COL;
			int indexY = rand() % ROW;

			if (table[indexY][indexX].dist == 0)
			{
				table[indexY][indexX].dist = -1;
				table[indexY][indexX].empty = false;
				drawBarrier(x, y, indexY, indexX, 255, 255, 255);
				i++;
				Sleep(SLEEP / 20);
			}
		}
		//for (int i = 0; i < ROW; i++)
		//{
		//	for (int j = 0; j < COL; j++)
		//	{
		//		cout << table[i][j].index << "  ";
		//	}
		//	cout << endl;
		//}
	}

	// Vẽ ma trận trên Console
	void drawTable(int x, int y)
	{
		// Vẽ tiêu đề của chương trình 
		for (int i = 0, j = 0; i <= COL || j <= ROW; i++, j++)
		{
			if (i == COL / 10)
				drawText(60, 700, "ROBOT", 200, 200, 200);
			if (i == COL / 5)
				drawText(120, 700, "PATH", 200, 200, 200);
			if (i == COL / 2)
				drawText(170, 700, "WITH", 200, 200, 200);
			if (i == COL)
			{
				drawText(220, 700, "DIJKSTRA", 0, 255, 0);
				for (int i = 0; i < 25; i++)
				{
					PEN = getPen(PS_SOLID, 3, RGB(0, 30 + i * 9, 0));
					line(65 + 9 * i, 722, 76 + 9 * i, 722);
				}
			}

			// Đánh số thứ tự theo trục X 
			if (i < COL)
			{
				string str = intToString(i);
				char const* indexX = str.c_str();
				drawText(x + 25 * i, y - 25, indexX, 0, 220, 0);
			}
			if (i == COL)
			{
				drawText(x - 7 + 25 * i, y - 25, "X", 0, 220, 0);
			}

			// Vẽ các cột của ma trận 
			if (i <= COL)
			{
				if (i == 0 || i == COL)
					PEN = getPen(PS_SOLID, 6, RGB(255, 255, 255));
				else
					PEN = getPen(PS_SOLID, 2, RGB(255, 255, 255));

				line(x + i * 25, y, x + i * 25, (y - 25) + (ROW + 1) * 25);
				Sleep(SLEEP - SLEEP / 5);
			}

			// Đánh số thứ tự theo trục Y
			if (j < ROW)
			{
				string str = intToString(i);
				char const* indexY = str.c_str();
				drawText(x - 30, y + 25 * j, indexY, 0, 220, 0);
			}
			if (j == ROW)
			{
				drawText(x - 28, y - 10 + 25 * j, "Y", 0, 220, 0);
			}

			// Vẽ các dòng của ma trận 
			if (j <= ROW)
			{
				if (j == 0 || j == ROW)
					PEN = getPen(PS_SOLID, 6, RGB(255, 255, 255));
				else
					PEN = getPen(PS_SOLID, 2, RGB(255, 255, 255));

				line(x, y + j * 25, (x - 25) + (COL + 1) * 25, y + j * 25);
				Sleep(SLEEP - SLEEP / 5);
			}
		}
	}

	// Vẽ điểm tại vị trí được chỉ định
	void drawPoint(int x, int y, int indexI, int indexJ, int r, int g, int b)
	{
		int X = x + 5 + 25 * indexJ;
		int Y = y + 5 + 25 * indexI;
		PEN = getPen(PS_SOLID, 5, RGB(r, g, b));
		for (int i = 0; i < 9; i++)
		{
			line(X - 1, Y - 1 + i * 2, X + 15, Y - 1 + i * 2);
		}
	}

	// Kiểm tra các điểm lân cận điểm hiện tại
	// Cập nhật khoảng cách của của điểm lân cận so với gốc
	// Thêm các điểm lân cận vào MinHeap
	// Di chuyển đến điểm Min của MinHeap, tiếp tục kiểm tra 
	void dijkstra(MinHeap minHeap, Point * *table, bool& found)
	{
		// Trả về khi MinHeap rỗng 
		if (minHeap.empty())
			return;

		// Lấy phần tử Min của MinHeap 
		Point point = minHeap.getMin();

		// Kiểm tra các điểm lân cận và thêm vào MinHeap 
		for (int k = 0; k < 4; k++)
		{
			if (found)
				break;

			int indexI = point.i + arrayIndexI[k];
			int indexJ = point.j + arrayIndexJ[k];

			if (indexI < 0 || indexI >= ROW || indexJ < 0 || indexJ >= COL)
				continue;

			if (table[indexI][indexJ].dist == -1)
				continue;

			if (!table[indexI][indexJ].empty)
				continue;

			if (table[indexI][indexJ].dist == -11)
			{
				table[indexI][indexJ].dist = table[point.i][point.j].dist + 1;
				table[indexI][indexJ].empty = false;
				found = true;
				return;
			}

			if (table[indexI][indexJ].dist == 0 && table[indexI][indexJ].empty)
			{
				table[indexI][indexJ].dist = table[point.i][point.j].dist + 1;
				table[indexI][indexJ].empty = false;
				minHeap.insertData(table[indexI][indexJ]);
				int green = ((255 - table[indexI][indexJ].dist * 3) > 0) ? (255 - table[indexI][indexJ].dist * 3) : 0;
				int red = ((table[indexI][indexJ].dist) * 3 > 255) ? 255 : (table[indexI][indexJ].dist) * 3;
				drawPoint(350, 50, indexI, indexJ, red, green, 0);
				Sleep(SLEEP / 20);
			}
		}

		// Xóa điểm đã kiểm tra xong các điểm lân cận khỏi MinHeap
		// Di chuyển đến điểm Min của MinHeap và tiếp tục kiểm tra 
		minHeap.deleteElementMinOfHeap();
		dijkstra(minHeap, table, found);
	}

	// Dựa vào ma trận đã cập nhật khoảng cách từ gốc đến các điểm
	// Đi ngược từ Destination, vẽ lại đường đi có khoảng cách ngắn nhất từ S đến Destination 
	void drawPathRobot(int desI, int desJ, Point * *table)
	{
		// Trả về khi duyệt đến điểm S
		if (desJ == startX && desI == startY)
			return;

		// Duyệt 4 điểm lân cận của điểm hiện tại
		// Tìm điểm có khoảng cách liền trứoc khoảng cách của điểm hiện tại
		// Vẽ điểm đó lên ma trận trân Console 
		// Di duyển sang điểm đó và tiếp tục duyệt 
		for (int i = 0; i < 4; i++)
		{
			int indexI = desI + arrayIndexI[i];
			int indexJ = desJ + arrayIndexJ[i];

			if (indexI < 0 || indexI >= ROW || indexJ < 0 || indexJ >= COL)
				continue;

			if (table[indexI][indexJ].dist == -1)
				continue;

			if (table[indexI][indexJ].empty)
				continue;

			if (indexJ == startX && indexI == startY)
				break;

			if (table[indexI][indexJ].dist == table[desI][desJ].dist - 1)
			{
				drawPoint(350, 50, indexI, indexJ, 0, 0, 255);
				drawPathRobot(indexI, indexJ, table);
				Sleep(SLEEP / 4);
				break;
			}
		}
	}

	// Cài đặt ma trận khoảng cách 
	// Tạo Minheap
	// Thêm S vào MinHeap
	// Tiến hành duyệt qua các điểm lân cận của Min trong MinHeap
	// Cập nhật khoảng cách từ S đến các điểm đó 
	// Lặp lại quá trình trên cho đến khi tìm được Destination hoặc MinHeap rỗng 
	void setUpTable(Point * *table)
	{
		MinHeap minHeap = MinHeap();
		minHeap.init();
		minHeap.insertData(table[startY][startX]);
		bool found = false;
		dijkstra(minHeap, table, found);

		// Nếu không tìm thấy đường đi đến Destination 
		if (!found)
		{
			drawText(100, 240, "NOT FOUND PATH!!", 222, 0, 0);
		}

	}

	// Vẽ mắt của Robot 
	void drawEyeRobot(int X, int Y, int r, int g, int b)
	{
		for (int i = 24; i >= 0; i--)
		{
			PEN = getPen(PS_SOLID, 5, RGB(r, g, b));
			line(X - 1, Y - 1 + i * 2, X + 50, Y - 1 + i * 2);

			PEN = getPen(PS_SOLID, 5, RGB(10, 10, 10));
			line(X + 52, Y - 1 + i * 2, X + 100, Y - 1 + i * 2);

			PEN = getPen(PS_SOLID, 5, RGB(r, g, b));
			line(X + 90, Y - 1 + i * 2, X + 141, Y - 1 + i * 2);
			Sleep(SLEEP / 5);
		}
	}

	// Làm mắt Robot chuyển động 
	void drawEyeRobotSleep(int X, int Y, int r, int g, int b)
	{
		for (int i = 0; i < 25; i++)
		{
			PEN = getPen(PS_SOLID, 5, RGB(r, g, b));
			line(X - 1, Y - 1 + i * 2, X + 50, Y - 1 + i * 2);

			PEN = getPen(PS_SOLID, 5, RGB(10, 10, 10));
			line(X + 52, Y - 1 + i * 2, X + 100, Y - 1 + i * 2);

			PEN = getPen(PS_SOLID, 5, RGB(r, g, b));
			line(X + 90, Y - 1 + i * 2, X + 141, Y - 1 + i * 2);
			Sleep(SLEEP / 5);
		}
	}

	// Vẽ Robot
	void drawRobot(int x, int y)
	{
		PEN = getPen(PS_SOLID, 6, RGB(200, 200, 200));
		line(x + 62, y - 40, x + 62, y + 20);

		PEN = getPen(PS_SOLID, 15, RGB(200, 200, 200));
		line(x + 140, y + 158, x + 210, y + 158);

		PEN = getPen(PS_SOLID, 10, RGB(255, 0, 128));
		line(x + 80, y, x + 270, y);
		line(x + 80, y + 150, x + 270, y + 150);

		PEN = getPen(PS_SOLID, 30, RGB(0, 200, 0));
		line(x + 65, y + 30, x + 65, y + 120);
		line(x + 285, y + 30, x + 285, y + 120);

		PEN = getPen(PS_SOLID, 15, RGB(255, 0, 128));
		line(x + 80, y + 5, x + 80, y + 145);
		line(x + 270, y + 5, x + 270, y + 145);

		PEN = getPen(PS_SOLID, 15, RGB(200, 200, 200));
		line(x + 150, y + 120, x + 200, y + 120);

		PEN = getPen(PS_SOLID, 15, RGB(0, 200, 0));
		line(x + 61, y - 40, x + 62, y - 40);

		Sleep(SLEEP);
		drawEyeRobot(x + 107, y + 30, 0, 222, 0);

		PEN = getPen(PS_SOLID, 15, RGB(200, 0, 0));
		line(x + 61, y - 40, x + 62, y - 40);

		Sleep(SLEEP * 20);
		drawEyeRobotSleep(x + 107, y + 30, 50, 50, 50);

		Sleep(SLEEP * 2);
		drawEyeRobot(x + 107, y + 30, 0, 222, 0);

		PEN = getPen(PS_SOLID, 15, RGB(0, 200, 0));
		line(x + 61, y - 40, x + 62, y - 40);
	}

	// Nhập vị trí của Destination trên ma trận 
	// Yêu cầu nhập lại khi người dùng nhập lỗi 
	bool enterPosition(int& position)
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
			drawText(140, 300, "ERROR!!", 222, 0, 0);
			drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
			_getch();
		}

		return reEnter;
	}

	// Nhập Destination trên ma trận trên Console
	// Chứa vòng lặp yêu cầu người dùng nhập lại khi đầu vào chưa chính xác 
	void enterDestination(int x, int y, Point * *table, int& desX, int& desY)
	{
		bool reEnter = false;
		do
		{
			clrscrText();
			desX = 0;
			desY = 0;
			reEnter = false;

			drawText(70, 140, "ENTER A DESTINATION POINT", 0, 222, 0);

			gotoxy(10, 12);
			cout << "X : ";
			reEnter = enterPosition(desX);

			if (!reEnter)
			{
				if (desX >= COL)
				{
					reEnter = true;
					drawText(140, 300, "ERROR!!", 222, 0, 0);
					drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
					_getch();
					continue;
				}
			}

			if (!reEnter)
			{
				gotoxy(22, 12);
				cout << "Y : ";
				reEnter = enterPosition(desY);

				if (!reEnter)
				{
					if (desY >= ROW)
					{
						reEnter = true;
						drawText(140, 300, "ERROR!!", 222, 0, 0);
						drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
						_getch();
						continue;
					}
				}
			}

			if (!reEnter)
			{
				if (table[desY][desX].dist == -1 || (desX == 0 && desY == ROW - 1))
				{
					reEnter = true;
					drawText(140, 300, "ERROR!!", 222, 0, 0);
					drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
					_getch();
					continue;
				}
			}
		} while (reEnter);

		if (!reEnter)
		{
			table[desY][desX].dist = -11;
			drawPoint(x, y, desY, desX, 0, 255, 0);

			int X = x + 5 + 25 * desX;
			int Y = y + 5 + 25 * desY;
			PEN = getPen(PS_SOLID, 15, RGB(255, 0, 0));
			line(X + 6, Y + 7, X + 7, Y + 7);
		}
	}

	void enterStart(int x, int y, Point * *table)
	{
		bool reEnter = false;
		do
		{
			clrscrText();
			startX = 0;
			startY = 0;
			reEnter = false;

			drawText(70, 140, "ENTER A STARTING POINT", 0, 222, 0);

			gotoxy(10, 12);
			cout << "X : ";
			reEnter = enterPosition(startX);

			if (!reEnter)
			{
				if (startX >= COL)
				{
					reEnter = true;
					drawText(140, 300, "ERROR!!", 222, 0, 0);
					drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
					_getch();
					continue;
				}
			}

			if (!reEnter)
			{
				gotoxy(22, 12);
				cout << "Y : ";
				reEnter = enterPosition(startY);

				if (!reEnter)
				{
					if (startY >= ROW)
					{
						reEnter = true;
						drawText(140, 300, "ERROR!!", 222, 0, 0);
						drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
						_getch();
						continue;
					}
				}
			}

			if (!reEnter)
			{
				if (table[startY][startX].dist == -1)
				{
					reEnter = true;
					drawText(140, 300, "ERROR!!", 222, 0, 0);
					drawText(95, 320, "PLEASE RE-ENTER", 222, 0, 0);
					_getch();
					continue;
				}
			}
		} while (reEnter);

		if (!reEnter)
		{
			drawPoint(x, y, startY, startX, 255, 0, 0);

			int X = x + 5 + 25 * startX;
			int Y = y + 5 + 25 * startY;

			drawText(X - 5, Y, "S", 0, 255, 0);
		}
		drawText(70, 140, "ENTER A STARTING POINT", 10, 10, 10);
	}

	// Bắt đầu chương trình
	void startProgram(int x, int y)
	{
		// Vẽ Robot ở đầu chương trình 
		// Sau một lúc, tiến hành xóa màn hình 
		Sleep(SLEEP * 20);
		drawRobot(300, 150);
		Sleep(SLEEP * 25);
		system("cls");

		// Vẽ hộp thoại yêu cầu người dùng mở toàn màn hình
		// Và nhấn Enter để bắt đầu chương trình 
		drawBorder(); Sleep(287);
		drawText(400, 200, "OPEN FULLSCREEN", 230, 30, 30);
		drawText(420, 230, "       AND     ", 230, 30, 30);
		drawText(340, 260, "PRESS ENTRER TO START PROGRAM", 230, 30, 30);
		getchar();

		// Tạo biến cờ xác định thoát chương trình 
		bool exitProgram = false;
		do {
			// Xóa màn hình Console 
			system("cls");

			// Tạo ma trận 
			struct Point** table;
			table = new Point * [ROW];
			for (int i = 0; i < ROW; i++)
			{
				table[i] = new Point[COL];
			}

			for (int i = 0; i < ROW; i++)
				for (int j = 0; j < COL; j++)
				{
					table[i][j].i = i;
					table[i][j].j = j;
					table[i][j].dist = 0;
					table[i][j].empty = true;
				}

			// Vẽ ma trận ROW hàng và COL cột trên Console 
			// Vẽ Robot ở phía bên phải màn hình 
			// Xác định vị trí Gốc S trên ma trận 
			drawTable(x, y);
			drawRobot(5, 800);
			randomBarrier(x, y, table); Sleep(SLEEP * 4);

			// Robot chớp mắt 
			drawEyeRobotSleep(5 + 107, 800 + 30, 50, 50, 50);
			Sleep(SLEEP * 2);
			drawEyeRobot(5 + 107, 800 + 30, 0, 222, 0);

			// Đặt gốc là đã duyệt 
			startX = 0;
			startY = 00;
			enterStart(x, y, table);
			table[startY][startX].empty = false;

			// Tiến hành nhập Destination và xác định nó trên ma trận 
			int desX = 0;
			int desY = 0;
			enterDestination(x, y, table, desX, desY);

			// Robot chớp mắt 
			drawEyeRobotSleep(5 + 107, 800 + 30, 50, 50, 50);
			Sleep(SLEEP * 2);
			drawEyeRobot(5 + 107, 800 + 30, 0, 222, 0);

			// Cài đặt, cập nhật các giá trị khoảng cách từ Gốc đến các điềm còn lại 
			// Vẽ đường đi có khoảng cách ngắn nhất lên ma trận 
			setUpTable(table);
			drawPathRobot(desY, desX, table);

			// Check giá trị khoảng cách từ gốc đến mỗi điểm trong ma trận 
			//for (int i = 0; i < ROW; i++)
			//{
			//	for (int j = 0; j < COL; j++)
			//	{
			//		cout << table[i][j].dist << "|" << table[i][j].empty << "	";
			//	}
			//	cout << endl;
			//}

			// Robot chớp mắt 
			drawEyeRobotSleep(5 + 107, 800 + 30, 50, 50, 50);
			Sleep(SLEEP * 2);
			drawEyeRobot(5 + 107, 800 + 30, 0, 222, 0);

			// Xuất chỉ thị tương tác với người dùng 
			drawText(100, 300, "Restart program [ R ]", 0, 222, 0);
			drawText(110, 330, "Exit Program [ X ]", 222, 0, 0);
			gotoxy(2, 25);
			string userSelect = "";
			cout << "Your select : ";   cin >> userSelect;

			// Thoát chương trình 
			if (userSelect == "x" || userSelect == "X")
			{
				// Xóa màn hình
				// Xóa ma trận 
				system("cls");
				for (int i = 0; i < ROW; i++)
				{
					delete(table[i]);
				}
				delete(table);

				exitProgram = true;
				drawRobot(800, 400);
			}
		} while (!exitProgram);
	}

	void robotPath()
	{
		initgraph();
		startProgram(350, 50);
	}

	int main()
	{
		robotPath();
		return 0;
	}



