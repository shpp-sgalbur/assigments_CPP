#include <iostream>
#include "huf.h"
#include "console.h"
#include <vector>
#include "vector.h"
#include "simpio.h"
#include "map.h"
#include <fstream>
using namespace std;

struct Node{
    char ch;
    Node * child_0;
    Node * child_1;
    int frequency;
};
void print(map <char, string> codeMap){
    map <char, string>::iterator itr;
    cout << " ----------Begin codeMap-----------------" <<  endl;

    for (itr = codeMap.begin(); itr != codeMap.end(); itr++){
        cout << itr->first << " : " << itr->second << endl;
    }
    cout << " ----------end codeMap-----------------" <<  endl;

}

void print (map <char, int> frequencyMap){
    //выводим карту на экран
    map <char, int>::iterator itr;
    for (itr = frequencyMap.begin(); itr != frequencyMap.end(); itr++){
        cout << itr->first << " : " << itr->second << endl;
    }
    cout << " ---------------------------" <<  endl;
}

void print(Vector <Node*> vec){
    /*Проверяем что в векторе hafmanTree */
    cout << "-------------hafmanTree------------" <<vec.size()<< endl;
    for(int i = 0; i < vec.size(); i++){
        cout << i+1 <<")"<< "frequency - "  << vec[i]->frequency << " symbol - " << vec[i]->ch << endl;
        cout << vec[i] << endl;
        cout  << "  child_0->"<< vec[i]->child_0 << "  " << "child_1->" << vec[i]->child_1 << endl;
    }
}

/**
  The function reads a map of the frequencies of the symbols
  --------------------------------------
 * @brief readFrequencyMap
 * @param pathFile
 * @return
 */
map <char,int> readMapSymbol (string pathFile){
    map <char,int> symbolMap;
    ifstream f(pathFile,ios::binary);
    if (!f.is_open()){
          cout << "The file cannot be opened!\n";
          return symbolMap;
    }
    char sizemap;
    f.read((char*)&sizemap,sizeof(sizemap));
    for(int i = 0; i<sizemap;i++){
        char ch;
        int freq;
        f.read((char*)&ch,sizeof(ch));

        f.read((char*)&freq,sizeof(freq));

        symbolMap.emplace(ch,freq);

    }
    f.close();

    return symbolMap;
}

/**
 * The initialization function of the first level of the tree (the node level sheets)
 * Multicards create nodes for each symbol from the map of frequencies of the characters.
 * Note that keys and values are swapped.
 * --------------------------------------
 * @brief firstLevelTree
 * @param frequencyMap
 * @param sortedMap
 * @param hafmanTree
 */
void firstLevelTree(map <char, int> frequencyMap,
                  multimap<int, Node *> &sortedMap,
                  Vector <Node*> &hafmanTree){
    map  <char, int>::iterator itr;
    for (itr = frequencyMap.begin(); itr != frequencyMap.end(); itr++){
        Node  *node = new Node;
        node->ch = itr->first;
        node->frequency = itr->second;
        node->child_0 = node->child_1 = NULL;
        sortedMap.emplace ( node->frequency , node );
        hafmanTree.add(node);
    }
}

/**
 * Function get a pointer to the first node of multimap with the removal of this pointer from multimap
 * ----------------------------------------------------------
 * @brief getChildNode
 * @param sortedMap
 * @return
 */
Node * getChildNode(multimap<int, Node *> &sortedMap){

    Node * child = begin(sortedMap)->second;
    sortedMap.erase(begin(sortedMap));
    return child;
}

/**
  Function build the next levels of the tree up to the root
  ----------------------------------------------------
 * @brief subsequentLevelsTree
 * @param sortedMap - the key (first) - frequency, the value (second) - symbol.
 * @param hafmanTree
 */
void subsequentLevelsTree (multimap<int, Node *> &sortedMap, Vector <Node*> &hafmanTree){

    while (sortedMap.size() > 1){

        /*pulling out two item cards create on checked-out items parent node (with pointers to them  descendants) */
        Node *parent = new Node;
        parent->child_0 = getChildNode(sortedMap);
        parent->child_1 = getChildNode(sortedMap);
        parent->frequency = parent->child_0->frequency + parent->child_1->frequency;

        //replace in the card  two remote node  their parent node
        sortedMap.emplace ( parent->frequency , parent );

        //add the node to the vector of pointers to node
        hafmanTree.add(parent);
    }
}


/**
  The function of the tree construction of Huffman
    from the map of frequencies of the characters
    -----------------------------------------------
 * @brief buildTree
 * @param frequencyMap
 * @return
 */
Vector <Node*> buildTree(map <char, int> frequencyMap){

    Vector <Node*> hafmanTree; //will contain the Huffman tree

    multimap<int, Node *> sortedMap;//

    /* -move the data from frequencyMap in sortedMap instead of sorting
        the key (first) frequency, the value (second) symbol.
        - create the first level of the tree consists of nodes-lists
    */
     firstLevelTree(frequencyMap, sortedMap,hafmanTree);

     //build the subsequent levels of the tree up to the root
     subsequentLevelsTree (sortedMap,hafmanTree);

    return  hafmanTree;

    }

/**
  The function of reading the encoded data
  -------------------------------------
 * @brief getData
 * @param pathFile
 */
Vector<bool> getDataBin(string pathFile){

    const int BITS_PER_BYTE = 8;

    ifstream fileCompres(pathFile,ios::binary);
    if (!fileCompres.is_open())
          cout << "The file cannot be opened!\n";

    if (!fileCompres.is_open())
          cout << "The file cannot be opened!\n";

    Vector<bool> data;//containing the value of all data bits

    //determine address of the initial byte of data
    char sizemap;
    fileCompres.read((char*)&sizemap,sizeof(sizemap));
    int dataBegin = 1 + sizemap * (sizeof(int) + sizeof(char));

    cout << "dataBegin = "<<dataBegin<< endl;

    //go to the start position of the data
    fileCompres.seekg ( dataBegin ,ios::beg );

    cout << "reading data" << endl;

    while(!fileCompres.eof()){

        unsigned char ch;

        fileCompres.read((char *)&ch, sizeof(ch));

        for (int i = 1; i <= BITS_PER_BYTE; ++i){
            if((ch & (1<<BITS_PER_BYTE - i)) != 0){
                data.push_back(1);
            }
            else{
                data.push_back(0);
            }
        }
    }
    fileCompres.close();
    cout << "read data completed successfully" <<endl;
    return data;

}

void getData(Vector <bool>& data, Vector <Node*> &tree, string pathFile, int fileSize){

    string pathFileDecompres = pathFile.substr(0,pathFile.length()-4);
    ofstream fileDecompres(pathFileDecompres,ios::out);

    cout << "data.size - " << data.size()<< endl;


    //определяем  корневой узел в векторе узлов дерева    
    Node *node = tree[tree.size() - 1];
    Node *root = node;
    for(int i = 0; i < data.size(); i++){
        if(data[i]){
            node = node->child_1;
        }
        else{
            node = node->child_0;
        }

        if(node->ch!=NULL){
            cout << node->ch <<" ";
            char ch = node->ch ;
            fileDecompres.write((char *)&ch, sizeof(ch));
            fileSize--;
            if(fileSize>0){
                node =  root;
            }
            else{
                break;
            }


        }
    }
    fileDecompres.close();
}

int getSizeFile(map <char,int> mapSymbol){
    int res = 0;
    map <char,int>::iterator i;
    for(i = mapSymbol.begin(); i != mapSymbol.end(); i++){
        res += i->second;
    }

    return res;
}



void unpack(string pathFile){
    Vector <bool> data;

    //читаем карту частот
    map <char,int> mapSymbol = readMapSymbol(pathFile);
    print (mapSymbol);

    //строим дерво
    Vector <Node*> tree;
    tree = buildTree(mapSymbol);

    print (tree);
    cout << "root " << endl;
    cout  << "-----" <<endl;
    cout <<tree[tree.size() - 1] << endl;

    //считываем данные  в двоичном формате в вектор
    data = getDataBin(pathFile);


    //определяем размер файла
    int fileSize = getSizeFile(mapSymbol);

    //распаковываем данные из вектора data с помощью дерева Хафмана
    getData(data, tree, pathFile, fileSize);


}

int main() {
    setLocale(LC_TYPE, "rus");
    //ввод имени сжатого файла
    string pathFile = getLine("Input the correct path file > ");
    unpack(pathFile);

    return 0;
}
