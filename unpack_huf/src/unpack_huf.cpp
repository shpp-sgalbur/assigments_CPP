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
  Функция считывающая карту частот символов
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
    //f.close;

    return symbolMap;
}

/* Функция инициализации первого уровня дерева (уровня узлов листов)
 * Функция создания мультикарты узлов для каждого символа из карты частот символов.
 *  При этом ключи и значения меняются местами
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

/*Функция получения указателя на первый узел мультикарты с удалением этого указателя из мультикарты.
 * Такой узел является дочерним для
 * создания родительского узла для первого элемента мультикарты с последующим удалением этого узла из мультикарты*/
Node * getChildNode(multimap<int, Node *> &sortedMap){

    Node * child = begin(sortedMap)->second;
    sortedMap.erase(begin(sortedMap));
    return child;
}

/* Функция постороения последующих уровней дерева вплоть до корня */
void subsequentLevelsTree (multimap<int, Node *> &sortedMap, Vector <Node*> &hafmanTree){

    while (sortedMap.size() > 1){

        //вытаскиваем из sortedMap два элемента с меньшей частотой, сохраняя значение байта и его частоты в переменных

        // вытаскивая два элемента карты
        //создаем над извлеченными элементами родителя (с указателями на них как на потомков)
        Node *parent = new Node;
        parent->child_0 = getChildNode(sortedMap);
            cout << "parent->child_0" << parent->child_0 << endl;//указатель на дочерний узел
            //cout << "parent->child_0" << &child << endl;//адрес первого элемента sortedMap, всегда имеет одно значение
            //cout << "parent->child_0" << begin(sortedMap)->second << endl;
        parent->child_1 = getChildNode(sortedMap);
        parent->ch = NULL;

            cout << "parent->child_1" << parent->child_1 << endl;//указатель на дочерний узел

        parent->frequency = parent->child_0->frequency + parent->child_1->frequency;
        sortedMap.emplace ( parent->frequency , parent );

        cout << "---sortedMap ----" << endl;
        //print (sortedMap);
        cout << "---sortedMap ----" << endl;
        hafmanTree.add(parent);
    }
}


/* Функция построения дерева хафмана из карты частот символов
 * -----------------------------------
 */
Vector <Node*> buildTree(map <char, int> frequencyMap){
    Vector <Node*> hafmanTree;
    multimap<int /*weight*/, Node */* index in the tree */> sortedMap;

    /* -переносим данные из frequencyMap в sortedMap вместо сортировки
      ключ (first) -частота, значение (second) - символ.
      - создаем первый уровень дерева состящий из  узлов-листов
    */
     firstLevelTree(frequencyMap, sortedMap,hafmanTree);
     //print (sortedMap);
     subsequentLevelsTree (sortedMap,hafmanTree);


     cout << "===========tree in buildTree=============" <<endl;

    print (hafmanTree);//del
    return  hafmanTree;

    }
/**
  Функция чтения закодированных данных
  -------------------------------------
 * @brief getData
 * @param pathFile
 */
Vector<bool> getDataBin(string pathFile){
    const int BITS_PER_BYTE = 8;
    ifstream fileCompres(pathFile,ios::binary);
    if (!fileCompres.is_open()) // если файл не открыт
          cout << "The file cannot be opened!\n";
    //ofstream fileDecompres(":d\\"+pathFile.substr(0,pathFile.length()-4),ios::out);
    Vector<bool> data;
    if (!fileCompres.is_open()) // если файл не открыт
          cout << "The file cannot be opened!\n";

    //определяем адрес начального байта данных
    char sizemap;
    fileCompres.read((char*)&sizemap,sizeof(sizemap));
    int dataBegin = 1 + sizemap * (sizeof(int) + sizeof(char));


    fileCompres.seekg ( dataBegin ,ios::beg );

    cout << "reading data" << endl;
    while(!fileCompres.eof()){
        unsigned char ch;

        fileCompres.read((char *)&ch, sizeof(ch));
        cout << "data " <<ch+0x0 << endl;
        for (int i = 1; i <= BITS_PER_BYTE; ++i){
            if((ch & (1<<BITS_PER_BYTE - i)) != 0){
                data.push_back(1);
            }
            else{
                data.push_back(0);
            }
        }

        //fileDecompres.write((char *)&ch, sizeof(ch));

    }
fileCompres.close();
//fileDecompres.close();
return data;

}

void getData(Vector <bool>& data, Vector <Node*> &tree, string pathFile, int fileSize){
    cout << "===========tree in getData=============" <<endl;
    print(tree);
    cout<<"=========data in getData============="<<endl;
    cout << data << endl;
    string pathFileDecompres = pathFile.substr(0,pathFile.length()-4);
    ofstream fileDecompres(pathFileDecompres,ios::out);


    //определяем номер корневого узла в векторе узлов дерева
    Node *node = tree[tree.size() - 1];
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
                node =  tree[tree.size() - 1];
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

    //считываем данные  в двоичном формате в вектор
    data = getDataBin(pathFile);
    cout << data << endl;

    //определяем размер файла
    int fileSize = getSizeFile(mapSymbol);

    //распаковываем данные из вектора data с помощью дерева Хафмана
    getData(data, tree, pathFile, fileSize);


}

int main() {
    //ввод имени сжатого файла
    string pathFile = getLine("Input the correct path file > ");
    unpack(pathFile);

    return 0;
}
