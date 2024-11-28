#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

// Constantes
#define MAX_PACIENTES 1000
#define MAX_SALAS 100
#define MAX_ESPECIALIDADES 50
#define MAX_MEDICOS 200
#define MAX_CONSULTAS 5000

// Estruturas
typedef struct
{
    int id;
    char nome[50];
    int idade;
    float altura;
    float peso;
    char sintomas[100];
    int prioridade;
    int especialidadeId;
} Paciente;

typedef struct
{
    int ini;
    int fim;
    int tamanho;
    Paciente elementos[MAX_PACIENTES];
} filaPacientes;

typedef struct
{
    int id;
    char nome[50];
    int especialidadeId;
    int horasTrabalhadas;
} Medico;

typedef struct
{
    int id;
    char nome[50];
} Sala;

typedef struct
{
    int pacienteId;
    int medicoId;
    int salaId;
    int horario;
    int retorno;
    struct tm dataConsulta; // Data da consulta
    struct tm dataRetorno;  // Data do retorno
    int compareceu;
    int diaDaSemana;
} Consulta;

void inicializarFila(filaPacientes *fila)
{
    fila->ini = 0;
    fila->fim = -1;
    fila->tamanho = 0;
}

int filaVazia(filaPacientes *fila)
{
    return fila->tamanho == 0;
}

void inserirNaFila(filaPacientes *fila, Paciente paciente)
{
    if (fila->tamanho >= MAX_PACIENTES)
    {
        printf("Erro: a fila está cheia.\n");
        return;
    }
    fila->fim = (fila->fim + 1) % MAX_PACIENTES;
    fila->elementos[fila->fim] = paciente;
    fila->tamanho++;
}

Paciente removerDaFila(filaPacientes *fila)
{
    if (filaVazia(fila))
    {
        printf("Erro: a fila está vazia.\n");
        Paciente pacienteVazio = {0}; // Retorna um paciente "vazio" em caso de erro
        return pacienteVazio;
    }
    Paciente paciente = fila->elementos[fila->ini];
    fila->ini = (fila->ini + 1) % MAX_PACIENTES;
    fila->tamanho--;
    return paciente;
}

void imprimirFilaPacientes(filaPacientes *fila)
{
    if (filaVazia(fila))
    {
        printf("A fila de pacientes está vazia.\n");
        return;
    }

    printf("=== Fila de Pacientes ===\n");
    int i = fila->ini;
    for (int j = 0; j < fila->tamanho; j++)
    {
        Paciente paciente = fila->elementos[i];
        printf("Paciente ID: %d, Nome: %s, Idade: %d, Prioridade: %d, Especialidade ID: %d\n",
               paciente.id, paciente.nome, paciente.idade, paciente.prioridade, paciente.especialidadeId);
        i = (i + 1) % fila->tamanho;
    }
    printf("\n");
}

void imprimirMedicos(Medico *medicos, int numMedicos)
{
    if (numMedicos == 0)
    {
        printf("Nenhum médico cadastrado.\n");
        return;
    }

    printf("=== Médicos ===\n");
    for (int i = 0; i < numMedicos; i++)
    {
        printf("Médico ID: %d, Nome: %s, Especialidade ID: %d, Horas Trabalhadas: %d\n",
               medicos[i].id, medicos[i].nome, medicos[i].especialidadeId, medicos[i].horasTrabalhadas);
    }
    printf("\n");
}

void imprimirSalas(Sala *salas, int numSalas)
{
    if (numSalas == 0)
    {
        printf("Nenhuma sala cadastrada.\n");
        return;
    }

    printf("=== Salas ===\n");
    for (int i = 0; i < numSalas; i++)
    {
        printf("Sala ID: %d, Nome: %s\n", salas[i].id, salas[i].nome);
    }
    printf("\n");
}

void heapifyFila(filaPacientes *fila, int n, int i)
{
    int maior = i;
    int esquerda = 2 * i + 1;
    int direita = 2 * i + 2;

    // Ajuste para trabalhar com fila circular
    int indiceAtual = (fila->ini + i) % fila->tamanho;
    int indiceEsquerda = (fila->ini + esquerda) % fila->tamanho;
    int indiceDireita = (fila->ini + direita) % fila->tamanho;

    // Verifica se o filho à esquerda tem maior prioridade
    if (esquerda < n && fila->elementos[indiceEsquerda].prioridade > fila->elementos[indiceAtual].prioridade)
    {
        maior = esquerda;
    }

    // Verifica se o filho à direita tem maior prioridade
    if (direita < n && fila->elementos[indiceDireita].prioridade > fila->elementos[(fila->ini + maior) % fila->tamanho].prioridade)
    {
        maior = direita;
    }

    // Se o maior não for a raiz, troque e reaplique o heapify
    if (maior != i)
    {
        int indiceMaior = (fila->ini + maior) % fila->tamanho;
        Paciente temp = fila->elementos[indiceAtual];
        fila->elementos[indiceAtual] = fila->elementos[indiceMaior];
        fila->elementos[indiceMaior] = temp;

        heapifyFila(fila, n, maior);
    }
}

void construirHeapFila(filaPacientes *fila)
{
    int n = fila->tamanho;
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapifyFila(fila, n, i);
    }
}

void ordenarFilaPorPrioridade(filaPacientes *fila)
{
    construirHeapFila(fila);

    // Extraindo os elementos da heap
    for (int i = fila->tamanho - 1; i > 0; i--)
    {
        int indiceInicio = fila->ini % fila->tamanho;
        int indiceAtual = (fila->ini + i) % fila->tamanho;

        // Troca o maior elemento (raiz) com o último elemento
        Paciente temp = fila->elementos[indiceInicio];
        fila->elementos[indiceInicio] = fila->elementos[indiceAtual];
        fila->elementos[indiceAtual] = temp;

        // Reduz o tamanho da heap e reaplica o heapify
        heapifyFila(fila, i, 0);
    }
}

// Função para ler dados
void lerDados(filaPacientes *filaPacientes, Medico *medicos, int *numMedicos, Sala *salas, int *numSalas)
{
    int numPacientes = 0;
    FILE *arquivo = fopen("entrada.txt", "r");
    if (!arquivo)
    {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return;
    }

    char linha[256];
    inicializarFila(filaPacientes); // Inicializa a fila de pacientes

    // Lendo pacientes
    fgets(linha, sizeof(linha), arquivo);
    while (fgets(linha, sizeof(linha), arquivo))
    {
        if (strncmp(linha, "Salas:", 6) == 0)
            break; // Próxima seção
        if (filaPacientes->tamanho >= MAX_PACIENTES)
        {
            printf("Número máximo de pacientes excedido.\n");
            break;
        }
        Paciente paciente;
        sscanf(linha, "%d %s %d %f %f %s %d",
               &paciente.id, paciente.nome, &paciente.idade, &paciente.altura,
               &paciente.peso, paciente.sintomas, &paciente.especialidadeId);
        paciente.prioridade = rand() % 10;
        inserirNaFila(filaPacientes, paciente);
        numPacientes++;
    }

    ordenarFilaPorPrioridade(filaPacientes);

    // Lendo salas
    while (fgets(linha, sizeof(linha), arquivo))
    {
        if (strncmp(linha, "Especialidades:", 15) == 0)
            break; // Próxima seção
        if (*numSalas >= MAX_SALAS)
        {
            printf("Número máximo de salas excedido.\n");
            break;
        }
        sscanf(linha, "%d %s",
               &salas[*numSalas].id,
               salas[*numSalas].nome);
        (*numSalas)++;
    }

    // Lendo especialidades
    int numEspecialidades = 0;
    char especialidades[MAX_ESPECIALIDADES][50];
    while (fgets(linha, sizeof(linha), arquivo))
    {
        if (strncmp(linha, "Médicos:", 8) == 0)
            break; // Próxima seção
        if (numEspecialidades >= MAX_ESPECIALIDADES)
        {
            printf("Número máximo de especialidades excedido.\n");
            break;
        }
        sscanf(linha, "%d %s",
               &numEspecialidades,
               especialidades[numEspecialidades]);
        numEspecialidades++;
    }

    // Lendo médicos
    while (fgets(linha, sizeof(linha), arquivo))
    {
        if (*numMedicos >= MAX_MEDICOS)
        {
            printf("Número máximo de médicos excedido.\n");
            break;
        }
        sscanf(linha, "%d %s %d", &medicos[(*numMedicos)].id, medicos[(*numMedicos)].nome, &medicos[(*numMedicos)].especialidadeId);

        medicos[*numMedicos].horasTrabalhadas = 0; // Inicializa horas trabalhadas
        (*numMedicos)++;
    }

    fclose(arquivo);
    printf("Dados lidos do arquivo com sucesso!\n");
    printf("Pacientes lidos: %d\n", numPacientes);
    printf("Salas lidas: %d\n", *numSalas);
    printf("Especialidades lidas: %d\n", numEspecialidades);
    printf("Médicos lidos: %d\n", *numMedicos);
}

// Função para alocar consultas
void alocarConsultas(filaPacientes *filaPacientes, Medico *medicos, int numMedicos, Sala *salas, int numSalas, Consulta *consultas, int *numConsultas)
{
    int horariosMedicos[MAX_MEDICOS][17][7] = {0};
    int horariosSalas[MAX_SALAS][17][7] = {0};

    while (!filaVazia(filaPacientes))
    { // Processar enquanto houver pacientes na fila
        Paciente pacienteAtual = removerDaFila(filaPacientes);

        int consultaAlocada = 0;

        for (int d = 0; d < 7 && !consultaAlocada; d++)
        {
            for (int h = 8; h < 17 && !consultaAlocada; h++)
            {
                for (int s = 0; s < numSalas && !consultaAlocada; s++)
                {
                    for (int m = 0; m < numMedicos && !consultaAlocada; m++)
                    {
                        if (pacienteAtual.especialidadeId != medicos[m].especialidadeId)
                        {
                            continue;
                        }
                        if (horariosMedicos[m][h][d] == 0 && horariosSalas[s][h][d] == 0)
                        {
                            // Aloca a consulta
                            Consulta novaConsulta = {
                                .pacienteId = pacienteAtual.id,
                                .medicoId = medicos[m].id,
                                .salaId = salas[s].id,
                                .horario = h,
                                .retorno = 30};

                            int numero_aleatorio = rand() % 100;
                            novaConsulta.compareceu = (numero_aleatorio < 5) ? 0 : 1;

                            time_t t = time(NULL);
                            struct tm dataAtual = *localtime(&t);
                            dataAtual.tm_mday += d - 7;
                            mktime(&dataAtual);
                            novaConsulta.dataConsulta = dataAtual;

                            horariosMedicos[m][h][d] = 1;
                            horariosSalas[s][h][d] = 1;

                            consultas[(*numConsultas)++] = novaConsulta;
                            medicos[m].horasTrabalhadas++;

                            consultaAlocada = 1;
                        }
                    }
                }
            }
        }

        if (!consultaAlocada)
        {
            printf("Paciente %s (ID %d) não conseguiu consulta, voltando para a fila.\n", pacienteAtual.nome, pacienteAtual.id);
            inserirNaFila(filaPacientes, pacienteAtual); // Reinsere na fila
        }
    }
}

// Função auxiliar para determinar o dia da semana
const char *obterDiaDaSemana(int horario)
{
    // Cada dia tem 9 horas (8h às 17h), assumindo que o expediente começa em "horário = 8"
    int diaIndex = (horario - 8) / 9;
    const char *diasDaSemana[] = {"Domingo", "Segunda-feira", "Terça-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira", "Sábado"};
    return diasDaSemana[diaIndex % 7]; // Cicla entre os dias úteis
}

// Função ajustada para gerar relatório
void gerarRelatorio(Consulta *consultas, int numConsultas, Medico *medicos, int numMedicos, Paciente *pacientes, int numPacientes, Sala *salas)
{

    char bufferConsulta[20], bufferRetorno[20];
    FILE *arquivo = fopen("relatorio.txt", "w");
    if (!arquivo)
    {
        printf("Erro ao abrir arquivo para salvar o relatório.\n");
        return;
    }

    int maxConsultasDia = numMedicos * 9;

    fprintf(arquivo, "=== Relatório de Consultas ===\n\n");
    fprintf(arquivo, "Consultas Realizadas por Dia da Semana:\n");

    const char *diasDaSemana[] = {"Domingo", "Segunda-feira", "Terça-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira", "Sábado"};

    for (int d = 0; d < 7; d++)
    { // Itera pelos dias úteis
        fprintf(arquivo, "\n%s:\n", diasDaSemana[d]);
        int consultasNoDia = 0;
        for (int i = 0; i < maxConsultasDia; i++)
        {
            int consultaId = i + (d * maxConsultasDia);

            if (consultas[consultaId].diaDaSemana != d)
            {
                continue;
            }
            strftime(bufferConsulta, sizeof(bufferConsulta), "%d/%m/%Y", &consultas[consultaId].dataConsulta);

            const char *diaConsulta = obterDiaDaSemana(consultas[consultaId].horario);

            int dias = consultas[consultaId].horario % 24;

            if (consultas[consultaId].horario == 0 && consultas[consultaId].salaId == 0 || consultas[consultaId].retorno == 0)
            {
                break;
            }

            if (consultas[consultaId].compareceu)
            {
                fprintf(arquivo, "Consulta %d: Paciente %s, Médico %s, Sala %s, %s as %d:00 \n\n",
                        i + 1,
                        pacientes[consultas[consultaId].pacienteId].nome,
                        medicos[consultas[consultaId].medicoId].nome,
                        salas[consultas[consultaId].salaId].nome,
                        bufferConsulta,
                        dias);
            }
            else
            {
                fprintf(arquivo, "Consulta %d: Paciente %s (ID %d), Médico %s, Sala %s, %s as %d:00 ( O paciente não compareceu )\n\n",
                        i + 1,
                        pacientes[consultas[consultaId].pacienteId].nome,
                        pacientes[consultas[consultaId].pacienteId].id,
                        medicos[consultas[consultaId].medicoId].nome,
                        salas[consultas[consultaId].salaId].nome,
                        bufferConsulta,
                        dias);
            }
            consultasNoDia++;
        }

        if (consultasNoDia == 0)
        {
            fprintf(arquivo, "  Nenhuma consulta agendada.\n");
        }
    }

    fprintf(arquivo, "\n\n=== Consultas de retorno ===\n\n");
    for (int i = 0; i < numConsultas; i++)
    {
        // Formatar datas da consulta e do retorno
        strftime(bufferRetorno, sizeof(bufferRetorno), "%d/%m/%Y", &consultas[i].dataConsulta);

        int horario = consultas[i].horario % 24;
        int dias = consultas[i].horario / 24;
        if (consultas[i].retorno == 0)
        {
            fprintf(arquivo, "Consulta %d: Paciente: %s, Médico: %s, Sala: %s, dia e hora: %d:00, %s \n\n",
                    i + 1,
                    pacientes[consultas[i].pacienteId].nome,
                    medicos[consultas[i].medicoId].nome,
                    salas[consultas[i].salaId].nome,
                    horario,
                    bufferRetorno);
        }
    }

    fprintf(arquivo, "\nResumo de Horas Trabalhadas:\n");
    for (int i = 0; i < numMedicos; i++)
    {
        fprintf(arquivo, "- Médico %s (ID %d): %d %s\n",
                medicos[i].nome, medicos[i].id, medicos[i].horasTrabalhadas,
                medicos[i].horasTrabalhadas == 1 ? "hora" : "horas");
    }

    fclose(arquivo);
    printf("Relatório salvo em 'relatorio.txt'.\n");
}

void gerenciarRetornos(Consulta *consultas, int *numConsultas, int maxConsultas)
{
    printf("Gerenciando retornos...\n");

    // Iterar sobre todas as consultas existentes
    for (int i = 0; i < *numConsultas; i++)
    {
        // Verificar se a consulta original tem um retorno agendado
        if (consultas[i].retorno != 0 && consultas[i].compareceu)
        {
            // Formatar datas da consulta e do retorno
            if (*numConsultas < maxConsultas)
            {
                Consulta novaConsulta = {
                    .pacienteId = consultas[i].pacienteId,
                    .medicoId = consultas[i].medicoId,
                    .salaId = consultas[i].salaId,
                    .horario = consultas[i].horario + (consultas[i].retorno * 24), // 30 dias em horas
                    .retorno = 0                                                   // Retorno do retorno não será definido
                };

                // Obter a data atual
                struct tm dataAtual = consultas[i].dataConsulta;

                // Definir a data da consulta como a data atual
                dataAtual.tm_mday += 30;
                mktime(&dataAtual);
                novaConsulta.dataConsulta = dataAtual;

                // Adicionar a nova consulta à lista
                consultas[(*numConsultas)++] = novaConsulta;

                int horario = novaConsulta.horario % 24;
                int dias = novaConsulta.horario / 24;
                printf("Retorno agendado: Paciente %d com Médico %d na Sala %d as %d horas em %d dias.\n",
                       novaConsulta.pacienteId, novaConsulta.medicoId,
                       novaConsulta.salaId, horario, dias);
            }
            else
            {
                printf("Capacidade máxima de consultas atingida. Não foi possível agendar retorno.\n");
            }
        }
    }
}

// Função principal
int main()
{
    filaPacientes filaPacientes;
    Medico medicos[MAX_MEDICOS];
    Sala salas[MAX_SALAS];
    Consulta consultas[MAX_CONSULTAS];
    int numPacientes = 0, numMedicos = 0, numSalas = 0, numConsultas = 0;

    // Gera seeds aleatórias
    srand(time(NULL));
    // Ler dados do arquivo
    lerDados(&filaPacientes, medicos, &numMedicos, salas, &numSalas);

    // Ordena os pacientes na fila por prioridade antes de alocar as consultas
    ordenarFilaPorPrioridade(&filaPacientes);

    imprimirFilaPacientes(&filaPacientes);
    imprimirMedicos(medicos, numMedicos);
    imprimirSalas(salas, numSalas);

    // Alocar consultas
    alocarConsultas(&filaPacientes, medicos, numMedicos, salas, numSalas, consultas, &numConsultas);

    // Gerar relatório (implementação dessa função não foi alterada, você pode chamá-la aqui)
    gerarRelatorio(consultas, numConsultas, medicos, numMedicos, filaPacientes.elementos, numPacientes, salas);

    return 0;
}