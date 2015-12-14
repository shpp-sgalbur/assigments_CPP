#include <iostream>
#include "console.h"
#include <vector>
#include <map>
#include <list>
#include "vector.h"
#include "simpio.h"
#include "map.h"
#include <cctype>
#include <fstream>

using namespace std;


struct Node{
    unsigned char ch;
    Node * child_0;
    Node * child_1;
    int frequency;
};

void print (map <char, int> frequencyMap){
    //выводим карту на экран
    map <char, int>::iterator itr;
    for (itr = frequencyMap.begin(); itr != frequencyMap.end(); itr++){
        cout << itr->first << " : " << itr->second << endl;
    }
    cout << " ---------------------------" <<  endl;
}

void print(map <char, string> codeMap){
    map <char, string>::iterator i;
    for(i = codeMap.begin();i != codeMap.end();i++){
        cout<< i->first<<" - " << i->second << endl;
    }
}


/**
 * Create a map of frequencies of characters from an array of frequencies of the characters
 * -------------------
 * @brief frequencyMap
 * @param frequencyArr
 * @return
 */
map <char,int> frequencyMap(int frequencyArr[]){
    map <char,int> frequencyMap;
    for (int i = 0; i < 256; i++){
        if (frequencyArr[i]>0 ){
            frequencyMap[(char)i] = frequencyArr[i];
        }
    }
    return frequencyMap;
}

/**
 * Map of pairs of symbol - frequency from file
 * --------------------------
 * @brief creatMapSymbols
 * @param filePath
 * @return
 */
map <char, int> creatMapSymbols(string filePath){

    //initialize the array of frequencies of the symbols with zeros
    int frequency [256];
    for (int i = 0; i < 256; i++)
            frequency[i] = 0;

    //open the file stream
    ifstream f (filePath, ios::binary );
    if (!f.is_open()) // если файл не открыт
          cout << "The file cannot be opened!\n";


    while (!f.eof()){

        //read 1 byte
        unsigned char ch;
        f.read((char *)&ch, sizeof(ch));
        if(f.eof()) break;

        //take into account the appearance of characters
        //if(ch !='\0')
            frequency[ch]++;

    }
    f.close();
    return frequencyMap(frequency);
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
        if (node->ch == '\0'){
            cout <<"node->ch == 0********************************";
        }
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
        parent->ch =NULL;

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
  function create card encoding
  -----------------------------
 * @brief buildCodeMap
 * @param root - the root node of the Huffman tree
 * @param codeMap - map character codes
 * @param codeSymbol - string containing the character code, needed for recursive calls to this function
 */
void buildCodeMap(Node *root, map<char, string> &codeMap, string codeSymbol){

    if(root->child_0 != NULL){
        string codeSymbol_0 = codeSymbol + "0";

        buildCodeMap(root->child_0, codeMap, codeSymbol_0);
    }
    if(root->child_1 != NULL){
        string codeSymbol_1 = codeSymbol + "1";
        buildCodeMap(root->child_1, codeMap, codeSymbol_1);
    }
    if(root->child_0 == NULL && root->child_1 == NULL){
        codeMap.emplace(root->ch,codeSymbol);

    }

}



/**
 * function encode the data and  writing it to a file
 * ------------------------------------------
 * @brief dataCoding
 * @param filePath - the path to the file from which to read data
 * @param comresFile - the path to the file to which write the data
 * @param codeMap - each symbol is mapped to a string of 0 and 1
 */
void dataCoding(string filePath, string comresFile, map <char,string> & codeMap){

    const int BITS_PER_BYTE = 8;

    //open file stream to read from the file
    ifstream originalFile (filePath, ios_base::binary);
    if (!originalFile.is_open())
          cout << "The file cannot be opened!\n";

    //open the FILESTREAM for write to end of file
    ofstream compressedFile(comresFile, ios_base::app | ios_base::binary);

    unsigned char chRead; //the encoded symbol is read from the original file
    unsigned char chWrite = 0; //contains the encoded data for writing to the compressed file
    int currentBit = BITS_PER_BYTE;//countdown
    int counter =0; //количество записанных байтов

    while (!originalFile.eof()){

        //read 1 byte from file
        originalFile.read((char *)&chRead, sizeof(chRead));
        counter++;


        //from card encode instead symbol of take his code
        string symbolCode = codeMap[chRead];

        //Fill a byte of ones and zeros
        for (int i = 0; i < symbolCode.size(); i++ ){
            currentBit--;
            if(currentBit >= 0){ //if bytes not filled
                if (symbolCode[i] == '1'){
                    chWrite = chWrite | 1 << currentBit;

                }
            }
            else{//if bytes  filled
                compressedFile.write((char *)&chWrite, sizeof(chWrite));//write a byte to a file

                //reset for a new byte
                currentBit = BITS_PER_BYTE;
                chWrite = 0;

                i--;//repeat this iteratio for a new byte
            }
        }


    }
    //recordable not complete the last byte, if it exists
    if(currentBit >= 0) {
        compressedFile.write((char *)&chWrite, sizeof(chWrite));
    }
    originalFile.close();
    compressedFile.close();
}

/**
The entry maps the frequencies of characters to a file
------------------------------------------------------
 * @brief saveMapToFile
 * @param filePath
 * @param mapSymbol
 */
void saveMapToFile(string filePath, map<char, int> mapSymbol){

    // open the FILESTREAM for write to file
    ofstream f (filePath,ios_base::binary);

    //write a number of map elements
    int sizeMap = mapSymbol.size();
    f.write((char*)&sizeMap,sizeof(sizeMap));


    //write the card itself
    map<char, int>::iterator i;
    for(i = mapSymbol.begin(); i!=mapSymbol.end();i++){

         unsigned char ch = i->first;
         int freq = i->second;
         f.write((char*)&ch, sizeof(ch));

         f.write((char*)&freq, sizeof(freq));
    }

    f.close();

}


int main()
{

    string filePath = getLine("Input the correct path file > ");  //compressed file

    /*determine the frequency of appearance of characters*/
    map <char, int>  mapSymbol;      //the symbol and its frequency
    mapSymbol  = creatMapSymbols(filePath);
    int c = mapSymbol.size();
    print (mapSymbol);
    cout <<"====mapSymbol created======="<<endl;


    /*build a Huffman tree*/
    Vector <Node*> tree;
    tree = buildTree(mapSymbol);
    cout << "==== the Huffman tree is built =======" <<endl;


    /*Encode characters*/
    Node * root = tree[tree.size()-1];
    map <char, string> codeMap;
    buildCodeMap(tree[tree.size()-1], codeMap,"");
    cout << "++++++++ codeMap  ++++++++++"<< endl;
    print(codeMap);
    cout << "++++++++ codeMap is built ++++++++++"<< endl;


    string compresFile = "..\\" +  filePath.substr(0,filePath.length()-4)+".huf"; //The path to the compressed file

    //copy a map of the frequencies of characters to a file
    saveMapToFile(compresFile, mapSymbol);


    //Write to the file data in a compressed form
    dataCoding(filePath, compresFile, codeMap);



   cout << "=== The file is compressed ===" << endl;
   cout << "The path to the compressed file " << compresFile << endl;

   return 0;
}
