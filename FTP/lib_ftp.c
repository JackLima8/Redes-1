#include "lib_ftp.h"

int Rawsocket() {           // Cria um RawSocket
       int sock = socket(PF_PACKET,SOCK_RAW,0);  // Cria um socket do tipo RawSocket.
       if(sock==-1) {
               error(strerror(errno));
       }

       // Descobre o Id da interface ("eth#").
       struct ifreq ifr;

       memset(&ifr, 0, sizeof(struct ifreq));       // Inicializa com '0'.

       memcpy(ifr.ifr_name, "eth0" , sizeof("eth0")); // Copia a parte da memoria de 'device' em 'ifr.ifr_name'.
       if(ioctl(sock, SIOCGIFINDEX, &ifr)==-1) {
               error(strerror(errno));
       }
       int deviceid = ifr.ifr_ifindex;


	 struct sockaddr_ll sll;               // "Binda", no socket, o endereco guardado em 'sll'.
	
	 memset(&sll, 0, sizeof(sll));         // Inicializa com '0'.

	 sll.sll_family = AF_PACKET;           // Sempre AF_PACKET.
	 sll.sll_ifindex = deviceid;           // Id da interface. ("eth#")
	 sll.sll_protocol = htons(ETH_P_ALL);  // Protocolo da Camada Fisica.
	
      if(bind(sock, (struct sockaddr *) &sll, sizeof(sll))==-1) {
           error(strerror(errno));
       } 

 // Coloca a interface em modo Promiscuo.
      struct packet_mreq mr;

      memset(&mr, 0, sizeof(mr));         // Inicializa com '0'.

      mr.mr_ifindex = deviceid;           // Id da interface que sera mudada. ("eth#")
      mr.mr_type = PACKET_MR_PROMISC;     // Modo Promiscuo.

      if (setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr,sizeof(mr)) == -1) {  // Coloca o socket 'sock' em modo promiscuo.
          error(strerror(errno));
      }
      return sock;
 }



void Zera_Buffer(char buffer[]){
  int i;
  for(i=0;i<=TAM_QUADRO;i++) buffer[i]=0x00;

}
void Zera_Buffer_u(unsigned char buffer[]){
  int i;
  for(i=0;i<=TAM_QUADRO;i++) buffer[i]=0x00;

}



Quadro * novo_quadro(){
  Quadro *p;
  	p=malloc(sizeof(Quadro));
  	return p;
	
	
}


void buffer2quadro(unsigned char buffer[], Quadro *q){
    int i,k=0;
      	
    q->marca=buffer[0];
	q->tam=buffer[1];
	q->seq=buffer[2];
	q->tipo=buffer[3];
 	q->crc=buffer[4];
 	for(i=5;i<5+q->tam;i+=1){
		 q->dados[k]=buffer[i];
 	     k+=1;
 	}
    
	Zera_Buffer_u(buffer);
 
}

void wait ( int seconds ) //para auxiliar timeout
{
 clock_t endwait;
 endwait = clock () + seconds * CLOCKS_PER_SEC ;
 while (clock() < endwait) {}
}

void Monta_MSG(char seq,char tipo,char dados[],Quadro *q){
   int i;
  
   q->marca=0x00;
   q->tam=0x00;
   q->seq=0x00;
   q->tipo=0x00;
   q->marca=MARCA; 
    
   if((dados==NULL)||(dados=="")){
	   
	   q->tam=0;
   }
   else q->tam=strlen(dados); 
   q->seq=seq; 
   q->tipo=tipo; 
   for(i=0;i<=15;i++) q->dados[i]=0; 
   if(q->tam!=0) for(i=0;i<=q->tam;i++){ printf("Ma oieieoie\n"); q->dados[i]=dados[i]; }
   
   //q->crc=retorna_crc(dados);
   
  
}

// seta os valores de q->marca,q->seq,q->tam	 	 
Quadro  Seta_Quadro(Quadro *q,int tipo){
       int i;
       q->marca=MARCA;
       q->tam=0x00;
       q->seq=0x00;
       q->crc=0x00;
       q->tipo=tipo;
       for(i=0;i<=15;i++) q->dados[i]=0x00;
		   

}	        
	       
    
void quadro2buffer_u(Quadro *q,unsigned char buffer[]){
	int i,tam,k=5;
   
    Zera_Buffer_u(buffer);
    
	buffer[0]=q->marca;
	buffer[1]=q->tam;
	buffer[2]=q->seq;
	buffer[3]=q->tipo; 
	buffer[4]=q->crc;
	for(i=0;i<q->tam;i++){
		// printf("copiando %c pra buffer\n",q->dados[i]);
		buffer[k]=q->dados[i];
		k+=1;
	}
    printf("buffer depois -> -");
    for(i=5;i<=k;i++) printf("%c",buffer[i]);
    printf("-\n");
 
 
 
} 
void quadro2buffer(Quadro *q,unsigned char buffer[]){
	int i,tam,k=5;
    Zera_Buffer_u(buffer);
	buffer[0]=q->marca;
	buffer[1]=q->tam;
	buffer[2]=q->seq;
	buffer[3]=q->tipo; 
	buffer[4]=q->crc;
	for(i=0;i<q->tam;i++){
		buffer[k]=q->dados[i];
		k+=1;
	}
    //~ printf("######################## k==%d\n",k);
    printf("buffer depois -> -");
    for(i=5;i<=k;i++) printf("%c",buffer[i]);
    printf("-\n");
 
 
 
} 


int Arq_Get(int conexao,char *param){
	Quadro *q;
	int s=FALSE;
	unsigned char *buffer;
	
	
	
	 q=novo_quadro();
	 Seta_Quadro(q,GET);
	 buffer=malloc(TAM_QUADRO*sizeof(char));
	 
	 strcpy(q->dados,param);
	 q->tam=strlen(q->dados);
	 quadro2buffer(q,buffer);
	 printf("Enviando GET |%s|\n",q->dados);
     send(conexao,buffer,TAM_QUADRO,0);  // ENVIA SOLICITAÇÃO DE GET !!!!! =========>>>>>>
     printf("GET ENVIADO foi %d\n",q->tipo);
     // FUNÇÂO DE TEMPO ?????
      
     Seta_Quadro(q,0x00);
     Zera_Buffer_u(buffer); 
     printf("RECEBENDO ACK q->tipo %d\n",q->tipo);
     Recebe_Msg(conexao,q);
     printf("ACK veio %d MARCA[%d]\n",q->marca,q->tipo);  
    
     if(q->tipo==ACK){
		 printf(" ACK , trasferência permitida\n\t\t\nRECEBENDO PACOTES!!!!\n");
	     free(buffer);
	     free(q);
	     Recebe_Pacote(conexao,param,GET);
     }
     else {
		 printf("recebeu um nack,Arquivo não encontrado ou sem permissão\n");
		 free(buffer);
		 free(q);
		 return FALSE;
     }
    
   return TRUE;	
	
}


void Arq_Put(int conexao,char *param){
	Quadro *q;
	unsigned char buffer[TAM_QUADRO];
	
	q=novo_quadro();
	Seta_Quadro(q,PUT);
	
	strcpy(q->dados,param);
	q->tam=strlen(param);
	quadro2buffer(q,buffer);
	printf("Solicitando put \n");
	send(conexao,buffer,TAM_QUADRO,0); //  ==>enviando put
	
	Monta_Pacotes(conexao,q->dados,PUT);
	
	free(q);
		
}
    
int valida_timeout(int conexao){

  struct pollfd pfd;
  int pollRet;
  int num_tent;
  int aceitou;
  int result;
	
  pfd.fd = conexao;                      // Indica qual arquivo ele se refere.
  pfd.events = POLLIN;                     // POLLIN = tem dados para ler.

	pollRet = poll(&pfd, 1, TIMEOUT_M);           // Verifica o Timeout com a funcao poll().
   if (pollRet < 0) {
     perror("poll");
     return ERRO; //erro
   }
	else if (pollRet == 0)
		return FALSE; // nao recebeu nada
	else
		return TRUE; //ok recebeu alguma coisa 

}

int Enviar(int conexao,char buffer[]){
	int cont=0,timeout_status=FALSE;
	char buffer2[TAM_QUADRO];
	printf("enviando frame >%s<\n",buffer);	
	do{
	  	if(cont!=0) printf("Enviando pacote [%d tentativa]\n",cont);
	  	send(conexao,buffer,TAM_QUADRO,0);
	  	cont+=1;
	  	timeout_status=valida_timeout(conexao);
	}while((timeout_status==FALSE)&&(cont<=1000));
    	
	if(timeout_status==FALSE){
		printf("CONEXÃO PERDIDA !!!!! \n");
		return 0;
	}
	
	
	

	return 1;
	
}

int Recebe_Msg(int conexao,Quadro *q){
	int timeout_status=FALSE,i,r,result;
	unsigned char buffer[TAM_QUADRO];
	
	for(i=0;i<TAM_QUADRO;i++) buffer[i]=0x00;
     while(timeout_status==FALSE){
	    result=recv(conexao,buffer,TAM_QUADRO,0);
	    if(buffer[0]==MARCA) timeout_status=TRUE;  // AQUI TALVEZ SEJA NECESSÁRIA UMA FUNÇAO QUE CONTE O TEMPO , FIQUE ATENTO !!!!!
	     if(buffer[3]==DADOS){
			printf("\nrecebeu dados\n");
			int i;
			printf("\nrecebeu '");
			for(i=4;i<4+buffer[1];i+=1) printf("%c",buffer[i]);  
	        printf("'\n");
	    }
	 }
	 
	 buffer2quadro(buffer,q);
			
 return 1;	
}

int Recebe_Msg_S(int conexao,Quadro *q,int seq){
	int timeout_status=FALSE,i,r,result;
	unsigned char buffer[TAM_QUADRO];
	
	for(i=0;i<TAM_QUADRO;i++) buffer[i]=0x00;
    
     while(timeout_status==FALSE){
	    result=recv(conexao,buffer,TAM_QUADRO,0);
	    if((buffer[0]==MARCA)&&(buffer[2]==seq)) timeout_status=TRUE;  // AQUI TALVEZ SEJA NECESSÁRIA UMA FUNÇAO QUE CONTE O TEMPO , FIQUE ATENTO !!!!!
	     if((buffer[3]==DADOS)&&(buffer[2]==seq)){
			printf("\nrecebeu dados\n");
			int i;
			printf("\nrecebeu '");
			for(i=4;i<4+buffer[1];i+=1) printf("%c",buffer[i]);  
	        printf("'\n");
	    }
	 }
	 
	 buffer2quadro(buffer,q);
			
 return 1;	
}

int Recebe_Msg_tipo(int conexao,Quadro *q,int tipo){
	int timeout_status=FALSE,i,r,result;
	char buffer[TAM_QUADRO];
	
	for(i=0;i<TAM_QUADRO;i++) buffer[i]=0x00;
    
     while(timeout_status==FALSE){
	    result=recv(conexao,buffer,TAM_QUADRO,0);
	    if(buffer[0]==MARCA){  
	     if(buffer[3]==tipo){
		     timeout_status=TRUE;
			 printf("\nrecebeu :");
			int i;
			printf("\nrecebeu '");
			for(i=4;i<4+buffer[1];i+=1) printf("%c",buffer[i]);  
	        printf("'\n");
	    }
	   }
	 }
	 
	 
	 buffer2quadro(buffer,q);
			
 return 1;	
}


int Conecta_SERVIDOR(int conexao){
	Quadro *q;
	char buffer[TAM_QUADRO];
	
	 q=novo_quadro();
	 
	
		Seta_Quadro(q,ACK);
		quadro2buffer(q,buffer);
		printf("enviando ACK");
		Enviar(conexao,buffer);
		printf("ack de conecta foi\n");
		Zera_Buffer(buffer);
		printf("recebendo resposta do cliente ...\n");
		recv(conexao,buffer,TAM_QUADRO,0);
		printf(" ACK ? %d\n",q->tipo);
		buffer2quadro(buffer,q);
		if(q->tipo==ACK){
			 printf("\t\tCONEXAO OK\n");
		}
		else{
			 printf("erro no pacote conecta\n");
	         return;
	     }
	 
	 free(q);
	 
}

int Conecta_CLIENTE(int conexao){
    Quadro *q;
    char buffer[TAM_QUADRO];
    
    q=novo_quadro();
    printf("recebendo ack do servidor\n");
    Recebe_Msg(conexao,q);
    printf("ok ... agora mandando ack\n");
    if(q->tipo==ACK){
		Seta_Quadro(q,ACK);
		quadro2buffer(q,buffer);
		send(conexao,buffer,TAM_QUADRO,0);
		printf("CONEXÃO pronta para uso\n");
	
	}	
    free(q);
        




}

int cd(char param[]){
   char ini[1024],*usu;
   
   if(!strcmp(param,"")){
        usu=getenv("USER");
        strcpy(ini,"home/jack/");
        strcat(ini,usu);
        return chdir(ini);
    }
    else{
	   getcwd(ini,sizeof(ini));
	   strcat(ini,"/");
	   strcat(ini,param);
	   return chdir(ini);
	
	}
 }


int ls(char param[]){
   char ini[1024];
   if(param==NULL) sprintf(ini,"ls");
   else sprintf(ini,"%s%s%s","ls"," ",param);			
   printf(" executar -%s-\n",ini);
   system(ini);

}

int ls_remoto(char param[]){
   char buffer[1024];
   
   if((param==NULL)||(param="")) sprintf(buffer,"ls > Rls.txt");
   else sprintf(buffer,"%s%s%s","ls ",param," > Rls.txt");
   printf("executando %s\n",buffer);  
   system(buffer);

}
// param nulo é nulo garanta isso!!!!
int find(char param[]){
   FILE *in;
   char *b;
   char buffer[255];
   
   printf("%s\n",param);
   sprintf(buffer,"%s%s%s","find ",param," >find.txt");
   system(buffer);
   Zera_Buffer(buffer);
   in=fopen("find.txt","r");
   if(!in){ 
	   printf("Não foi possivel criar arquivo find.txt\n");
	   return FALSE;
   }
   else{
	   fgets(buffer,255,in);
       b=(char *)strtok(buffer,"\n");
       fclose(in);
       if(b==NULL) return FALSE;
	   if(!strcmp(b,param)) return TRUE;
	   else return FALSE;
    }	   
	 
	   
    
    
}


int Comando_Terminal_Local(int conexao,char *param,int tipo){
  int ret;
   if(tipo==CD){
	   if(ret=cd(param)==-1) printf("Diretório inexistente ou sem acesso\n");
   }
   else ls(param);
   
   if(param!=NULL) strcpy(param,"");
  return;  	     

}



void Resposta_A_N(Quadro *q,int r){
 
    q->marca=MARCA;
    q->seq=0x00;
    q->tam=0x00;
    q->tipo=r;
    q->crc=0x00;
     
	
}
void escreve_pacote(FILE *in,Quadro *q){
  
       fprintf(in,"%s",q->dados);
    
 }


int retorna_crc(Quadro *q){
  //~ char *string;
  //~ tam=strlen(q->dados);	 
  //~ string=malloc(sizeof(tam));
  //~ strcpy(buffer,q->dados);
  	//~ 
  //~ divisor.bits = POLINOMIO_CRC;
  //~ r.bits = string[0] << 1;
  //~ for (i=1; i < tam+2; i++) //percorrendo toda a string
  //~ {
     //~ while (j > -1) //percorrendo todos os bits de um char
     //~ {
        //~ c = string[i] << 8-j-1; //seleciona somente o bit indicado por j
        //~ c = c >> 7; //seleciona somente o bit indicado por j
//~ 
        //~ r.bits |= c; //encaixar c no fim de bits
//~ 
        //~ if (r.bits & 0x100) //caso bit mais significativo == 1, faz o xor
           //~ r.bits ^= divisor.bits;
//~ 
        //~ if ((r.bits == 0x000) && (i == tam+1)) //testa se o resto da divição é zero e se não há mais o que dividir
           //~ return 0x00; //retorna o valor para ser colocado no campo CRC da mensagem, ou se deu erro (não deu erro, nesse caso)
//~ 
        //~ j--;
        //~ if (j != -1)
           //~ r.bits = (r.bits << 1);
     //~ }
     //~ j = 7;
  //~ }
	//~ 
	//~ 
	
	
	return TRUE;
}

int Recebe_Pacote(int conexao,char nome_arq[],int tipo){  //ls ou arq
    FILE *in;
    Quadro *q,*p;
    int i,cont=-1,k=-1,pacotes_rec=0,fim=FALSE,reenvio=FALSE,q_pacotes,j;
    char buffer[TAM_QUADRO];
    
    if(tipo==LS) in=fopen(nome_arq,"w");
    else in=fopen(nome_arq,"wb");
    
    q=novo_quadro();
    Seta_Quadro(q,0x00);
    p=novo_quadro();
    Seta_Quadro(p,0x00);
    
    //recebe atributos do arquivo
    Seta_Quadro(q,0x00);
    //Recebe_Msg(conexao,q);
    recv(conexao,buffer,TAM_QUADRO,0);
    buffer2quadro(buffer,q);
    printf("ATIBUTOS\n\t q->tipo == %d q->dados ==%s q->marca==%d\n",q->tipo,q->dados,q->marca);
    
    
    q_pacotes=atoi(q->dados);
    printf("total de pacotes a receber %d\n",q_pacotes);
    for(k=0;k<=q_pacotes+1;k++){
		    printf("recebendo pacote %d \n",k);
			Recebe_Msg(conexao,p);
			if(p->tipo==FIM){
			     printf("recebeu ultimo pacote\n");
					fim=TRUE;
					break;
			}
			else{
				if(retorna_crc(q)==TRUE){
					printf(" escrevendo em %s : [%s]\n",nome_arq,p->dados);
				    fprintf(in,"%s",p->dados);
					printf("enviando ack de %d\n",k);
					Seta_Quadro(q,ACK);
					quadro2buffer(q,buffer);
					Enviar(conexao,buffer);
				
				}
				else{
				   Seta_Quadro(q,NACK);
				   quadro2buffer(q,buffer);
				   Enviar(conexao,buffer);
				}
		   }
	        Seta_Quadro(p,0x00);
	}
    if(fim==TRUE) printf("ACABOU pq recebeu ultimo pacote!!!\n");
    else printf("ACABOU pq recebeu todos os pacotes esperados!!!\n");
    free(q);
	free(p);
	fclose(in);
    
    
    
}	

int Comando_Terminal_Remoto(int conexao, char *param,int tipo){
    Quadro *q;
    char buffer[TAM_QUADRO],ini[1024];
      printf("comando terminal remoto\n");
     q=novo_quadro();
     Seta_Quadro(q,tipo);
     
     if(param!=NULL){
	    q->tam=strlen(param);
	    strcpy(q->dados,param);
	    printf(" parametro : %s\n",q->dados);
	 }
	 else{
		 q->tam=0;
	     strcpy(q->dados,"");
	 
	 }
	 quadro2buffer(q,buffer);
     printf("enviando solicitação de Rls ou Rcd\n");
     send(conexao,buffer,TAM_QUADRO,0); //enviou solicitação de ls ou cd
     //Enviar(conexao,buffer);
    
     
     if(tipo==LS){
	    Recebe_Pacote(conexao,".txls",LS);
	    system("clear");
        printf(" # LS Remoto :\n");
        sprintf(ini,"%s","cat .txls");
        system(ini);
     }
     else{
		 Seta_Quadro(q,0x00);
		 printf("Recebendo ack ou nachk de cd ...");
		 Recebe_Msg(conexao,q);
		 printf("recebeu %d\n",q->tipo);
		 
         if(q->tipo==ACK){
		     printf("diretótio alvo :\n\t/%s\n",param);
             system("clear");
          }
         else{
			  printf ("Diretório alvo não encontrado \n");
			 free(q);
			  return FALSE;
		  }
	 }
     free(q);  
     return TRUE;  
      
      
      
      
}



int le_comando(char param[]){
    char buffer[255];
    char *cmd,*p;     
    
    printf("\n$ "); 
    fgets(buffer,255,stdin);
    cmd=(char *)strtok(buffer," ");
    p=(char *)strtok(NULL," ");
    if(!p){
		cmd=(char *)strtok(cmd,"\n");
	}
    else p=(char *)strtok(p,"\n");
    if(p==NULL) strcpy(p,"");
    printf("Comando: ->%s<- parametro: ->%s<-\n",cmd,p);
   
    strcpy(param,p); 
    
    if(!strcmp(cmd,"get")) return GET;
	else if(!strcmp(cmd,"put")) return PUT;
	else if(!strcmp(cmd,"ls")) return LS_LOCAL;
	else if(!strcmp(cmd,"cd")) return CD_LOCAL;
	else if(!strcmp(cmd,"Rls")) return LS_REMOTO;
	else if(!strcmp(cmd,"Rcd")) return CD_REMOTO;
	else if(!strcmp(cmd,"exit")) return FIM;    
    else return FALSE;


}


int a_local(){
   Quadro *q;
   int conexao=Rawsocket(),tipo;
   char param[255],buffer[TAM_QUADRO];
   conexao=Rawsocket();
   
    
    q=novo_quadro();
    Seta_Quadro(q,0x00);
    
    Conecta_CLIENTE(conexao);
       
    tipo=le_comando(param);
    
    while(tipo!=FIM){ 
		printf("tipo == %d\n",tipo);
		if(tipo==GET) Arq_Get(conexao,param);
		else if(tipo==PUT) Arq_Put(conexao,param);
		else if(tipo==LS_LOCAL) Comando_Terminal_Local(conexao,param,LS);
		else if(tipo==CD_LOCAL) Comando_Terminal_Local(conexao,param,CD);
		else if(tipo==LS_REMOTO) Comando_Terminal_Remoto(conexao,param,LS);
		else if(tipo==CD_REMOTO) Comando_Terminal_Remoto(conexao,param,CD);
        else printf("Comando Inexistente !!!\n");
       

        tipo=le_comando(param);
   }
   free(q);

	

	
	
 return 1;	
}


int qtd_quadros(int t){
	
  if((t%15)==0) return (t/15);
  else return (t/15)+1;
 

 
}


void le_dado(FILE *in, Quadro *q, int *tam){ //retorna dados referente a 1 frame  ----
       int i; char c;
       *tam = 15;
       for(i=0;i<=15;++i){ // 15 char  = 15 bytes
         if(!feof(in)){ //se tem algo para ler
               q->dados[i] = 0x00;
               q->dados[i] = fgetc(in);//c recebe 1 byte = 8 bits
			   
         }
         else{
               *tam = i-1; // contem dados ate i inclusive
			   for(i=*tam+1;i<=15;++i)
         	   q->dados[i] = 0x00;
               break;
         }
       }
       printf(" LEU : [%s]\n",q->dados);
}
   
 
    
   
    

int num_quadro(int k){
	switch(k){
	   case 1: return Q1; break;
	   case 2: return Q2; break;
	   case 3: return Q3;break;
	 default: return Q1; break;

	}

}



void le_frame(FILE *in,Quadro *q,int *tam){
    int i;
    *tam=15; // 15 bytes de dados
    for(i=0;i<=15;i++){
	    if(!feof(in)){
			q->dados[i]=0x00;
			q->dados[i]=fgetc(in);
	    }
	    else{
		 printf("Montando ultimo quadro tam<-%d\n",i-1);
	     *tam=i-1;
	     for(i=*tam+1;i<=15;i++) q->dados[i]=0x00;
	     break;
	    }
	  }    	



}

int tamanho_dados(char dados[]){
   int tam,i;
   
   printf("tam %d\n",tam);
   tam=strlen(dados);
   printf("tam %d\n",tam);
   if(tam>15) return 15;
   else return tam;
 	
 
}



int get_tamanho_arquivo(char file_name[]){
   FILE *tmp;
   int cont;
   
   tmp=fopen(file_name,"r");
   if(!tmp) return -1;
   else{
	   fseek(tmp,0L,SEEK_END);
	   cont=ftell(tmp);
	   fclose(tmp);
	   return cont;	   
   }





}

//~ int controle_pacotes(char file_name[],Quadro *pacotes[],int conexao,int tipo){
    //~ Quadro *resposta[3],*aux;
    //~ int tam_arq,qtd_pacotes,pac_enviados,quadro_atual=1;
    //~ int tam_mensagens[3],fim=FALSE;
    //~ FILE *in;
    //~ aux=novo_quadro();
    //~ printf("\n->%s<-\n",file_name);
    //~ tam_arq=get_tamanho_arquivo(file_name);
    //~ if(tam_arq==-1) return ERRO;
    //~ 
    //~ if(tipo==LS) in=fopen(file_name,"r");  // LS
    //~ else in=fopen(file_name,"rb");         // ARQUIVO
    //~ if(!in) return ERRO;
    //~ 
    //~ qtd_pacotes=qtd_quadros(tam_arq);
    //~ qtd_pacotes+=1;  // Ultimo, pacote terminador
    //~ while((fim==FALSE)&&(qtd_pacotes!=pac_enviados)){
	    //~ printf("Enviando pacotes ......\n");
	    //~ printf("montando quadro %d do pacote \n",quadro_atual-1);
	    //~ if(quadro_atual <=3){
		   //~ if(!feof(in)){
			   //~ printf("montado parte %d do pacote\n",qtd_pacotes);
			   //~ le_frame(in,pacotes[quadro_atual],&tam_mensagens[quadro_atual-1]);
		       //~ // montando quadro corrente
		       //~ pacotes[quadro_atual-1]->tam=0x00;
		       //~ pacotes[quadro_atual-1]->tam=tam_mensagens[quadro_atual-1];
		       //~ 
		       //~ //monta crc para o quadro atual
		  //~ //     pacote[num_quadro-1]->crc=retorna_crc(pacote[num_quadro-1]->dados,pacote[num_quadro-1]->tipo);
		        //~ printf("frame %d pronto!\n",quadro_atual-1);
		    //~ }
		    //~ else{
		       //~ printf("montando ultimo quadro %d !!!!\n",quadro_atual-1);
		       //~ Monta_MSG(num_quadro(quadro_atual),FIM,"",pacotes[quadro_atual-1]);
		       //~ fim=TRUE; 
		    //~ }
		//~ } 
		              //~ 
		//~ 
		//~ 
		//~ 
		//~ 
		//~ }
	//~ 
	//~ 
	//~ 
	//~ }
 
 
int qtd_pacotes(int t){
	
  if((t%4)==0) return (t/4);
  else return (t/4)+1;

} 

int le_buffer(FILE *in,char buffer[]){
	int i;
	Zera_Buffer(buffer);
    fgets(buffer,15,in);
    printf("[%s]\n",buffer);
	return 1;	
}

int Envia_P(int conexao,Quadro *p,int t){
    Quadro *q;
    int i,fim=FALSE,r=FALSE;
    unsigned char *buffer,cont=0;
    
    q=novo_quadro();
    Seta_Quadro(q,0x00);
    buffer=malloc(TAM_QUADRO*sizeof(char));
    
    printf("\n\nEnviando pacote  .... %d\n",t);                                             // enquanto não recebeu o ack
    Zera_Buffer(buffer);
    while(fim==FALSE){     
    		quadro2buffer(p,buffer);;
			
			send(conexao,buffer,TAM_QUADRO,0);
			 //r=valida_timeout(conexao);
			 //if(r==TRUE){
			    Recebe_Msg(conexao,q);
	            if(q->tipo==ACK) fim=TRUE;
	   //      }
	     //    else printf("problema na conexão\n");
	}
	printf("pacote %d enviado com sucesso\n",t);
    
    free(buffer);
    free(q);
}    

int enviou_tudo (int n1, int n2){

   if((n2-n1)==0) return 0;
  else return 1;

}

 
 
int Monta_Pacotes(int conexao, char nome_arq[],int tipo){
    FILE *in;
    Quadro *p,*q;
    int tam_arq,q_env=0,q_total,q_atual,q_pac;
    int pac_env=1,i,fim=FALSE,tam,w;
    char buffer[TAM_QUADRO],status,buff[TAM_QUADRO],*a,ini[1024];
    
    if(tipo==LS) in=fopen(nome_arq,"r");
	else in=fopen(nome_arq,"rb");
    
    printf("arquivo [%s]\n",nome_arq);
    tam_arq=get_tamanho_arquivo(nome_arq);
    if(tam_arq==-1){
		printf("Erro ao abrir arquivo [%s]\n",nome_arq);
		return FALSE;
	}
    q_total=qtd_quadros(tam_arq);
    q_total+=1;  // ultimo quadro
    printf("Quadros necessários + ultimo %d\n",q_total);
	
	q=novo_quadro();
	p=novo_quadro();
	Seta_Quadro(q,ATRIBUTOS);
    
    //enviando atributos do arquivo
    sprintf(ini,"%d%s",q_total," quadros");
    a=(char *)strtok(ini," ");
    printf("trollagem maxima %s\n",a);
    strcpy(q->dados,a);
    q->tam=strlen(a);
    printf("Enviando Atributos q->tipo=%d q->dados=%s\n",q->tipo,q->dados);
    quadro2buffer(q,buffer);
    //~ Enviar(conexao,buffer);
    send(conexao,buffer,TAM_QUADRO,0);

    q_atual=-1;
    printf("\n######################################################################\n");
    while(fim==FALSE){
	      q_atual++;
	      if(!feof(in)){
		        printf("montando pacote %d\n",q_atual); 
			    Seta_Quadro(p,DADOS);
				le_dado(in,p,&tam);
				printf("pacote[%d]->dados -%s- ",q_atual,p->dados);
				p->tam=tam;
			    printf(" tam == %d\n",p->tam);
			    if(feof(in)){
				   printf("Acabou arquivo no meio da leitura\n");
				   fim=TRUE;
				}
			    p->crc=retorna_crc(p);
		        Envia_P(conexao,p,q_atual);
		 }
         if((fim==TRUE)||(feof(in))){
			    printf("Enviando ultimo pacote !!\n");
                Seta_Quadro(q,FIM);
     			quadro2buffer(q,buffer);
     			send(conexao,buffer,TAM_QUADRO,0);
     			fim=TRUE;
		}
	}
	
    printf("\n######################################################################\n");
	printf("FIM DE TRANSFERENCIA !!!!\n");
	   
	fclose(in);
	return TRUE;
	
} 



int a_remoto(){
	Quadro *q,*p;
	int conexao=Rawsocket();
	unsigned char *buffer;
	int tipo,i,ret=FALSE;

	q=novo_quadro();
	Seta_Quadro(q,0x00);
	p=novo_quadro();
	Seta_Quadro(p,0x00);
    buffer=malloc(TAM_QUADRO*sizeof(char));
	 
	Conecta_SERVIDOR(conexao);
    
     Recebe_Msg(conexao,q);
     
      while(q->tipo!=FIM){
		if(q->tipo==LS){
	       printf("recebeu solicitação de ls remoto\n");
	       ls_remoto(q->dados);
	       printf("arquivo do RLS criado\n");
	       Monta_Pacotes(conexao,"Rls.txt",tipo);   
	    }
	   else if(q->tipo==GET){
	    	printf("recebeu solicitação de get -|%s|- ",q->dados);
 	        ret=find(q->dados);
	     
	      if(ret==TRUE){
			 Seta_Quadro(p,ACK);
			 Zera_Buffer(buffer);
			 quadro2buffer(p,buffer);
			 printf("Enviando ACK\n ");
			 send(conexao,buffer,TAM_QUADRO,0);
		     printf("ACK foi %d\n",q->tipo);
		     Monta_Pacotes(conexao,q->dados,tipo);
	       }
	       else{
		     Seta_Quadro(p,NACK);
	         Zera_Buffer(buffer);
			 quadro2buffer(p,buffer);
			 printf("Enviando NACK\n ");
			 send(conexao,buffer,TAM_QUADRO,0);
			 printf("Arquivo %s nao encontrado!!!!\n",q->dados);
	       }
	  }
	  else if(q->tipo==PUT){
		 printf(" Recebendo Arquivo |%s| \n",q->dados);
	     Recebe_Pacote(conexao,q->dados,PUT);
	  
	  }
	  else if(q->tipo==CD){
		  printf("Rcebeu solicitação de CD\n");
		 
		  if(cd(q->dados)==-1) Seta_Quadro(q,NACK);
		  else Seta_Quadro(q,ACK);	  
		  quadro2buffer(q,buffer);
		  send(conexao,buffer,TAM_QUADRO,0);
	  }  	
      Seta_Quadro(q,0x00);
      Recebe_Msg(conexao,q);
	}
	
	
	
	
return 0;	
}
