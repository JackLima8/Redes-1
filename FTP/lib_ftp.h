#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <unistd.h>
#include <netpacket/packet.h>
#include <sys/poll.h>
#include <net/ethernet.h>  // Cabecalho Ethernet.
#include <net/if.h>        // Estrutura 'ifr'.
#include <netinet/in.h>    // Definicao de protocolos.
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include<math.h>
#define FALSE 0
#define TRUE 1
#define Q1 1
#define Q2 2
#define Q3 3
#define LS_LOCAL -4
#define LS_REMOTO -5
#define CD_LOCAL -6
#define CD_REMOTO -7
#define TIMEOUT_M 1000 //em milisegundos
#define T_MAX 10
#define TAM_QUADRO 19
#define TAM_A_DADOS 15
#define MARCA 10
#define CD 0
#define ACK 1
#define NACK 2
#define LS 3
#define MOSTRA 4
#define PUT 5
#define GET 6
#define ERRO_I 7
#define ERRO_P 8
#define ATRIBUTOS 10
#define DADOS 12
#define ERRO 13
#define FIM 14 


typedef struct{
	unsigned char marca:6;
	unsigned char tam:4;
	unsigned char seq:2;
	unsigned char tipo:4;
	unsigned char dados[16];
	unsigned char crc;	
}Quadro;

typedef struct crc {
  unsigned bits:9;
} crc;


int Rawsocket(); // cria o socket
void Zera_Buffer(char buffer[]);
void Zera_Buffer_u(unsigned char buffer[]);
Quadro * novo_quadro(); 
void buffer2quadro(unsigned char buffer[], Quadro *q);;
void wait ( int seconds ); //auxilia timeout
void Monta_MSG(char seq,char tipo,char dados[],Quadro *q);
Quadro  Seta_Quadro(Quadro *q,int tipo);
void quadro2buffer_u(Quadro *q,unsigned char buffer[]);
void quadro2buffer_u(Quadro *q,unsigned char buffer[]);
int Arq_Get(int conexao,char *param);
void Arq_Put(int conexao,char *param);
int valida_timeout(int conexao);
int Enviar(int conexao,char buffer[]);
int Recebe_Msg(int conexao,Quadro *q);
int Recebe_Msg_S(int conexao,Quadro *q,int seq);
int Recebe_Msg_tipo(int conexao,Quadro *q,int tipo);
int Conecta_SERVIDOR(int conexao);
int Conecta_CLIENTE(int conexao);
int cd(char param[]);
int ls(char param[]);
int ls_remoto(char param[]);
int find(char param[]);
int Comando_Terminal_Local(int conexao,char *param,int tipo);
void Resposta_A_N(Quadro *q,int r);
void escreve_pacote(FILE *in,Quadro *q);
int retorna_crc(Quadro *q);
int Recebe_Pacotes_Dados(int conexao,char nome_arq[],int tipo);
int Comando_Terminal_Remoto(int conexao, char *param,int tipo);
int le_comando(char param[]);
int a_local();
int qtd_quadros(int t);
void le_dado(FILE *in, Quadro *q, int *tam);
int num_quadro(int k);
void le_frame(FILE *in,Quadro *q,int *tam);
int tamanho_dados(char dados[]);
int get_tamanho_arquivo(char file_name[]);
int qtd_pacotes(int t);
int le_buffer(FILE *in,char buffer[]);
int Envia_Pacotes(Quadro *p[],int max,int conexao,int qtd);
int enviou_tudo (int n1, int n2);
int Monta_Pacotes(int conexao, char nome_arq[],int tipo);
int a_remoto();
