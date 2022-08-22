#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <string>
#include <locale>

enum dcodERR{ // definindo erros
    ERR_OPEN = -4, // abertura incorreta do arquivo
    ERR_UNKNOWN, // erro desconhecido
    ERR_INPUT, // entrada incorreta pelo usuario
    ERR_VAR, // erro de objeto vazio ou especificado indevidamente
    ERR_OK, // sucesso
};

using namespace std;

struct estrutura_wav{
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

struct tempo_inicial_final{
    int inicio;
    int fim;
    int total;
};

string showID(char id[])
{
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
    cout << "digite o nome do arquivo a ser aberto." << endl;
    cout << "(digite tambem sua extensao. ex: \"arquivo.wav\")" << endl;
    cout << "arquivo: ";

    cin >> arquivo_abertura;
    cout << endl;

    inFile.open(arquivo_abertura, ios::in|ios::binary); // abre o arquivo para leitura em binario
    if(!inFile){ // testa se o arquivo nao foi aberto corretamente
        cerr << "O arquivo '" << arquivo_abertura << "' nao pode ser aberto. Verifique se o nome do arquivo esta correto e tente novamente." << endl;
        return ERR_INPUT;
    }

//TESTE IF .WAV

    // definindo o nome do
    string arquivo_saida = arquivo_abertura;
    arquivo_saida.insert(arquivo_abertura.rfind("."), "_corte");


    // identificando cabecalho e corpo
    estrutura_wav cab;

    // riff

    inFile.read((char *)&cab,sizeof(cab));
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

    cout << endl;
    cout << "Deseja fazer um corte de um tempo especifico na musica?" << endl;
    cout << "entre 1 para sim, 2 para não" << endl;
    cout << "entrada: ";
    int entrada;
    cin >> entrada;




    // abrindo arquivo para gravacao

    ofstream outFile;
    outFile.open(arquivo_saida, ios::out|ios::binary);


    // gravando cabecalho
    outFile.write((char *)&cab,sizeof(cab));
    cout << (char *)&cab << endl;


    // lendo um corte da musica

    tempo_inicial_final tempo;

    string sample;
    tempo.inicio = 3;
    tempo.fim = 8;
    tempo.total = tempo.fim - tempo.inicio;

    inFile.clear();
    inFile.seekg(sizeof(cab));

    char leitura;

    for(int i = 0; i < tempo.inicio*cab.ByteRate; i++){
        inFile.read((char *)&leitura, sizeof(char));
    }

    for(int i = 0; i < tempo.total*cab.ByteRate; i++){
        inFile.read((char *)&leitura, sizeof(char));
        outFile.write((char *)&leitura, sizeof(char));
    }





   outFile.close(); // fecha o arquivo para gravacao
   inFile.close(); // fecha o arquivo para leitura

}
