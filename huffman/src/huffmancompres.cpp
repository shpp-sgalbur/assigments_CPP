#include <iostream>

#include "console.h"
#include <vector>
#include <map>
#include <list>
#include "vector.h"
#include "map.h"
#include <cctype>

#include <fstream>

using namespace std;

struct Node1
   {
       char ch;
       int parent;
       int zero;
       int one;
       bool branch;
   };
struct Node{
    char ch;
    Node * child_0;
    Node * child_1;
    int frequency;
};
void print(Vector <Node*> vec){
    /*Проверяем что в векторе hafmanTree */
    cout << "-------------hafmanTree------------" <<vec.size()<< endl;
    for(int i = 0; i < vec.size(); i++){
        cout << i+1 <<")"<< "frequency - "  << vec[i]->frequency << " symbol - " << vec[i]->ch << endl;
        cout << vec[i] << endl;
        cout  << "  child_0->"<< vec[i]->child_0 << "  " << "child_1->" << vec[i]->child_1 << endl;
    }
}
void print (map <char, int> frequencyMap){
    //выводим карту на экран
    map <char, int>::iterator itr;
    for (itr = frequencyMap.begin(); itr != frequencyMap.end(); itr++){
        cout << itr->first+0 << " : " << itr->second << endl;
    }
    cout << " ---------------------------" <<  endl;
}

void print (multimap <int, Node*> multimapNode){
    multimap <int, Node*>::iterator mp;
    for(mp = multimapNode.begin(); mp != multimapNode.end(); mp++){
        //cout << "symbol[" << *&mp->second->ch << "] "<< &mp->second << endl;
        cout << "symbol[" << *&mp->second->ch << "] "<< mp->second << endl;
    }
}

/*Создание карты частот символов из массива частот символов*/
map <char,int> frequencyMap(int frequencyArr[]){
    map <char,int> frequencyMap;
    for (int i = 0; i < 256; i++){
        if (frequencyArr[i]>0 ){
            frequencyMap[(char)i] = frequencyArr[i];
        }


    }
    return frequencyMap;
}

/*Карта пар символ - частота*/
map <char, int> creatMapSymbols(string filePath){

    //инициализируем массив частот символов нулями
    int frequency [256];
    for (int i = 0; i < 256; i++)
            frequency[i] = 0;

    //открываем файловый поток
    ifstream f (filePath, ifstream::in);
    if (!f.is_open()) // если файл не открыт
          cout << "The file cannot be opened!\n";


    while (!f.eof()){
        //считываем 1 байт

        int ch = f.get();
        //if (ch == EOF) break;

           ++frequency[ch];


    }
    frequency[10]--;
    frequency[13]--;

    f.close();

    return frequencyMap(frequency);
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

            cout << "parent->child_1" << parent->child_1 << endl;//указатель на дочерний узел

        parent->frequency = parent->child_0->frequency + parent->child_1->frequency;
        sortedMap.emplace ( parent->frequency , parent );

        cout << "---sortedMap ----" << endl;
        print (sortedMap);
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
     print (sortedMap);
     subsequentLevelsTree (sortedMap,hafmanTree);



    //print (hafmanTree);//del
    return  hafmanTree;

    }

/*функция создания карты кодирования*/
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

/*функция кодирования данных*/
void dataCoding(string filePath, map <char,string> & codeMap){

    //открываем файловый поток чтения из файла
    ifstream f (filePath, ifstream::in);
    if (!f.is_open()) // если файл не открыт
          cout << "The file cannot be opened!\n";

    //открываем файловый поток записи в файл
    ofstream f2("..\\" + filePath + ".huf", ios_base::app);

    unsigned char chRead;
    unsigned char chWrite = 0;
    int currentBit = 7;//обратный отсчет


    while (!f.eof()){

        //считываем 1 байт из файла
        f.read((char *)&chRead, sizeof(chRead));

        //из карты кодирования  вместо символа  берем его код
        string symbolCode = codeMap[chRead];

        for (int i = 0; i < symbolCode.size(); i++ ){
            if(currentBit >= 0){//байт не заполнен
                if (symbolCode[i] == '1'){
                    chWrite = chWrite | 0 | 1 << currentBit;
                }
                cout << currentBit << " - " << ""+chWrite << endl;

                currentBit--;
            }
            else{//байт заполнен
                f2.write((char *)&chWrite, sizeof(chWrite));
                currentBit = 7;
                chWrite = 0;
                i--;//повторно пройти эту итарацию для нового байта
            }
        }


    }
    if (chWrite != 0) {
        f2.write((char *)&chWrite, sizeof(chWrite));
    }
    f.close();
}
/*Запись карты частот символов в файл*/
void saveMapToFile(string filePath, map<char, int> mapSymbol){

    // открываем файл для записи
    ofstream f (filePath,ios_base::binary);

    //записываем в него количество элементов карты
    cout << "begin write" <<endl;
    char sizeMap = sizeof(mapSymbol)/sizeof(int);
    f.write((char*)&sizeMap,sizeof(sizeMap));


    //записываем саму карту
    map<char, int>::iterator i;
    for(i = mapSymbol.begin(); i!=mapSymbol.end();i++){

         char ch = i->first;
         int freq = i->second;
         f.write((char*)&ch, sizeof(ch));

         f.write((char*)&freq, sizeof(freq));
    }

    f.close();
}

map <char, int> readFromFile(string filePath){
    map <char, int> symbolMap;
    ifstream f2(filePath,ios::binary|ios::out);
    char sizemap;
    f2.read((char*)&sizemap,sizeof(sizemap));
    for(int i = 0; i<sizemap;i++){
        char ch;
        int freq;
        f2.read((char*)&ch,sizeof(ch));

        f2.read((char*)&freq,sizeof(freq));

        symbolMap.emplace(ch,freq);

    }


    print(symbolMap);

    //читаем данные
    cout << "---arh data---"<< endl;
    while(!f2.eof()){
        unsigned char ch;

        f2.read((char *)&ch, sizeof(ch));
        cout << "data " <<ch << endl;

    }
 f2.close();
    return symbolMap;
}

int main()
{
    /*ofstream fout("./cppstudio.txt"); // создаём объект класса ofstream для записи и связываем его с файлом cppstudio.txt
       fout << "Работа с файлами в С++"; // запись строки в файл
       fout.close(); // закрываем файл
       system("pause");

    /**********************
    std::ifstream ifs ("test.txt", ifstream::binary);

    if (!ifs.is_open()) // если файл не открыт
          cout << "The file cannot be opened!\n";

      // get pointer to associated buffer object
      filebuf* pbuf = ifs.rdbuf();

      // get file size using buffer's members
      size_t size = pbuf->pubseekoff (0,ifs.end,ifs.in);
      pbuf->pubseekpos (0,ifs.in);

      // allocate memory to contain file data
      char* buffer=new char[size];

      // get file data
      pbuf->sgetn (buffer,size);

      ifs.close();

      // write content to stdout
      cout.write (buffer,size);

      delete[] buffer;
    //**********************/

    string filePath ="test1.txt";//сжимаемый файл

    /*определяем частоты появления символов*/
    map <char, int>  mapSymbol;//символ и его частота
    mapSymbol  = creatMapSymbols(filePath);
    cout <<"====mapSymbol======="<<endl;
    print(mapSymbol);

    /*строим дерево Хаффмана*/
    Vector <Node*> tree;
    tree = buildTree(mapSymbol);

    cout << "in mane" <<endl;
     print (tree);//del
    for(int i; i< tree.size();i++){
        cout << "Freqancy " << tree[i]->frequency << endl;
    }

    Node * root = tree[tree.size()-1];
    cout << "root" << root << endl;
    cout << root->frequency <<endl;
    map <char, string> codeMap;
            buildCodeMap(tree[tree.size()-1], codeMap,"");
    cout << "++++++++codeMap builde++++++++++"<< endl;

    map <char, string>::iterator mp;
    for(mp=codeMap.begin();mp!=codeMap.end();mp++){
        cout << mp->first << " - "<< mp->second << endl;
    }
    cout << "++++++++codeMap builde++++++++++"<< endl;
    //копируем карту частот символов в файл в файл
    saveMapToFile("..\\" + filePath+".huf", mapSymbol);



    dataCoding("test1.txt", codeMap);

    //контрольное чтение из файла
   map<char, int> symbolMapFromFile = readFromFile(filePath+".huf");











    return 0;
}
