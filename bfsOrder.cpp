#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

void readFile( string fileName, vector<vector<int>> & edgeList, int & numOfNodes ) {
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
} // readFile

// 廣度優先搜索 (BFS)
vector <int> bfs( vector<int> row_ptr, vector<int> col_idx, int startNode ) {
    int numNodes = row_ptr.size() - 1;
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
        for ( int i = row_ptr.at(currentNode); i < row_ptr.at(currentNode + 1); i++ ) {
            int neighbor = col_idx.at(i);

            // 如果鄰居節點尚未被訪問過
            if ( !visited.at(neighbor) ) {
                q.push(neighbor);
                visited.at(neighbor) = true;
            } // if
        } // for
    } // while

    cout << "BFS Finish." << endl;
    return result;
} // bfs

// 將圖的edge list格式轉換為CSR格式
void convertToCSR( vector<vector<int>> & edgeList, vector<int> & row_ptr, vector<int> & col_idx ) {
    int numNodes = 0;
    
    // 確定節點數量
    for ( auto & edge : edgeList )
        numNodes = max( numNodes, max( edge.at(0), edge.at(1) ) + 1 ); 

    row_ptr.resize( numNodes + 1, 0 ); 

    // 計算每個節點的鄰居數量
    for ( auto & edge : edgeList )
        row_ptr.at(edge.at(0) + 1)++;

    // 累積計算每個節點的起始位置
    for ( int i = 1; i <= numNodes; i++ )
        row_ptr.at(i) += row_ptr.at(i - 1);
    // ---------------------------------------- row_ptr 完成
    col_idx.resize( edgeList.size() );

    // 將邊緣列表中的節點添加到對應的位置
    vector<int> next_idx( numNodes, 0 );

    for ( auto & edge : edgeList ) {
        int node1 = edge.at(0);
        int node2 = edge.at(1);
        int idx = row_ptr.at(node1) + next_idx.at(node1);
        col_idx.at(idx) = node2;
        next_idx.at(node1)++;
    } // for
    // ---------------------------------------- col_idx 完成

} // convertToCSR

int findIndex( vector<int> v, int target ) {
    if ( v.empty() )
        return -1;
    else {
        int vSize = v.size();
        for ( int i = 0; i < vSize; i++ ) {
            if ( v.at(i) == target )
                return i;
        } // for

        return -1;
    } // else
}

void bfsOrder( vector<vector<int>> & edgeList, int numOfNodes ) {
    vector<int> row_ptr, col_idx;

    // 將圖的 edge list 格式轉換為 CSR 格式
    convertToCSR( edgeList, row_ptr, col_idx );

    vector <int> bfsList = bfs( row_ptr, col_idx, 0 );
    vector <int> bfsID2Index( numOfNodes, -1 );
    vector <int> notInBFSList;

    for( int i = 0; i < bfsList.size(); i++ )
        bfsID2Index.at(bfsList.at(i)) = i;

    int temp = 0, tempIndex = 0;
    for ( int i = 0; i < edgeList.size(); i++ ) {
        // 取得ID
        temp = edgeList.at(i).at(0);
        // 找出 ID 是否在 bfsList 中，有的話回傳 index，沒有則回傳-1
        tempIndex = bfsID2Index.at(temp);
        // 有，將 index 換成新 ID
        if ( tempIndex != -1 )
            edgeList.at(i).at(0) = tempIndex;
        // 無，去找不在BFS的ID list 依序給他新ID
        else {
            tempIndex = findIndex( notInBFSList, temp );
            // 有找到，將 index 換成新 ID
            if ( tempIndex != -1 )
                edgeList.at(i).at(0) = bfsList.size() + tempIndex;
            else {
                notInBFSList.push_back( temp );
                edgeList.at(i).at(0) = bfsList.size() + notInBFSList.size() - 1;
            } // else
        } // else

        // 同樣的事情而已
        temp = edgeList.at(i).at(1);
        tempIndex = bfsID2Index.at(temp);
        if ( tempIndex != -1 )
            edgeList.at(i).at(1) = tempIndex;
        else {
            tempIndex = findIndex( notInBFSList, temp );
            // 有找到，將 index 換成新 ID
            if ( tempIndex != -1 )
                edgeList.at(i).at(1) = bfsList.size() + tempIndex;
            else {
                notInBFSList.push_back( temp );
                edgeList.at(i).at(1) = bfsList.size() + notInBFSList.size() - 1;
            } // else
        } // else
    } // for

} // bfsOrder

void writeFile( string fileName, vector<vector<int>> edgeList ) {
    string name = fileName.substr( 0, fileName.find(".") );
    ofstream outputFile( name + "_bfsOrder.txt" );

    for ( int i = 0; i < edgeList.size(); i++ ) {
        outputFile << edgeList.at(i).at(0);
        outputFile << " ";
        outputFile << edgeList.at(i).at(1);
        outputFile << " \n";
    } // for
    
    outputFile.close();
} // writeFile

int main() {

    cout << "Please input the file: ";
    string fileName = "";
    cin >> fileName;

    int numOfNodes = 0;
    vector<vector<int>> edgeList;
    readFile( fileName, edgeList, numOfNodes );
    cout << "readFile finish!" << endl;
    bfsOrder( edgeList, numOfNodes );
    cout << "bfsOrder finish!" << endl;
    writeFile( fileName, edgeList );

} // main()