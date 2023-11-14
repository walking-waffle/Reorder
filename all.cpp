#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <queue>
#include <stack>
#include <chrono>
#include <algorithm>
#include <random>

using namespace std;

struct Node {
    int id;
    int numOfDegree;
};

struct Edge {
    int src;
    int dst;
};

// 降序排列
bool moreThan( Node a, Node b ) {
    return a.numOfDegree > b.numOfDegree;
} // moreThan

// 升序
bool srcLessThan( Edge a, Edge b ) {
    return a.src < b.src;
} // srcLessThan

void init( string fileName, vector<Edge> edgeList ) {
    ifstream inputFile( fileName );
    if ( !inputFile ) {
        cerr << "Error: Unable to open input file." << endl;
        exit(1);
    } // if

    // konect 資料集中，開頭兩行需去除
    char temp;
    temp = inputFile.peek();
    while ( temp == '%' ) {
        inputFile.get(temp);
        while ( temp != '\n' )
            inputFile.get(temp);

        temp = inputFile.peek();
    } // while

    int node1, node2;
    int startID = 0;

    inputFile >> node1 >> node2;

    edgeList.push_back({node1, node2});
    if ( node1 <= node2 )
        startID = node1;
    else
        startID = node2;

    // 讀取邊緣列表數據
    while ( inputFile >> node1 >> node2 ) {
        edgeList.push_back({node1, node2});
        if ( node1 < startID )
            startID = node1;
        if ( node2 < startID )
            startID = node2;
    } // while

    if ( startID != 0 ) {
        for ( int i = 0; i < edgeList.size(); i++ ) {
            edgeList.at(i).src = edgeList.at(i).src - startID;
            edgeList.at(i).dst = edgeList.at(i).dst - startID;
        } // for
    } // if

    inputFile.close();

    string name = fileName.substr( 0, fileName.find(".") );
    ofstream outputFile( name + ".txt" );
    for ( int i = 0; i < edgeList.size(); i++ ) {
        outputFile << edgeList.at(i).src << " ";
        outputFile << edgeList.at(i).dst << "\n";
    } // for
    outputFile.close();
} // init

void readEdgeList( string fileName, vector<Edge> & edgeList, int & numOfNodes ) {
    ifstream inputFile( fileName );
    if ( !inputFile ) {
        cerr << "Error: Unable to open input file." << endl;
        exit(1);
    } // if

    int node1, node2;

    // 讀取邊緣列表數據
    while ( inputFile >> node1 >> node2 ) {
        edgeList.push_back({node1, node2});
        if ( node1 > numOfNodes )
            numOfNodes = node1;
        if ( node2 > numOfNodes )
            numOfNodes = node2;
    } // while

    numOfNodes++;
    inputFile.close();
} // readEdgeList

void readCSR( string fileName, vector<int> & csrOffsetArray, vector<int> & csrEdgeArray ) {
    ifstream inputFile( fileName );
    if ( !inputFile ) {
        cerr << "Error: Unable to open input file." << endl;
        exit(1);
    } // if

    char ch;
    int offset = 0, edge = 0;

    while( inputFile.get(ch) && ch != '\n' ) {
        if ( isdigit(ch) )
            offset = offset * 10 + ( ch - '0' );
        else if ( ch == ' ' ) {
            csrOffsetArray.push_back(offset);
            offset = 0;
        } // else if
        else {
            cout << "file illegal";
            exit(1);
        } // else
    } // while

    while( inputFile.get(ch) ) {
        if ( isdigit(ch) )
            edge = edge * 10 + ( ch - '0' );
        else if ( ch == ' ' ) {
            csrEdgeArray.push_back(edge);
            edge = 0;
        } // else if
        else {
            cout << "file illegal";
            exit(1);
        } // else
    } // while

    inputFile.close();
} // readCSR

// 將圖的edge list格式轉換為CSR格式
void convertToCSR( vector<Edge> & edgeList, vector<int> & csrOffsetArray, vector<int> & csrEdgeArray ) {
    int numNodes = 0;

    // 確定節點數量
    for ( auto & edge : edgeList )
        numNodes = max( numNodes, max( edge.src, edge.dst ) + 1 );

    csrOffsetArray.resize( numNodes + 1, 0 );

    // 計算每個節點的鄰居數量
    for ( auto & edge : edgeList )
        csrOffsetArray.at(edge.src + 1)++;

    // 累積計算每個節點的起始位置
    for ( int i = 1; i <= numNodes; i++ )
        csrOffsetArray.at(i) += csrOffsetArray.at(i - 1);
    // ---------------------------------------- csrOffsetArray 完成
    csrEdgeArray.resize( edgeList.size() );

    // 將邊緣列表中的節點添加到對應的位置
    vector<int> next_idx( numNodes, 0 );

    for ( auto & edge : edgeList ) {
        int node1 = edge.src;
        int node2 = edge.dst;
        int idx = csrOffsetArray.at(node1) + next_idx.at(node1);
        csrEdgeArray.at(idx) = node2;
        next_idx.at(node1)++;
    } // for
    // ---------------------------------------- csrEdgeArray 完成

} // convertToCSR

vector <int> shuffleList( int numOfNodes ) {
    vector <int> v;
    for ( int i = 0; i < numOfNodes; i++ )
        v.push_back(i);

    unsigned seed = 0;
    shuffle( v.begin(), v.end(), default_random_engine(seed));

    return v;
} // shuffleList

void randomOrder( vector<Edge> & edgeList, int numOfNodes ) {
    vector <int> randomList;
    for ( int i = 0; i < numOfNodes; i++ )
        randomList.push_back(i);

    unsigned seed = 0;
    shuffle( randomList.begin(), randomList.end(), default_random_engine(seed));

    int temp = 0;
    for ( int i = 0; i < edgeList.size(); i++ ) {
        temp = edgeList.at(i).src;
        edgeList.at(i).src = randomList.at(temp);
        temp = edgeList.at(i).dst;
        edgeList.at(i).dst = randomList.at(temp);
    } // for
} // randomOrder

void degreeSort( vector<Edge> & edgeList, int numOfNodes ) {
    vector<Node> inDegreeList( numOfNodes, {0, 0} );
    vector<Node> outDegreeList( numOfNodes, {0, 0} );

    // 把 ID 設定好
    for ( int i = 0; i < numOfNodes; i++ ) {
        outDegreeList.at(i).id = i;
        inDegreeList.at(i).id = i;
    } // for

    // 紀錄每個 node 的 in/out degree
    int numOfEdges = edgeList.size();
    // for ( int i = 0; i < numOfEdges; i++ )
    //     outDegreeList.at(edgeList.at(i).at(0)).numOfDegree++;

    for ( int i = 0; i < numOfEdges; i++ )
        inDegreeList.at(edgeList.at(i).dst).numOfDegree++;

    sort( inDegreeList.begin(), inDegreeList.end(), moreThan );

    int temp = 0;
    for ( int i = 0; i < edgeList.size(); i++ ) {
        temp = edgeList.at(i).src;
        edgeList.at(i).src = inDegreeList.at(temp).id;
        temp = edgeList.at(i).dst;
        edgeList.at(i).dst = inDegreeList.at(temp).id;
    } // for

} // degreeSort

void hubCluster( vector<Edge> & edgeList, int numOfNodes ) {
    vector<Node> inDegreeList( numOfNodes, {0, 0} );
    vector<Node> outDegreeList( numOfNodes, {0, 0} );

    // 把 ID 設定好
    for ( int i = 0; i < numOfNodes; i++ ) {
        outDegreeList.at(i).id = i;
        inDegreeList.at(i).id = i;
    } // for

    // 紀錄每個 node 的 in/out degree
    int numOfEdges = edgeList.size();
    // for ( int i = 0; i < numOfEdges; i++ )
    //     outDegreeList.at(edgeList.at(i).at(0)).numOfDegree++;

    for ( int i = 0; i < numOfEdges; i++ )
        inDegreeList.at(edgeList.at(i).dst).numOfDegree++;

    // 算出平均 degree
    int sum = 0;
    for ( int i = 0; i < numOfNodes; i++ )
        sum += inDegreeList.at(i).numOfDegree;

    int averageDegree = sum / numOfNodes;

    vector<int> hot;
    vector<int> cold;
    for ( int i = 0; i < numOfNodes; i++ ) {
        if ( inDegreeList.at(i).numOfDegree > averageDegree )
            hot.push_back( inDegreeList.at(i).id );
        else
            cold.push_back( inDegreeList.at(i).id );
    } // for

    int temp = 0;
    vector<int> newOrder;
    newOrder.insert( newOrder.end(), hot.begin(), hot.end() );
    newOrder.insert( newOrder.end(), cold.begin(), cold.end() );
    for ( int i = 0; i < edgeList.size(); i++ ) {
        temp = edgeList.at(i).src;
        edgeList.at(i).src = newOrder.at(temp);
        temp = edgeList.at(i).dst;
        edgeList.at(i).dst = newOrder.at(temp);
    } // for

} // hubCluster

// 輸入 CSR 圖的 offset array，輸出最大 degree 的 index
int findMaxDegreeIndex( vector<int> csrOffsetArray ) {
    int max = 0;
    int numNeighbor = 0;
    int index = -1;
    for ( int i = 1; i < csrOffsetArray.size(); i++ ) {
        numNeighbor = csrOffsetArray.at(i) - csrOffsetArray.at(i-1);
        if ( max < numNeighbor ) {
            max = numNeighbor;
            index = i-1;
        } // if
    } // for

    cout << "maximum degree index: " << index << endl;
    cout << "maximum degree: " << max << endl;
    return index;
} // findMaxDegreeIndex

// 廣度優先搜索 (BFS)
vector <int> bfs( vector<int> csrOffsetArray, vector<int> csrEdgeArray, int startNode ) {
    int numNodes = csrOffsetArray.size() - 1;
    vector <int> result;

    // 記錄節點是否被訪問過
    vector<bool> visited( numNodes, false );

    // 起始節點先 push 進 queue
    queue<int> q;
    q.push(startNode);
    visited.at(startNode) = true;

    while ( !q.empty() ) {
        int currentNode = q.front();
        q.pop();

        // 找到目標
        result.push_back(currentNode);

        // 將當前節點的鄰居節點加入 queue
        for ( int i = csrOffsetArray.at(currentNode); i < csrOffsetArray.at(currentNode + 1); i++ ) {
            int neighbor = csrEdgeArray.at(i);

            // 如果鄰居節點尚未被訪問過
            if ( !visited.at(neighbor) ) {
                q.push(neighbor);
                visited.at(neighbor) = true;
            } // if
        } // for
    } // while

    return result;
} // bfs

// 深度優先搜索 (DFS)
vector<int> dfs( vector<int> csrOffsetArray, vector<int> csrEdgeArray, int startNode ) {
    int numNodes = csrOffsetArray.size() - 1;
    vector<bool> visited( numNodes, false );
    vector <int> result;;

    stack<int> s;
    s.push(startNode);
    visited.at(startNode) = true;

    while ( !s.empty() ) {
        int currentNode = s.top();
        s.pop();

        // 找到目標
        result.push_back(currentNode);

        // 將當前節點的鄰接節點加入堆疊（反向加入）
        for ( int i = csrOffsetArray.at(currentNode + 1) - 1; i >= csrOffsetArray.at(currentNode); i-- ) {
            int neighbor = csrEdgeArray.at(i);
            if ( !visited.at(neighbor) ) {
                s.push(neighbor);
                visited.at(neighbor) = true;
            } // if
        } // for
    } // while

    return result;
} // dfs

// 把 CSR 寫入檔案
void writeCSRFile( string fileName, vector<int> csrOffsetArray, vector<int> csrEdgeArray ) {
    string name = fileName.substr( 0, fileName.find(".") );
    ofstream outputFile( name + "CSR.txt" );

    for ( int val : csrOffsetArray )
        outputFile << val << " ";

    outputFile << "\n";

    for ( int val : csrEdgeArray )
        outputFile << val << " ";

    outputFile.close();
} // writeCSRFile

// 把 reordering 後的 edgeList 寫入檔案
void writeEdgeListFile( string fileName, vector<Edge> edgeList, string oper ) {
    srand(time(0));
    clock_t start, end;
    start=clock();
    sort( edgeList.begin(), edgeList.end(), srcLessThan );
    end=clock();
    cout << "SortEdgeList Time Cost: " << (1000.0)*(double)(end-start)/CLOCKS_PER_SEC << "ms" << endl;

    string name = fileName.substr( 0, fileName.find(".") );
    ofstream outputFile( name + oper + ".txt" );

    for ( int i = 0; i < edgeList.size(); i++ ) {
        outputFile << edgeList.at(i).src;
        outputFile << " ";
        outputFile << edgeList.at(i).dst;
        outputFile << " \n";
    } // for

    outputFile.close();
} // writeEdgeListFile

int getCommand() {
    cout << "==================" << endl;
    cout << "init graph       0" << endl;
    cout << "edgeList -> CSR  1" << endl;
    cout << "Random Order     2" << endl;
    cout << "Degree Sort      3" << endl;
    cout << "HubCluster       4" << endl;
    cout << "graphAlgo        5" << endl;
    cout << "==================" << endl;
    cout << "Please input the command: ";
    int command = 0;
    cin >> command;
    return command;
} // getCommand

int main() {

    int command = getCommand();
    int numOfNodes = 0;
    vector<Edge> edgeList;
    vector<int> csrOffsetArray, csrEdgeArray;

    cout << "Please input the file: ";
    string fileName = "";
    cin >> fileName;

    srand(time(0));
    clock_t start, end;

    // 把輸入圖一律變成 ID 從 0 開始
    if ( command == 0 ) {
        init( fileName, edgeList );
    } // if

    else if ( command == 1 ) {
        readEdgeList( fileName, edgeList, numOfNodes );

        start=clock();
        convertToCSR( edgeList, csrOffsetArray, csrEdgeArray );
        end=clock();
        cout << "ConvertToCSR Time Cost: " << (1000.0)*(double)(end-start)/CLOCKS_PER_SEC << "ms" << endl;

        writeCSRFile( fileName, csrOffsetArray, csrEdgeArray );
    } // else if

    else if ( command == 2 ) {
        readEdgeList( fileName, edgeList, numOfNodes );

        start=clock();
        randomOrder( edgeList, numOfNodes );
        end=clock();
        cout << "Random Time Cost: " << (1000.0)*(double)(end-start)/CLOCKS_PER_SEC << "ms" << endl;

        writeEdgeListFile( fileName, edgeList, "_Random" );
    } // else if

    else if ( command == 3 ) {
        readEdgeList( fileName, edgeList, numOfNodes );

        start=clock();
        degreeSort( edgeList, numOfNodes );
        end=clock();
        cout << "DegreeSort Time Cost: " << (1000.0)*(double)(end-start)/CLOCKS_PER_SEC << "ms" << endl;

        writeEdgeListFile( fileName, edgeList, "_DegreeSort" );
    } // else if

    else if ( command == 4 ) {
        readEdgeList( fileName, edgeList, numOfNodes );

        start=clock();
        hubCluster( edgeList, numOfNodes );
        end=clock();
        cout << "HubCluster Time Cost: " << (1000.0)*(double)(end-start)/CLOCKS_PER_SEC << "ms" << endl;

        writeEdgeListFile( fileName, edgeList, "_HubCluster" );
    } // else if

    else if ( command == 5 ) {
        vector<int> csrOffsetArray, csrEdgeArray;
        readCSR( fileName, csrOffsetArray, csrEdgeArray );

        int maxDegreeIndex = findMaxDegreeIndex( csrOffsetArray );

        start=clock();
        // BFS
        vector<int> bfsTravelList;
        bfsTravelList = bfs( csrOffsetArray, csrEdgeArray, maxDegreeIndex );
        end=clock();
        cout << "BFS Finish." << endl;
        cout << "Time Cost: " << (1000.0)*(double)(end-start)/CLOCKS_PER_SEC << "ms" << endl;

        start=clock();
        // DFS
        vector<int> dfsTravelList;
        dfsTravelList = dfs( csrOffsetArray, csrEdgeArray, maxDegreeIndex );
        end=clock();
        cout << "DFS Finish." << endl;
        cout << "Time Cost: " << (1000.0)*(double)(end-start)/CLOCKS_PER_SEC << "ms" << endl;
    } // else if
    else {
        cout << "command error!";
    } // else

} // main()
