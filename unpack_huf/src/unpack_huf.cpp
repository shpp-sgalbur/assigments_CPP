
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
    unsigned char ch;
    Node * child_0;
    Node * child_1;
    int frequency;
};

/*************************************************************/
/*----- auxiliary procedures for debugging - begin ----------*/

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

void print (Vector <bool> vec){
    for(int i = 0; i < vec.size(); i++){
        cout << vec[i];
    }
    cout <<endl;
}
/*----- auxiliary procedures for debugging - end ----------*/
/*************************************************************/

/**
  The function reads a map of the frequencies of the symbols from file
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
    int sizemap;
    f.read((char*)&sizemap,sizeof(sizemap));
    for(int i = 0; i<sizemap;i++){
        unsigned char ch;
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

        /*pulls out two first elements from the map of symbols and creates the parent
         * node above checked-out elements   (with pointers to descendants)*/
        Node *parent = new Node;
        parent->child_0 = getChildNode(sortedMap);
        parent->child_1 = getChildNode(sortedMap);
        parent->frequency = parent->child_0->frequency + parent->child_1->frequency;
        parent->ch = NULL;

        //replace in the map  two remote node  their parent node
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
        - create the first level of the tree consists of nodes-lists*/
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

    Vector<bool> data;//containing the value of all data bits

    //determine address of the initial byte of data
    int sizemap;
    fileCompres.read((char*)&sizemap,sizeof(sizemap));
    int dataBegin = sizeof(sizemap) + sizemap * (sizeof(int) + sizeof(char));

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
/** from binary code receives initial data and writes them to a file
 * -----------------------------------------------------------------------
 * @brief getDataAndWrite
 * @param data
 * @param tree
 * @param pathFile
 * @param fileSize
 */
void getDataAndWrite(Vector <bool>& data, Vector <Node*> &tree, string pathFile, int fileSize){

    string pathFileDecompres = pathFile.substr(0,pathFile.length()-4);
    ofstream fileDecompres(pathFileDecompres,ios::binary);

    // allocate memory for file content
      char* buffer = new char[fileSize-1];
      int indChar = 0;

    //define the root node in the vector of nodes of the tree
    Node *node = tree[tree.size() - 1];
    Node *root = node;

    //move from the tree root to the leaf using the binary code
    for(int i = 0; i < data.size(); i++){
        if(data[i]){
            node = node->child_1;            
        }
        else{
            node = node->child_0;           
        }


        if(node->child_1==NULL || node->ch!='\0'){//reached the leaf

            //the recorded value of the sheet to the clipboard
            unsigned char ch = node->ch ;
            buffer[indChar++] = ch;            


            if(fileSize > indChar){//if  still have the binary code for decoding
                node =  root;
            }
            else{
                break;//if there is spurious code
            }
        }
    }

    fileDecompres.write(buffer, fileSize);
    fileDecompres.close();
    cout << "The file has been successfully decoded." << endl;
    cout << "The location of the file " << pathFileDecompres << endl;
}
/** the calculation function of the initial size of the compressed file
 * -------------------------
 * @brief getSizeFile
 * @param mapSymbol
 * @return
 */
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

    //read a map of the frequencies of the symbols from file
    map <char,int> mapSymbol = readMapSymbol(pathFile);    

    //builds a tree
    Vector <Node*> tree;
    tree = buildTree(mapSymbol);

    //read data in binary format into the vector
    data = getDataBin(pathFile);

    //determine the size of the source file
    int fileSize = getSizeFile(mapSymbol);    

    //unpack data from the vector data using the Huffman tree
    getDataAndWrite(data, tree, pathFile, fileSize);


}

int main() {


    string pathFile = getLine("Input the correct path file > ");
    unpack(pathFile);

    return 0;
}
