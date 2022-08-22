/* *******************************************************************
 @brief   programa que possui como objetivo ler informacoes de um
          arquivo '.wav' e fazer cortes caso o usuario desejar
 @file    main.cpp
 @author  Artur Ritzel e Thiago Godoy Cunha
 @date    agosto/2022
 @version 1
******************************************************************** */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <string>
#include <locale>

enum dcodERR{ // definindo erros
    ERR_UNKNOWN, // erro desconhecido
    ERR_INPUT, // entrada incorreta pelo usuario
    ERR_OK, // sucesso
};

using namespace std;

struct estrutura_wav{ // gera uma estrutura para leitura do cabecalho e corpo.
    char ChunkID[4];
    uint32_t ChunkSize;
    char Format[4];
    char Subchunk1ID[4];
    uint32_t Subchunk1Size;
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
    char Subchunk2ID[4];
    uint32_t Subchunk2Size;
};

struct tempo_inicial_final{ // estrutura para facilitacao de leitura para o tempo do corte
    int inicio;
    int fim;
    int total;
};

/*************************************************
* @brief le um char e retorna seu id
* @param char a ser lido
* @retval id lido
*************************************************/
string showID(char id[]){
    string sid;
    for(int i=0;i<4;i++){
        sid.push_back(id[i]);
    }

    return sid;
}

int main()
{
    // entrada do arquivo

    string arquivo_abertura; // cria a string para o nome do arquivo de abertura
    ifstream inFile; // cria uma variável do tipo ifstream

    cout << endl;
    cout << "manipulacao de arquivos wav";
    cout << endl;
    cout << "digite o nome do arquivo a ser aberto." << endl;
    cout << "(digite tambem sua extensao. ex: \"arquivo.wav\")" << endl;
    cout << "arquivo: ";

    cin >> arquivo_abertura; // entrada para o nome do arquivo para leitura
    cout << endl;

    if(arquivo_abertura.size() - arquivo_abertura.find(".wav", 0) != 4){
        // testa se o nome do arquivo termina em '.wav'
        // caso nao termina em '.wav', encerra.
        // (a posicao da substring '.wav' deve estar na posicao x-4, onde x e o tamanho total do nome do arquivo)
        cerr << "O arquivo '" << arquivo_abertura << "' nao e um arquivo do tipo wav.";
        return ERR_INPUT;
    }

    inFile.open(arquivo_abertura, ios::in|ios::binary); // abre o arquivo para leitura em binario

    if(!inFile){ // testa se o arquivo nao foi aberto corretamente
        cerr << "O arquivo '" << arquivo_abertura << "' nao pode ser aberto. Verifique se o nome do arquivo esta correto e tente novamente." << endl;
        return ERR_INPUT;
    }

    // definindo o nome do
    string arquivo_saida = arquivo_abertura; // copia o nome do arquivo para outra string
    arquivo_saida.insert(arquivo_abertura.rfind("."), "_corte"); // adiciona '_corte' antes do '.' no nome do arquivo
    // exemplo: 'arquivo.wav' ira se tornar 'arquivo_corte.wav'


    // identificando cabecalho e corpo
    estrutura_wav cab; // cria a variavel 'cab' do tipo 'estrutura_wav'

    inFile.read((char *)&cab,sizeof(cab)); // le os dados do cabecalho de acordo com o tamanho total



    // imprimindo informacoes do arquivo

    // riff

    cout << "ChunkID: " << showID(cab.ChunkID) << "(possui as letras \"RIFF\" em ASCII)" << endl;
    cout << "Tamanho do Chunk: " << cab.ChunkSize << " bytes" << endl;
    cout << "Formato: " << showID(cab.Format) << "(possui as letras \"WAVE\")" << endl;

    cout << endl;

    // fmt

    cout << "Subchunk1ID: " << showID(cab.Subchunk1ID) << "(possui as letras \"fmt\")" << endl;
    cout << "Tamanho do Subchunk1: " << cab.Subchunk1Size << " bytes (16 para PCM)" << endl;
    cout << "Formato de audio: " << cab.AudioFormat << " (1 para PCM)" << endl;
    cout << "Número de canais: " << cab.NumChannels << " (1 para mono, 2 para stereo)" << endl;
    cout << "Taxa de amostragem: " << cab.SampleRate << " (8000, 44100, etc)" << endl;
    cout << "Taxa de bytes: " << cab.ByteRate << "(SampleRate * NumChannels * BitsPerSample/8)" << endl;
    cout << "Block align: " << cab.BlockAlign << "(NumChannels * BitsPerSample/8)" << endl;
    cout << "Bits por amostra: " << cab.BitsPerSample << endl;

    cout << endl;

    // data

    cout << "Subchunk2ID: " << showID(cab.Subchunk2ID) << " (contém as letras \"data\")" << endl;
    cout << "Tamanho do Subchunk2: " << cab.Subchunk2Size << endl;


    // leitura dos dados

    cout << endl;
    cout << "Deseja fazer um corte de um tempo especifico na musica?" << endl;
    cout << "entre 1 para sim, 2 para nao" << endl;
    cout << "entrada: ";
    int entrada;
    cin >> entrada;

    while(entrada != 2 && entrada !=1){ // caso entrada invalida, pede entrada de novo
        cout << "entrada invalida." << endl;
        cout << "entre 1 para sim, 2 para nao" << endl;
        cout << "entrada: ";
        cin >> entrada;
    }

    if(entrada == 2){ // caso nao queira um corte, programa encerra
        cout << endl;
        cout << "o programa ira encerrar";
        return ERR_OK; // retorna sucesso
    }

    cout << endl;
    cout << "qual estilo voce deseja usar para o corte?" << endl;
    cout << "1- X segundos a partir do segundo Y" << endl;
    cout << "2- segundo X ate segundo Y" << endl;
    cout << "entrada: ";
    cin >> entrada;

    while(entrada != 2 && entrada !=1){ // caso entrada invalida, pede entrada de novo
        cout << "entrada invalida." << endl;
        cout << "entre 1 ou 2 de acordo com sua escolha" << endl;
        cout << "entrada: ";
        cin >> entrada;
    }

    cout << endl;

    tempo_inicial_final tempo; // cria a variavel 'tempo' do tipo 'tempo_inicial_final'
    cout << "iniciar a partir de qual segundo?" << endl;
    cout << "entrada: ";
    cin >> tempo.inicio; // entrada para tempo inicial

    while(tempo.inicio < 0){
        cout << "entrada invalida." << endl;
        cout << "verifique se a entrada esta em segundos positivos e tente novamente" << endl;
        cin >> tempo.inicio;
    }

    if(entrada == 1){ // caso estilo de corte 1
        cout << "o corte deve ter quantos segundos?" << endl;
        cout << "entrada: ";
        cin >> tempo.total;
    }

    if(entrada == 2){ // caso estilo de corte 2
        cout << "o corte deve acabar em qual segundo?";
        cout << "entrada: ";
        cin >> tempo.fim;

        while(tempo.fim <= tempo.inicio){
            cout << "o tempo final deve ser maior que o tempo inicial!" << endl;
            cout << "entre outro valor" << endl;
            cin >> tempo.fim;
        }

        tempo.total = tempo.fim - tempo.inicio; // calcula o tempo do corte de acordo com as informacoes
    }




    // abrindo arquivo para gravacao

    ofstream outFile; // cria a variavel 'outFile' do tipo 'ofstream'
    outFile.open(arquivo_saida, ios::out|ios::binary); // abre o arquivo para escrita em binario


    // gravando cabecalho
    outFile.write((char *)&cab,sizeof(cab)); // escreve o cabecalho lido no arquivo corte


    // lendo um corte da musica


    inFile.clear();
    inFile.seekg(sizeof(cab)); // define a posicao para leitura do arquivo

    char leitura; // cria uma variavel para transportar informacoes de leitura para escrita

    for(int i = 0; i < tempo.inicio*cab.ByteRate; i++){
        inFile.read((char *)&leitura, sizeof(char)); // le todos as amostras ate o tempo inicial (mas nao faz nada com elas)
    }

    for(int i = 0; i < tempo.total*cab.ByteRate; i++){
        inFile.read((char *)&leitura, sizeof(char)); // le todas as amostras do tempo inicial ate o tempo final
        outFile.write((char *)&leitura, sizeof(char)); // grava as amostras no arquivo corte
    }

    cout << endl;
    cout << "arquivo gerado com sucesso.";
    cout << "o programa ira encerrar.";


   outFile.close(); // fecha o arquivo para gravacao
   inFile.close(); // fecha o arquivo para leitura
   return ERR_OK; // retorna sucesso

}
