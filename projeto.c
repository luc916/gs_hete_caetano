#define _USE_MATH_DEFINES // Necessário para M_PI (principalmente em compiladores MSVC)
#include <stdio.h>        // Para funções de entrada/saída padrão (printf, fgets, fopen, etc.)
#include <string.h>       // Para manipulação de strings (strlen, strcpy, strcmp, strchr)
#include <stdlib.h>       // Para conversões de tipo (atoi, atof) e alocação de memória
#include <math.h>         // Para funções matemáticas (sqrt, sin, cos, M_PI)
#include <ctype.h>        // Para verificação e conversão de caracteres (isalpha, isdigit, toupper)
#include <stdbool.h>      // Para utilizar o tipo booleano 'bool' e os literais true/false

// --- DEFINICOES GLOBAIS E ESTRUTURAS ---
#define MAX_NOME 101
#define MAX_CPF 12
#define MAX_TELEFONE 12
#define MAX_EMAIL 101
#define MAX_ENDERECO 101
#define MAX_NUMERO_END 11
#define MAX_COMPLEMENTO 51
#define MAX_BAIRRO 51
#define MAX_CIDADE 51
#define MAX_ESTADO 3
#define MAX_TIPO_CATASTROFE 51
#define MAX_DATA 11 // DD-MM-AAAA
#define MAX_HORA 6  // HH:MM
#define MAX_DESCRICAO 512
#define MAX_LINHA_ARQUIVO 600

const double RAIO_TERRA_KM = 6371.0;    // Raio da Terra em km.
const char NOME_ARQUIVO_DADOS[] = "relatos_c.txt"; // Arquivo de dados.
const double RAIO_MAXIMO_CONSULTA_KM = 10.0; // Raio máximo para consulta.
const int MAX_RELATOS = 100;            // Máximo de relatos.

// Estrutura 'Relato': modela os dados de um relato.
typedef struct {
    int id;
    char nomeRelator[MAX_NOME], cpfRelator[MAX_CPF], telefoneRelator[MAX_TELEFONE], emailRelator[MAX_EMAIL];
    char ruaRelator[MAX_ENDERECO], numeroEnderecoRelator[MAX_NUMERO_END], complementoRelator[MAX_COMPLEMENTO];
    char bairroRelator[MAX_BAIRRO], cidadeRelator[MAX_CIDADE], estadoRelator[MAX_ESTADO];
    char tipoCatastrofe[MAX_TIPO_CATASTROFE], dataEvento[MAX_DATA], horaEvento[MAX_HORA];
    char descricaoCatastrofe[MAX_DESCRICAO];
    double latitudeCatastrofe, longitudeCatastrofe;
} Relato;

Relato todosRelatos[MAX_RELATOS]; // Array global para relatos.
int numeroDeRelatos = 0;          // Quantidade atual de relatos.
int proximoId = 1;                // Próximo ID para relato.
const char DELIMITADOR_ARQUIVO_DESCRICAO[] = "<<FIM_DESC_ARQUIVO>>"; // Delimitador no arquivo.

// --- FUNCOES UTILITARIAS ---
// Remove '\n' do final de uma string (lida por fgets).
void removerNovaLinha(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}
// Consome caracteres remanescentes no buffer de entrada (stdin).
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
// Lê string não vazia do usuário via fgets.
void lerStringNaoVazia(const char *prompt, char *destino, int tamanhoMax) {
    do {
        printf("%s", prompt);
        if (fgets(destino, tamanhoMax, stdin)) {
            removerNovaLinha(destino);
            if (strlen(destino) > 0) break;
        }
        printf("Entrada inválida. O campo não pode ser vazio.\n");
    } while (true);
}
// Lê string opcional (pode ser vazia) do usuário via fgets.
void lerStringOpcional(const char *prompt, char *destino, int tamanhoMax) {
    printf("%s", prompt);
    if (fgets(destino, tamanhoMax, stdin)) removerNovaLinha(destino);
    else destino[0] = '\0';
}
// Lê um valor double do usuário, com validação.
double lerDouble(const char *prompt) {
    char buffer[50]; double valor;
    while (true) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin)) {
            if (sscanf(buffer, "%lf", &valor) == 1) return valor;
        }
        printf("Entrada inválida. Insira um valor numérico.\n");
    }
}

// --- FUNCOES DE VALIDACAO ---
// Valida nome: não vazio, contém letras.
bool validarNome(const char *nome) {
    if (strlen(nome) == 0) return false;
    for (int i = 0; nome[i] != '\0'; i++) if (isalpha(nome[i])) return true;
    return false;
}
// Valida CPF: 11 dígitos numéricos.
bool validarCPF(const char *cpf) {
    if (strlen(cpf) != 11) return false;
    for (int i = 0; cpf[i] != '\0'; i++) if (!isdigit(cpf[i])) return false;
    return true;
}
// Valida telefone: 10 ou 11 dígitos numéricos.
bool validarTelefone(const char *tel) {
    size_t len = strlen(tel);
    if (len < 10 || len > 11) return false;
    for (int i = 0; tel[i] != '\0'; i++) if (!isdigit(tel[i])) return false;
    return true;
}
// Valida email: formato básico com '@' e '.'.
bool validarEmail(const char *email) {
    const char *arroba = strchr(email, '@');
    if (!arroba || arroba == email || *(arroba + 1) == '\0') return false;
    const char *ponto = strchr(arroba + 1, '.');
    return (ponto && ponto != arroba + 1 && *(ponto + 1) != '\0');
}
// Valida UF: 2 caracteres, converte para maiúsculas.
bool validarUF(char *uf) {
    if (strlen(uf) != 2) return false;
    uf[0] = toupper(uf[0]); uf[1] = toupper(uf[1]);
    return isupper(uf[0]) && isupper(uf[1]);
}
// Valida coordenadas: latitude (-90 a 90) e longitude (-180 a 180).
bool validarCoordenadas(double lat, double lon) {
    return (lat >= -90.0 && lat <= 90.0 && lon >= -180.0 && lon <= 180.0);
}
// Valida data (DD-MM-AAAA): formato e intervalo. (Simplificado: não valida dias/mês específico)
bool validarData(const char *data) {
    int d, m, a;
    if (strlen(data) != 10 || data[2] != '-' || data[5] != '-') return false;
    if (sscanf(data, "%d-%d-%d", &d, &m, &a) != 3) return false;
    return (a >= 1900 && a <= 2025 && m >= 1 && m <= 12 && d >= 1 && d <= 31);
}
// Valida hora (HH:MM): formato e intervalo.
bool validarHora(const char *hora) {
    int h, m;
    if (strlen(hora) != 5 || hora[2] != ':') return false;
    if (sscanf(hora, "%d:%d", &h, &m) != 2) return false;
    return (h >= 0 && h <= 23 && m >= 0 && m <= 59);
}

// --- CALCULO DE DISTANCIA ---
// Converte graus para radianos.
double toRadians(double grau) { return grau * M_PI / 180.0; }
// Calcula distância (km) entre coordenadas geográficas (Haversine).
double calcularDistancia(double lat1, double lon1, double lat2, double lon2) {
    lat1 = toRadians(lat1); lon1 = toRadians(lon1); lat2 = toRadians(lat2); lon2 = toRadians(lon2);
    double dLat = lat2 - lat1, dLon = lon2 - lon1;
    double a = sin(dLat/2)*sin(dLat/2) + cos(lat1)*cos(lat2)*sin(dLon/2)*sin(dLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return RAIO_TERRA_KM * c;
}

// --- PERSISTENCIA DE DADOS ---
// Salva todos os relatos no arquivo.
void salvarRelatos() {
    FILE *arq = fopen(NOME_ARQUIVO_DADOS, "w");
    if (!arq) { fprintf(stderr, "Erro ao salvar dados.\n"); return; }
    for (int i = 0; i < numeroDeRelatos; ++i) {
        Relato *r = &todosRelatos[i];
        fprintf(arq, "%d\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%f\n%f\n%s\n%s\n",
                r->id, r->nomeRelator, r->cpfRelator, r->telefoneRelator, r->emailRelator,
                r->ruaRelator, r->numeroEnderecoRelator, r->complementoRelator, r->bairroRelator,
                r->cidadeRelator, r->estadoRelator, r->tipoCatastrofe, r->dataEvento,
                r->horaEvento, r->latitudeCatastrofe, r->longitudeCatastrofe,
                r->descricaoCatastrofe, DELIMITADOR_ARQUIVO_DESCRICAO);
    }
    fclose(arq); printf("Dados salvos em %s\n", NOME_ARQUIVO_DADOS);
}
// Lê uma linha do arquivo, removendo '\n'. Retorna false em EOF/erro.
bool lerCampoArquivo(FILE *arq, char *destino, int tamMax) {
    if (fgets(destino, tamMax, arq) == NULL) return false;
    removerNovaLinha(destino);
    return true;
}
// Carrega relatos do arquivo para a memória.
void carregarRelatos() {
    FILE *arq = fopen(NOME_ARQUIVO_DADOS, "r");
    if (!arq) { printf("Arquivo '%s' não encontrado. Iniciando sem dados.\n", NOME_ARQUIVO_DADOS); return; }
    char linhaBuffer[MAX_LINHA_ARQUIVO]; int maxIdLido = 0; numeroDeRelatos = 0;

    while (numeroDeRelatos < MAX_RELATOS && lerCampoArquivo(arq, linhaBuffer, sizeof(linhaBuffer))) {
        Relato *r = &todosRelatos[numeroDeRelatos];
        if (sscanf(linhaBuffer, "%d", &r->id) != 1) { fprintf(stderr, "Erro ID. Parando carga.\n"); break;}
        if (r->id > maxIdLido) maxIdLido = r->id;

        if (!lerCampoArquivo(arq,r->nomeRelator,MAX_NOME) || !lerCampoArquivo(arq,r->cpfRelator,MAX_CPF) ||
            !lerCampoArquivo(arq,r->telefoneRelator,MAX_TELEFONE) || !lerCampoArquivo(arq,r->emailRelator,MAX_EMAIL) ||
            !lerCampoArquivo(arq,r->ruaRelator,MAX_ENDERECO) || !lerCampoArquivo(arq,r->numeroEnderecoRelator,MAX_NUMERO_END) ||
            !lerCampoArquivo(arq,r->complementoRelator,MAX_COMPLEMENTO) || !lerCampoArquivo(arq,r->bairroRelator,MAX_BAIRRO) ||
            !lerCampoArquivo(arq,r->cidadeRelator,MAX_CIDADE) || !lerCampoArquivo(arq,r->estadoRelator,MAX_ESTADO) ||
            !lerCampoArquivo(arq,r->tipoCatastrofe,MAX_TIPO_CATASTROFE) || !lerCampoArquivo(arq,r->dataEvento,MAX_DATA) ||
            !lerCampoArquivo(arq,r->horaEvento,MAX_HORA) ||
            !lerCampoArquivo(arq,linhaBuffer,sizeof(linhaBuffer)) || sscanf(linhaBuffer,"%lf",&r->latitudeCatastrofe)!=1 ||
            !lerCampoArquivo(arq,linhaBuffer,sizeof(linhaBuffer)) || sscanf(linhaBuffer,"%lf",&r->longitudeCatastrofe)!=1) {
            fprintf(stderr, "Erro dados relato ID %d. Arquivo corrompido?\n", r->id); break;
        }
        r->descricaoCatastrofe[0] = '\0'; char linhaDesc[MAX_DESCRICAO];
        while (lerCampoArquivo(arq, linhaDesc, sizeof(linhaDesc)) && strcmp(linhaDesc, DELIMITADOR_ARQUIVO_DESCRICAO) != 0) {
            if (strlen(r->descricaoCatastrofe) > 0) strcat(r->descricaoCatastrofe, "\n");
            if (strlen(r->descricaoCatastrofe) + strlen(linhaDesc) < MAX_DESCRICAO -1 ) {
                 strcat(r->descricaoCatastrofe, linhaDesc);
            } else {
                fprintf(stderr, "Aviso: Descricao relato ID %d truncada.\n", r->id);
                while (lerCampoArquivo(arq, linhaDesc, sizeof(linhaDesc)) && strcmp(linhaDesc, DELIMITADOR_ARQUIVO_DESCRICAO) != 0);
                break;
            }
        }
        numeroDeRelatos++;
    }
    proximoId = maxIdLido + 1; fclose(arq);
    if (numeroDeRelatos > 0) printf("%d relatos carregados.\n", numeroDeRelatos);
}

// --- FUNCOES PRINCIPAIS DO SISTEMA ---
// Coleta dados e cadastra novo relato.
void cadastrarRelato() {
    if (numeroDeRelatos >= MAX_RELATOS) { printf("Capacidade máxima atingida.\n"); return; }
    Relato *nr = &todosRelatos[numeroDeRelatos]; nr->id = proximoId++;
    printf("\n--- Cadastro Novo Relato (ID: %d) ---\nDados do Relator:\n", nr->id);
    do { lerStringNaoVazia("Nome completo: ", nr->nomeRelator, MAX_NOME); if(!validarNome(nr->nomeRelator)) printf("Nome inválido.\n"); } while(!validarNome(nr->nomeRelator));
    do { lerStringNaoVazia("CPF (11 números): ", nr->cpfRelator, MAX_CPF); if(!validarCPF(nr->cpfRelator)) printf("CPF inválido.\n"); } while(!validarCPF(nr->cpfRelator));
    do { lerStringNaoVazia("Telefone (DDD+Num): ", nr->telefoneRelator, MAX_TELEFONE); if(!validarTelefone(nr->telefoneRelator)) printf("Telefone inválido.\n"); } while(!validarTelefone(nr->telefoneRelator));
    do { lerStringNaoVazia("Email: ", nr->emailRelator, MAX_EMAIL); if(!validarEmail(nr->emailRelator)) printf("Email inválido.\n"); } while(!validarEmail(nr->emailRelator));
    printf("\nEndereço do Relator:\n");
    lerStringNaoVazia("Rua/Avenida: ", nr->ruaRelator, MAX_ENDERECO); lerStringNaoVazia("Número: ", nr->numeroEnderecoRelator, MAX_NUMERO_END);
    lerStringOpcional("Complemento (Enter p/pular): ", nr->complementoRelator, MAX_COMPLEMENTO); lerStringNaoVazia("Bairro: ", nr->bairroRelator, MAX_BAIRRO);
    lerStringNaoVazia("Cidade: ", nr->cidadeRelator, MAX_CIDADE);
    do { lerStringNaoVazia("Estado (UF): ", nr->estadoRelator, MAX_ESTADO); if(!validarUF(nr->estadoRelator)) printf("UF inválida.\n"); } while(!validarUF(nr->estadoRelator));
    printf("\nDados da Catástrofe:\n");
    lerStringNaoVazia("Tipo: ", nr->tipoCatastrofe, MAX_TIPO_CATASTROFE);
    do { lerStringNaoVazia("Data (DD-MM-AAAA): ", nr->dataEvento, MAX_DATA); if(!validarData(nr->dataEvento)) printf("Data inválida.\n"); } while(!validarData(nr->dataEvento));
    do { lerStringNaoVazia("Hora (HH:MM): ", nr->horaEvento, MAX_HORA); if(!validarHora(nr->horaEvento)) printf("Hora inválida.\n"); } while(!validarHora(nr->horaEvento));
    do { nr->latitudeCatastrofe = lerDouble("Latitude (-90 a 90): "); nr->longitudeCatastrofe = lerDouble("Longitude (-180 a 180): "); if(!validarCoordenadas(nr->latitudeCatastrofe, nr->longitudeCatastrofe)) printf("Coords. inválidas.\n"); } while(!validarCoordenadas(nr->latitudeCatastrofe, nr->longitudeCatastrofe));
    printf("Descrição (Enter linha vazia p/finalizar):\n"); nr->descricaoCatastrofe[0] = '\0';
    char linhaDesc[MAX_DESCRICAO];
    while (true) { fgets(linhaDesc, sizeof(linhaDesc), stdin); removerNovaLinha(linhaDesc); if (strlen(linhaDesc) == 0) break;
        if (strlen(nr->descricaoCatastrofe) > 0) strncat(nr->descricaoCatastrofe, "\n", MAX_DESCRICAO - strlen(nr->descricaoCatastrofe) - 1);
        strncat(nr->descricaoCatastrofe, linhaDesc, MAX_DESCRICAO - strlen(nr->descricaoCatastrofe) - 1);
    }
    if(strlen(nr->descricaoCatastrofe) == 0) strcpy(nr->descricaoCatastrofe, "Nenhuma descrição fornecida.");
    numeroDeRelatos++; printf("Relato cadastrado!\n"); salvarRelatos();
}
// Exibe detalhes de um relato.
void exibirRelato(const Relato *r) {
    printf("\n----------------------------------------\nID: %d\n  Relator:\n    Nome: %s\n    CPF: %s\n    Telefone: %s\n    Email: %s\n",
           r->id, r->nomeRelator, r->cpfRelator, r->telefoneRelator, r->emailRelator);
    printf("    Endereco: %s, %s%s%s\n              %s - %s/%s\n", r->ruaRelator, r->numeroEnderecoRelator,
           strlen(r->complementoRelator) > 0 ? " - " : "", r->complementoRelator,
           r->bairroRelator, r->cidadeRelator, r->estadoRelator);
    printf("  Catastrofe:\n    Tipo: %s\n    Data/Hora: %s as %s\n    Localizacao: Lat %.6f, Lon %.6f\n    Descricao: \n%s\n----------------------------------------\n",
           r->tipoCatastrofe, r->dataEvento, r->horaEvento, r->latitudeCatastrofe, r->longitudeCatastrofe, r->descricaoCatastrofe);
}
// Lista todos os relatos.
void listarTodosRelatos() {
    if (numeroDeRelatos == 0) { printf("Nenhum relato cadastrado.\n"); return; }
    printf("\n--- Lista de Todos os Relatos (%d) ---\n", numeroDeRelatos);
    for (int i = 0; i < numeroDeRelatos; ++i) exibirRelato(&todosRelatos[i]);
}
// Consulta relatos por raio.
void consultarPorRaio() {
    if (numeroDeRelatos == 0) { printf("Nenhum relato para consulta.\n"); return; }
    printf("\n--- Consulta por Raio (ate %.1f km) ---\n", RAIO_MAXIMO_CONSULTA_KM);
    double latR = lerDouble("Latitude de referencia: "); double lonR = lerDouble("Longitude de referencia: ");
    if(!validarCoordenadas(latR, lonR)) { printf("Coords. de referencia invalidas.\n"); return; }
    int encontrados = 0; printf("Relatos encontrados:\n");
    for (int i = 0; i < numeroDeRelatos; ++i) {
        double distancia = calcularDistancia(latR, lonR, todosRelatos[i].latitudeCatastrofe, todosRelatos[i].longitudeCatastrofe);
        if (distancia <= RAIO_MAXIMO_CONSULTA_KM) { exibirRelato(&todosRelatos[i]); printf("   (Distancia: %.2f km)\n", distancia); encontrados++; }
    }
    if (encontrados == 0) printf("Nenhum relato no raio de %.1f km.\n", RAIO_MAXIMO_CONSULTA_KM);
}

// --- ALGORITMOS DE ORDENACAO ---
// Troca a posição de dois relatos (auxiliar para Bubble Sort).
void trocarRelatos(Relato *a, Relato *b) { Relato temp = *a; *a = *b; *b = temp; }
// Converte data DD-MM-AAAA para AAAA-MM-DD (para ordenação).
void dataParaFormatoOrdenavel(const char *dataIn, char *dataOut) {
    if (strlen(dataIn) == 10) sprintf(dataOut, "%s-%s-%s", dataIn + 6, dataIn + 3, dataIn);
    else dataOut[0] = '\0';
}
// Ordena relatos pelo critério escolhido (Bubble Sort).
void ordenarRelatos(int criterio) {
    char dJ_ord[MAX_DATA], dJ1_ord[MAX_DATA];
    for (int i=0; i<numeroDeRelatos-1; ++i) {
        for (int j=0; j<numeroDeRelatos-i-1; ++j) {
            bool troca = false;
            if (criterio == 1) { if (todosRelatos[j].id > todosRelatos[j+1].id) troca = true; }
            else if (criterio == 2) {
                dataParaFormatoOrdenavel(todosRelatos[j].dataEvento, dJ_ord);
                dataParaFormatoOrdenavel(todosRelatos[j+1].dataEvento, dJ1_ord);
                int comp = strcmp(dJ_ord, dJ1_ord);
                if (comp > 0) troca = true;
                else if (comp == 0 && strcmp(todosRelatos[j].horaEvento, todosRelatos[j+1].horaEvento) > 0) troca = true;
            } else if (criterio == 3) { if (strcmp(todosRelatos[j].tipoCatastrofe, todosRelatos[j+1].tipoCatastrofe) > 0) troca = true; }
            if (troca) trocarRelatos(&todosRelatos[j], &todosRelatos[j+1]);
        }
    }
    printf("Relatos ordenados.\n"); listarTodosRelatos();
}

// --- MENUS E MAIN ---
// Exibe menu de ordenação e processa escolha.
void menuOrdenarRelatos() {
    if (numeroDeRelatos == 0) { printf("Nenhum relato para ordenar.\n"); return; }
    printf("\n--- Ordenar Relatos ---\n1. ID\n2. Data Evento\n3. Tipo Catastrofe\n0. Voltar\nOpcao: ");
    char buffer[10]; int crit = -1;
    fgets(buffer, sizeof(buffer), stdin); sscanf(buffer, "%d", &crit);
    if (crit >= 1 && crit <= 3) ordenarRelatos(crit);
    else if (crit != 0) printf("Criterio invalido.\n");
}
// Exibe menu principal.
void exibirMenu() {
    printf("\n===== Sistema de Relatos de Catastrofes (C) =====\n");
    printf("Relatos: %d/%d\n", numeroDeRelatos, MAX_RELATOS);
    printf("1. Cadastrar\n2. Listar\n3. Consultar Raio (%.1f km)\n4. Ordenar\n5. Salvar\n0. Sair\n", RAIO_MAXIMO_CONSULTA_KM);
    printf("=================================================\nEscolha: ");
}
// Função principal: gerencia o loop do menu e fluxo da aplicação.
int main() {
    carregarRelatos(); int opcao = -1; char bufferEntrada[10];
    do {
        exibirMenu();
        if (fgets(bufferEntrada, sizeof(bufferEntrada), stdin)) {
            if (sscanf(bufferEntrada, "%d", &opcao) != 1) opcao = -1;
        } else { opcao = 0; } // EOF (Ctrl+D/Ctrl+Z) para sair

        switch (opcao) {
            case 1: cadastrarRelato(); break;    case 2: listarTodosRelatos(); break;
            case 3: consultarPorRaio(); break;   case 4: menuOrdenarRelatos(); break;
            case 5: salvarRelatos(); break;      case 0: printf("Encerrando...\n"); break;
            default: printf("Opcao invalida.\n");
        }
    } while (opcao != 0);
    return 0;
}
